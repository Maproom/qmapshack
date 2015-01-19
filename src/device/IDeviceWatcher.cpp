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

#include "device/IDeviceWatcher.h"
#include "device/CDeviceGarmin.h"
#include "device/CDeviceTwoNav.h"
#include "gis/CGisListWks.h"

#include <QtCore>

IDeviceWatcher::IDeviceWatcher(CGisListWks *parent)
    : QObject(parent)
    , listWks(parent)
{
    QTimer::singleShot(600, this, SLOT(slotUpdate()));
}

IDeviceWatcher::~IDeviceWatcher()
{

}

void IDeviceWatcher::probeForDevice(const QString& mountPoint, const QString& path, const QString& vendor, const QString& model)
{
    QDir dir(mountPoint);
    if(!dir.exists())
    {
        return;
    }

    qDebug() << "Probe device at" << mountPoint << path << vendor << model ;
    QStringList entries = dir.entryList();

    if(entries.contains("Garmin"))
    {
        qDebug() << "It's a Garmin!";
        new CDeviceGarmin(mountPoint, path, listWks);

    }
    else if(entries.contains("TwoNavData"))
    {
        qDebug() << "It's a TwoNav!";
        new CDeviceTwoNav(mountPoint, path, listWks);
    }
    else
    {
        qDebug() << "Don't know it :(";
    }
}
