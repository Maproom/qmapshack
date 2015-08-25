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

#ifndef CDEVICEWATCHERMAC_H
#define CDEVICEWATCHERMAC_H

#include <QThread>
#include <QtCore/QStringList>
#include <QStorageInfo>
#include <DiskArbitration/DiskArbitration.h>

#include "device/IDeviceWatcher.h"


class CDeviceWorker : public QThread
{
    Q_OBJECT
public:
    CDeviceWorker();
    virtual ~CDeviceWorker();
    
    void run() Q_DECL_OVERRIDE;
    void stop();
    
    void eventDiskAppear(DADiskRef disk);
    void eventDiskDisappear(DADiskRef disk);
    
signals:
    void sigDeviceAdded(const QString& dev, const QString& path);
    void sigDeviceRemoved(const QString& dev, const QString& path);
    
private:
    void init();
    QString getMountPoint(DADiskRef disk);
    
    volatile bool mStop;
    DASessionRef mSession;
};


class CDeviceWatcherMac : public IDeviceWatcher
{
    Q_OBJECT
public:
    CDeviceWatcherMac(CGisListWks *parent);
    virtual ~CDeviceWatcherMac();
    
    CDeviceWorker worker;

private slots:
    void slotUpdate();
    void slotDeviceAdded(const QString& dev, const QString& path);
    void slotDeviceRemoved(const QString& dev, const QString& path);
    void slotEndListing();
    
private:
    void addDevice(QStorageInfo storage);
    
    QStringList sDevices;
};

#endif //CDEVICEWATCHERMAC_H

