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

#include "device/CDeviceWatcherMac.h"
#include "device/IDevice.h"
#include "gis/CGisListWks.h"

#include <QtWidgets>

#include <CoreServices/CoreServices.h>

#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>


CDeviceWatcherMac::CDeviceWatcherMac(CGisListWks *parent)
    : IDeviceWatcher(parent), worker()
{
    connect(&worker, &CDeviceWorker::sigDeviceAdded,   this, &CDeviceWatcherMac::slotDeviceAdded);
    connect(&worker, &CDeviceWorker::sigDeviceRemoved, this, &CDeviceWatcherMac::slotDeviceRemoved);
    connect(qApp,    &QApplication::aboutToQuit,       this, &CDeviceWatcherMac::slotEndListing);
}


CDeviceWatcherMac::~CDeviceWatcherMac()
{
}


void CDeviceWatcherMac::slotEndListing()
{
    worker.stop();
}


// Aufruf 1s. nach instanzierung
void CDeviceWatcherMac::slotUpdate()
{
    foreach(const QStorageInfo &storage, QStorageInfo::mountedVolumes())
    {
        addDevice(storage);
    }
}

void CDeviceWatcherMac::slotDeviceAdded(const QString& dev, const QString& path)
{
    qDebug() << "slotDeviceAdded" << dev << " " << path;

    QStorageInfo storage(path);
    addDevice(storage);
}


void CDeviceWatcherMac::addDevice(QStorageInfo storage)
{
    if (storage.isValid() && storage.isReady())
    {
        QString dev = QString(storage.device()).section('/', -1);
        if (!sDevices.contains(dev))
        {
            // TODO only add Garmin devices...
            sDevices.append(dev);
            //QString label = QFileInfo(path).fileName();
            QString path = storage.rootPath();
            QString device = storage.device();
            QString label = storage.name();
            QString name = storage.displayName();

            qDebug() << "name: " << storage.name() << " display name: " << name << " root path: " << path << " label (name): " << label << " device: " <<device << " " << dev;
            //probeForDevice(path, path, storage.name());
            probeForDevice(path, dev, label);

            // beim senden von *.gpx Dateien an Device:
            // 1. wird im Ordner Garmin/NewFiles als gpx abgelegt.
            // danach (z.B. wenn device abgehängt wird) übernimmt das GPS diese als *.fit im Ordner Garmin/Courses
        }
    }
}


void CDeviceWatcherMac::slotDeviceRemoved(const QString& dev, const QString& path)
{
    qDebug() << "slotDeviceRemoved" << dev;

    sDevices.removeAll(dev); //erase?
    listWks->removeDevice(dev);
}



static void onDiskAppear(DADiskRef disk, void *context)
{
    CDeviceWorker *p = static_cast<CDeviceWorker*>(context);
    p->eventDiskAppear(disk);
}


static void onDiskDisappear(DADiskRef disk, void *context)
{
    CDeviceWorker *p = static_cast<CDeviceWorker*>(context);
    p->eventDiskDisappear(disk);
}


void CDeviceWorker::eventDiskAppear(DADiskRef disk)
{
    QString disk_name = DADiskGetBSDName(disk);
    QString path = getMountPoint(disk);

    qDebug() << "onDiskAppear " << path << " " << disk_name;

    if (!QMetaObject::invokeMethod(this, "sigDeviceAdded", Qt::QueuedConnection, Q_ARG(QString, disk_name), Q_ARG(QString, path)))
    {
        qWarning("invoke deviceAdded failed");
    }
}


void CDeviceWorker::eventDiskDisappear(DADiskRef disk)
{
    QString disk_name = DADiskGetBSDName(disk);
    qDebug() << "onDiskDisappear " << disk_name;

    if (!QMetaObject::invokeMethod(this, "sigDeviceRemoved", Qt::QueuedConnection, Q_ARG(QString, disk_name), Q_ARG(QString, "")))
    {
        qWarning("invoke deviceRemoved failed");
    }
}


QString CDeviceWorker::getMountPoint(DADiskRef disk)
{
    //QString path = getMountPoint(disk);
    CFDictionaryRef dict = DADiskCopyDescription(disk);

    QString path;
    CFURLRef fspath = (CFURLRef) CFDictionaryGetValue(dict, kDADiskDescriptionVolumePathKey);
    if(fspath)
    {
        char buf[512];
        CFURLGetFileSystemRepresentation(fspath, false, (UInt8 *)buf, sizeof(buf));
        path = buf;
    }
    else
    {
        // wieso braucht es diesen Workaround für gewisse Devices?
        // bei manchen Devices ist der Volumen Path Key (fspath) nicht gesetzt.
        // TODO anderen Key verwenden?
        QString disk_name = DADiskGetBSDName(disk);
        foreach(const QStorageInfo &storage, QStorageInfo::mountedVolumes())
        {
            QString dev = QString(storage.device()).section('/', -1);
            if(dev == disk_name)
            {
                path = storage.rootPath();
            }
        }
    }

    CFRelease(dict);
    return path;
}


CDeviceWorker::CDeviceWorker() : QThread()
{
    init();
    QThread::start();
}


CDeviceWorker::~CDeviceWorker()
{
}


void CDeviceWorker::stop()
{
    mStop = true;
    wait();

    DAUnregisterCallback(mSession, (void*)onDiskAppear, this);
    DAUnregisterCallback(mSession, (void*)onDiskDisappear, this);

    qDebug() << "Thread.stop";
}


void CDeviceWorker::run()
{
    qDebug() << "Thread.run";
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


void CDeviceWorker::init()
{
    mSession = DASessionCreate(kCFAllocatorDefault);

    DARegisterDiskAppearedCallback(mSession, NULL, onDiskAppear, this);
    DARegisterDiskDisappearedCallback(mSession, NULL, onDiskDisappear, this);
}
