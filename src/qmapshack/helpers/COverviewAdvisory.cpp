/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/COverviewAdvisory.h"

#include <gdal.h>
#include <gdal_priv.h>

#include <QDebug>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QSet>
#include <QSettings>
#include <QStringList>
#include <algorithm>

#include "canvas/IDrawContext.h"
#include "helpers/CGdalVrtUtil.h"

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

/// @brief Open a path read-only. GDAL filenames are UTF-8.
GDALDatasetUniquePtr openReadOnly(const QString& path) {
  return GDALDatasetUniquePtr(GDALDataset::FromHandle(GDALOpen(path.toUtf8().constData(), GA_ReadOnly)));
}

/// @brief Open path and probe its own overview sizes (native to path's own pixel grid -
///        no rescaling into the container's pixel grid is needed; GDAL matches overviews
///        to the requested read resolution itself, gdalwarp's "-ovr AUTO"). Also records
///        the source's own width (for meetsTarget()).
probed_source_t probeSource(const QString& path) {
  GDALDatasetUniquePtr sub = openReadOnly(path);
  GDALRasterBand* subBand = sub ? sub->GetRasterBand(1) : nullptr;
  if (subBand == nullptr) {
    return {};
  }
  probed_source_t probed;
  probed.xsize = subBand->GetXSize();
  probed.sizes = ownOverviewSizes(subBand);
  return probed;
}

/// @brief Cycle/runaway guard on the VRT nesting depth; one level is what the usual
///        per-file `gdalwarp -of VRT` + gdalbuildvrt recipe produces.
constexpr qint32 kMaxVrtNesting = 8;

struct source_tree_t {
  QStringList leaves;        /**< non-VRT rasters - the only files that can carry overview data */
  QStringList intermediates; /**< nested VRTs passed through, for the disk-usage sum */
};

/// @brief True if `path` is a VRT. GDALIdentifyDriver() sniffs the header (~5us, no XML
///        parse); the file extension is not a reliable test.
bool isVrtFile(const QString& path) {
  GDALDriverH driver = GDALIdentifyDriver(path.toUtf8().constData(), nullptr);
  return driver != nullptr && qstrcmp(GDALGetDriverShortName(driver), "VRT") == 0;
}

/// @brief Collect dataset's sources, following a nested VRT down to the real rasters -
///        GetFileList() reports one level only, and gdaladdo on a warped VRT writes a
///        virtual <OverviewList> instead of building anything. A nested VRT with its own
///        .ovr is a leaf: that .ovr is real data.
void collectLeafSources(GDALDataset* dataset, const QString& ownPath, qint32 depth, QSet<QString>& visited,
                        source_tree_t& tree) {
  QStringList files;
  char** fileList = dataset->GetFileList();
  for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
    files << QString::fromUtf8(fileList[n]);
  }
  CSLDestroy(fileList);

  for (const QString& file : files) {
    if (!isSourceFile(file, ownPath) || visited.contains(file)) {
      continue;
    }
    visited.insert(file);

    if (depth >= kMaxVrtNesting || QFileInfo::exists(file + ".ovr") || !isVrtFile(file)) {
      tree.leaves << file;
      continue;
    }
    GDALDatasetUniquePtr sub = openReadOnly(file);
    if (!sub) {
      tree.leaves << file;  // unreadable - keep it listed, probeSource() reports it
      continue;
    }
    tree.intermediates << file;
    collectLeafSources(sub.get(), file, depth + 1, visited, tree);
  }
}

/// @brief Suggest gdaladdo decimation levels for a raster of the given size. Doubles from
///        2 until the overview would be smaller than the primary screen's longest
///        dimension (fallback 1920px) - a smaller one would only be upscaled.
QVector<qint32> suggestOverviewLevels(qint32 xsize, qint32 ysize, qint32 maxFactor) {
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

COverviewAdvisory::advice_t buildAdvice(GDALDataset* dataset, GDALRasterBand* band, bool isPaletteIndexed,
                                        const QVector<qint32>& suggestedLevels) {
  COverviewAdvisory::advice_t result;
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

  // The container's own file list: the .vrt, its own .ovr (if any), and each direct
  // source's main file. Sources are reasoned about through the flattened tree instead.
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

  source_tree_t tree;
  {
    QSet<QString> visited{ownPath};
    collectLeafSources(dataset, ownPath, 0, visited, tree);
  }
  qDebug() << "OVR: sources:" << tree.leaves.size() << "leaf raster(s) behind" << tree.intermediates.size()
           << "nested VRT(s)";

  if (result.containerSufficient) {
    // Step 2: the container alone already meets the target, so source files are moot
    // for read speed - list them unprobed so the dialog can show them as "not checked".
    for (const QString& leaf : tree.leaves) {
      result.perFileInfo << COverviewAdvisory::file_info_t{leaf, {}, /*sufficient=*/false, /*checked=*/false};
    }
    qDebug() << "OVR: step 2: container already sufficient - not probing" << result.perFileInfo.size()
             << "source file(s)";
  } else {
    // Step 3: the container falls short (or is unverified) - probe every leaf raster in
    // full, so perFileInfo reflects every file's true state, not just the first
    // bottleneck.
    bool allSourcesHaveOverviews = true;
    const bool sawAnySource = !tree.leaves.isEmpty();
    for (const QString& leaf : tree.leaves) {
      const probed_source_t probed = probeSource(leaf);
      if (probed.sizes.isEmpty()) {
        allSourcesHaveOverviews = false;
      }
      const bool sufficient = meetsTarget(probed.xsize, probed.sizes, targetFactor);
      result.perFileInfo << COverviewAdvisory::file_info_t{leaf, probed.sizes, sufficient, /*checked=*/true};
    }
    for (const COverviewAdvisory::file_info_t& info : result.perFileInfo) {
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
  // omits every source's own .ovr/.aux.xml sidecars, so sources are summed from the tree.
  qint64 subfileBytes = 0;   // sum of the leaf rasters only
  qint64 allFilesBytes = 0;  // every file belonging to the dataset (sub-files + overviews)
  auto addWithSidecars = [](const QString& path, qint64& total) {
    total += QFileInfo(path).size();
    for (const QString& suffix : {QStringLiteral(".ovr"), QStringLiteral(".aux.xml")}) {
      const QString side = path + suffix;
      if (QFileInfo::exists(side)) {
        total += QFileInfo(side).size();
      }
    }
  };
  for (const QString& file : files) {
    // container + its own .ovr; the sources are covered by the tree
    if (!isSourceFile(file, ownPath)) {
      allFilesBytes += QFileInfo(file).size();
    }
  }
  for (const QString& vrt : tree.intermediates) {
    addWithSidecars(vrt, allFilesBytes);
  }
  for (const QString& leaf : tree.leaves) {
    subfileBytes += QFileInfo(leaf).size();
    addWithSidecars(leaf, allFilesBytes);
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
  auto sortKey = [](const COverviewAdvisory::file_info_t& info) {
    if (!info.checked) {
      return 2;
    }
    return info.sufficient ? 1 : 0;
  };
  std::sort(result.perFileInfo.begin(), result.perFileInfo.end(),
            [&](const COverviewAdvisory::file_info_t& a, const COverviewAdvisory::file_info_t& b) {
              return sortKey(a) < sortKey(b);
            });

  qDebug() << "OVR: pre-warp result: containerSufficient =" << result.containerSufficient
           << "containerHasOwnOvr =" << result.containerHasOwnOvr << "subfileCount =" << result.perFileInfo.size()
           << "tooManySubfiles =" << result.hasTooManySubfiles();

  return result;
}
}  // namespace

void COverviewAdvisory::probe(GDALDataset* dataset, GDALRasterBand* band, bool isPaletteIndexed, qint32 maxFactor) {
  if (!enabled_) {
    return;
  }
  // The factors stay in the dataset's pre-warp pixel grid, the same grid the suggested
  // levels are computed from, so the verdict compares like with like whatever warp the
  // caller sets up afterwards.
  advice_ = buildAdvice(dataset, band, isPaletteIndexed,
                        suggestOverviewLevels(band->GetXSize(), band->GetYSize(), maxFactor));
  needsAttention_ = advice_.needsAttention();
  qDebug() << (needsAttention_ ? "OVR: assessment: needs attention - advisory will fire on slow render"
                               : "OVR: assessment: OK");
}

void COverviewAdvisory::setGeometry(GDALDataset* preWarpSource) {
  geometry_ = geometry_t();
  qreal gt[6];
  if (preWarpSource == nullptr || preWarpSource->GetGeoTransform(gt) != CE_None) {
    return;
  }
  const OGRSpatialReference* srs = preWarpSource->GetSpatialRef();
  const bool latLong = (srs != nullptr) && srs->IsGeographic();
  geometry_.xsizePx = preWarpSource->GetRasterXSize();
  geometry_.ysizePx = preWarpSource->GetRasterYSize();
  geometry_.pixelSizeX = CGdalVrtUtil::toMeters(qAbs(gt[1]), latLong);
  geometry_.pixelSizeY = CGdalVrtUtil::toMeters(qAbs(gt[5]), latLong);
}

void COverviewAdvisory::save(QSettings& cfg) const { cfg.setValue("suppressOverviewAdvisory", suppress_.load()); }

void COverviewAdvisory::load(QSettings& cfg) {
  suppress_ = cfg.value("suppressOverviewAdvisory", suppress_.load()).toBool();
}

int COverviewAdvisory::progressCallbackWithDeadline(double /*dfComplete*/, const char* /*message*/,
                                                    void* pProgressArg) {
  auto* deadline = reinterpret_cast<read_deadline_t*>(pProgressArg);
  if (deadline->timer.hasExpired(deadline->timeoutMs)) {
    deadline->timedOut = true;
    return false;
  }
  return !deadline->drawCtx->needsRedraw();
}
