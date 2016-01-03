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

#include "device/CDeviceWatcherWindows.h"
#include "gis/CGisListWks.h"

#include "CMainWindow.h"
#include <QtCore>

CDeviceWatcherWindows * CDeviceWatcherWindows::pSelf = 0;

CDeviceWatcherWindows::CDeviceWatcherWindows(CGisListWks *parent)
    : IDeviceWatcher(parent)
{
    pSelf = this;
}

CDeviceWatcherWindows::~CDeviceWatcherWindows()
{
}

void CDeviceWatcherWindows::slotUpdate()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach(const QStorageInfo &storage, QStorageInfo::mountedVolumes())
    {
        if (storage.isValid() && storage.isReady())
        {
            probeForDevice(storage.rootPath(), storage.rootPath(), storage.name());
        }
    }
    QApplication::restoreOverrideCursor();
}

bool CDeviceWatcherWindows::event(QEvent * e)
{
    if (e->type() == CEventDevice::eEvtDeviceWindows)
    {
        CEventDevice * evt = (CEventDevice*)e;
        qDebug() << "CDeviceWatcherWindows::event()" << evt->path << evt->add;

        if (evt->add)
        {
            QStorageInfo storage(evt->path);
            if (storage.isValid() && storage.isReady())
            {
                QApplication::setOverrideCursor(Qt::WaitCursor);
                probeForDevice(evt->path, evt->path, storage.name());
                QApplication::restoreOverrideCursor();
            }
        }
        else
        {
            listWks->removeDevice(evt->path);
        }
    }

    return QObject::event(e);
}