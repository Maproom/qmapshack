/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "helpers/CGdalVrtUtil.h"

#include <gdal.h>
#include <gdal_priv.h>

#include <QDebug>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QStringList>
#include <algorithm>

#include "canvas/IDrawContext.h"

bool CGdalVrtUtil::allReferencedFilesExist(GDALDataset* dataset, QString& missingFile) {
  char** fileList = dataset->GetFileList();
  bool allExist = true;
  for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
#if defined(Q_OS_WIN32)
    missingFile = QString::fromLocal8Bit(fileList[n]);
    if (QFileInfo::exists(missingFile)) {
      continue;
    }
#endif  // defined(Q_OS_WIN32)
    missingFile = QString::fromUtf8(fileList[n]);
    if (QFileInfo::exists(missingFile)) {
      continue;
    }
    allExist = false;
    break;
  }
  CSLDestroy(fileList);
  return allExist;
}

namespace {
/// @brief This dataset's own overview pixel widths, sorted descending (finest/largest
///        first, coarsest/smallest last); empty if it reports none.
QVector<qint32> ownOverviewSizes(GDALRasterBand* band) {
  QVector<qint32> result;
  for (qint32 i = 0; i < band->GetOverviewCount(); ++i) {
    result << band->GetOverview(i)->GetXSize();
  }
  std::sort(result.begin(), result.end(), std::greater<qint32>());
  return result;
}

/// @brief True if the coarsest of `sizes` already reduces a raster of width `xsize` by at
///        least `targetFactor`. `requiredSize` mirrors gdaladdo's own ceiling-division
///        overview sizing, so a freshly-built overview at exactly `targetFactor` passes.
bool meetsTarget(qint32 xsize, const QVector<qint32>& sizes, qint32 targetFactor) {
  if (sizes.isEmpty()) {
    return targetFactor <= 1;
  }
  const qint32 requiredSize = (xsize + targetFactor - 1) / targetFactor;
  return sizes.last() <= requiredSize;
}

/// @brief True if `file` (a GetFileList() entry) is a real source raster - i.e. not the
///        container itself (`ownPath`) and not one of its sidecars (.ovr/.aux.xml/.aux).
bool isSourceFile(const QString& file, const QString& ownPath) {
  return file != ownPath && !file.endsWith(".ovr", Qt::CaseInsensitive) &&
         !file.endsWith(".aux.xml", Qt::CaseInsensitive) && !file.endsWith(".aux", Qt::CaseInsensitive);
}

/// @brief One referenced source file's own width and overview pixel sizes (see
///        ownOverviewSizes()). All zero/empty if the file can't be opened.
struct probed_source_t {
  qint32 xsize = 0;
  QVector<qint32> sizes;
};

/// @brief Open path and probe its own overview sizes (native to path's own pixel grid -
///        no rescaling into the container's pixel grid is needed; GDAL matches overviews
///        to the requested read resolution itself, gdalwarp's "-ovr AUTO"). Also records
///        the source's own width (for meetsTarget()).
probed_source_t probeSource(const char* path) {
  GDALDatasetUniquePtr sub(GDALDataset::FromHandle(GDALOpen(path, GA_ReadOnly)));
  GDALRasterBand* subBand = sub ? sub->GetRasterBand(1) : nullptr;
  if (subBand == nullptr) {
    return {};
  }
  probed_source_t probed;
  probed.xsize = subBand->GetXSize();
  probed.sizes = ownOverviewSizes(subBand);
  return probed;
}
}  // namespace

void CGdalVrtUtil::closeDataset(GDALDataset*& dataset) {
  if (dataset != nullptr) {
    GDALClose(dataset);
    dataset = nullptr;
  }
}

int CGdalVrtUtil::progressCallback(double /*dfComplete*/, const char* /*message*/, void* pProgressArg) {
  auto* drawCtx = reinterpret_cast<IDrawContext*>(pProgressArg);
  return !drawCtx->needsRedraw();
}

CGdalVrtUtil::raster_geometry_t CGdalVrtUtil::sourceGeometry(GDALDataset* source) {
  raster_geometry_t geom;
  qreal gt[6];
  if (source == nullptr || source->GetGeoTransform(gt) != CE_None) {
    return geom;
  }
  const OGRSpatialReference* srs = source->GetSpatialRef();
  const bool latLong = (srs != nullptr) && srs->IsGeographic();
  geom.xsizePx = source->GetRasterXSize();
  geom.ysizePx = source->GetRasterYSize();
  geom.pixelSizeX = toMeters(qAbs(gt[1]), latLong);
  geom.pixelSizeY = toMeters(qAbs(gt[5]), latLong);
  return geom;
}

QVector<qint32> CGdalVrtUtil::suggestOverviewLevels(qint32 xsize, qint32 ysize, qint32 maxFactor) {
  const QScreen* screen = QGuiApplication::primaryScreen();
  const qint32 screenSize = screen ? qMax(screen->size().width(), screen->size().height()) : 1920;
  QVector<qint32> levels;
  qint32 factor = 2;
  while (qMax(xsize, ysize) / factor > screenSize && factor <= maxFactor) {
    levels << factor;
    factor *= 2;
  }
  return levels;
}

CGdalVrtUtil::overview_advice_t CGdalVrtUtil::buildOverviewAdvice(GDALDataset* dataset, GDALRasterBand* band,
                                                                  bool isPaletteIndexed,
                                                                  const QVector<qint32>& suggestedLevels) {
  overview_advice_t result;
  result.suggestedLevels = suggestedLevels;
  result.isPaletteIndexed = isPaletteIndexed;
  const qint32 targetFactor = suggestedLevels.isEmpty() ? 1 : suggestedLevels.last();
  qDebug() << "OVR: GetOverviewCount =" << band->GetOverviewCount() << "targetFactor =" << targetFactor;

  // Step 1: the container's own claim. dataset is always a VRT here (CMapVRT/CDemVRT
  // only ever open a .vrt file directly; every other raster format goes through its
  // own IMap/IDem class instead), and GDAL will report GetOverviewCount() > 0 purely
  // from a declared <OverviewList>, even with nothing behind it (a stale or
  // hand-edited declaration). Trust it immediately only if a real .ovr file is listed
  // (cheap, no GDALOpen); otherwise defer to step 3, once every source has been
  // probed anyway.
  // containerVerified only ever becomes true via the physical-.ovr-file check below, so
  // it doubles as "the container has its own .ovr file" - step 3 further down is the
  // only other way containerOverviewSizes/containerSufficient get trusted, and that path never
  // involves a file on disk.
  const QString ownPath = QString::fromUtf8(dataset->GetDescription());

  // One GetFileList() pass for the whole function: the .vrt, its own .ovr (if any), and each
  // source's main file - all as absolute paths. Reused by every step below.
  QStringList files;
  {
    char** fileList = dataset->GetFileList();
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      files << QString::fromUtf8(fileList[n]);
    }
    CSLDestroy(fileList);
  }

  const qint32 containerXSize = band->GetXSize();
  QVector<qint32> rawContainerSizes;
  bool containerVerified = false;
  if (band->GetOverviewCount() != 0) {
    rawContainerSizes = ownOverviewSizes(band);
    for (const QString& file : files) {
      // the container's own .ovr - distinct from a source's sidecar (isSourceFile excludes both)
      if (file != ownPath && file.endsWith(".ovr", Qt::CaseInsensitive)) {
        containerVerified = true;
        break;
      }
    }
  }
  result.containerOverviewSizes = containerVerified ? rawContainerSizes : QVector<qint32>{};
  result.containerHasOwnOvr = containerVerified;
  result.containerSufficient = containerVerified && meetsTarget(containerXSize, rawContainerSizes, targetFactor);
  qDebug() << "OVR: step 1: rawContainerSizes =" << rawContainerSizes << "hasOwnOvr =" << containerVerified
           << "sufficient =" << result.containerSufficient;

  if (result.containerSufficient) {
    // Step 2: the container alone already meets the target, so source files are moot
    // for read speed - skip opening them. Still list them (no GDALOpen needed) so the
    // dialog can show them as "not checked" instead of omitting them.
    for (const QString& file : files) {
      if (!isSourceFile(file, ownPath)) {
        continue;
      }
      result.perFileInfo << file_overview_info_t{file, {}, /*sufficient=*/false, /*checked=*/false};
    }
    qDebug() << "OVR: step 2: container already sufficient - not probing" << result.perFileInfo.size()
             << "source file(s)";
  } else {
    // Step 3: the container falls short (or is unverified) - probe every source file in
    // full, so perFileInfo reflects every file's true state, not just the first
    // bottleneck.
    bool allSourcesHaveOverviews = true;
    bool sawAnySource = false;
    for (const QString& file : files) {
      if (!isSourceFile(file, ownPath)) {
        continue;
      }
      sawAnySource = true;
      const probed_source_t probed = probeSource(file.toUtf8().constData());
      if (probed.sizes.isEmpty()) {
        allSourcesHaveOverviews = false;
      }
      const bool sufficient = meetsTarget(probed.xsize, probed.sizes, targetFactor);
      result.perFileInfo << file_overview_info_t{file, probed.sizes, sufficient, /*checked=*/true};
    }
    for (const file_overview_info_t& info : result.perFileInfo) {
      qDebug() << "OVR:  " << QFileInfo(info.path).fileName() << "checked:" << info.checked
               << "sufficient:" << info.sufficient << "sizes:" << info.overviewSizes;
    }

    // An unverified VRT-level claim becomes trustworthy once every source turns out to
    // have its own overview, matching how gdalbuildvrt derives a composite. Otherwise
    // it was stale/hand-edited, and stays discarded.
    const bool fallbackTrusted =
        !containerVerified && sawAnySource && allSourcesHaveOverviews && !rawContainerSizes.isEmpty();
    if (fallbackTrusted) {
      result.containerOverviewSizes = rawContainerSizes;
      result.containerSufficient = meetsTarget(containerXSize, rawContainerSizes, targetFactor);
    }
    qDebug() << "OVR: step 3: sawAnySource =" << sawAnySource << "allSourcesHaveOverviews =" << allSourcesHaveOverviews
             << "fallbackTrusted =" << fallbackTrusted;
  }

  // Disk-usage figure = the dataset's real on-disk footprint, matching `du`. GetFileList()
  // returns absolute paths for the .vrt, its own .ovr, and each source's MAIN file - but
  // NOT the source-level .ovr/.aux.xml sidecars, so those are added per source.
  qint64 subfileBytes = 0;   // sum of source main files only
  qint64 allFilesBytes = 0;  // every file belonging to the dataset (sub-files + overviews)
  for (const QString& file : files) {
    allFilesBytes += QFileInfo(file).size();
    if (isSourceFile(file, ownPath)) {
      subfileBytes += QFileInfo(file).size();
      for (const QString& suffix : {QStringLiteral(".ovr"), QStringLiteral(".aux.xml")}) {
        const QString side = file + suffix;
        if (QFileInfo::exists(side)) {
          allFilesBytes += QFileInfo(side).size();
        }
      }
    }
  }

  // Shallow, missing or no overviews: estimate the fully-built footprint from the base
  // (sub-files x 1 2/3). Fully qualified: the real total is already on disk.
  if (result.needsOverviewFix()) {
    result.diskUsageBytes = subfileBytes * 5 / 3;
    result.diskUsageIsEstimate = true;
  } else {
    result.diskUsageBytes = allFilesBytes;
    result.diskUsageIsEstimate = false;
  }

  // sort weakest-first: the dialog's table leads with the bottleneck. A raw pixel size
  // isn't comparable across files of different native resolution, so this is a 3-way
  // partition (insufficient, sufficient, unchecked) rather than a fine-grained ranking -
  // good enough for "which rows need a look first", not meant to rank severity.
  auto sortKey = [](const file_overview_info_t& info) {
    if (!info.checked) {
      return 2;
    }
    return info.sufficient ? 1 : 0;
  };
  std::sort(result.perFileInfo.begin(), result.perFileInfo.end(),
            [&](const file_overview_info_t& a, const file_overview_info_t& b) { return sortKey(a) < sortKey(b); });

  qDebug() << "OVR: pre-warp result: containerSufficient =" << result.containerSufficient
           << "containerHasOwnOvr =" << result.containerHasOwnOvr << "subfileCount =" << result.perFileInfo.size()
           << "tooManySubfiles =" << result.hasTooManySubfiles();

  return result;
}

int CGdalVrtUtil::progressCallbackWithDeadline(double /*dfComplete*/, const char* /*message*/, void* pProgressArg) {
  auto* deadline = reinterpret_cast<read_deadline_t*>(pProgressArg);
  if (deadline->timer.hasExpired(deadline->timeoutMs)) {
    deadline->timedOut = true;
    return false;
  }
  return !deadline->drawCtx->needsRedraw();
}
