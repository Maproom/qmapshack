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

#include "device/CDeviceAccessGvfsMtp.h"

#include <QMessageBox>
#include <QPixmap>

#include "CMainWindow.h"

CDeviceAccessGvfsMtp::CDeviceAccessGvfsMtp(const GVFSMount& mount, const QString& storagePath, QObject* parent)
    : IDeviceAccess(parent), _description(storagePath) {
  storage = new org::gtk::vfs::Mount(mount.dbusId, mount.objectPath.path(), QDBusConnection::sessionBus(), this);

  const QDir& d = QDir(mount.fuseMountPoint.constData()).filePath(storagePath);

  // Find the "GARMIN" or "Garmin" folder
  const QStringList& topLevelDirectories = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString& directory : topLevelDirectories) {
    if (directory.toUpper() == "GARMIN") {
      dir.setPath(d.filePath(directory));
      pathOnDevice.setPath("/" + QDir(storagePath).filePath(directory));
    }
  }
}

QPixmap CDeviceAccessGvfsMtp::getIcon() {
  QPixmap pixmap;
  if (dir.exists("Garmintriangletm.ico")) {
    pixmap.load(dir.filePath("Garmintriangletm.ico"));
  }
  return pixmap;
}

QString CDeviceAccessGvfsMtp::decription() { return _description; }

bool CDeviceAccessGvfsMtp::readFileFromStorage(const QString& path, QFile& file) {
  try {
    if (!dir.exists(path)) {
      throw QString("File does not exist: %1").arg(dir.filePath(path));
    }

    QFile f(dir.filePath(path));
    if (!f.open(QIODevice::ReadOnly)) {
      throw QString("Failed to open %1").arg(dir.filePath(path));
    }

    if (file.isOpen()) {
      file.seek(0);
    } else {
      file.open(QIODevice::WriteOnly);
    }

    file.write(f.readAll());

  } catch (const QString& msg) {
    qWarning() << msg;
    file.close();
    return false;
  }

  file.close();
  return true;
}

bool CDeviceAccessGvfsMtp::sendFileToStorage(const QString& path, QFile& file) {
  file.flush();
  file.close();
  QByteArray source = file.fileName().toLocal8Bit();
  QByteArray target = pathOnDevice.filePath(path).toLocal8Bit();
  source.append('\0');
  target.append('\0');
  auto res = storage->Push(target, source, false, 0, QDBusObjectPath("/"), false);
  res.waitForFinished();
  if (res.isError()) {
    QMessageBox::warning(
        CMainWindow::getBestWidgetForParent(), tr("Send to Devices..."),
        tr("Failed to send file \"%1\" to device. Reason: %2").arg(target.constData(), res.reply().errorMessage()),
        QMessageBox::Ok);
  }
  return !res.isError();
}

bool CDeviceAccessGvfsMtp::removeFileFromStorage(const QString& path) {
  QByteArray target = pathOnDevice.filePath(path).toLocal8Bit();
  target.append('\0');
  auto res = storage->Delete(target);
  res.waitForFinished();
  if (res.isError()) {
    QMessageBox::warning(
        CMainWindow::getBestWidgetForParent(), tr("Delete from Devices..."),
        tr("Failed to delete file \"%1\" from device. Reason: %2").arg(target.constData(), res.reply().errorMessage()),
        QMessageBox::Ok);
  }
  return !res.isError();
}

QStringList CDeviceAccessGvfsMtp::listFilesOnStorage(const QString& path) {
  QDir d(dir.filePath(path));
  const QStringList& result = d.entryList(QDir::Files);
  return result;
}

QStringList CDeviceAccessGvfsMtp::listDirsOnStorage(const QString& path) {
  QDir d(dir.filePath(path));
  const QStringList& result = d.entryList(QDir::Dirs);
  return result;
}
