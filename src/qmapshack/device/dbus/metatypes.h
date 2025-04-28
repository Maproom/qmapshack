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

class GVFSMtpDrive {
 public:
  QString id;
  QString name;
  QString giconData;
  QString symbolicGiconData;
  bool canEject;
  bool canPollForMedia;
  bool hasMedia;
  bool isMediaRemovable;
  bool isMediaCheckAutomatic;
  bool canStart;
  bool canStartDegraded;
  bool canStop;
  uint32_t startStopType;
  QVector<QString> volumeIds;
  QMap<QString, QString> identifiers;
  QString sort_key;
  QMap<QString, QDBusVariant> expansion;
};

class GVFSMtpVolume {
 public:
  QString id;
  QString name;
  QString giconData;
  QString symbolicGiconData;
  QString uuid;
  QString activationUri;
  bool canMount;
  bool shouldAutomount;
  QString driveId;
  QString mountId;
  QMap<QString, QString> identifiers;
  QString sortKey;
  QMap<QString, QDBusVariant> expansion;
};

class GVFSMtpMount {
 public:
  QString id;
  QString name;
  QString giconData;
  QString symbolicGiconData;
  QString uuid;
  QString rootUri;
  bool canUnmount;
  QString volumeId;
  QList<QString> xContentTypes;
  QString sortKey;
  QMap<QString, QDBusVariant> expansion;
};

class GVFSMountSpec {
 public:
  QByteArray mountPrefix;  //??
  QMap<QString, QDBusVariant> items;
};

class GVFSMount {
 public:
  // return g_variant_new ("(sossssssb^ay@(aya{sv})^ay)",
  QString dbusId;                    // mount->dbus_id,
  QDBusObjectPath objectPath;        // mount->object_path,
  QString displayName;               // mount->display_name,
  QString stableName;                // mount->stable_name,
  QString xContentTypes;             // mount->x_content_types,
  QString mountIcon;                 // mount->icon,
  QString symbolicIcon;              // mount->symbolic_icon,
  QString preferedFilenameEncoding;  // mount->prefered_filename_encoding,
  bool userVisible;                  // mount->user_visible,
  QByteArray fuseMountPoint;         // (fuse_available && mount->fuse_mountpoint) ? mount->fuse_mountpoint : "",
  GVFSMountSpec mountSpec;           // g_mount_spec_to_dbus (mount->mount_spec),
  QByteArray defaultLocation;        // mount->default_location ? mount->default_location : "");
};

class GVFSMountable {
 public:
  // return g_variant_new ("(ss^asib)",
  QString type;               // mountable->type,
  QString scheme;             // mountable->scheme ? mountable->scheme : "",
  QStringList schemeAliases;  // mountable->scheme_aliases ? mountable->scheme_aliases : empty,
  int32_t defaultPort;        // mountable->default_port,
  bool hostnameIsInet;        // mountable->hostname_is_inet);
};

class GVFSMountSource {
 public:
  // return g_variant_new ("(so)",
  QString dbusId;           // source->dbus_id,
  QDBusObjectPath objPath;  // source->obj_path);
};

class GVFSInfo {
 public:
  // return g_variant_new ("(suv)",
  QString attribute;     // attribute,
  uint32_t status;       // status,
  QDBusVariant variant;  // v);
};

class GVFSAttributeInfo {
 public:
  // g_variant_builder_add (&builder, "(suu)",
  QString name;    // list->infos[i].name,
  uint32_t type;   // list->infos[i].type,
  uint32_t flags;  // list->infos[i].flags);
};

typedef QList<KMTPFile> KMTPFileList;
typedef QList<GVFSMtpDrive> GVFSMtpDriveList;
typedef QList<GVFSMtpVolume> GVFSMtpVolumeList;
typedef QList<GVFSMtpMount> GVFSMtpMountList;
typedef QList<GVFSMount> GVFSMountList;
typedef QList<GVFSMountable> GVFSMountableList;
typedef QList<GVFSInfo> GVFSInfoList;
typedef QList<GVFSAttributeInfo> GVFSAttributeInfoList;

Q_DECLARE_METATYPE(KMTPFile)
Q_DECLARE_METATYPE(KMTPFileList)
Q_DECLARE_METATYPE(GVFSMtpDrive)
Q_DECLARE_METATYPE(GVFSMtpDriveList)
Q_DECLARE_METATYPE(GVFSMtpVolume)
Q_DECLARE_METATYPE(GVFSMtpVolumeList)
Q_DECLARE_METATYPE(GVFSMtpMount)
Q_DECLARE_METATYPE(GVFSMtpMountList)
Q_DECLARE_METATYPE(GVFSMountSpec)
Q_DECLARE_METATYPE(GVFSMount)
Q_DECLARE_METATYPE(GVFSMountList)
Q_DECLARE_METATYPE(GVFSMountable)
Q_DECLARE_METATYPE(GVFSMountableList)
Q_DECLARE_METATYPE(GVFSMountSource)
Q_DECLARE_METATYPE(GVFSInfo)
Q_DECLARE_METATYPE(GVFSInfoList)
Q_DECLARE_METATYPE(GVFSAttributeInfo)
Q_DECLARE_METATYPE(GVFSAttributeInfoList)

QDBusArgument &operator<<(QDBusArgument &argument, const KMTPFile &mtpFile);
const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFile &mtpFile);

QDBusArgument &operator<<(QDBusArgument &argument, const KMTPFileList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, KMTPFileList &list);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpDrive &drive);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpDrive &drive);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpDriveList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpDriveList &list);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpVolume &volume);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpVolume &volume);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpVolumeList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpVolumeList &list);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpMount &mount);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpMount &mount);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMtpMountList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMtpMountList &list);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountSpec &mountSpec);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountSpec &mountSpec);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMount &mount);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMount &mount);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountList &list);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountable &mountable);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountable &mountable);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountableList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountableList &list);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSMountSource &mountsource);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSMountSource &mountsource);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSInfo &info);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSInfoList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSInfoList &list);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSAttributeInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSAttributeInfo &info);

QDBusArgument &operator<<(QDBusArgument &argument, const GVFSAttributeInfoList &list);
const QDBusArgument &operator>>(const QDBusArgument &argument, GVFSAttributeInfoList &list);

#endif  // DEVICE_DBUS_METATYPES_H
