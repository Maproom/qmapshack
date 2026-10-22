/**********************************************************************************************
    Copyright (C) 2026 Jean Jacques de Jong

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

#ifndef CDEVICEACCESSLIBMTP_H
#define CDEVICEACCESSLIBMTP_H

#include <QList>
#include <memory>
#include <optional>

#include "device/IDeviceAccess.h"

/**
 * @brief MTP access to one storage of a device through libmtp, for systems without an MTP service (macOS)
 *
 * libmtp is not thread safe, so one worker thread per device owns the session and runs every operation for all
 * of the device's storages, and no two libmtp calls run at the same time. The calls stay synchronous, but wait in
 * a local event loop, so the GUI keeps repainting meanwhile.
 */
class CDeviceAccessLibMtp : public IDeviceAccess {
  Q_OBJECT
 public:
  struct DeviceInfo {
    quint16 vendor;
    quint16 product;
    quint8 bus;
    quint8 address;
    QString key;
  };

  /**
   * @brief Open a device and create an access for each storage with a Garmin folder
   *
   * No other USB device is touched. A device missing from libmtp's device table is probed for MTP first.
   * @return Nothing if the device can not be opened and listed, an empty list if it has no GarminDevice.xml
   */
  static std::optional<QList<CDeviceAccessLibMtp*>> openDevice(const DeviceInfo& info);

  ~CDeviceAccessLibMtp() override;

  const QString& getKey() const { return key; }
  /**
   * @brief True while a call to the device runs
   *
   * A call waits in local event loops, its message boxes included, so the device must not be deleted meanwhile.
   */
  bool isBusy() const;

  bool foundValidStoragePath() const override;
  QPixmap getIcon(const QString& iconPath) override;
  QString decription() override;
  bool readFileFromStorage(const QString& path, QFile& file) override;
  bool sendFileToStorage(const QString& path, QFile& file) override;
  bool removeFileFromStorage(const QString& path) override;
  QStringList listFilesOnStorage(const QString& path) override;
  QStringList listDirsOnStorage(const QString& path) override;

 private:
  struct Connection;
  struct Storage;
  CDeviceAccessLibMtp(const std::shared_ptr<Connection>& connection, std::unique_ptr<Storage> storage,
                      const QString& key);
  QStringList listEntries(const QString& path, bool folders);

  std::shared_ptr<Connection> connection;
  std::unique_ptr<Storage> storage;
  QString key;
};

#endif
