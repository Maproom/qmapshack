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

class CDeviceGarmin : public IDevice
{
public:
    CDeviceGarmin(const QString &path, const QString &key, const QString& model, QTreeWidget * parent);
    virtual ~CDeviceGarmin();

    void insertCopyOfProject(IGisProject *project)  override;
    void startSavingProject(IGisProject *project)   override;
    void saveImages(CGisItemWpt& wpt)               override;
    void loadImages(CGisItemWpt& wpt)               override;
    void aboutToRemoveProject(IGisProject *project) override;

private:
    void createProjectsFromFiles(QString subdirecoty, QString fileEnding);

    QString id;
    QString partno;
    QString description;
    QString pathGpx           = "Garmin/GPX";
    QString pathPictures      = "Garmin/JPEG";
    QString pathSpoilers      = "Garmin/GeocachePhotos";
    QString pathFitActivities = "Garmin/Activities";
    QString pathFitCourses    = "Garmin/Courses";

    int cntImages = 0;
};

#endif //CDEVICEGARMIN_H

