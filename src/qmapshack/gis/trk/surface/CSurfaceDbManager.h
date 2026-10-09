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

/**
   @brief Knows the surface databases (see CSurfaceDb) in the configured directory

   All `*.surface.sqlite` files in the directory are used unless the user switched them off.
   The default directory is "Surface" next to the first Routino database path, or in the user
   data directory if no Routino path is set up.
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
    QString error;  ///< not empty if the file can't be used
    QRectF boundingBox;
    QDateTime osmTimestamp;
    QDateTime created;
    qint64 chunks = 0;
    bool active = true;
  };

  QString getPath() const { return path; }
  void setPath(const QString& path);

  const QList<db_t>& getDatabases() const { return databases; }
  /// usable and active database files
  QStringList getActiveFiles() const;
  /// true if an active database's area intersects the rectangle (x = longitude, y = latitude)
  bool covers(const QRectF& area) const;
  void setActive(const QString& file, bool yes);

  /// changes whenever the set of usable databases changes
  quint32 getGeneration() const { return generation; }

  /// the directory used if the user did not choose one
  static QString defaultPath();

 public slots:
  /// read the directory again, emits sigChanged() if anything changed
  void rescan();

 signals:
  void sigChanged();

 private:
  CSurfaceDbManager();
  void save();

  QString path;
  QStringList inactive;
  QList<db_t> databases;
  quint32 generation = 1;
};

#endif  // CSURFACEDBMANAGER_H
