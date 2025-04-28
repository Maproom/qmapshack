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
  for (const KMTPFile &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFileList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    KMTPFile item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpDrive &drive) {
  argument.beginStructure();
  argument << drive.id << drive.name << drive.giconData << drive.symbolicGiconData << drive.canEject
           << drive.canPollForMedia << drive.hasMedia << drive.isMediaRemovable << drive.isMediaCheckAutomatic
           << drive.canStart << drive.canStartDegraded << drive.canStop << drive.startStopType << drive.volumeIds
           << drive.identifiers << drive.sort_key << drive.expansion;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpDrive &drive) {
  argument.beginStructure();
  argument >> drive.id >> drive.name >> drive.giconData >> drive.symbolicGiconData >> drive.canEject >>
      drive.canPollForMedia >> drive.hasMedia >> drive.isMediaRemovable >> drive.isMediaCheckAutomatic >>
      drive.canStart >> drive.canStartDegraded >> drive.canStop >> drive.startStopType >> drive.volumeIds >>
      drive.identifiers >> drive.sort_key >> drive.expansion;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpDriveList &list) {
  argument.beginArray(qMetaTypeId<GVFSMtpDrive>());
  for (const GVFSMtpDrive &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpDriveList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    GVFSMtpDrive item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpVolume &volume) {
  argument.beginStructure();
  argument << volume.id << volume.name << volume.giconData << volume.symbolicGiconData << volume.uuid
           << volume.activationUri << volume.canMount << volume.shouldAutomount << volume.driveId << volume.mountId
           << volume.identifiers << volume.sortKey << volume.expansion;
  argument.endStructure();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpVolume &volume) {
  argument.beginStructure();
  argument >> volume.id >> volume.name >> volume.giconData >> volume.symbolicGiconData >> volume.uuid >>
      volume.activationUri >> volume.canMount >> volume.shouldAutomount >> volume.driveId >> volume.mountId >>
      volume.identifiers >> volume.sortKey >> volume.expansion;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpVolumeList &list) {
  argument.beginArray(qMetaTypeId<GVFSMtpVolume>());
  for (const GVFSMtpVolume &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpVolumeList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    GVFSMtpVolume item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpMount &mount) {
  argument.beginStructure();
  argument << mount.id << mount.name << mount.giconData << mount.symbolicGiconData << mount.uuid << mount.rootUri
           << mount.canUnmount << mount.volumeId << mount.xContentTypes << mount.sortKey << mount.expansion;
  argument.endStructure();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpMount &mount) {
  argument.beginStructure();
  argument >> mount.id >> mount.name >> mount.giconData >> mount.symbolicGiconData >> mount.uuid >> mount.rootUri >>
      mount.canUnmount >> mount.volumeId >> mount.xContentTypes >> mount.sortKey >> mount.expansion;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpMountList &list) {
  argument.beginArray(qMetaTypeId<GVFSMtpMount>());
  for (const GVFSMtpMount &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpMountList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    GVFSMtpMount item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountSpec &mountSpec) {
  argument.beginStructure();
  argument << mountSpec.mountPrefix << mountSpec.items;
  argument.endStructure();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountSpec &mountSpec) {
  argument.beginStructure();
  argument >> mountSpec.mountPrefix >> mountSpec.items;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMount &mount) {
  argument.beginStructure();
  argument << mount.dbusId << mount.objectPath << mount.displayName << mount.stableName << mount.xContentTypes
           << mount.mountIcon << mount.symbolicIcon << mount.preferedFilenameEncoding << mount.userVisible
           << mount.fuseMountPoint << mount.mountSpec << mount.defaultLocation;
  argument.endStructure();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMount &mount) {
  argument.beginStructure();
  argument >> mount.dbusId >> mount.objectPath >> mount.displayName >> mount.stableName >> mount.xContentTypes >>
      mount.mountIcon >> mount.symbolicIcon >> mount.preferedFilenameEncoding >> mount.userVisible >>
      mount.fuseMountPoint >> mount.mountSpec >> mount.defaultLocation;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountList &list) {
  argument.beginArray(qMetaTypeId<GVFSMount>());
  for (const GVFSMount &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    GVFSMount item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountable &mountable) {
  argument.beginStructure();
  argument << mountable.type << mountable.scheme << mountable.schemeAliases << mountable.defaultPort
           << mountable.hostnameIsInet;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountable &mountable) {
  argument.beginStructure();
  argument >> mountable.type >> mountable.scheme >> mountable.schemeAliases >> mountable.defaultPort >>
      mountable.hostnameIsInet;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountableList &list) {
  argument.beginArray(qMetaTypeId<GVFSMountable>());
  for (const GVFSMountable &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountableList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    GVFSMountable item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountSource &mountsource) {
  argument.beginStructure();
  argument << mountsource.dbusId << mountsource.objPath;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountSource &mountsource) {
  argument.beginStructure();
  argument >> mountsource.dbusId >> mountsource.objPath;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSInfo &info) {
  argument.beginStructure();
  argument << info.attribute << info.status << info.variant;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSInfo &info) {
  argument.beginStructure();
  argument >> info.attribute >> info.status >> info.variant;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSInfoList &list) {
  argument.beginArray(qMetaTypeId<GVFSInfo>());
  for (const GVFSInfo &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSInfoList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    GVFSInfo item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSAttributeInfo &info) {
  argument.beginStructure();
  argument << info.name << info.type << info.flags;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSAttributeInfo &info) {
  argument.beginStructure();
  argument >> info.name >> info.type >> info.flags;
  argument.endStructure();
  return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSAttributeInfoList &list) {
  argument.beginArray(qMetaTypeId<GVFSAttributeInfo>());
  for (const GVFSAttributeInfo &item : list) {
    argument << item;
  }
  argument.endArray();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSAttributeInfoList &list) {
  argument.beginArray();
  list.clear();
  while (!argument.atEnd()) {
    GVFSAttributeInfo item;
    argument >> item;
    list.append(item);
  }

  argument.endArray();
  return argument;
}
