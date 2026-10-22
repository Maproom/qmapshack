/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg <>

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

#ifndef CDEVICEWATCHERMAC_H
#define CDEVICEWATCHERMAC_H

#include <DiskArbitration/DiskArbitration.h>

#include <QDeadlineTimer>
#include <QMap>
#include <QPointer>
#include <QStorageInfo>
#include <QThread>
#include <QTimer>
#include <QtCore/QStringList>
#include <atomic>

#include "device/CDeviceAccessLibMtp.h"
#include "device/IDeviceWatcher.h"

struct libusb_context;

class CDeviceWorker : public QThread {
  Q_OBJECT
 public:
  CDeviceWorker();

  void run() override;
  void stop();

 signals:
  void sigDeviceAdded(QString dev);
  void sigDeviceRemoved(QString dev);

 private:
  volatile bool mStop;
  DASessionRef mSession;
};

/**
 * @brief Reports Garmin USB devices coming and going through libusb hotplug
 *
 * Scanning the bus with libmtp would open every unknown USB device, so only Garmin devices are looked at.
 */
class CUsbWorker : public QThread {
  Q_OBJECT
 public:
  CUsbWorker() = default;
  ~CUsbWorker() override;

  /**
   * @brief Register for Garmin devices and start the event thread
   *
   * Devices already connected are reported, too.
   */
  bool startHotplug();
  void stop();
  void run() override;

 signals:
  void sigUsbDeviceAdded(quint8 bus, quint8 address, quint16 vendor, quint16 product);
  void sigUsbDeviceRemoved(quint8 bus, quint8 address, quint16 product);

 private:
  libusb_context* context = nullptr;
  int handle = 0;
  std::atomic_bool stopping{false};
};

class CDeviceWatcherMac : public IDeviceWatcher {
  Q_OBJECT
 public:
  CDeviceWatcherMac(CGisListWks* parent);

 private slots:
  void slotUpdate() override;
  void slotDeviceAdded(QString dev);
  void slotDeviceRemoved(QString dev);
  void slotEndListing();
  void slotUsbDeviceAdded(quint8 bus, quint8 address, quint16 vendor, quint16 product);
  void slotUsbDeviceRemoved(quint8 bus, quint8 address, quint16 product);
  void slotUpdateMtp();

 private:
  void addDevice(const QStorageInfo& storage);
  /**
   * @brief Bring the workspace in line with the Garmin devices plugged in, opening at most one device
   * @param next  The time in ms until the next pass is due, or -1
   * @return True if a device was opened, which needs another pass
   */
  bool updateMtpOnce(qint64& next);

  CDeviceWorker worker;
  CUsbWorker usbWorker;
  QStringList deviceList;
  QTimer mtpTimer;

  struct plugged_t {
    CDeviceAccessLibMtp::DeviceInfo info;
    quint64 serial = 0; /**< tells a device plugged in again at the same address from the earlier one */
    QDeadlineTimer next;
    qint32 failures = 0;
    bool done = false; /**< opened, or given up */
  };
  struct storage_t {
    QString key; /**< the device in the workspace */
    QPointer<CDeviceAccessLibMtp> access;
  };
  struct opened_t {
    quint64 serial = 0;
    QList<storage_t> storages;
  };

  QMap<QString, plugged_t> pluggedMtpDevices; /**< the Garmin USB devices plugged in */
  QMap<QString, opened_t> openedMtpDevices;   /**< the USB devices with storages in the workspace */
  quint64 mtpSerial = 0;
  bool updatingMtp = false;
  bool updateMtpAgain = false; /**< called again while updating */
};

#endif  // CDEVICEWATCHERMAC_H
