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

#include "device/dbus/metatypes.h"

KMTPFile::KMTPFile() : m_itemId(0), m_parentId(0), m_storageId(0), m_filesize(0), m_modificationdate(0) {}

KMTPFile::KMTPFile(quint32 itemId, quint32 parentId, quint32 storageId, const char *filename, quint64 filesize,
                   qint64 modificationdate, const QString &filetype)
    : m_itemId(itemId),
      m_parentId(parentId),
      m_storageId(storageId),
      m_filename(QString::fromUtf8(filename)),
      m_filesize(filesize),
      m_modificationdate(modificationdate),
      m_filetype(filetype) {}

bool KMTPFile::isValid() const { return m_itemId != 0; }

bool KMTPFile::isFolder() const { return m_filetype == QLatin1String("inode/directory"); }

quint32 KMTPFile::itemId() const { return m_itemId; }

quint32 KMTPFile::parentId() const { return m_parentId; }

quint32 KMTPFile::storageId() const { return m_storageId; }

QString KMTPFile::filename() const { return m_filename; }

quint64 KMTPFile::filesize() const { return m_filesize; }

qint64 KMTPFile::modificationdate() const { return m_modificationdate; }

QString KMTPFile::filetype() const { return m_filetype; }

QDBusArgument &operator<<(QDBusArgument &argument, const KMTPFile &mtpFile) {
  argument.beginStructure();
  argument << mtpFile.m_itemId << mtpFile.m_parentId << mtpFile.m_storageId << mtpFile.m_filename << mtpFile.m_filesize
           << mtpFile.m_modificationdate << mtpFile.m_filetype;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFile &mtpFile) {
  argument.beginStructure();
  argument >> mtpFile.m_itemId >> mtpFile.m_parentId >> mtpFile.m_storageId >> mtpFile.m_filename >>
      mtpFile.m_filesize >> mtpFile.m_modificationdate >> mtpFile.m_filetype;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const KMTPFileList &list) {
  argument.beginArray(qMetaTypeId<KMTPFile>());
  for (const KMTPFile &file : list) {
    argument << file;
  }
  argument.endArray();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFileList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    KMTPFile file;
    argument >> file;
    list.append(file);
  }

  argument.endArray();
  return argument;
}
