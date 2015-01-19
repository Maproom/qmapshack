/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "CDeviceWatcherLinux.h"

#include <QtWidgets>
#include <QtDBus>

CDeviceWatcherLinux::CDeviceWatcherLinux(QObject *parent)
    : IDeviceWatcher(parent)
{
    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2",
                   "/org/freedesktop/UDisks2",
                   "org.freedesktop.DBus.ObjectManager",
                   "InterfacesAdded",
                   this,
                   SLOT(slotDeviceAdded(QDBusObjectPath,QVariantMap)));

    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2",
                   "/org/freedesktop/UDisks2",
                   "org.freedesktop.DBus.ObjectManager",
                   "InterfacesRemoved",
                   this,
                   SLOT(slotDeviceRemoved(QDBusObjectPath,QStringList)));
}

CDeviceWatcherLinux::~CDeviceWatcherLinux()
{
}


void CDeviceWatcherLinux::slotDeviceAdded(const QDBusObjectPath& path, const QVariantMap& map)
{
    // ignore all paths other than a filesystem
    if(!map.contains("org.freedesktop.UDisks2.Filesystem"))
    {
        return;
    }

    QString strPath = path.path();

    mount(strPath);
    probeForDevice(readMountPoint(strPath));
    unmount(strPath);
}

void CDeviceWatcherLinux::slotDeviceRemoved(const QDBusObjectPath& path, const QStringList& list)
{
    // ignore all paths other than a filesystem
    if(!list.contains("org.freedesktop.UDisks2.Filesystem"))
    {
        return;
    }

    qDebug() << "slotDeviceRemoved" << path.path() << list;
}



QString CDeviceWatcherLinux::readMountPoint(const QString& path)
{
    QStringList points;
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",
                                                          path,
                                                          "org.freedesktop.DBus.Properties",
                                                          "Get");

    QList<QVariant> args;
    args << "org.freedesktop.UDisks2.Filesystem" << "MountPoints";
    message.setArguments(args);

    QDBusMessage reply = QDBusConnection::systemBus().call(message);

    QList<QByteArray> list;
    foreach (QVariant arg, reply.arguments())
    {
        arg.value<QDBusVariant>().variant().value<QDBusArgument>() >> list;
    }

    foreach (QByteArray point, list)
    {
        points.append(point);
    }

    if(!points.isEmpty())
    {
        return points.first();
    }
    return "";
}

void CDeviceWatcherLinux::mount(const QString &path)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",path,"org.freedesktop.UDisks2.Filesystem","Mount");
    QVariantMap args;
    args.insert("options", "flush");
    message << args;
    QDBusConnection::systemBus().call(message);
}

void CDeviceWatcherLinux::unmount(const QString &path)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2",path,"org.freedesktop.UDisks2.Filesystem","Unmount");
    QVariantMap args;
    message << args;
    QDBusConnection::systemBus().call(message);
}

