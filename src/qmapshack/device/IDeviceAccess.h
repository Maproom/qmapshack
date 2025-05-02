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

#ifndef IDEVICEACCESS_H
#define IDEVICEACCESS_H

#include <qdir.h>
#include <qobject.h>

/**
 * @brief The abstract (MTP) access for different systems (KDE, GTK,...)
 */
class IDeviceAccess : public QObject {
  Q_OBJECT
 public:
  IDeviceAccess(QObject* parent);
  virtual ~IDeviceAccess() = default;
  /**
   * @brief Return true if a vaild path has been found
   *
   * A valid path usually is a path that has a folder named Garmin.
   *
   */
  virtual bool foundValidStoragePath() const { return !dir.dirName().isEmpty(); }
  /**
   * @brief Get an icon from the storage if any
   * @return A valid pixmap ion success or an empty one if no icon has been found.
   */
  virtual QPixmap getIcon() = 0;
  /**
   * @brief A user readable description of the storage
   */
  virtual QString decription() = 0;
  /**
   * @brief Read a file from the storage
   * @param path    a path relative to the storage path
   * @param file    the file can be opened with write access or closed.
   * @return True on success.
   */
  virtual bool readFileFromStorage(const QString& path, QFile& file) = 0;
  /**
   * @brief Send a file to the storage
   * @param path    a path relative to the storage path
   * @param file    the file can be opened with read access or closed.
   * @return True on success.
   */
  virtual bool sendFileToStorage(const QString& path, QFile& file) = 0;
  /**
   * @brief Remove a file from the storage
   * @param path    a path relative to the storage path
   * @return
   */
  virtual bool removeFileFromStorage(const QString& path) = 0;
  /**
   * @brief List all files in a path on the storage
   * @param path    a path relative to the storage path
   * @return
   */
  virtual QStringList listFilesOnStorage(const QString& path) = 0;

 protected:
  QDir dir;
};

#endif  // IDEVICEACCESS_H
