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

#include "device/CDeviceWatcherMac.h"

#include <libusb.h>

#include <QtWidgets>
#include <algorithm>

#include "device/CDeviceAccessLibMtp.h"
#include "device/CDeviceGarminMtp.h"
#include "device/IDevice.h"
#include "gis/CGisListWks.h"

CDeviceWatcherMac::CDeviceWatcherMac(CGisListWks* parent) : IDeviceWatcher(parent), worker() {
  connect(&worker, &CDeviceWorker::sigDeviceAdded, this, &CDeviceWatcherMac::slotDeviceAdded);
  connect(&worker, &CDeviceWorker::sigDeviceRemoved, this, &CDeviceWatcherMac::slotDeviceRemoved);
  connect(qApp, &QApplication::aboutToQuit, this, &CDeviceWatcherMac::slotEndListing);
  // macOS has no MTP service to report devices, so USB hotplug does
  connect(&usbWorker, &CUsbWorker::sigUsbDeviceAdded, this, &CDeviceWatcherMac::slotUsbDeviceAdded,
          Qt::QueuedConnection);
  connect(&usbWorker, &CUsbWorker::sigUsbDeviceRemoved, this, &CDeviceWatcherMac::slotUsbDeviceRemoved,
          Qt::QueuedConnection);
  mtpTimer.setSingleShot(true);
  connect(&mtpTimer, &QTimer::timeout, this, &CDeviceWatcherMac::slotUpdateMtp);
}

void CDeviceWatcherMac::addDevice(const QStorageInfo& storage) {
  if (storage.isValid() && storage.isReady()) {
    QString dev = QString(storage.device()).section('/', -1);
    if (!deviceList.contains(dev)) {
      // We don't care about content of device. This is done in the probeForDevice() method.
      deviceList.append(dev);
      QString path = storage.rootPath();
      QString label = storage.name();

      qDebug() << "label:" << label << " root path: " << path << " device: " << dev;
      probeForDevice(path, dev, label);
    }
  }
}

void CDeviceWatcherMac::slotEndListing() {
  mtpTimer.stop();
  usbWorker.stop();
  worker.stop();
}

// Aufruf 1s. nach instanzierung
void CDeviceWatcherMac::slotUpdate() {
  qDebug() << "slotUpdate";
  for (const QStorageInfo& storage : QStorageInfo::mountedVolumes()) {
    addDevice(storage);
  }
  if (!usbWorker.startHotplug()) {
    qWarning() << "USB hotplug is not available. Garmin MTP devices will not be found.";
  }
}

static QString mtpKey(quint8 bus, quint8 address, quint16 product) {
  return QString("libmtp:%1:%2:%3").arg(bus).arg(address).arg(product);
}

void CDeviceWatcherMac::slotUsbDeviceAdded(quint8 bus, quint8 address, quint16 vendor, quint16 product) {
  const QString key = mtpKey(bus, address, product);
  qDebug() << "slotUsbDeviceAdded" << key;
  // the enumeration at registration and a hotplug event may report the same device
  if (pluggedMtpDevices.contains(key)) {
    return;
  }
  plugged_t device;
  device.info = {vendor, product, bus, address, key};
  device.serial = ++mtpSerial;
  // a booting device shows up for a moment and leaves again, so it is opened once it has settled
  device.next.setRemainingTime(3000);
  pluggedMtpDevices.insert(key, device);
  slotUpdateMtp();
}

void CDeviceWatcherMac::slotUsbDeviceRemoved(quint8 bus, quint8 address, quint16 product) {
  const QString key = mtpKey(bus, address, product);
  qDebug() << "slotUsbDeviceRemoved" << key;
  pluggedMtpDevices.remove(key);
  slotUpdateMtp();
}

void CDeviceWatcherMac::slotUpdateMtp() {
  // opening and removing devices run local event loops, which may call this again
  if (updatingMtp) {
    updateMtpAgain = true;
    return;
  }
  updatingMtp = true;
  qint64 next = -1;
  bool again = true;
  while (again) {
    updateMtpAgain = false;
    again = updateMtpOnce(next) || updateMtpAgain;
  }
  updatingMtp = false;
  if (next >= 0) {
    mtpTimer.start(next);
  } else {
    mtpTimer.stop();
  }
}

bool CDeviceWatcherMac::updateMtpOnce(qint64& next) {
  next = -1;
  auto dueIn = [&next](qint64 milliseconds) { next = next < 0 ? milliseconds : qMin(next, milliseconds); };

  const QStringList openedKeys = openedMtpDevices.keys();
  for (const QString& key : openedKeys) {
    const opened_t opened = openedMtpDevices.value(key);
    const auto plugged = pluggedMtpDevices.constFind(key);
    if (plugged != pluggedMtpDevices.constEnd() && plugged->serial == opened.serial) {
      continue;
    }
    // a storage with a call running waits in local event loops and must not be deleted
    if (std::any_of(opened.storages.begin(), opened.storages.end(),
                    [](const storage_t& storage) { return storage.access && storage.access->isBusy(); })) {
      dueIn(1000);
      continue;
    }
    openedMtpDevices.remove(key);
    for (const storage_t& storage : opened.storages) {
      listWks->removeDevice(storage.key);
    }
    emit sigChanged();
  }

  for (auto device = pluggedMtpDevices.cbegin(); device != pluggedMtpDevices.cend(); ++device) {
    if (device->done || openedMtpDevices.contains(device.key())) {
      continue;
    }
    if (!device->next.hasExpired()) {
      dueIn(device->next.remainingTime());
      continue;
    }

    // opening runs a local event loop, which may report devices coming and going, so this pass ends with it
    const plugged_t attempt = *device;
    const auto accesses = CDeviceAccessLibMtp::openDevice(attempt.info);
    const auto plugged = pluggedMtpDevices.find(attempt.info.key);
    if (plugged == pluggedMtpDevices.end() || plugged->serial != attempt.serial) {
      if (accesses) {
        qDeleteAll(*accesses);
      }
      return true;
    }
    if (!accesses) {
      // a booting device or one another application holds fails, so it is tried again less and less often
      qDebug() << "Failed to open MTP device" << attempt.info.key;
      if (++plugged->failures < 6) {
        plugged->next.setRemainingTime(2500 << (plugged->failures - 1));
      } else {
        plugged->done = true;
        qWarning() << "Garmin USB device" << attempt.info.key
                   << "is no MTP device or can not be opened. If Garmin Express or another MTP application holds it, "
                      "close that and plug the device in again.";
      }
      return true;
    }
    plugged->done = true;
    // a device without GarminDevice.xml gets no storages
    if (accesses->isEmpty()) {
      return true;
    }
    opened_t opened{attempt.serial, {}};
    for (CDeviceAccessLibMtp* access : *accesses) {
      opened.storages.append({access->getKey(), access});
    }
    openedMtpDevices.insert(attempt.info.key, opened);
    for (CDeviceAccessLibMtp* access : *accesses) {
      new CDeviceGarminMtp(access, access->getKey(), listWks);
    }
    emit sigChanged();
    return true;
  }
  return false;
}

void CDeviceWatcherMac::slotDeviceAdded(QString dev) {
  // get mount point
  QStorageInfo storageInfo;
  for (const QStorageInfo& storage : QStorageInfo::mountedVolumes()) {
    QString diskName = QString(storage.device()).section('/', -1);
    if (dev == diskName) {
      storageInfo = storage;
    }
  }

  qDebug() << "slotDeviceAdded" << dev << " " << storageInfo.rootPath();

  addDevice(storageInfo);
}

void CDeviceWatcherMac::slotDeviceRemoved(QString dev) {
  qDebug() << "slotDeviceRemoved" << dev;

  deviceList.removeAll(dev);
  listWks->removeDevice(dev);
}

static void onDiskAppear(DADiskRef disk, CFArrayRef keys, void* context) {
  CDeviceWorker* p = static_cast<CDeviceWorker*>(context);
  QString diskName = DADiskGetBSDName(disk);
  qDebug() << "onDiskAppear" << diskName;

  emit p->sigDeviceAdded(diskName);
}

static void onDiskDisappear(DADiskRef disk, void* context) {
  CDeviceWorker* p = static_cast<CDeviceWorker*>(context);
  QString diskName = DADiskGetBSDName(disk);
  qDebug() << "onDiskDisappear" << diskName;

  emit p->sigDeviceRemoved(diskName);
}

CDeviceWorker::CDeviceWorker() : QThread() {
  mSession = DASessionCreate(kCFAllocatorDefault);
  DARegisterDiskDescriptionChangedCallback(mSession, nullptr, kDADiskDescriptionWatchVolumePath, onDiskAppear, this);
  DARegisterDiskDisappearedCallback(mSession, nullptr, onDiskDisappear, this);

  QThread::start();
}

void CDeviceWorker::stop() {
  mStop = true;
  wait();

  DAUnregisterCallback(mSession, (void*)onDiskAppear, this);
  DAUnregisterCallback(mSession, (void*)onDiskDisappear, this);

  qDebug() << "CDeviceWorker Thread.stop";
}

void CDeviceWorker::run() {
  qDebug() << "CDeviceWorker Thread.run";
  mStop = false;

  DASessionScheduleWithRunLoop(mSession, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
  SInt32 result;
  do {
    result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1, true);
  } while (!mStop && result);

  DASessionUnscheduleFromRunLoop(mSession, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
}

static int onUsbHotplug(libusb_context*, libusb_device* device, libusb_hotplug_event event, void* context) {
  CUsbWorker* p = static_cast<CUsbWorker*>(context);
  libusb_device_descriptor descriptor;
  if (libusb_get_device_descriptor(device, &descriptor) != LIBUSB_SUCCESS) {
    return 0;
  }
  const quint8 bus = libusb_get_bus_number(device);
  const quint8 address = libusb_get_device_address(device);
  if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
    emit p->sigUsbDeviceAdded(bus, address, descriptor.idVendor, descriptor.idProduct);
  } else {
    emit p->sigUsbDeviceRemoved(bus, address, descriptor.idProduct);
  }
  return 0;
}

CUsbWorker::~CUsbWorker() { stop(); }

bool CUsbWorker::startHotplug() {
  if (libusb_init(&context) != LIBUSB_SUCCESS) {
    context = nullptr;
    return false;
  }
  // the vendor filter keeps every other device untouched
  if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG) == 0 ||
      libusb_hotplug_register_callback(
          context,
          static_cast<libusb_hotplug_event>(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
          LIBUSB_HOTPLUG_ENUMERATE, 0x091e, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, onUsbHotplug, this,
          &handle) != LIBUSB_SUCCESS) {
    libusb_exit(context);
    context = nullptr;
    return false;
  }
  QThread::start();
  return true;
}

void CUsbWorker::stop() {
  if (context == nullptr) {
    return;
  }
  stopping = true;
  libusb_hotplug_deregister_callback(context, handle);
  libusb_interrupt_event_handler(context);
  wait();
  libusb_exit(context);
  context = nullptr;
}

void CUsbWorker::run() {
  while (!stopping) {
    timeval timeout{1, 0};
    libusb_handle_events_timeout_completed(context, &timeout, nullptr);
  }
}
