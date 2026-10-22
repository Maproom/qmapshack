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

#include "device/CDeviceAccessLibMtp.h"

#include <libmtp.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QHash>
#include <QMessageBox>
#include <QPixmap>
#include <QSemaphore>
#include <QTemporaryFile>
#include <QThread>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <functional>
#include <mutex>
#include <vector>

#include "CMainWindow.h"

namespace {
using file_ptr = std::unique_ptr<LIBMTP_file_t, decltype(&LIBMTP_destroy_file_t)>;

// held by every libmtp call, as libmtp is not thread safe
std::mutex libmtpMutex;

// set when the application quits, which aborts the transfers of every connection
std::atomic_bool quitting{false};

/**
 * @brief Called on the GUI thread before the first libmtp call
 */
void initializeLibMtp() {
  static std::once_flag initialized;
  std::call_once(initialized, []() {
    LIBMTP_Init();
    // aboutToQuit comes before a quit ends the local event loops of waiting calls, whose transfers must stop
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, []() { quitting.store(true); });
  });
}

/**
 * @brief A path relative to the Garmin folder, or nothing if it leaves the folder
 */
std::optional<QString> cleanDevicePath(QString path) {
  path.replace('\\', '/');
  if (path.startsWith('/') || path.split('/').contains("..")) {
    return std::nullopt;
  }
  path = QDir::cleanPath(path);
  return path == "." ? QString() : path;
}

QString folderOf(const QString& path) {
  const QString folder = QFileInfo(path).path();
  return folder == "." ? QString() : folder;
}
}  // namespace

struct CDeviceAccessLibMtp::Connection {
  QThread thread;
  QObject* worker = new QObject;
  LIBMTP_mtpdevice_t* device = nullptr;
  std::atomic_bool cancelled{false};
  std::atomic_int busy{0};

  bool isCancelled() const { return cancelled.load() || quitting.load(); }

  Connection() {
    worker->moveToThread(&thread);
    QObject::connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    thread.start();
  }

  ~Connection() {
    cancelled.store(true);
    // a destructor must not run an event loop, and a cancelled transfer ends quickly
    QMetaObject::invokeMethod(
        worker,
        [this]() {
          const std::lock_guard lock(libmtpMutex);
          if (device != nullptr) {
            LIBMTP_Release_Device(device);
            device = nullptr;
          }
        },
        Qt::BlockingQueuedConnection);
    thread.quit();
    thread.wait();
  }

  void run(const std::function<void()>& operation) {
    QEventLoop loop;
    QSemaphore done;
    // the worker is done with the loop and every captured local once it releases done
    QMetaObject::invokeMethod(worker, [&]() {
      {
        const std::lock_guard lock(libmtpMutex);
        operation();
      }
      QMetaObject::invokeMethod(&loop, &QEventLoop::quit, Qt::QueuedConnection);
      done.release();
    });
    // deferring user input keeps anything from starting another operation on this device meanwhile
    loop.exec(QEventLoop::ExcludeUserInputEvents);
    // a quit ends the loop early, after aboutToQuit has cancelled every transfer
    done.acquire();
  }
};

namespace {
/**
 * @brief Marks a call to the device, which waits in local event loops and must not see its device deleted
 */
class CBusy {
 public:
  explicit CBusy(std::atomic_int& busy) : busy(busy) { ++busy; }
  ~CBusy() { --busy; }

 private:
  std::atomic_int& busy;
};
}  // namespace

/**
 * @brief One storage of a device, used on the connection's worker thread only
 *
 * Files are addressed by object ids from remembered folder listings. Listing a folder, sending and removing ask the
 * device for the folder concerned, and reading reuses it. A lookup that fails forgets every listing and is tried
 * once more, and a read that fails forgets them, as the device may have changed.
 */
struct CDeviceAccessLibMtp::Storage {
  struct Entry {
    quint32 identifier = 0;
    QString name;
    bool folder = false;
  };
  struct Folder {
    quint32 identifier = 0;
    QList<Entry> entries;
  };

  Connection* connection;
  quint32 id;
  QString description;
  quint32 root = 0;                      /**< the Garmin folder */
  QHash<QString, QList<Entry>> listings; /**< the most recent listing of each folder */

  Storage(Connection* connection, quint32 id, const QString& description)
      : connection(connection), id(id), description(description) {}

  static int reportTransfer(uint64_t, uint64_t, const void* data) {
    return static_cast<const Connection*>(data)->isCancelled() ? 1 : 0;
  }

  bool cancelled() const { return connection->isCancelled(); }

  std::optional<QList<Entry>> entries(quint32 parent) {
    if (cancelled()) {
      return std::nullopt;
    }
    LIBMTP_mtpdevice_t* device = connection->device;
    LIBMTP_Clear_Errorstack(device);
    QList<Entry> result;
    LIBMTP_file_t* file = LIBMTP_Get_Files_And_Folders(device, id, parent);
    while (file != nullptr) {
      result.append({file->item_id, QString::fromUtf8(file->filename), file->filetype == LIBMTP_FILETYPE_FOLDER});
      LIBMTP_file_t* next = file->next;
      file->next = nullptr;
      LIBMTP_destroy_file_t(file);
      file = next;
    }
    // an empty folder and a failed listing both return no files
    if (LIBMTP_Get_Errorstack(device) != nullptr || cancelled()) {
      qWarning() << "MTP directory listing failed";
      LIBMTP_Dump_Errorstack(device);
      return std::nullopt;
    }
    return result;
  }

  /**
   * @brief A folder's listing, remembered unless fresh, else from the device
   */
  std::optional<QList<Entry>> contents(const QString& folder, quint32 identifier, bool fresh) {
    if (!fresh) {
      const auto remembered = listings.constFind(folder);
      if (remembered != listings.constEnd()) {
        return *remembered;
      }
    }
    auto result = entries(identifier);
    if (result) {
      listings.insert(folder, *result);
    } else {
      listings.remove(folder);
    }
    return result;
  }

  /**
   * @brief Walk down from the Garmin folder to a folder
   * @param folder  A cleaned path
   * @param fresh   List the folder itself from the device, while its ancestors may be remembered
   * @return Nothing if a folder on the path is missing or can not be listed
   */
  std::optional<Folder> folderOnce(const QString& folder, bool fresh) {
    if (root == 0) {
      return std::nullopt;
    }
    quint32 identifier = root;
    QString walked;
    for (const QString& name : folder.split('/', Qt::SkipEmptyParts)) {
      const auto siblings = contents(walked, identifier, false);
      if (!siblings) {
        return std::nullopt;
      }
      const auto match = std::find_if(siblings->begin(), siblings->end(), [&name](const Entry& entry) {
        return entry.folder && entry.name.compare(name, Qt::CaseInsensitive) == 0;
      });
      if (match == siblings->end()) {
        return std::nullopt;
      }
      identifier = match->identifier;
      walked = walked.isEmpty() ? name : walked + "/" + name;
    }
    const auto listing = contents(walked, identifier, fresh);
    if (!listing) {
      return std::nullopt;
    }
    return Folder{identifier, *listing};
  }

  /**
   * @param path   A cleaned path
   * @param fresh  List the file's folder from the device
   * @return Nothing if the file or a folder on its path is missing or can not be listed
   */
  std::optional<Entry> fileOnce(const QString& path, bool fresh) {
    const auto parent = folderOnce(folderOf(path), fresh);
    if (!parent) {
      return std::nullopt;
    }
    const QString name = QFileInfo(path).fileName();
    for (const Entry& entry : parent->entries) {
      if (!entry.folder && entry.name.compare(name, Qt::CaseInsensitive) == 0) {
        return entry;
      }
    }
    return std::nullopt;
  }

  /**
   * @brief Run a lookup, and once more with nothing remembered if it fails
   *
   * A remembered listing may hold the id of a file or folder another client has removed or made anew since.
   */
  template <typename Lookup>
  auto retryLookup(const Lookup& lookup) -> decltype(lookup()) {
    for (qint32 attempt = 0; attempt < 2 && !cancelled(); ++attempt) {
      if (auto result = lookup()) {
        return result;
      }
      listings.clear();
    }
    return std::nullopt;
  }

  std::optional<QList<Entry>> list(const QString& path) {
    const auto cleaned = cleanDevicePath(path);
    if (!cleaned) {
      return std::nullopt;
    }
    const auto folder = retryLookup([&]() { return folderOnce(*cleaned, true); });
    return folder ? std::optional(folder->entries) : std::nullopt;
  }

  bool read(const QString& path, int descriptor) {
    const auto cleaned = cleanDevicePath(path);
    if (!cleaned || cleaned->isEmpty()) {
      return false;
    }
    // reads reuse their folder's listing, as the loader lists a folder and then reads every file in it
    const auto entry = retryLookup([&]() { return fileOnce(*cleaned, false); });
    if (!entry || cancelled()) {
      return false;
    }
    LIBMTP_Clear_Errorstack(connection->device);
    if (LIBMTP_Get_File_To_File_Descriptor(connection->device, entry->identifier, descriptor, reportTransfer,
                                           connection) != 0) {
      qWarning() << "MTP download failed:" << path;
      LIBMTP_Dump_Errorstack(connection->device);
      listings.clear();
      return false;
    }
    return true;
  }

  bool deleteObject(quint32 identifier, const QString& folder) {
    LIBMTP_Clear_Errorstack(connection->device);
    const bool success = LIBMTP_Delete_Object(connection->device, identifier) == 0;
    listings.remove(folder);
    if (!success) {
      LIBMTP_Dump_Errorstack(connection->device);
    }
    return success;
  }

  bool remove(const QString& path) {
    const auto cleaned = cleanDevicePath(path);
    if (!cleaned || cleaned->isEmpty()) {
      return false;
    }
    // other MTP clients may have changed the folder since it was listed
    const auto entry = retryLookup([&]() { return fileOnce(*cleaned, true); });
    return entry && deleteObject(entry->identifier, folderOf(*cleaned));
  }

  /**
   * @brief Send a file to a name not taken yet
   *
   * Updating a project removes its file first, so a taken name belongs to another file, which is kept.
   * @return An empty string on success, else a user readable reason
   */
  QString upload(const QString& path, int descriptor, quint64 bytes) {
    const auto cleaned = cleanDevicePath(path);
    if (!cleaned || cleaned->isEmpty() || cancelled()) {
      return CDeviceAccessLibMtp::tr("The MTP session is not available.");
    }
    const QString folder = folderOf(*cleaned);
    const QString filename = QFileInfo(*cleaned).fileName();
    // other MTP clients may have changed the folder since it was listed
    const auto parent = retryLookup([&]() { return folderOnce(folder, true); });
    if (!parent) {
      return CDeviceAccessLibMtp::tr("The folder Garmin/%1 does not exist or can not be read.").arg(folder);
    }
    for (const Entry& entry : parent->entries) {
      if (entry.name.compare(filename, Qt::CaseInsensitive) == 0) {
        return CDeviceAccessLibMtp::tr("Another file Garmin/%1 is already on the device.").arg(*cleaned);
      }
    }

    file_ptr metadata(LIBMTP_new_file_t(), LIBMTP_destroy_file_t);
    if (!metadata || (metadata->filename = strdup(filename.toUtf8().constData())) == nullptr) {
      return CDeviceAccessLibMtp::tr("Could not prepare the upload.");
    }
    metadata->filesize = bytes;
    metadata->filetype = LIBMTP_FILETYPE_UNKNOWN;
    metadata->parent_id = parent->identifier;
    metadata->storage_id = id;
    LIBMTP_Clear_Errorstack(connection->device);
    const int result = LIBMTP_Send_File_From_File_Descriptor(connection->device, descriptor, metadata.get(),
                                                             reportTransfer, connection);
    listings.remove(folder);
    if (result != 0) {
      LIBMTP_Dump_Errorstack(connection->device);
      // a failed or aborted transfer may leave an incomplete file behind
      if (metadata->item_id != 0) {
        LIBMTP_Delete_Object(connection->device, metadata->item_id);
      }
      return CDeviceAccessLibMtp::tr("The transfer failed or was cancelled.");
    }
    return {};
  }
};

CDeviceAccessLibMtp::CDeviceAccessLibMtp(const std::shared_ptr<Connection>& connection,
                                         std::unique_ptr<Storage> storage, const QString& key)
    : IDeviceAccess(nullptr), connection(connection), storage(std::move(storage)), key(key) {}

CDeviceAccessLibMtp::~CDeviceAccessLibMtp() = default;

std::optional<QList<CDeviceAccessLibMtp*>> CDeviceAccessLibMtp::openDevice(const DeviceInfo& info) {
  initializeLibMtp();
  auto connection = std::make_shared<Connection>();
  std::vector<std::unique_ptr<Storage>> storages;
  bool opened = false;
  bool hasDeviceXml = false;
  connection->run([&]() {
    if (connection->isCancelled()) {
      return;
    }
    LIBMTP_raw_device_t raw{};
    raw.device_entry.vendor_id = info.vendor;
    raw.device_entry.product_id = info.product;
    raw.bus_location = info.bus;
    raw.devnum = info.address;
    bool known = false;
    LIBMTP_device_entry_t* table = nullptr;
    int count = 0;
    if (LIBMTP_Get_Supported_Devices_List(&table, &count) == 0) {
      for (int index = 0; index < count && !known; ++index) {
        if (table[index].vendor_id == info.vendor && table[index].product_id == info.product) {
          raw.device_entry.device_flags = table[index].device_flags;
          known = true;
        }
      }
    }
    // probes this device only, unlike LIBMTP_Detect_Raw_Devices()
    if (!known && LIBMTP_Check_Specific_Device(info.bus, info.address) == 0) {
      return;
    }
    connection->device = LIBMTP_Open_Raw_Device_Uncached(&raw);
    // a device still mounting its storage reports none
    if (connection->device == nullptr || connection->device->storage == nullptr) {
      return;
    }
    for (LIBMTP_devicestorage_t* candidate = connection->device->storage; candidate != nullptr;
         candidate = candidate->next) {
      auto storage =
          std::make_unique<Storage>(connection.get(), candidate->id, QString::fromUtf8(candidate->StorageDescription));
      const auto topLevel = storage->entries(LIBMTP_FILES_AND_FOLDERS_ROOT);
      if (!topLevel) {
        return;
      }
      for (const Storage::Entry& entry : *topLevel) {
        if (!entry.folder || entry.name.compare("Garmin", Qt::CaseInsensitive) != 0) {
          continue;
        }
        const auto children = storage->entries(entry.identifier);
        if (!children) {
          return;
        }
        for (const Storage::Entry& child : *children) {
          hasDeviceXml |= !child.folder && child.name.compare("GarminDevice.xml", Qt::CaseInsensitive) == 0;
        }
        storage->root = entry.identifier;
        storage->listings.insert({}, *children);
        storages.push_back(std::move(storage));
        break;
      }
    }
    opened = true;
  });
  if (!opened) {
    return std::nullopt;
  }
  // like on Linux, a device with GarminDevice.xml on one storage gets all its storages with a Garmin folder
  QList<CDeviceAccessLibMtp*> accesses;
  if (hasDeviceXml) {
    for (auto& storage : storages) {
      const QString key = QString("%1:%2").arg(info.key).arg(storage->id);
      accesses.append(new CDeviceAccessLibMtp(connection, std::move(storage), key));
    }
  }
  return accesses;
}

bool CDeviceAccessLibMtp::isBusy() const { return connection->busy.load() > 0; }

bool CDeviceAccessLibMtp::foundValidStoragePath() const { return storage->root != 0; }

QPixmap CDeviceAccessLibMtp::getIcon(const QString& iconPath) {
  QTemporaryFile file;
  QPixmap pixmap;
  if (readFileFromStorage(iconPath, file) && file.open()) {
    pixmap.loadFromData(file.readAll());
  }
  return pixmap;
}

QString CDeviceAccessLibMtp::decription() { return storage->description; }

bool CDeviceAccessLibMtp::readFileFromStorage(const QString& path, QFile& file) {
  const CBusy busy(connection->busy);
  if ((!file.isOpen() && !file.open(QIODevice::WriteOnly)) || !file.seek(0) || !file.resize(0)) {
    file.close();
    return false;
  }
  bool success = false;
  const int descriptor = file.handle();
  connection->run([this, path, descriptor, &success]() { success = storage->read(path, descriptor); });
  file.close();
  return success;
}

bool CDeviceAccessLibMtp::sendFileToStorage(const QString& path, QFile& file) {
  const CBusy busy(connection->busy);
  QString error;
  if ((!file.isOpen() && !file.open(QIODevice::ReadOnly)) || (file.isWritable() && !file.flush()) || !file.seek(0)) {
    error = tr("Could not read the local file.");
  } else {
    const int descriptor = file.handle();
    const quint64 bytes = file.size();
    connection->run([this, path, descriptor, bytes, &error]() { error = storage->upload(path, descriptor, bytes); });
  }
  file.close();
  if (!error.isEmpty()) {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Send to Devices..."),
                         tr("Failed to send file \"%1\" to device.").arg(path) + "\n\n" + error, QMessageBox::Ok);
    return false;
  }
  return true;
}

bool CDeviceAccessLibMtp::removeFileFromStorage(const QString& path) {
  const CBusy busy(connection->busy);
  bool success = false;
  connection->run([this, path, &success]() { success = storage->remove(path); });
  if (!success) {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Delete from Devices..."),
                         tr("Failed to delete file \"%1\" from device.").arg(path), QMessageBox::Ok);
  }
  return success;
}

QStringList CDeviceAccessLibMtp::listEntries(const QString& path, bool folders) {
  const CBusy busy(connection->busy);
  QStringList names;
  connection->run([this, path, folders, &names]() {
    const auto entries = storage->list(path);
    for (const auto& entry : entries.value_or(QList<Storage::Entry>())) {
      if (entry.folder == folders) {
        names.append(entry.name);
      }
    }
  });
  names.sort(Qt::CaseInsensitive);
  return names;
}

QStringList CDeviceAccessLibMtp::listFilesOnStorage(const QString& path) { return listEntries(path, false); }

QStringList CDeviceAccessLibMtp::listDirsOnStorage(const QString& path) { return listEntries(path, true); }
