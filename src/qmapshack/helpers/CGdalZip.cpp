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

#include "helpers/CGdalZip.h"

#include <cpl_string.h>
#include <cpl_vsi.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <cstdio>

QStringList CGdalZip::fileList(const QString& archive) {
  QStringList entries;
  char** files = VSIReadDirRecursive(vsiPath(archive).toUtf8().constData());
  if (files == nullptr) {
    return entries;
  }
  for (qint32 n = 0; files[n] != nullptr; ++n) {
    const QString& entry = QString::fromUtf8(files[n]);
    // GDAL marks a directory entry with a trailing slash
    if (!entry.endsWith('/')) {
      entries << entry;
    }
  }
  CSLDestroy(files);
  return entries;
}

QByteArray CGdalZip::readFile(const QString& archive, const QString& entry) {
  QByteArray content;
  if (!readEntry(vsiPath(archive, entry), content)) {
    return QByteArray();
  }
  return content;
}

QStringList CGdalZip::extractAll(const QString& archive, const QString& destDir) {
  const QDir dir(destDir);
  const QString& destPrefix = dir.absolutePath() + "/";
  const QStringList& entries = fileList(archive);
  if (entries.isEmpty()) {
    return QStringList();
  }

  QStringList written;
  for (const QString& entry : entries) {
    const QString& target = QDir::cleanPath(dir.absoluteFilePath(entry));
    if (!target.startsWith(destPrefix)) {
      return QStringList();
    }
    QByteArray content;
    if (!readEntry(vsiPath(archive, entry), content)) {
      return QStringList();
    }
    if (!dir.mkpath(QFileInfo(target).absolutePath())) {
      return QStringList();
    }
    QFile out(target);
    if (!out.open(QIODevice::WriteOnly) || out.write(content) != content.size()) {
      return QStringList();
    }
    written << target;
  }
  return written;
}

QString CGdalZip::vsiPath(const QString& archive, const QString& entry) {
  // the braces bypass GDAL's list of accepted archive suffixes, which lacks .jar
  QString path = "/vsizip/{" + QFileInfo(archive).absoluteFilePath() + "}";
  if (!entry.isEmpty()) {
    path += "/" + entry;
  }
  return path;
}

bool CGdalZip::readEntry(const QString& vsiEntryPath, QByteArray& content) {
  VSILFILE* file = VSIFOpenL(vsiEntryPath.toUtf8().constData(), "rb");
  if (file == nullptr) {
    return false;
  }

  // seeking to the end gives the decompressed size
  VSIFSeekL(file, 0, SEEK_END);
  const qsizetype size = qsizetype(VSIFTellL(file));
  VSIFSeekL(file, 0, SEEK_SET);

  content.resize(size);
  const bool success = VSIFReadL(content.data(), 1, size_t(size), file) == size_t(size);
  VSIFCloseL(file);
  return success;
}
