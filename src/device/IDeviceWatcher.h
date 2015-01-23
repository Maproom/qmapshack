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

#ifndef IDEVICEWATCHER_H
#define IDEVICEWATCHER_H

#include <QObject>

class CGisListWks;
class IDevice;

class IDeviceWatcher : public QObject
{
    Q_OBJECT
public:
    IDeviceWatcher(CGisListWks *parent);
    virtual ~IDeviceWatcher();


signals:
    void sigChanged();

private slots:
    /**
       @brief Find all devices already connected

       This slot is called only once at startup. All pathes to plug-n-play memory have
       to be passed to probeForDevice().

     */
    virtual void slotUpdate() = 0;

protected:
    /**
       @brief Probe a path for a device

       @param mountPoint    This is the mount point in a Linux sense (e.g. "/run/media/GARMIN"). For Windows this will be the drive letter (e.g. "d:\")
       @param path          This is the device path in a Linux sense (e.g. "/org/freedesktop/UDisks2/block_devices/sdc1") For Windows this will be the drive letter, too (e.g. "d:\")
       @param label         A name for the device. In Linux the last part of the mount point is used. In Windos it should be the name of the drive.
     */
    void probeForDevice(const QString &mountPoint, const QString& path, const QString &label);

    CGisListWks * listWks;
};

#endif //IDEVICEWATCHER_H

