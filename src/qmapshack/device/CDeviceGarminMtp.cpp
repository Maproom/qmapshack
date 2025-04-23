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

#include "gis/CGisListWks.h"

CDeviceGarminMtp::CDeviceGarminMtp(const QDBusObjectPath& objectPathStorage, const QString& model, const QString& key,
                                   QTreeWidget* parent)
    : IDevice("", eTypeGarminMtp, key, parent), QObject(parent) {
  storage = new org::kde::kmtp::Storage("org.kde.kiod6", objectPathStorage.path(), QDBusConnection::sessionBus(), this);

  setText(CGisListWks::eColumnName, QString("%1 (%2)").arg(model, storage->description()));
  // todo: find root path
  dir.setPath("/GARMIN");

  // try to read the icon from the device
  QTemporaryFile icon;
  if (getFileFromStorage(dir.filePath("Garmintriangletm.ico"), icon)) {
    QPixmap pixmap;
    pixmap.loadFromData(icon.readAll());
    if (!pixmap.isNull()) {
      setIcon(CGisListWks::eColumnIcon, pixmap);
    }
  }

  // Read information from GarminDevice.xml
  QTemporaryFile garminDeviceXmlFile;
  if (!getFileFromStorage(dir.filePath("GarminDevice.xml"), garminDeviceXmlFile)) {
    return;
  }

  QDomDocument dom;
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
      pathGpx = xmlPath.toElement().text().trimmed();
    } else if (name == "GeotaggedPhotos") {
      pathPictures = xmlPath.toElement().text().trimmed();
    } else if (name == "GeocachePhotos") {
      pathSpoilers = xmlPath.toElement().text().trimmed();
    } else if (name == "FIT_TYPE_4") {
      pathActivities = xmlPath.toElement().text().trimmed();
    } else if (name == "FIT_TYPE_6") {
      // courses
      pathCourses = xmlPath.toElement().text().trimmed();
    } else if (name == "FIT_TYPE_8") {
      pathLocations = xmlPath.toElement().text().trimmed();
    } else if (name == "Adventures") {
      pathAdventures = xmlPath.toElement().text().trimmed();
    } else if (name == "FitnessCourses") {
      pathTcx = xmlPath.toElement().text().trimmed();
    }
  }

  qDebug() << pathGpx;
  qDebug() << pathPictures;
  qDebug() << pathSpoilers;
  qDebug() << pathActivities;
  qDebug() << pathCourses;
  qDebug() << pathLocations;
  qDebug() << pathAdventures;
  qDebug() << pathTcx;

  createProjectsFromFiles(pathGpx, "gpx");
}

void CDeviceGarminMtp::insertCopyOfProject(IGisProject* project) {}

void CDeviceGarminMtp::createProjectsFromFiles(QString subdirecoty, QString fileEnding) {}

bool CDeviceGarminMtp::getFileFromStorage(const QString& path, QTemporaryFile& file) {
  file.open();
  QDBusUnixFileDescriptor descriptor(file.handle());
  int res = storage->getFileToFileDescriptor(descriptor, path);
  if (res) {
    qWarning() << "Failed to load" << path;
    return false;
  }
  if (waitForCopyOperation(storage)) {
    qWarning() << "Failed to copy" << path;
    return false;
  }
  file.seek(0);
  return true;
}

int CDeviceGarminMtp::waitForCopyOperation(const org::kde::kmtp::Storage* storage) {
  QEventLoop loop;
  connect(storage, &org::kde::kmtp::Storage::copyProgress, &loop,
          [](qulonglong sent, qulonglong total) { qDebug() << "processed size:" << sent << "of" << total; });

  // any chance to 'miss' the copyFinished signal and dead lock the worker?
  connect(storage, &org::kde::kmtp::Storage::copyFinished, &loop, &QEventLoop::exit);
  return loop.exec();
}
