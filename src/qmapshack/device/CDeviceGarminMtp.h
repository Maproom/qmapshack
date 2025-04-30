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

class QDBusObjectPath;
class IDeviceAccess;
class GVFSMount;

class CDeviceGarminMtp : public IDevice, private QObject {
  Q_DECLARE_TR_FUNCTIONS(CDeviceGarminMtp)
 public:
  CDeviceGarminMtp(const QDBusObjectPath& objectPathStorage, const QString& model, const QString& key,
                   QTreeWidget* parent);

  CDeviceGarminMtp(const GVFSMount& mount, const QString& storagePath, const QString& key, QTreeWidget* parent);

  virtual ~CDeviceGarminMtp() = default;

  bool removeFromDevice(const QString& filename);

 protected:
  void insertCopyOfProject(IGisProject* project) override;

  // org::kde::kmtp::Storage* storage;

 private:
  void setup();
  void createProjectsFromFiles(QString subdirectory, QString extension);
  QString createFileName(IGisProject* project, const QString& path, const QString& suffix) const;
  QString simplifiedName(IGisProject* project) const;
  void reorderProjects(IGisProject* project);

  IDeviceAccess* device;

  QString id;
  QString partno;
  QString description;
  QString pathGpx;
  QString pathPictures;
  QString pathSpoilers;
  QString pathActivities;
  QString pathCourses;
  QString pathLocations;
  QString pathAdventures;  // no default
  QString pathTcx;         // no default
};

#endif  // CDEVICEGARMINMTP_H
