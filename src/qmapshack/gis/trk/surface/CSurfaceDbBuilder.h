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

#ifndef CSURFACEDBBUILDER_H
#define CSURFACEDBBUILDER_H

#include <QCoreApplication>
#include <QRectF>
#include <QString>
#include <atomic>
#include <functional>

/**
   @brief Build a surface database (see CSurfaceDb) from an OpenStreetMap .osm.pbf extract

   Reads the file twice: first all ways with a highway tag, then the locations of their nodes.
   Memory use is roughly 20 bytes per node of a highway (300 MB for Belgium). Runs in the
   calling thread; call it from a worker thread and pass a cancel flag.
 */
class CSurfaceDbBuilder {
  Q_DECLARE_TR_FUNCTIONS(CSurfaceDbBuilder)
 public:
  struct options_t {
    QString pbf;     ///< source file
    QString target;  ///< database file to create (replaced if it exists)
    QString name;    ///< human readable name of the region
    QRectF area;     ///< optional: only ways with a node in this area (x = longitude, y = latitude)
    qint32 chunkSegments = 16;
  };

  struct stats_t {
    qint64 ways = 0;
    qint64 nodes = 0;
    qint64 chunks = 0;
    qint64 tags = 0;
  };

  /// fraction 0..1 and a short description of the current step
  using fProgress = std::function<void(qreal fraction, const QString& step)>;

  /// the file extension of surface databases
  static QString suffix() { return "surface.sqlite"; }

  explicit CSurfaceDbBuilder(const options_t& options);
  ~CSurfaceDbBuilder() = default;

  /// @return false on error or cancel, see errorString()
  bool build(const fProgress& progress, const std::atomic_bool* cancel = nullptr);

  QString errorString() const { return error; }
  const stats_t& statistics() const { return stats; }

 private:
  options_t opts;
  QString error;
  stats_t stats;
};

#endif  // CSURFACEDBBUILDER_H
