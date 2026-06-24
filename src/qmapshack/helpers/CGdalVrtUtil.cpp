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

QVector<qint32> CGdalVrtUtil::collectOverviewFactors(GDALDataset* dataset, GDALRasterBand* pBand, qreal pixelSizeX) {
  QSet<qint32> factors;

  if (pBand->GetOverviewCount() != 0) {
    for (qint32 i = 0; i < pBand->GetOverviewCount(); ++i) {
      factors << qRound((qreal)pBand->GetXSize() / pBand->GetOverview(i)->GetXSize());
    }
  } else if (pixelSizeX > 0) {
    char** fileList = dataset->GetFileList();
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      GDALDatasetUniquePtr subDataset(GDALDataset::FromHandle(GDALOpen(fileList[n], GA_ReadOnly)));
      GDALRasterBand* subBand = subDataset ? subDataset->GetRasterBand(1) : nullptr;
      qreal subGeoTransform[6];
      if (subBand == nullptr || subBand->GetOverviewCount() == 0 ||
          subDataset->GetGeoTransform(subGeoTransform) != CE_None) {
        continue;
      }

      const qreal subPixelSizeX = qAbs(subGeoTransform[1]);
      for (qint32 i = 0; i < subBand->GetOverviewCount(); ++i) {
        const qreal overviewPixelSizeX = subPixelSizeX * subBand->GetXSize() / subBand->GetOverview(i)->GetXSize();
        factors << qRound(overviewPixelSizeX / pixelSizeX);
      }
    }
    CSLDestroy(fileList);
  }

  QVector<qint32> result(factors.begin(), factors.end());
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
