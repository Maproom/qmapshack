/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg <>

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

#ifndef CDEVICEWATCHERMAC_H
#define CDEVICEWATCHERMAC_H

#include <DiskArbitration/DiskArbitration.h>
#include <QStorageInfo>
#include <QtCore/QStringList>
#include <QThread>

#include "device/IDeviceWatcher.h"


class CDeviceWorker : public QThread
{
    Q_OBJECT
public:
    CDeviceWorker();

    void run() override;
    void stop();

signals:
    void sigDeviceAdded(QString dev);
    void sigDeviceRemoved(QString dev);

private:
    volatile bool mStop;
    DASessionRef mSession;
};


class CDeviceWatcherMac : public IDeviceWatcher
{
    Q_OBJECT
public:
    CDeviceWatcherMac(CGisListWks* parent);

private slots:
    void slotUpdate() override;
    void slotDeviceAdded(QString dev);
    void slotDeviceRemoved(QString dev);
    void slotEndListing();

private:
    void addDevice(const QStorageInfo& storage);

    CDeviceWorker worker;
    QStringList deviceList;
};

#endif //CDEVICEWATCHERMAC_H

