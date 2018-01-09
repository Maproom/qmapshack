/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CDEVICEGARMINARCHIVE_H
#define CDEVICEGARMINARCHIVE_H

#include "device/IDevice.h"

#include <QCoreApplication>
#include <QDir>
#include <QObject>

class CDeviceGarmin;

class CDeviceGarminArchive : public QObject, public IDevice
{
    Q_OBJECT
public:
    CDeviceGarminArchive(const QString& path, CDeviceGarmin * parent);
    virtual ~CDeviceGarminArchive() = default;

protected:
    void insertCopyOfProject(IGisProject * project) override {}

private slots:
    void slotExpanded(QTreeWidgetItem * item);
};

#endif //CDEVICEGARMINARCHIVE_H

