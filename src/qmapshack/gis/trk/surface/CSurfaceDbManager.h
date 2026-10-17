/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#ifndef CSURFACEDBMANAGER_H
#define CSURFACEDBMANAGER_H

#include <QDateTime>
#include <QObject>
#include <QRectF>
#include <QStringList>
#include <QThreadPool>
#include <QTimer>

/**
   @brief Knows the surface databases (see CSurfaceDb) in the configured paths

   Like the paths for maps, DEM and Routino it is a list, so data can be on several disks. All
   `*.surface.sqlite` files in the paths are used unless the user switched them off; sub-paths
   are not parsed. On the first start the list is set to "Surface" next to the first Routino
   database path, or in the user data directory if no Routino path is set up, and saved.

   Lives in the GUI thread and must only be used there. Background jobs get a copy of
   getActiveFiles() and open the files themselves. They run in getPool(), which is waited for
   when the application quits, after sigQuit() asked the tracks to cancel their jobs.
 */
class CSurfaceDbManager : public QObject {
  Q_OBJECT
 public:
  static CSurfaceDbManager& self();
  ~CSurfaceDbManager() override = default;

  struct db_t {
    QString file;
    QString name;
    QString source;
    QString error; /**< not empty if the file can't be used */
    QRectF boundingBox;
    QDateTime osmTimestamp;
    QDateTime created;
    qint64 chunks = 0;
    bool active = true;
  };

  const QStringList& getPaths() const { return paths; }
  void setPaths(const QStringList& paths);

  /** @brief The databases for an area */
  struct coverage_t {
    QStringList files;        /**< usable and active databases whose area intersects it */
    bool switchedOff = false; /**< a usable database switched off intersects it */
    size_t key = 0;           /**< changes if anything above changes, or one of the files is rebuilt */
  };

  const QList<db_t>& getDatabases() const { return databases; }
  /** @brief usable and active database files */
  QStringList getActiveFiles() const;
  /** @brief the databases for a rectangle, x = longitude, y = latitude */
  coverage_t getCoverage(const QRectF& area) const;
  void setActive(const QString& file, bool yes);

  /** @brief the path set up on the first start, see the class description */
  static QString defaultPath();

  /** @brief The threads the surface analyses of the tracks run in */
  QThreadPool& getPool() { return pool; }

  /**
     @brief Redraw the tracks on the map soon

     Bundles the redraws of many tracks whose analysis finished, or whose surfaces changed with
     the databases, into one.
   */
  void triggerCanvasUpdate();

 public slots:
  /** @brief read the paths again, emits sigChanged() if anything changed */
  void rescan();

 signals:
  void sigChanged();
  /** @brief The application quits: running jobs must be canceled now */
  void sigQuit();

 private:
  CSurfaceDbManager();
  void save();
  /** @brief The canonical path of @p file, so the same file reached by another path is one entry */
  static QString keyOf(const QString& file);
  void slotAboutToQuit();

  QStringList paths;
  /** @brief Canonical paths of the databases switched off, see keyOf() */
  QStringList inactive;
  QList<db_t> databases;
  QThreadPool pool;
  QTimer timerCanvasUpdate;
};

#endif  // CSURFACEDBMANAGER_H
