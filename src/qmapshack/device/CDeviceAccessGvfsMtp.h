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

#ifndef CDEVICEACCESSGVFSMTP_H
#define CDEVICEACCESSGVFSMTP_H

#include <QCoreApplication>

#include "device/IDeviceAccess.h"
#include "device/dbus/org.gtk.vfs.Mount.h"

class GVFSMount;

class CDeviceAccessGvfsMtp : public IDeviceAccess {
  Q_DECLARE_TR_FUNCTIONS(CDeviceAccessGvfsMtp)
 public:
  CDeviceAccessGvfsMtp(const GVFSMount& mount, const QString& storagePath, QObject* parent);
  virtual ~CDeviceAccessGvfsMtp() = default;

  QPixmap getIcon() override;
  QString decription() override;

  bool readFileFromStorage(const QString& path, QFile& file) override;
  bool sendFileToStorage(const QString& path, QFile& file) override;
  bool removeFileFromStorage(const QString& path) override;
  QStringList listFilesOnStorage(const QString& path) override;

 private:
  org::gtk::vfs::Mount* storage;
};

#endif //CDEVICEACCESSGVFSMTP_H

