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

#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QSet>
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

CGdalVrtUtil::overview_factors_t CGdalVrtUtil::collectOverviewFactors(GDALDataset* dataset, GDALRasterBand* pBand,
                                                                      qreal pixelSizeX) {
  QSet<qint32> factors;
  overview_factors_t result;
  result.weakestMaxFactor = 1;

  // usePerFileFallback starts true; Branch 1 sets it false only when it confirms that
  // the VRT's reported overviews are actually backed by data in a source file or sidecar.
  // The fallback (Branch 2) handles both "no overviews at all" and the case where
  // <OverviewList> is declared in the VRT XML but the source TIF has no overview data —
  // GDAL reports GetOverviewCount() > 0 from the declaration alone, but reads then fall
  // back to full resolution, causing the same slowness as missing overviews.
  bool usePerFileFallback = pixelSizeX > 0;

  if (pBand->GetOverviewCount() != 0) {
    const QString ownPath = QString::fromUtf8(dataset->GetDescription());
    char** fileList = dataset->GetFileList();
    // allSourcesHaveOverviews starts true; set to false as soon as any source TIF has no
    // overview data. A VRT-level .ovr sidecar covers all tiles at once — short-circuit.
    bool allSourcesHaveOverviews = true;
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      const QString file = QString::fromUtf8(fileList[n]);
      if (file == ownPath) {
        continue;
      }
      if (file.endsWith(".ovr", Qt::CaseInsensitive)) {
        break;  // VRT-level sidecar covers all tiles
      }
      if (file.endsWith(".aux.xml", Qt::CaseInsensitive) || file.endsWith(".aux", Qt::CaseInsensitive)) {
        continue;
      }
      GDALDatasetUniquePtr sub(GDALDataset::FromHandle(GDALOpen(fileList[n], GA_ReadOnly)));
      GDALRasterBand* subBand = sub ? sub->GetRasterBand(1) : nullptr;
      if (subBand == nullptr || subBand->GetOverviewCount() == 0) {
        allSourcesHaveOverviews = false;
        break;  // any source without overviews → fall through to Branch 2
      }
    }
    CSLDestroy(fileList);

    if (allSourcesHaveOverviews) {
      QVector<qint32> fileFactors;
      for (qint32 i = 0; i < pBand->GetOverviewCount(); ++i) {
        const qint32 factor = qRound((qreal)pBand->GetXSize() / pBand->GetOverview(i)->GetXSize());
        factors << factor;
        fileFactors << factor;
      }
      std::sort(fileFactors.begin(), fileFactors.end());
      result.weakestMaxFactor = fileFactors.isEmpty() ? 1 : fileFactors.last();
      result.perFileInfo << file_overview_info_t{ownPath, fileFactors};
      usePerFileFallback = false;
    }
  }

  if (usePerFileFallback) {
    // GDAL's own GetFileList() includes the dataset's own path (e.g. a VRT mosaic lists
    // itself first, before its referenced tiles) - skip that entry, it's the container,
    // not an independent source file, and (lacking its own overviews, or it wouldn't be
    // here) would otherwise drag the weakest-file tracking down to "none" unconditionally
    const QString ownPath = QString::fromUtf8(dataset->GetDescription());
    char** fileList = dataset->GetFileList();
    bool firstFile = true;
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      const QString file = QString::fromUtf8(fileList[n]);
      if (file == ownPath) {
        continue;
      }

      // a file contributing no usable overviews can only ever be read at native
      // resolution - the weakest possible case, not something to silently skip
      QVector<qint32> fileFactors;

      GDALDatasetUniquePtr subDataset(GDALDataset::FromHandle(GDALOpen(fileList[n], GA_ReadOnly)));
      GDALRasterBand* subBand = subDataset ? subDataset->GetRasterBand(1) : nullptr;
      qreal subGeoTransform[6];
      if (subBand != nullptr && subBand->GetOverviewCount() != 0 &&
          subDataset->GetGeoTransform(subGeoTransform) == CE_None) {
        const qreal subPixelSizeX = qAbs(subGeoTransform[1]);
        for (qint32 i = 0; i < subBand->GetOverviewCount(); ++i) {
          const qreal overviewPixelSizeX = subPixelSizeX * subBand->GetXSize() / subBand->GetOverview(i)->GetXSize();
          const qint32 factor = qRound(overviewPixelSizeX / pixelSizeX);
          factors << factor;
          fileFactors << factor;
        }
      }
      std::sort(fileFactors.begin(), fileFactors.end());

      const qint32 fileMaxFactor = fileFactors.isEmpty() ? 1 : fileFactors.last();
      result.weakestMaxFactor = firstFile ? fileMaxFactor : qMin(result.weakestMaxFactor, fileMaxFactor);
      firstFile = false;
      result.perFileInfo << file_overview_info_t{file, fileFactors};
    }
    CSLDestroy(fileList);
  }

  result.factors = QVector<qint32>(factors.begin(), factors.end());
  std::sort(result.factors.begin(), result.factors.end());
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

QVector<qint32> CGdalVrtUtil::suggestOverviewLevels(qint32 xsize, qint32 ysize) {
  const QScreen* screen = QGuiApplication::primaryScreen();
  const qint32 screenSize = screen ? qMax(screen->size().width(), screen->size().height()) : 1920;
  QVector<qint32> levels;
  qint32 factor = 2;
  while (qMax(xsize, ysize) / factor > screenSize) {
    levels << factor;
    factor *= 2;
  }
  return levels;
}

CGdalVrtUtil::overview_advice_t CGdalVrtUtil::buildOverviewAdvice(GDALDataset* dataset, GDALRasterBand* band,
                                                                  const QString& filename, bool isPaletteIndexed,
                                                                  overview_factors_t overviewFactors) {
  overview_advice_t advice;
  advice.overviewsMissing = overviewFactors.factors.isEmpty();
  advice.weakestMaxFactor = overviewFactors.weakestMaxFactor;

  const qint64 basePixels = static_cast<qint64>(band->GetXSize()) * band->GetYSize();
  const qint64 bytesPerPixel = GDALGetDataTypeSizeBytes(band->GetRasterDataType()) * dataset->GetRasterCount();
  // an infinite decimation pyramid (1/4 + 1/16 + 1/64 + ...) sums to 1/3 of the base
  // layer; truncating it at suggestOverviewLevels()'s stopping point changes this
  // negligibly once there are more than a couple of levels
  advice.estimatedOverviewBytes = basePixels * bytesPerPixel / 3;

  auto maxFactor = [](const file_overview_info_t& info) { return info.factors.isEmpty() ? 1 : info.factors.last(); };
  std::sort(overviewFactors.perFileInfo.begin(), overviewFactors.perFileInfo.end(),
            [&](const file_overview_info_t& a, const file_overview_info_t& b) { return maxFactor(a) < maxFactor(b); });
  advice.perFileInfo = std::move(overviewFactors.perFileInfo);

  advice.suggestedLevels = suggestOverviewLevels(band->GetXSize(), band->GetYSize());
  if (advice.suggestedLevels.isEmpty()) {
    return advice;
  }

  advice.isPaletteIndexed = isPaletteIndexed;

  // derive source file list from GetFileList(), filtering out the VRT itself and sidecars
  const QString ownPath = QString::fromUtf8(dataset->GetDescription());
  char** fileList = dataset->GetFileList();
  for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
    const QString file = QString::fromUtf8(fileList[n]);
    if (file == filename || file == ownPath) {
      continue;
    }
    if (file.endsWith(".ovr", Qt::CaseInsensitive) || file.endsWith(".aux.xml", Qt::CaseInsensitive) ||
        file.endsWith(".aux", Qt::CaseInsensitive)) {
      continue;
    }
    if (!advice.sourceFilePaths.contains(file)) {
      advice.sourceFilePaths << file;
    }
  }
  CSLDestroy(fileList);
  if (advice.sourceFilePaths.isEmpty()) {
    advice.sourceFilePaths << filename;
  }

  advice.vrtNeedsOverviewList = filename.endsWith(".vrt", Qt::CaseInsensitive);
  advice.vrtHasOverviewList = band->GetOverviewCount() > 0;

  return advice;
}

int CGdalVrtUtil::progressCallbackWithDeadline(double /*dfComplete*/, const char* /*message*/, void* pProgressArg) {
  auto* deadline = reinterpret_cast<read_deadline_t*>(pProgressArg);
  if (deadline->timer.hasExpired(deadline->timeoutMs)) {
    deadline->timedOut = true;
    return false;
  }
  return !deadline->drawCtx->needsRedraw();
}
