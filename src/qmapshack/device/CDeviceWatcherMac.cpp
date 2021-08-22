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

#include "device/CDeviceWatcherMac.h"
#include "device/IDevice.h"
#include "gis/CGisListWks.h"

#include <QtWidgets>


CDeviceWatcherMac::CDeviceWatcherMac(CGisListWks* parent)
    : IDeviceWatcher(parent), worker()
{
    connect(&worker, &CDeviceWorker::sigDeviceAdded, this, &CDeviceWatcherMac::slotDeviceAdded);
    connect(&worker, &CDeviceWorker::sigDeviceRemoved, this, &CDeviceWatcherMac::slotDeviceRemoved);
    connect(qApp, &QApplication::aboutToQuit, this, &CDeviceWatcherMac::slotEndListing);
}


void CDeviceWatcherMac::addDevice(const QStorageInfo& storage)
{
    if (storage.isValid() && storage.isReady())
    {
        QString dev = QString(storage.device()).section('/', -1);
        if (!deviceList.contains(dev))
        {
            // We don't care about content of device. This is done in the probeForDevice() method.
            deviceList.append(dev);
            QString path = storage.rootPath();
            QString label = storage.name();

            qDebug() << "label:" << label << " root path: " << path << " device: " << dev;
            probeForDevice(path, dev, label);
        }
    }
}


void CDeviceWatcherMac::slotEndListing()
{
    worker.stop();
}


// Aufruf 1s. nach instanzierung
void CDeviceWatcherMac::slotUpdate()
{
    qDebug() << "slotUpdate";
    for(const QStorageInfo& storage : QStorageInfo::mountedVolumes())
    {
        addDevice(storage);
    }
}


void CDeviceWatcherMac::slotDeviceAdded(QString dev)
{
    // get mount point
    QStorageInfo storageInfo;
    for(const QStorageInfo& storage : QStorageInfo::mountedVolumes())
    {
        QString diskName = QString(storage.device()).section('/', -1);
        if(dev == diskName)
        {
            storageInfo = storage;
        }
    }

    qDebug() << "slotDeviceAdded" << dev << " " << storageInfo.rootPath();

    addDevice(storageInfo);
}


void CDeviceWatcherMac::slotDeviceRemoved(QString dev)
{
    qDebug() << "slotDeviceRemoved" << dev;

    deviceList.removeAll(dev);
    listWks->removeDevice(dev);
}



static void onDiskAppear(DADiskRef disk, CFArrayRef keys, void* context)
{
    CDeviceWorker* p = static_cast<CDeviceWorker*>(context);
    QString diskName = DADiskGetBSDName(disk);
    qDebug() << "onDiskAppear" << diskName;

    emit p->sigDeviceAdded(diskName);
}


static void onDiskDisappear(DADiskRef disk, void* context)
{
    CDeviceWorker* p = static_cast<CDeviceWorker*>(context);
    QString diskName = DADiskGetBSDName(disk);
    qDebug() << "onDiskDisappear" << diskName;

    emit p->sigDeviceRemoved(diskName);
}


CDeviceWorker::CDeviceWorker() : QThread()
{
    mSession = DASessionCreate(kCFAllocatorDefault);
    DARegisterDiskDescriptionChangedCallback(mSession, nullptr, kDADiskDescriptionWatchVolumePath, onDiskAppear, this);
    DARegisterDiskDisappearedCallback(mSession, nullptr, onDiskDisappear, this);

    QThread::start();
}


void CDeviceWorker::stop()
{
    mStop = true;
    wait();

    DAUnregisterCallback(mSession, (void*)onDiskAppear, this);
    DAUnregisterCallback(mSession, (void*)onDiskDisappear, this);

    qDebug() << "CDeviceWorker Thread.stop";
}


void CDeviceWorker::run()
{
    qDebug() << "CDeviceWorker Thread.run";
    mStop = false;

    DASessionScheduleWithRunLoop(mSession, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    SInt32 result;
    do
    {
        result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1, true);
    }
    while (!mStop && result);

    DASessionUnscheduleFromRunLoop(mSession, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
}
