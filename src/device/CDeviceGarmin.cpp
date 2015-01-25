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

#include "device/CDeviceGarmin.h"
#include "gis/CGisListWks.h"
#include "gis/gpx/CGpxProject.h"

#include <QtWidgets>
#include <QtXml>

CDeviceGarmin::CDeviceGarmin(const QString &path, const QString &key, const QString &model, QTreeWidget *parent)
    : IDevice(path, key, parent)
{
    setText(CGisListWks::eColumnName, "Garmin");

    QFile file(dir.absoluteFilePath("Garmin/GarminDevice.xml"));
    file.open(QIODevice::ReadOnly);

    QDomDocument dom;
    dom.setContent(&file);
    file.close();

    const QDomElement& xmlDevice    = dom.documentElement();
    const QDomNode& xmlModel        = xmlDevice.namedItem("Model");

    id              = xmlDevice.namedItem("Id").toElement().text();
    description     = xmlModel.namedItem("Description").toElement().text();
    partno          = xmlModel.namedItem("PartNumber").toElement().text();

    setText(CGisListWks::eColumnName, QString("%1 (%2)").arg(description).arg(model));
    setToolTip(CGisListWks::eColumnName, QString("%1 (%2, %3)").arg(description).arg(partno).arg(model));

    const QDomNode& xmlMassStorageMode  = xmlDevice.namedItem("MassStorageMode");
    const QDomNodeList& xmlDataTypes    = xmlMassStorageMode.toElement().elementsByTagName("DataType");

    const int N = xmlDataTypes.count();
    for(int n = 0; n < N; n++)
    {
        const QDomNode& xmlDataType = xmlDataTypes.item(n);
        const QDomNode& xmlName     = xmlDataType.namedItem("Name");
        const QDomNode& xmlFile     = xmlDataType.namedItem("File");
        const QDomNode& xmlLocation = xmlFile.namedItem("Location");
        const QDomNode& xmlPath     = xmlLocation.namedItem("Path");

        QString name = xmlName.toElement().text();
        if(name == "UserDataSync")
        {
            pathGpx = xmlPath.toElement().text();
        }
        else if(name == "GeotaggedPhotos")
        {
            pathPictures = xmlPath.toElement().text();
        }
        else if(name == "GeocachePhotos")
        {
            pathSpoilers = xmlPath.toElement().text();
        }
    }

    if(!dir.exists(pathGpx))
    {
        dir.mkpath(pathGpx);
    }
    if(!dir.exists(pathPictures))
    {
        dir.mkpath(pathPictures);
    }
    if(!dir.exists(pathSpoilers))
    {
        dir.mkpath(pathSpoilers);
    }

    QDir dirGpx(dir.absoluteFilePath(pathGpx));
    QStringList entries = dirGpx.entryList(QStringList("*.gpx"));
    foreach(const QString &entry, entries)
    {
        IGisProject * project =  new CGpxProject(dirGpx.absoluteFilePath(entry), this);
        if(!project->isValid())
        {
            delete project;
        }
    }

    QDir dirGpxCurrent(dir.absoluteFilePath(pathGpx + "/Current"));
    entries = dirGpxCurrent.entryList(QStringList("*.gpx"));
    foreach(const QString &entry, entries)
    {
        IGisProject * project =  new CGpxProject(dirGpxCurrent.absoluteFilePath(entry), this);
        if(!project->isValid())
        {
            delete project;
        }
    }

}

CDeviceGarmin::~CDeviceGarmin()
{
}

void CDeviceGarmin::insertCopyOfProject(IGisProject * project)
{

    QString name = project->getName();
    name = name.remove(QRegExp("[^A-Za-z0-9_]"));

    QDir dirGpx = dir.absoluteFilePath(pathGpx);
    QString filename = dirGpx.absoluteFilePath(name + ".gpx");

    CGpxProject * gpx = new CGpxProject(filename, project, this);
    if(!gpx->isValid())
    {
        delete gpx;
        return;
    }

    if(!gpx->save())
    {
        delete gpx;
        return;
    }

}
