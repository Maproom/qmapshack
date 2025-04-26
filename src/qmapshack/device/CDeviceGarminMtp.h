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

#ifndef CDEVICEGARMINMTP_H
#define CDEVICEGARMINMTP_H

#include <functional>

#include "device/IDevice.h"
#include "device/dbus/org.kde.kmtp.Storage.h"

class QDBusObjectPath;

class CDeviceGarminMtp : public IDevice, private QObject {
  Q_DECLARE_TR_FUNCTIONS(CDeviceGarminMtp)
 public:
  CDeviceGarminMtp(const QDBusObjectPath& objectPathStorage, const QString& model, const QString& key,
                   QTreeWidget* parent);
  virtual ~CDeviceGarminMtp() = default;

  bool removeFromDevice(const QString& filename);

 protected:
  void insertCopyOfProject(IGisProject* project) override;

  org::kde::kmtp::Storage* storage;

 private:
  using fn_operation = std::function<int()>;
  bool readFileFromStorage(const QString& path, QTemporaryFile& file);
  bool sendFileToStorage(const QString& path, QTemporaryFile& file);
  int waitForCopyOperation(const org::kde::kmtp::Storage* storage, fn_operation operation);
  void createProjectsFromFiles(QString subdirectory, QString extension);
  QString createFileName(IGisProject* project, const QString& path, const QString& suffix) const;
  QString simplifiedName(IGisProject* project) const;
  void reorderProjects(IGisProject* project);

  QString id;
  QString partno;
  QString description;
  QString pathGpx = "Garmin/GPX";
  QString pathPictures = "Garmin/JPEG";
  QString pathSpoilers = "Garmin/GeocachePhotos";
  QString pathActivities = "Garmin/Activities";
  QString pathCourses = "Garmin/Courses";
  QString pathLocations = "Garmin/Locations";
  QString pathAdventures;  // no default
  QString pathTcx;         // no default
};

#endif  // CDEVICEGARMINMTP_H
