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
#include "device/CDeviceGarminArchive.h"
#include "gis/CGisListWks.h"
#include "gis/fit/CFitProject.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/wpt/CGisItemWpt.h"


#include <QtWidgets>
#include <QtXml>

CDeviceGarmin::CDeviceGarmin(const QString &path, const QString &key, const QString &model, QTreeWidget *parent)
    : IDevice(path, eTypeGarmin, key, parent)
    , cntImages(0)
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
        if(name == "GPSData")
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
        else if(name == "FIT_TYPE_4")
        {
            pathActivities = xmlPath.toElement().text();
        }
        else if(name == "FIT_TYPE_6")
        {
            // courses
            pathCourses = xmlPath.toElement().text();
        }
        else if(name == "Adventures")
        {
            pathAdventures = xmlPath.toElement().text();
        }
    }

    qDebug() << dir.absoluteFilePath(pathGpx);
    qDebug() << dir.absoluteFilePath(pathPictures);
    qDebug() << dir.absoluteFilePath(pathSpoilers);
    qDebug() << dir.absoluteFilePath(pathActivities);
    qDebug() << dir.absoluteFilePath(pathCourses);
    qDebug() << dir.absoluteFilePath(pathAdventures);

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
    if(!pathAdventures.isEmpty() && !dir.exists(pathAdventures))
    {
        dir.mkpath(pathAdventures);
    }


    this->createProjectsFromFiles(pathGpx, "gpx");
    this->createProjectsFromFiles(pathGpx + "/Current", "gpx");

    QDir dirArchive(dir.absoluteFilePath(pathGpx + "/Archive"));
    if(dirArchive.exists() && (dirArchive.entryList(QStringList("*.gpx")).count() != 0))
    {
        archive = new CDeviceGarminArchive(dir.absoluteFilePath(pathGpx + "/Archive"), this);
    }

    this->createProjectsFromFiles(pathActivities, "fit");
    this->createProjectsFromFiles(pathCourses, "fit");
}

void CDeviceGarmin::createProjectsFromFiles(QString subdirecoty, QString fileEnding)
{
    QDir dirLoop(dir.absoluteFilePath(subdirecoty));
    qDebug() << "reading files from device: " << dirLoop.path();
    QStringList entries = dirLoop.entryList(QStringList("*." + fileEnding));
    foreach(const QString &entry, entries)
    {
        const QString filename = dirLoop.absoluteFilePath(entry);
        IGisProject * project = nullptr;
        if (fileEnding == "fit")
        {
            project = new CFitProject(filename, this);
        }
        if (fileEnding == "gpx")
        {
            project = new CGpxProject(filename, this);
        }

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

    if(testForExternalProject(filename))
    {
        return;
    }

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

    createAdventureFromProject(project, pathGpx + "/" + name + ".gpx");

    // move new project to top of any sub-folder/sub-device item
    int newIdx      = NOIDX;
    const int myIdx = childCount() - 1;
    for(int i = myIdx - 1; i >= 0; i--)
    {
        IDevice * device = dynamic_cast<IDevice*>(child(i));
        if(0 == device)
        {
            break;
        }

        newIdx = i;
    }

    if(newIdx != NOIDX)
    {
        takeChild(myIdx);
        insertChild(newIdx, gpx);
    }
}

void CDeviceGarmin::saveImages(CGisItemWpt& wpt)
{
    if(wpt.isGeocache())
    {
        QString name = wpt.getName();
        quint32 size = name.size();
        QString path = QString("%1/%2/%3").arg(name.at(size-1)).arg(name.at(size -2)).arg(name);

        const QDir dirSpoilers(dir.absoluteFilePath(pathSpoilers));
        const QDir dirCache(dirSpoilers.absoluteFilePath(path));
        const QList<CGisItemWpt::image_t>& images = wpt.getImages();

        if(!dirCache.exists())
        {
            dirCache.mkpath(dirCache.absolutePath());
        }

        QString filename;
        foreach(const CGisItemWpt::image_t& image, images)
        {
            filename = image.info;
            filename = filename.remove(QRegExp("[^A-Za-z0-9_]"));

            if(!filename.endsWith("jpg"))
            {
                filename += ".jpg";
            }
            image.pixmap.save(dirCache.absoluteFilePath(filename));
        }
    }
    else
    {
        const QDir dirImages(dir.absoluteFilePath(pathPictures));
        const QString& key = wpt.getKey().project;
        const QList<CGisItemWpt::image_t>& images = wpt.getImages();
        QList<IGisItem::link_t> links;

        QString filename;
        foreach(const CGisItemWpt::image_t& image, images)
        {
            filename = QString("%1.%2.jpg").arg(key).arg(cntImages);
            image.pixmap.save(dirImages.absoluteFilePath(filename));

            IGisItem::link_t link;
            link.uri  = pathPictures + "/" + filename;
            link.text = tr("Picture%1").arg(cntImages);
            link.type = "Garmin";

            links << link;

            cntImages++;
        }

        wpt.appendLinks(links);
    }
}

void CDeviceGarmin::loadImages(CGisItemWpt& wpt)
{
    if(wpt.isGeocache())
    {
        QString name = wpt.getName();
        quint32 size = name.size();
        QString path = QString("%1/%2/%3").arg(name.at(size-1)).arg(name.at(size -2)).arg(name);

        const QDir dirSpoilers(dir.absoluteFilePath(pathSpoilers));
        const QDir dirCache(dirSpoilers.absoluteFilePath(path));

        QList<CGisItemWpt::image_t> images;
        QStringList entries = dirCache.entryList(QStringList("*.jpg"), QDir::Files);
        foreach(const QString &file, entries)
        {
            CGisItemWpt::image_t image;
            image.pixmap.load(dirCache.absoluteFilePath(file));

            if(!image.pixmap.isNull())
            {
                image.fileName  = file;
                image.info      = QFileInfo(file).baseName();
                images << image;
            }
        }

        if(!images.isEmpty())
        {
            wpt.appendImages(images);
        }
    }
    else
    {
        const QList<IGisItem::link_t>& links = wpt.getLinks();
        QList<CGisItemWpt::image_t> images;

        foreach(const IGisItem::link_t& link, links)
        {
            if(link.type != "Garmin")
            {
                continue;
            }
            CGisItemWpt::image_t image;
            image.fileName = link.text;
            image.pixmap.load(dir.absoluteFilePath(link.uri.toString()));

            images << image;
        }

        if(!images.isEmpty())
        {
            wpt.appendImages(images);
            wpt.removeLinksByType("Garmin");
        }
    }
}

void CDeviceGarmin::startSavingProject(IGisProject * project)
{
    cntImages = 0;
}

void CDeviceGarmin::aboutToRemoveProject(IGisProject * project)
{
    // remove images attached to project
    const QString& key = project->getKey();
    const QDir dirImages(dir.absoluteFilePath(pathPictures));
    QStringList entries = dirImages.entryList(QStringList("*.jpg"), QDir::Files);
    foreach(const QString &entry, entries)
    {
        QString filename = dirImages.absoluteFilePath(entry);
        QFileInfo fi(filename);

        if(fi.baseName() == key)
        {
            QFile::remove(filename);
        }
    }

    // remove geo cache spoiler images
    const int N = project->childCount();
    for(int n = 0; n < N; n++)
    {
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(project->child(n));
        if(wpt && wpt->isGeocache())
        {
            QString name = wpt->getName();
            quint32 size = name.size();
            QString path = QString("%1/%2/%3").arg(name.at(size-1)).arg(name.at(size -2)).arg(name);

            QDir dirSpoilers(dir.absoluteFilePath(pathSpoilers));
            QDir dirCache(dirSpoilers.absoluteFilePath(path));

            dirCache.removeRecursively();
        }
    }

    if(!pathAdventures.isEmpty())
    {
        const QDir dirAdventures(dir.absoluteFilePath(pathAdventures));
        QString filename = dirAdventures.absoluteFilePath(key + ".adv");
        QFile::remove(filename);
    }
}

