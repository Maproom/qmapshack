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

#include "device/CDeviceGenericMtp.h"

#include "device/CDeviceAccessGvfsMtp.h"
#include "device/CDeviceAccessKMtp.h"
#include "gis/CGisListWks.h"
#include "gis/fit2/CFit2Project.h"
#include "gis/gpx/CGpxProject.h"
#include "misc.h"

// Used by gvfs
CDeviceGenericMtp::CDeviceGenericMtp(const GVFSMount& mount, const QString& storagePath, const QString& key,
                                     QTreeWidget* parent)
    : IDevice("", eTypeGenericMtp, key, parent), QObject(parent) {
  device = new CDeviceAccessGvfsMtp(mount, storagePath, this);
  setup();
}

// Used by kiod6
CDeviceGenericMtp::CDeviceGenericMtp(const QDBusObjectPath& objectPathStorage, const QString& key, QTreeWidget* parent)
    : IDevice("", eTypeGenericMtp, key, parent), QObject(parent) {
  device = new CDeviceAccessKMtp(objectPathStorage, this);
  setup();
}

void CDeviceGenericMtp::setup() {
  if (!device->foundValidStoragePath()) {
    return;
  }

  QString description(tr("Unknown MTP"));

  // Read information from QmsMtpDevice.json
  QTemporaryFile jFile;
  if (device->readFileFromStorage(kQmsMtpDeviceJson, jFile) && jFile.open()) {
    QJsonParseError error;
    const QJsonDocument& jDoc = QJsonDocument::fromJson(jFile.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
      qWarning() << error.errorString();
      return;
    }

    const QJsonObject& jObj = jDoc.object();
    description = jObj.value("description").toString();
    QDir rootPath(jObj.value("rootPath").toString());
    const QStringList& exportPathsRelativ = jObj.value("exportPaths").toVariant().toStringList();
    const QStringList& importPathsRelativ = jObj.value("importPaths").toVariant().toStringList();
    const QString& icon = jObj.value("icon").toString();

    // Try to read the icon from the device
    const QString& iconPath = QDir::cleanPath(rootPath.filePath(icon));
    const QPixmap& pixmap = device->getIcon(iconPath);
    if (!pixmap.isNull()) {
      IWksItem::icon = pixmap;
    }

    for (const QString& path : exportPathsRelativ) {
      const QString& _path = QDir::cleanPath(rootPath.filePath(path));
      createProjectsFromFiles(_path);
      exportPaths << _path;
    }

    for (const QString& path : importPathsRelativ) {
      createProjectsFromFiles(QDir::cleanPath(rootPath.filePath(path)));
    }
  }
  setText(CGisListWks::eColumnName, QString("%1 (%2)").arg(description, device->decription()));
}

bool CDeviceGenericMtp::removeFromDevice(const QString& filename) {
  qDebug() << "CDeviceGenericMtp::removeFromDevice(" << filename << ")";
  return device->removeFileFromStorage(filename);
}

void CDeviceGenericMtp::insertCopyOfProject(IGisProject* project) {
  for (const QString& path : exportPaths) {
    const QString& filename = createFileName(project, path, ".gpx");
    qDebug() << "CDeviceGenericMtp::insertCopyOfProject(" << filename << ")";

    CGpxProject* gpx = new CGpxProject(filename, project, this);
    if (!gpx->isValid()) {
      delete gpx;
      return;
    }

    QTemporaryFile file;
    openFileCheckSuccess(QIODevice::ReadWrite, file); // saveAs will close the file
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
}

void CDeviceGenericMtp::createProjectsFromFiles(const QString& subdirectory) {
  static const QStringList knownSuffix = {"gpx", "fit"};
  QDir d(subdirectory);

  const QStringList& files = device->listFilesOnStorage(subdirectory);
  for (const QString& file : files) {
    const QString suffix = QFileInfo(file).suffix().toLower();
    if (knownSuffix.contains(suffix)) {
      QTemporaryFile tempFile;
      if (!device->readFileFromStorage(d.filePath(file), tempFile)) {
        return;
      }
      openFileCheckSuccess(QIODevice::ReadWrite, tempFile);
      IGisProject* project = nullptr;
      if (suffix == "gpx") {
        project = new CGpxProject(tempFile, d.filePath(file), this);
      } else if (suffix == "fit") {
        project = new CFit2Project(tempFile, d.filePath(file), this);
      }
      if (project && !project->isValid()) {
        delete project;
      }
    }
  }
}

QString CDeviceGenericMtp::createFileName(IGisProject* project, const QString& path, const QString& suffix) const {
  return QDir(path).filePath(simplifiedName(project) + suffix);
}

QString CDeviceGenericMtp::simplifiedName(IGisProject* project) const {
  static const QRegularExpression re("[^A-Za-z0-9_]");
  QString tempName = project->getName();
  return tempName.remove(re);
}

void CDeviceGenericMtp::reorderProjects(IGisProject* project) {
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
