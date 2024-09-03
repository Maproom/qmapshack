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

#include "device/IDevice.h"
#include "gis/CGisListWks.h"

CDeviceWatcherLinux::CDeviceWatcherLinux(CGisListWks* parent) : IDeviceWatcher(parent) {
  QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2",
                                       "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this,
                                       SLOT(slotDeviceAdded(QDBusObjectPath, QVariantMap)));

  QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2",
                                       "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", this,
                                       SLOT(slotDeviceRemoved(QDBusObjectPath, QStringList)));
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
  QStringList points;
  QDBusMessage message =
      QDBusMessage::createMethodCall("org.freedesktop.UDisks2", path, "org.freedesktop.DBus.Properties", "Get");

  {
    QList<QVariant> args;
    args << "org.freedesktop.UDisks2.Filesystem"
         << "MountPoints";
    message.setArguments(args);
  }

  QDBusMessage reply = QDBusConnection::systemBus().call(message);

#if defined(Q_OS_FREEBSD)
  for (const QVariant& arg : reply.arguments()) {
    if (!arg.value<QDBusVariant>().variant().value<QStringList>().isEmpty()) {
      points.append(arg.value<QDBusVariant>().variant().value<QStringList>().first());
    }
  }
#else
  QList<QByteArray> list;
  {
    const QList<QVariant>& args = reply.arguments();
    for (const QVariant& arg : args) {
      arg.value<QDBusVariant>().variant().value<QDBusArgument>() >> list;
    }
  }

  for (const QByteArray& point : std::as_const(list)) {
    points.append(point);
  }
#endif

  if (!points.isEmpty()) {
    return points.first();
  }
  return "";
}
