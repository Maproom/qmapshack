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
#include <QSet>
#include <QStringList>
#include <algorithm>
#include <limits>

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
/// @brief This dataset's own overview factors, sorted ascending; empty if it reports
///        none. Same dataset, same pixel grid, so no rescaling is needed.
QVector<qint32> ownOverviewFactors(GDALRasterBand* band) {
  QVector<qint32> result;
  for (qint32 i = 0; i < band->GetOverviewCount(); ++i) {
    result << qRound(static_cast<qreal>(band->GetXSize()) / band->GetOverview(i)->GetXSize());
  }
  std::sort(result.begin(), result.end());
  return result;
}

/// @brief Open path and return its own overview factors, rescaled into the container's
///        pixel scale via its own geotransform. Empty if it has no overviews, its
///        geotransform can't be read, or pixelSizeX is 0 (unknown).
QVector<qint32> probeSourceFactors(const char* path, qreal pixelSizeX) {
  QVector<qint32> result;
  GDALDatasetUniquePtr sub(GDALDataset::FromHandle(GDALOpen(path, GA_ReadOnly)));
  GDALRasterBand* subBand = sub ? sub->GetRasterBand(1) : nullptr;
  qreal subGeoTransform[6];
  if (subBand != nullptr && subBand->GetOverviewCount() != 0 && pixelSizeX > 0 &&
      sub->GetGeoTransform(subGeoTransform) == CE_None) {
    const qreal subPixelSizeX = qAbs(subGeoTransform[1]);
    for (qint32 i = 0; i < subBand->GetOverviewCount(); ++i) {
      const qreal ovPixelSizeX = subPixelSizeX * subBand->GetXSize() / subBand->GetOverview(i)->GetXSize();
      result << qRound(ovPixelSizeX / pixelSizeX);
    }
    std::sort(result.begin(), result.end());
  }
  return result;
}
}  // namespace

QVector<qint32> CGdalVrtUtil::intersectSourceOverviewFactors(const QStringList& sourcePaths, qreal pixelSizeX) {
  QSet<qint32> intersection;
  bool first = true;
  for (const QString& path : sourcePaths) {
    const QVector<qint32> factors = probeSourceFactors(path.toUtf8().constData(), pixelSizeX);
    if (factors.isEmpty()) {
      return {};  // any source without a real overview means the composite can offer nothing
    }
    const QSet<qint32> factorSet(factors.begin(), factors.end());
    intersection = first ? factorSet : (intersection & factorSet);
    first = false;
  }
  QVector<qint32> result(intersection.begin(), intersection.end());
  std::sort(result.begin(), result.end());
  return result;
}

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
                                                                  qreal pixelSizeX, bool isPaletteIndexed,
                                                                  const QVector<qint32>& suggestedLevels) {
  overview_advice_t result;
  result.suggestedLevels = suggestedLevels;
  result.isPaletteIndexed = isPaletteIndexed;
  const qint32 targetFactor = suggestedLevels.isEmpty() ? 1 : suggestedLevels.last();
  qDebug() << "OVR: GetOverviewCount =" << band->GetOverviewCount() << "targetFactor =" << targetFactor
           << "pixelSizeX =" << pixelSizeX;

  // Step 1: the container's own claim. dataset is always a VRT here (CMapVRT/CDemVRT
  // only ever open a .vrt file directly; every other raster format goes through its
  // own IMap/IDem class instead), and GDAL will report GetOverviewCount() > 0 purely
  // from a declared <OverviewList>, even with nothing behind it (a stale or
  // hand-edited declaration). Trust it immediately only if a real .ovr file is listed
  // (cheap, no GDALOpen); otherwise defer to step 3, once every source has been
  // probed anyway.
  // containerVerified only ever becomes true via the physical-.ovr-file check below, so
  // it doubles as "the container has its own .ovr file" - step 3 further down is the
  // only other way containerFactors/containerFactor get trusted, and that path never
  // involves a file on disk.
  const QString ownPath = QString::fromUtf8(dataset->GetDescription());
  QVector<qint32> rawContainerFactors;
  bool containerVerified = false;
  if (band->GetOverviewCount() != 0) {
    rawContainerFactors = ownOverviewFactors(band);
    char** fileList = dataset->GetFileList();
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      const QString file = QString::fromUtf8(fileList[n]);
      if (file != ownPath && file.endsWith(".ovr", Qt::CaseInsensitive)) {
        containerVerified = true;
        break;
      }
    }
    CSLDestroy(fileList);
  }
  result.containerFactors = containerVerified ? rawContainerFactors : QVector<qint32>{};
  result.containerFactor = result.containerFactors.isEmpty() ? 0 : result.containerFactors.last();
  result.containerHasOwnOvr = containerVerified;
  qDebug() << "OVR: step 1: rawContainerFactors =" << rawContainerFactors << "hasOwnOvr =" << containerVerified;

  if (result.containerFactor >= targetFactor) {
    // Step 2: the container alone already meets the target, so source files are moot
    // for read speed - skip opening them. Still list them (no GDALOpen needed) so the
    // dialog can show them as "not checked" instead of omitting them.
    result.weakestMaxFactor = result.containerFactor;
    char** fileList = dataset->GetFileList();
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      const QString file = QString::fromUtf8(fileList[n]);
      if (file == ownPath || file.endsWith(".ovr", Qt::CaseInsensitive) ||
          file.endsWith(".aux.xml", Qt::CaseInsensitive) || file.endsWith(".aux", Qt::CaseInsensitive)) {
        continue;
      }
      result.perFileInfo << file_overview_info_t{file, {}, /*checked=*/false};
    }
    CSLDestroy(fileList);
    qDebug() << "OVR: step 2: container factor" << result.containerFactor << ">= target" << targetFactor
             << "- not probing" << result.perFileInfo.size() << "source file(s)";
  } else {
    // Step 3: the container falls short (or is unverified) - probe every source file in
    // full, so perFileInfo reflects every file's true state, not just the first
    // bottleneck.
    char** fileList = dataset->GetFileList();
    bool allSourcesHaveOverviews = true;
    bool sawAnySource = false;
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      const QString file = QString::fromUtf8(fileList[n]);
      if (file == ownPath || file.endsWith(".ovr", Qt::CaseInsensitive) ||
          file.endsWith(".aux.xml", Qt::CaseInsensitive) || file.endsWith(".aux", Qt::CaseInsensitive)) {
        continue;
      }
      sawAnySource = true;
      const QVector<qint32> fileFactors = probeSourceFactors(fileList[n], pixelSizeX);
      if (fileFactors.isEmpty()) {
        allSourcesHaveOverviews = false;
      }
      result.perFileInfo << file_overview_info_t{file, fileFactors, /*checked=*/true};
    }
    CSLDestroy(fileList);
    for (const file_overview_info_t& info : result.perFileInfo) {
      qDebug() << "OVR:  " << QFileInfo(info.path).fileName() << "checked:" << info.checked
               << "factors:" << info.factors;
    }

    // An unverified VRT-level claim becomes trustworthy once every source turns out to
    // have its own overview, matching how gdalbuildvrt derives a composite. Otherwise
    // it was stale/hand-edited, and stays discarded.
    const bool fallbackTrusted =
        !containerVerified && sawAnySource && allSourcesHaveOverviews && !rawContainerFactors.isEmpty();
    if (fallbackTrusted) {
      result.containerFactors = rawContainerFactors;
      result.containerFactor = rawContainerFactors.last();
    }
    qDebug() << "OVR: step 3: sawAnySource =" << sawAnySource << "allSourcesHaveOverviews =" << allSourcesHaveOverviews
             << "fallbackTrusted =" << fallbackTrusted;

    qint32 weakestSource = 1;
    bool first = true;
    for (const file_overview_info_t& info : result.perFileInfo) {
      const qint32 fileMax = info.factors.isEmpty() ? 1 : info.factors.last();
      weakestSource = first ? fileMax : qMin(weakestSource, fileMax);
      first = false;
    }
    // The container's own factor covers every region uniformly, so it can only help,
    // never hurt, the worst source - see this function's doc comment for why max() here
    // is exact, not an approximation.
    result.weakestMaxFactor = qMax(result.containerFactor, weakestSource);
  }

  const qint64 basePixels = static_cast<qint64>(band->GetXSize()) * band->GetYSize();
  const qint64 bytesPerPixel = GDALGetDataTypeSizeBytes(band->GetRasterDataType()) * dataset->GetRasterCount();
  // a full decimation pyramid (1/4+1/16+1/64+...) sums to 1/3 of the base layer; real
  // size is usually smaller thanks to compression
  result.estimatedOverviewBytes = basePixels * bytesPerPixel / 3;

  // sort weakest-first: the dialog's table leads with the bottleneck. An unchecked entry
  // (already covered by the container) is never the bottleneck, so it sorts last.
  auto maxFactor = [](const file_overview_info_t& info) {
    if (!info.checked) {
      return std::numeric_limits<qint32>::max();
    }
    return info.factors.isEmpty() ? 1 : info.factors.last();
  };
  std::sort(result.perFileInfo.begin(), result.perFileInfo.end(),
            [&](const file_overview_info_t& a, const file_overview_info_t& b) { return maxFactor(a) < maxFactor(b); });

  qDebug() << "OVR: pre-warp result: containerFactor =" << result.containerFactor
           << "containerHasOwnOvr =" << result.containerHasOwnOvr << "weakestMaxFactor =" << result.weakestMaxFactor;

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
