/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CFOLDERWATCHER_H
#define CFOLDERWATCHER_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QStringList>
#include <QTimer>

/**
   @brief Emits sigChanged() once the files matching the filters changed and all files in the folders stayed unchanged
          for one poll; silent while a modal window or a progress dialog is open. Missing folders are checked until
          they appear, e.g. on a disk mounted later. A mount point itself is an existing empty folder while
          unmounted, so mounting it is not noticed: configure a folder on the disk.
 */
class CFolderWatcher : public QObject {
  Q_OBJECT
 public:
  explicit CFolderWatcher(const QStringList& nameFilters, QObject* parent = nullptr);

  /** @brief Watch these folders and take their files as seen; missing folders are watched once they appear */
  void setPaths(const QStringList& paths);

 signals:
  void sigChanged();

 private slots:
  void slotDirectoryChanged();
  void slotPoll();
  void slotCheckMissing();

 private:
  /** @return path, size and modification time of each file matching @p filters, all files if empty */
  QStringList snapshot(const QStringList& filters) const;
  /** @brief Watch the existing folders of #configured, check the missing ones by timer; @return true if the watched
   * folders changed */
  bool updateWatch();

  QFileSystemWatcher watcher;
  QTimer timerPoll;
  QTimer timerMissing;
  const QStringList nameFilters;
  QStringList configured; /**< the folders as given to setPaths() */
  QStringList paths;      /**< the folders of #configured that exist and are watched */
  QStringList known;      /**< the matching files last reported or seen by setPaths() */
  QStringList polled;     /**< all files at the last poll */
};

#endif  // CFOLDERWATCHER_H
