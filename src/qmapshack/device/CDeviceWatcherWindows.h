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

#ifndef CDEVICEWATCHERWINDOWS_H
#define CDEVICEWATCHERWINDOWS_H

#include "device/IDeviceWatcher.h"

#include <QEvent>

class CEventDevice : public QEvent
{
public:

    enum event_types_e
    {
        eEvtDeviceWindows = QEvent::User + 200
    };

    CEventDevice(const QString& path, bool add) : QEvent(QEvent::Type(eEvtDeviceWindows)), add(add), path(path)
    {
    }

    bool add;
    QString path;
};

class CDeviceWatcherWindows : public IDeviceWatcher
{
    Q_OBJECT
public:
    virtual ~CDeviceWatcherWindows();

    static CDeviceWatcherWindows* self()
    {
        return pSelf;
    }

    bool event(QEvent* e);

private slots:
    void slotUpdate();

private:
    friend class CGisListWks;
    CDeviceWatcherWindows(CGisListWks* parent);
    static CDeviceWatcherWindows* pSelf;
};

#endif //CDEVICEWATCHERWINDOWS_H

