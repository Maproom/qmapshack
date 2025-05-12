/**********************************************************************************************
    Copyright (C) 2025 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CDEVICEGARMINARCHIVEMTP_H
#define CDEVICEGARMINARCHIVEMTP_H

#include "device/IDevice.h"

class CDeviceGarminMtp;
class IDeviceAccess;

class CDeviceGarminArchiveMtp : public QObject, public IDevice {
  Q_OBJECT
 public:
  CDeviceGarminArchiveMtp(const QString& path, IDeviceAccess* device, CDeviceGarminMtp* parent);
  virtual ~CDeviceGarminArchiveMtp() = default;

 protected:
  void insertCopyOfProject(IGisProject* project) override {}

 private slots:
  void slotExpanded(QTreeWidgetItem* item);
  void slotCollapsed(QTreeWidgetItem* item);

 private:
  IDeviceAccess* device;
};

#endif //CDEVICEGARMINARCHIVEMTP_H

