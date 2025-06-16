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

#ifndef CDEVICEGENERICMTP_H
#define CDEVICEGENERICMTP_H

#include <functional>

#include "device/IDevice.h"

class QDBusObjectPath;
class IDeviceAccess;
class GVFSMount;

class CDeviceGenericMtp : public IDevice, private QObject {
  Q_DECLARE_TR_FUNCTIONS(CDeviceGenericMtp)
 public:
  CDeviceGenericMtp(const QDBusObjectPath& objectPathStorage, const QString& key, QTreeWidget* parent);

  CDeviceGenericMtp(const GVFSMount& mount, const QString& storagePath, const QString& key, QTreeWidget* parent);

  virtual ~CDeviceGenericMtp() = default;

  bool removeFromDevice(const QString& filename);

  static constexpr const char* kQmsMtpDeviceJson = "QmsMtpDevice.json";

 protected:
  void insertCopyOfProject(IGisProject* project) override;

 private:
  struct readFromPath_t {
    QString format;
    QString dir;
  };

  void setup();
  void createProjectsFromFiles(const QString& subdirectory);
  QString createFileName(IGisProject* project, const QString& path, const QString& suffix) const;
  QString simplifiedName(IGisProject* project) const;
  void reorderProjects(IGisProject* project);

  IDeviceAccess* device;
  QStringList exportPaths;
};

#endif  // CDEVICEGENERICMTP_H
