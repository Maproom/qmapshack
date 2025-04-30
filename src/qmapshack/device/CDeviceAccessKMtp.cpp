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

#include "CDeviceAccessKMtp.h"

#include <QPixmap>

CDeviceAccessKMtp::CDeviceAccessKMtp(const QDBusObjectPath& objectPathStorage, QObject* parent)
    : IDeviceAccess(parent) {
  storage = new org::kde::kmtp::Storage("org.kde.kiod6", objectPathStorage.path(), QDBusConnection::sessionBus(), this);

  // Find the "GARMIN" or "Garmin" folder
  const KMTPFileList topLevelFiles = storage->getFilesAndFolders("/");
  for (const KMTPFile& file : topLevelFiles) {
    if (file.isFolder() && (file.filename().toUpper() == "GARMIN")) {
      dir.setPath("/" + file.filename());
    }
  }
}

QPixmap CDeviceAccessKMtp::getIcon() {
  QPixmap pixmap;
  QTemporaryFile icon;
  if (readFileFromStorage(dir.filePath("Garmintriangletm.ico"), icon)) {
    icon.open();
    pixmap.loadFromData(icon.readAll());
  }
  return pixmap;
}

QString CDeviceAccessKMtp::decription() { return storage->description(); }

bool CDeviceAccessKMtp::readFileFromStorage(const QString& path, QFile& file) {
  if (file.isOpen()) {
    file.seek(0);
  } else {
    file.open(QIODevice::WriteOnly);
  }

  const QString& _path = dir.filePath(path);

  QDBusUnixFileDescriptor descriptor(file.handle());
  if (waitForCopyOperation(storage, [descriptor, _path, this]() {
        return storage->getFileToFileDescriptor(descriptor, _path).value();
      })) {
    qWarning() << "Failed to read file" << _path;
    return false;
  }
  file.close();
  return true;
}

bool CDeviceAccessKMtp::sendFileToStorage(const QString& path, QFile& file) {
  if (file.isOpen()) {
    file.seek(0);
  } else {
    file.open(QIODevice::ReadOnly);
  }

  const QString& _path = dir.filePath(path);

  QDBusUnixFileDescriptor descriptor(file.handle());
  if (waitForCopyOperation(storage, [descriptor, _path, this]() {
        return storage->sendFileFromFileDescriptor(descriptor, _path).value();
      })) {
    qWarning() << "Failed to send file" << _path;
    return false;
  }
  file.close();
  return true;
}

bool CDeviceAccessKMtp::removeFileFromStorage(const QString& path) {
  const QString& _path = dir.filePath(path);
  return storage->deleteObject(_path) == 0;
}

QStringList CDeviceAccessKMtp::listFilesOnStorage(const QString& path) {
  QStringList result;
  const KMTPFileList& files = storage->getFilesAndFolders(dir.filePath(path)).value();
  for (const KMTPFile& file : files) {
    if (!file.isFolder()) {
      result << file.filename();
    }
  }
  return result;
}

int CDeviceAccessKMtp::waitForCopyOperation(const org::kde::kmtp::Storage* storage, fn_operation operation) {
  QEventLoop loop;
  connect(storage, &org::kde::kmtp::Storage::copyProgress, &loop,
          [](qulonglong sent, qulonglong total) { qDebug() << "processed size:" << sent << "of" << total; });
  connect(storage, &org::kde::kmtp::Storage::copyFinished, &loop, &QEventLoop::exit);

  if (operation() == 0) {
    return loop.exec();
  } else {
    return -1;
  }
}
