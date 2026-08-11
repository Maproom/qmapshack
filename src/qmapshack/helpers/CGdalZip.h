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

#ifndef CGDALZIP_H
#define CGDALZIP_H

#include <QByteArray>
#include <QString>
#include <QStringList>

/**
   @brief Read-only ZIP access through GDAL's /vsizip/ virtual filesystem.
 */
class CGdalZip {
 public:
  /**
     @brief List the file entries of an archive.
     @param archive path of the archive
     @return entry paths relative to the archive root, directories omitted; empty if the
             archive cannot be read
   */
  static QStringList fileList(const QString& archive);

  /**
     @brief Read one entry into memory.
     @param archive path of the archive
     @param entry   entry path as returned by fileList()
     @return the decompressed content; empty if the entry cannot be read
   */
  static QByteArray readFile(const QString& archive, const QString& entry);

  /**
     @brief Unpack every entry of an archive into a directory.

     Entries resolving outside destDir are rejected.

     @param archive path of the archive
     @param destDir directory to unpack into; missing sub-directories are created
     @return absolute paths of the written files; empty on any failure, leaving the files
             written so far in place
   */
  static QStringList extractAll(const QString& archive, const QString& destDir);

 private:
  /// @brief Build the /vsizip/ path of an archive, or of one entry in it
  static QString vsiPath(const QString& archive, const QString& entry = QString());

  /// @brief Read one /vsizip/ entry; content is valid only on success
  static bool readEntry(const QString& vsiEntryPath, QByteArray& content);
};

#endif  // CGDALZIP_H
