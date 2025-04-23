/**********************************************************************************************
    Copyright (C) 2021 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef DEVICE_DBUS_METATYPES_H
#define DEVICE_DBUS_METATYPES_H

#include <qdbusargument.h>
#include <qtypes.h>

class KMTPFile {
 public:
  KMTPFile();
  KMTPFile(const KMTPFile &other) = default;
  explicit KMTPFile(quint32 itemId, quint32 parentId, quint32 storageId, const char *filename, quint64 filesize,
                    qint64 modificationdate, const QString &filetype);

  bool isValid() const;
  bool isFolder() const;

  quint32 itemId() const;
  quint32 parentId() const;
  quint32 storageId() const;
  QString filename() const;
  quint64 filesize() const;
  qint64 modificationdate() const;
  QString filetype() const;

 private:
  quint32 m_itemId;          /**< Unique item ID */
  quint32 m_parentId;        /**< ID of parent folder */
  quint32 m_storageId;       /**< ID of storage holding this file */
  QString m_filename;        /**< Filename of this file */
  quint64 m_filesize;        /**< Size of file in bytes */
  qint64 m_modificationdate; /**< Date of last alteration of the file */
  QString m_filetype;        /**< Filetype used for the current file */

  friend QDBusArgument &operator<<(QDBusArgument &argument, const KMTPFile &mtpFile);
  friend const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFile &mtpFile);
};

typedef QList<KMTPFile> KMTPFileList;

Q_DECLARE_METATYPE(KMTPFile)
Q_DECLARE_METATYPE(KMTPFileList)

QDBusArgument &operator<<(QDBusArgument &argument, const KMTPFile &mtpFile);
const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFile &mtpFile);

QDBusArgument &operator<<(QDBusArgument &argument, const KMTPFileList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFileList &list);

#endif  // DEVICE_DBUS_METATYPES_H
