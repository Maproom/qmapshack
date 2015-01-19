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

private slots:
    virtual void slotUpdate() = 0;

protected:
    void probeForDevice(const QString &mountPoint, const QString& path, const QString &vendor, const QString &model);

    CGisListWks * listWks;

};

#endif //IDEVICEWATCHER_H

