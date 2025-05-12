/**********************************************************************************************
    Copyright (C) 2021 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "device/CDeviceGarminMtp.h"

#include "device/CDeviceAccessGvfsMtp.h"
#include "device/CDeviceAccessKMtp.h"
#include "device/CDeviceGarminArchiveMtp.h"
#include "gis/CGisListWks.h"
#include "gis/fit2/CFit2Project.h"
#include "gis/gpx/CGpxProject.h"

CDeviceGarminMtp::CDeviceGarminMtp(const GVFSMount& mount, const QString& storagePath, const QString& key,
                                   QTreeWidget* parent)
    : IDevice("", eTypeGarminMtp, key, parent), QObject(parent) {
  device = new CDeviceAccessGvfsMtp(mount, storagePath, this);
  setup();
}

CDeviceGarminMtp::CDeviceGarminMtp(const QDBusObjectPath& objectPathStorage, const QString& key, QTreeWidget* parent)
    : IDevice("", eTypeGarminMtp, key, parent), QObject(parent) {
  device = new CDeviceAccessKMtp(objectPathStorage, this);
  setup();
}

void CDeviceGarminMtp::setup() {
  if (!device->foundValidStoragePath()) {
    return;
  }
  // Try to read the icon from the device
  const QPixmap& pixmap = device->getIcon();
  if (!pixmap.isNull()) {
    setIcon(CGisListWks::eColumnIcon, pixmap);
  }

  // Try to read detailed information from GarminDevice.xml
  QTemporaryFile garminDeviceXmlFile;
  if (device->readFileFromStorage("GarminDevice.xml", garminDeviceXmlFile)) {
    QDomDocument dom;
    garminDeviceXmlFile.open();
    const QDomDocument::ParseResult& result = dom.setContent(&garminDeviceXmlFile);
    if (!result) {
      qDebug() << QString("Failed to read: %1\nline %2, column %3:\n %4")
                      .arg(garminDeviceXmlFile.fileName())
                      .arg(result.errorLine)
                      .arg(result.errorColumn)
                      .arg(result.errorMessage);
      return;
    }

    const QDomElement& xmlDevice = dom.documentElement();
    const QDomNode& xmlModel = xmlDevice.namedItem("Model");
    id = xmlDevice.namedItem("Id").toElement().text().trimmed();
    description = xmlModel.namedItem("Description").toElement().text().trimmed();
    partno = xmlModel.namedItem("PartNumber").toElement().text().trimmed();

    // update the tool tip with information from the device xml
    setToolTip(CGisListWks::eColumnName,
               QString("%1 (%2, V%3)")
                   .arg(description, partno, xmlModel.namedItem("SoftwareVersion").toElement().text().trimmed()));

    const QDomNode& xmlMassStorageMode = xmlDevice.namedItem("MassStorageMode");
    const QDomNodeList& xmlDataTypes = xmlMassStorageMode.toElement().elementsByTagName("DataType");

    const int N = xmlDataTypes.count();
    for (int n = 0; n < N; n++) {
      const QDomNode& xmlDataType = xmlDataTypes.item(n);
      const QDomNode& xmlName = xmlDataType.namedItem("Name");
      const QDomNode& xmlFile = xmlDataType.namedItem("File");
      const QDomNode& xmlLocation = xmlFile.namedItem("Location");
      const QDomNode& xmlPath = xmlLocation.namedItem("Path");

      QString name = xmlName.toElement().text().trimmed();

      if (name == "GPSData") {
        pathGpx = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      } else if (name == "GeotaggedPhotos") {
        pathPictures = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      } else if (name == "GeocachePhotos") {
        pathSpoilers = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      } else if (name == "FIT_TYPE_4") {
        pathActivities = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      } else if (name == "FIT_TYPE_6") {
        // courses
        pathCourses = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      } else if (name == "FIT_TYPE_8") {
        pathLocations = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      } else if (name == "Adventures") {
        pathAdventures = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      } else if (name == "FitnessCourses") {
        pathTcx = xmlPath.toElement().text().trimmed().replace("Garmin/", "", Qt::CaseInsensitive);
      }
    }
  } else {
    // build paths for memory card storages that do not have a GarminDevice.xml
    pathGpx = "GPX";
    pathActivities = "Activities";
    pathCourses = "Courses";
  }

  setText(CGisListWks::eColumnName, QString("%1 (%2)").arg(description, device->decription()));

  qDebug() << pathGpx;
  qDebug() << pathPictures;
  qDebug() << pathSpoilers;
  qDebug() << pathActivities;
  qDebug() << pathCourses;
  qDebug() << pathLocations;
  qDebug() << pathAdventures;
  qDebug() << pathTcx;

  createProjectsFromFiles(pathGpx, "gpx");
  createProjectsFromFiles(pathActivities, "fit");
  createProjectsFromFiles(pathCourses, "fit");
  if (!pathLocations.isEmpty()) {
    createProjectsFromFiles(pathLocations, "fit");
  }

  if (device->listDirsOnStorage(pathGpx).contains("Archive") &&
      !device->listFilesOnStorage(QDir(pathGpx).filePath("Archive")).isEmpty()) {
    new CDeviceGarminArchiveMtp(QDir(pathGpx).filePath("Archive"), device, this);
  };
}

bool CDeviceGarminMtp::removeFromDevice(const QString& filename) {
  qDebug() << "CDeviceGarminMtp::removeFromDevice(" << filename << ")";
  return device->removeFileFromStorage(filename);
}

void CDeviceGarminMtp::insertCopyOfProject(IGisProject* project) {
  const QString& filename = createFileName(project, pathGpx, ".gpx");
  qDebug() << "CDeviceGarminMtp::insertCopyOfProject(" << filename << ")";

  CGpxProject* gpx = new CGpxProject(filename, project, this);
  if (!gpx->isValid()) {
    delete gpx;
    return;
  }

  QTemporaryFile file;
  file.open();  // saveAs will close the file
  if (!CGpxProject::saveAs(file, filename, *gpx, false)) {
    delete gpx;
    return;
  }

  if (!device->sendFileToStorage(filename, file)) {
    delete gpx;
    return;
  }

  // move new project to top of any sub-folder/sub-device item
  reorderProjects(gpx);
}

void CDeviceGarminMtp::createProjectsFromFiles(QString subdirectory, QString extension) {
  QDir d(subdirectory);
  const QStringList& files = device->listFilesOnStorage(subdirectory);
  for (const QString& file : files) {
    if (file.endsWith(extension)) {
      QTemporaryFile tempFile;
      if (!device->readFileFromStorage(d.filePath(file), tempFile)) {
        return;
      }
      tempFile.open();
      IGisProject* project = nullptr;
      if (extension == "gpx") {
        project = new CGpxProject(tempFile, d.filePath(file), this);
      } else if (extension == "fit") {
        project = new CFit2Project(tempFile, d.filePath(file), this);
      }
      if (project && !project->isValid()) {
        delete project;
      }
    }
  }
}

QString CDeviceGarminMtp::createFileName(IGisProject* project, const QString& path, const QString& suffix) const {
  return QDir(path).filePath(simplifiedName(project) + suffix);
}

QString CDeviceGarminMtp::simplifiedName(IGisProject* project) const {
  static const QRegularExpression re("[^A-Za-z0-9_]");
  return project->getName().remove(re);
}

void CDeviceGarminMtp::reorderProjects(IGisProject* project) {
  // move new project to top of any sub-folder/sub-device item
  int newIdx = NOIDX;
  const int myIdx = childCount() - 1;
  for (int i = myIdx - 1; i >= 0; i--) {
    IDevice* device = dynamic_cast<IDevice*>(child(i));
    if (0 == device) {
      break;
    }

    newIdx = i;
  }

  if (newIdx != NOIDX) {
    takeChild(myIdx);
    insertChild(newIdx, project);
  }
}
