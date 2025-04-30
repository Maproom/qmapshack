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

class IDeviceAccess : public QObject {
  Q_OBJECT
 public:
  IDeviceAccess(QObject* parent);
  virtual ~IDeviceAccess() = default;
  virtual bool foundValidStoragePath() const { return !dir.dirName().isEmpty(); }
  virtual QPixmap getIcon() = 0;
  virtual QString decription() = 0;
  virtual bool readFileFromStorage(const QString& path, QFile& file) = 0;
  virtual bool sendFileToStorage(const QString& path, QFile& file) = 0;
  virtual bool removeFileFromStorage(const QString& path) = 0;
  virtual QStringList listFilesOnStorage(const QString& path) = 0;

 protected:
  QDir dir;
};

#endif  // IDEVICEACCESS_H
