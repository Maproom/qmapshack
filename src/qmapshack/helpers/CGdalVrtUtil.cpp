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

#include <QFile>
#include <QFileInfo>
#include <QStringConverter>

#include "canvas/IDrawContext.h"

bool CGdalVrtUtil::allReferencedFilesExist(GDALDataset* dataset, QString& missingFile) {
  bool allExist = true;
  char** fileList = dataset->GetFileList();
  if (fileList == nullptr) {
    return allExist;
  }
  for (qint32 n = 0; fileList[n] != nullptr; ++n) {
    missingFile = QString::fromUtf8(fileList[n]);
    if (!QFileInfo::exists(missingFile)) {
      allExist = false;
      break;
    }
  }
  CSLDestroy(fileList);
  return allExist;
}

bool CGdalVrtUtil::isFileUtf8(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    return true;  // unreadable: let GDALOpen() surface the failure
  }
  QStringDecoder decoder(QStringConverter::Utf8);
  // Materialize into a QString: the decode is lazy and only sets hasError() once consumed.
  const QString decoded = decoder(file.readAll());
  Q_UNUSED(decoded)
  return !decoder.hasError();
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
