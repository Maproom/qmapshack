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

#include "device/CDeviceWatcherLinux.h"

#include <QtDBus>
#include <QtWidgets>
#include <QtXml>

#include "canvas/CCanvas.h"
#include "device/CDeviceGarminMtp.h"
#include "device/IDevice.h"
#include "device/dbus/org.kde.kmtp.Device.h"
#include "device/dbus/org.kde.kmtp.Storage.h"
#include "gis/CGisListWks.h"

CDeviceWatcherLinux::CDeviceWatcherLinux(CGisListWks* parent) : IDeviceWatcher(parent) {
  qDBusRegisterMetaType<KMTPFile>();
  qDBusRegisterMetaType<KMTPFileList>();
  qDBusRegisterMetaType<GVFSMtpDrive>();
  qDBusRegisterMetaType<GVFSMtpDriveList>();
  qDBusRegisterMetaType<GVFSMtpVolume>();
  qDBusRegisterMetaType<GVFSMtpVolumeList>();
  qDBusRegisterMetaType<GVFSMtpMount>();
  qDBusRegisterMetaType<GVFSMtpMountList>();
  qDBusRegisterMetaType<GVFSMountSpec>();
  qDBusRegisterMetaType<GVFSMount>();
  qDBusRegisterMetaType<GVFSMountList>();
  qDBusRegisterMetaType<GVFSMountable>();
  qDBusRegisterMetaType<GVFSMountableList>();
  qDBusRegisterMetaType<GVFSMountSource>();
  qDBusRegisterMetaType<GVFSInfo>();
  qDBusRegisterMetaType<GVFSInfoList>();
  qDBusRegisterMetaType<GVFSAttributeInfo>();
  qDBusRegisterMetaType<GVFSAttributeInfoList>();

  QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2",
                                       "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this,
                                       SLOT(slotDeviceAdded(QDBusObjectPath, QVariantMap)));

  QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2",
                                       "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", this,
                                       SLOT(slotDeviceRemoved(QDBusObjectPath, QStringList)));

  kMtpDaemon = new org::kde::kmtp::Daemon("org.kde.kiod6", "/modules/kmtpd", QDBusConnection::sessionBus(), this);
  connect(kMtpDaemon, &org::kde::kmtp::Daemon::devicesChanged, this, &CDeviceWatcherLinux::slotKMTPDeviceChanged);
  slotKMTPDeviceChanged();

  gvfsMtpVolumeMonitor = new org::gtk::Private::RemoteVolumeMonitor(
      "org.gtk.vfs.MTPVolumeMonitor", "/org/gtk/Private/RemoteVolumeMonitor", QDBusConnection::sessionBus(), this);
  connect(gvfsMtpVolumeMonitor, &org::gtk::Private::RemoteVolumeMonitor::VolumeAdded, this,
          &CDeviceWatcherLinux::slotGVFSMtpVolumeAdded);
  connect(gvfsMtpVolumeMonitor, &org::gtk::Private::RemoteVolumeMonitor::VolumeRemoved, this,
          &CDeviceWatcherLinux::slotGVFSMtpVolumeRemoved);

  auto reply = gvfsMtpVolumeMonitor->List();
  reply.waitForFinished();
  const GVFSMtpVolumeList& volumes = reply.argumentAt<1>();
  for (const GVFSMtpVolume& volume : volumes) {
    gvfsMtpVolumeMonitor->VolumeMount(volume.id, "", 0, "").waitForFinished();
  }

  gvfsMountTracker = new org::gtk::vfs::MountTracker("org.gtk.vfs.Daemon", "/org/gtk/vfs/mounttracker",
                                                     QDBusConnection::sessionBus(), this);
  connect(gvfsMountTracker, &org::gtk::vfs::MountTracker::Mounted, this, &CDeviceWatcherLinux::slotGVFSMounted);
  connect(gvfsMountTracker, &org::gtk::vfs::MountTracker::Unmounted, this, &CDeviceWatcherLinux::slotGVFSUnmounted);
  const GVFSMountList& mountlist = gvfsMountTracker->ListMounts2(true).value();
  for (const GVFSMount& mount : mountlist) {
    slotGVFSMounted(mount);
  }
}

CDeviceWatcherLinux::~CDeviceWatcherLinux() {}

void CDeviceWatcherLinux::slotDeviceAdded(const QDBusObjectPath& path, const QVariantMap& map) {
  // ignore all paths other than a filesystem
  if (!map.contains("org.freedesktop.UDisks2.Filesystem")) {
    return;
  }

  // create path of to drive the block device belongs to
  QDBusInterface* blockIface = new QDBusInterface("org.freedesktop.UDisks2", path.path(),
                                                  "org.freedesktop.UDisks2.Block", QDBusConnection::systemBus(), this);
  QDBusObjectPath drive_object = blockIface->property("Drive").value<QDBusObjectPath>();
  QString idLabel = blockIface->property("IdLabel").toString().toUpper();

  // read vendor string attached to drive
  QDBusInterface* driveIface = new QDBusInterface("org.freedesktop.UDisks2", drive_object.path(),
                                                  "org.freedesktop.UDisks2.Drive", QDBusConnection::systemBus(), this);
  QString vendor = driveIface->property("Vendor").toString();
  QString model = driveIface->property("Model").toString();
  bool removable = driveIface->property("Removable").toBool();

  delete blockIface;
  delete driveIface;

  // GPS device are always removable
  if (!removable) {
    return;
  }

#if !defined(Q_OS_FREEBSD)
  // currently bsdisks does not report model or vendor
  qDebug() << "model:" << model << "vendor:" << vendor << "idLabel:" << idLabel;

  if (!idLabel.contains("TWONAV")) {
    if (model.isEmpty() || vendor.isEmpty()) {
      return;
    }
  }
#endif

  QString strPath = path.path();
  IDevice::mount(strPath);
  QString mountPoint = readMountPoint(strPath);
  probeForDevice(mountPoint, strPath, QFileInfo(mountPoint).fileName());
  IDevice::umount(strPath);
}

void CDeviceWatcherLinux::slotDeviceRemoved(const QDBusObjectPath& path, const QStringList& list) {
  // ignore all paths other than a filesystem
  if (!list.contains("org.freedesktop.UDisks2.Filesystem")) {
    return;
  }

  qDebug() << "slotDeviceRemoved" << path.path() << list;
  listWks->removeDevice(path.path());
}

void CDeviceWatcherLinux::slotUpdate() {
  QList<QDBusObjectPath> paths;
  QDBusMessage call =
      QDBusMessage::createMethodCall("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2/block_devices",
                                     "org.freedesktop.DBus.Introspectable", "Introspect");
  QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
  if (!reply.isValid()) {
    qWarning("UDisks2Manager: error: %s", qPrintable(reply.error().name()));
    return;
  }

  QDomDocument doc;
  doc.setContent(reply);
  const QDomElement& xmlRoot = doc.documentElement();
  const QDomNodeList& xmlNodes = xmlRoot.elementsByTagName("node");
  const int N = xmlNodes.count();
  for (int n = 0; n < N; n++) {
    const QDomNode& xmlNode = xmlNodes.item(n);
    const QDomNamedNodeMap& attr = xmlNode.attributes();

    QString name = attr.namedItem("name").nodeValue();
    if (!name.isEmpty()) {
      paths << QDBusObjectPath("/org/freedesktop/UDisks2/block_devices/" + name);
    }
  }

  for (const QDBusObjectPath& path : std::as_const(paths)) {
    QDBusMessage call = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", path.path(),
                                                       "org.freedesktop.DBus.Introspectable", "Introspect");
    QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);

    if (!reply.isValid()) {
      qWarning("UDisks2Manager: error: %s", qPrintable(reply.error().name()));
      continue;
    }

    QDomDocument doc;
    doc.setContent(reply);
    const QDomElement& xmlRoot = doc.documentElement();
    const QDomNodeList& xmlInterfaces = xmlRoot.elementsByTagName("interface");
    const int N = xmlInterfaces.count();
    for (int n = 0; n < N; n++) {
      const QDomNode& xmlInterface = xmlInterfaces.item(n);
      const QDomNamedNodeMap& attr = xmlInterface.attributes();
      if (attr.namedItem("name").nodeValue() == "org.freedesktop.UDisks2.Filesystem") {
        QVariantMap map;
        map["org.freedesktop.UDisks2.Filesystem"] = QVariant();
        slotDeviceAdded(path, map);
      }
    }
  }
}

QString CDeviceWatcherLinux::readMountPoint(const QString& path) {
  QDBusMessage message =
      QDBusMessage::createMethodCall("org.freedesktop.UDisks2", path, "org.freedesktop.DBus.Properties", "Get");

  {
    QList<QVariant> args;
    args << "org.freedesktop.UDisks2.Filesystem" << "MountPoints";
    message.setArguments(args);
  }

  const QDBusMessage& reply = QDBusConnection::systemBus().call(message);

#if defined(Q_OS_FREEBSD)
  // this is probably broken code and needs to be fixed
  for (const QVariant& arg : reply.arguments()) {
    if (!arg.value<QDBusVariant>().variant().value<QStringList>().isEmpty()) {
      points.append(arg.value<QDBusVariant>().variant().value<QStringList>().first());
    }
  }
#else
  QStringList mountPoints;

  for (const QVariant& arg : reply.arguments()) {
    // This seems to be the only way to get the underlying DBus data type.
    // It's is a very poor design. Maybe we do not understand Qt.
    const QVariant& dbusVar = arg.value<QDBusVariant>().variant();
    const QDBusArgument& dbusArg = dbusVar.value<QDBusArgument>();

    // Test for the correct signature. It's not really expected to fail.
    // But better safe than sorry
    if (dbusArg.currentSignature() != "aay") {
      continue;
    }

    // As the signature is "aay" it's an array of an array of bytes.
    // This translates to an array of QByteArray. Don't know why there
    // should be several mount points and if that is ever happening.
    // But we do not expect less wackiness from a DBus definition.

    // That is the way Qt wants us to iterate
    dbusArg.beginArray();
    while (!dbusArg.atEnd()) {
      QByteArray data;
      dbusArg >> data;
      // Not sure about this one. But my guts feeling tells me that
      // we should decode the stuff according to the system's requirements.
      auto decoder = QStringDecoder(QStringDecoder::System);
      mountPoints << decoder(data.data());
    }
    dbusArg.endArray();
  }
#endif

  if (!mountPoints.isEmpty()) {
    return mountPoints.first();
  }
  return "";
}

void CDeviceWatcherLinux::slotKMTPDeviceChanged() {
  // As the signal only signals a change in the device list, we need to find out
  // which device is new, which is already attached and which one is removed
  QMap<QString, QStringList> oldKnownMtpDevices = knownMtpDevices;
  knownMtpDevices.clear();

  // iterate over all MTP devices
  const QList<QDBusObjectPath>& devices = kMtpDaemon->listDevices().value();
  for (const QDBusObjectPath& devicePath : devices) {
    org::kde::kmtp::Device device("org.kde.kiod6", devicePath.path(), QDBusConnection::sessionBus(), this);
    const QString& key = device.udi();
    // skip known devices
    if (oldKnownMtpDevices.contains(key)) {
      knownMtpDevices[key] = oldKnownMtpDevices.take(key);
    } else {
      // check unknown devices for a storage with GarminDevice.xml in the path
      const QList<QDBusObjectPath>& storages = device.listStorages();
      for (const QDBusObjectPath& storagePath : storages) {
        org::kde::kmtp::Storage storage("org.kde.kiod6", storagePath.path(), QDBusConnection::sessionBus(), this);
        const KMTPFile& fileGarminDeviceXml = storage.getFileMetadata("/GARMIN/GarminDevice.xml").value();
        if (fileGarminDeviceXml.isValid() && !fileGarminDeviceXml.isFolder()) {
          // If the device is a Garmin add each storage of the device as a single device.
          addKMtpDevice(device, key);
          break;
        }
      }
    }
  }

  // oldKnownMtpDevices now contains all devices that need to be removed.
  for (const QStringList& keys : std::as_const(oldKnownMtpDevices)) {
    for (const QString& key : keys) {
      listWks->removeDevice(key);
    }
  }
}

void CDeviceWatcherLinux::addKMtpDevice(org::kde::kmtp::Device& device, const QString& deviceKey) {
  CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
  const QList<QDBusObjectPath>& storages = device.listStorages();
  for (const QDBusObjectPath& storagePath : storages) {
    org::kde::kmtp::Storage storage("org.kde.kiod6", storagePath.path(), QDBusConnection::sessionBus(), this);
    const QString& key = QString("%1@%2").arg(storage.description(), device.udi());
    new CDeviceGarminMtp(storagePath, key, listWks);
    knownMtpDevices[deviceKey] << key;
  }
  emit sigChanged();
}

void CDeviceWatcherLinux::slotGVFSMtpVolumeAdded(const QString& dbus_name, const QString& id, GVFSMtpVolume volume) {
  qDebug() << "CDeviceWatcherLinux::slotGVFSMtpVolumeAdded" << dbus_name << id << volume.activationUri;
  if (volume.canMount && volume.shouldAutomount && volume.activationUri.contains("GARMIN", Qt::CaseInsensitive)) {
    gvfsMtpVolumeMonitor->VolumeMount(id, "", 0, "").waitForFinished();
  }
}

void CDeviceWatcherLinux::slotGVFSMtpVolumeRemoved(const QString& dbus_name, const QString& id, GVFSMtpVolume volume) {
  qDebug() << "CDeviceWatcherLinux::slotGVFSMtpVolumeRemoved" << dbus_name << id << volume.activationUri;
}

void CDeviceWatcherLinux::slotGVFSMounted(GVFSMount mount) {
  qDebug() << "CDeviceWatcherLinux::slotGVFSMounted" << mount.dbusId << mount.objectPath << mount.fuseMountPoint;
  QDir dir(mount.fuseMountPoint.constData());
  const QStringList& paths = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString& path : paths) {
    if (dir.exists(path + "/Garmin/GarminDevice.xml") || dir.exists(path + "/GARMIN/GarminDevice.xml")) {
      addGVFSMtpDevice(mount, paths);
      break;
    }
  }
}

void CDeviceWatcherLinux::slotGVFSUnmounted(GVFSMount mount) {
  qDebug() << "CDeviceWatcherLinux::slotGVFSUnmounted" << mount.dbusId << mount.objectPath << mount.fuseMountPoint;
  const QStringList& devices = knownMtpDevices.keys();
  for (const QString& device : devices) {
    const QStringList& keys = knownMtpDevices[device];
    for (const QString& key : keys) {
      listWks->removeDevice(key);
    }
    knownMtpDevices.remove(device);
  }
}

void CDeviceWatcherLinux::addGVFSMtpDevice(const GVFSMount& mount, const QStringList& storages) {
  CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
  for (const QString& storagePath : storages) {
    const QString& key = QString("%1@%2").arg(storagePath, mount.dbusId);
    new CDeviceGarminMtp(mount, storagePath, key, listWks);
    knownMtpDevices[mount.dbusId] << key;
  }
  emit sigChanged();
}
