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

  if (pBand->GetOverviewCount() != 0) {
    QVector<qint32> fileFactors;
    for (qint32 i = 0; i < pBand->GetOverviewCount(); ++i) {
      const qint32 factor = qRound((qreal)pBand->GetXSize() / pBand->GetOverview(i)->GetXSize());
      factors << factor;
      fileFactors << factor;
    }
    std::sort(fileFactors.begin(), fileFactors.end());
    result.weakestMaxFactor = fileFactors.isEmpty() ? 1 : fileFactors.last();
    result.perFileInfo << file_overview_info_t{QString::fromUtf8(dataset->GetDescription()), fileFactors};
  } else if (pixelSizeX > 0) {
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
  QVector<qint32> levels;
  qint32 factor = 2;
  while (qMax(xsize, ysize) / factor > 256) {
    levels << factor;
    factor *= 2;
  }
  return levels;
}

CGdalVrtUtil::overview_advice_t CGdalVrtUtil::buildOverviewAdvice(GDALDataset* dataset, GDALRasterBand* band,
                                                                  const QString& filename, bool isCategorical,
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

  const QVector<qint32> levels = suggestOverviewLevels(band->GetXSize(), band->GetYSize());
  if (levels.isEmpty()) {
    // already smaller than one tile - nothing sensible to suggest
    return advice;
  }

  const QString resampleAlg = isCategorical ? "nearest" : "average";
  QStringList levelArgs;
  for (qint32 level : levels) {
    levelArgs << QString::number(level);
  }

  auto gdaladdoCommand = [&](const QString& path) {
    return QString("gdaladdo -ro -r %1 --config COMPRESS_OVERVIEW DEFLATE \"%2\" %3")
        .arg(resampleAlg, path, levelArgs.join(' '));
  };

  // collectOverviewFactors() already enumerated dataset's referenced files (to inspect
  // their overviews) and left the result in advice.perFileInfo above - reuse that instead
  // of calling GetFileList() a second time
  QStringList sourceFiles;
  for (const file_overview_info_t& info : advice.perFileInfo) {
    if (info.path != filename && !sourceFiles.contains(info.path)) {
      sourceFiles << info.path;
    }
  }

  if (sourceFiles.isEmpty()) {
    // filename is a plain file, not a multi-file container - one command targeting itself
    advice.filesCommand = gdaladdoCommand(filename);
  } else {
    advice.vrtCommand = gdaladdoCommand(filename);

    // one standalone command per line rather than a shell for-loop: pastes and runs
    // unmodified in bash/zsh/sh, cmd.exe and PowerShell alike, with no platform-specific
    // loop syntax to get right
    QStringList commands;
    for (const QString& file : sourceFiles) {
      commands << gdaladdoCommand(file);
    }
    advice.filesCommand = commands.join('\n');
  }

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
