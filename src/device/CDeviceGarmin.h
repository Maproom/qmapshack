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

#ifndef CDEVICEGARMIN_H
#define CDEVICEGARMIN_H

#include "device/IDevice.h"

class CGisItemWpt;

class CDeviceGarmin : public IDevice
{
public:
    CDeviceGarmin(const QString &path, const QString &key, const QString& model, QTreeWidget * parent);
    virtual ~CDeviceGarmin();

    void insertCopyOfProject(IGisProject * project);

    void saveImages(CGisItemWpt& wpt);
    void loadImages(CGisItemWpt& wpt);

private:
    QString id;
    QString partno;
    QString description;
    QString pathPictures;
    QString pathGpx;
    QString pathSpoilers;
};

#endif //CDEVICEGARMIN_H

