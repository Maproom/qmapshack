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

#ifndef CSURFACEDB_H
#define CSURFACEDB_H

#include <QDateTime>
#include <QHash>
#include <QRectF>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <optional>

#include "gis/trk/surface/CSurface.h"

/**
   @brief Read access to one surface database file

   A surface database is a SQLite file built from an OpenStreetMap extract by CSurfaceDbBuilder:

   - `meta(key, value)`: schema version, name, source file, OSM timestamp, bounding box
   - `tags(id, highway, surface, tracktype, footway)`: the distinct tag combinations
   - `chunks(id, way, tag, nodes)`: pieces of up to 16 segments of a highway, nodes as
     little endian int32 latitude/longitude pairs in 1e-7 degrees
   - `chunk_index`: an R*Tree over the chunks' bounding boxes

   The classification is done when reading, so a changed classification needs no rebuild.

   An object holds its own database connection and must only be used in the thread it was
   created in.
 */
class CSurfaceDb {
 public:
  static constexpr qint32 kSchemaVersion = 1;
  /// edge length of a lookup cell in degrees latitude (~20 m)
  static constexpr qreal kCellsPerDegLat = 5500;
  /// edge length of a lookup cell in degrees longitude (~20 m at 50 deg N)
  static constexpr qreal kCellsPerDegLon = 3500;

  explicit CSurfaceDb(const QString& filename);
  ~CSurfaceDb();

  bool isValid() const { return valid; }
  QString errorString() const { return error; }
  QString filename() const { return file; }
  QString name() const { return meta.value("name"); }
  QString source() const { return meta.value("source"); }
  QDateTime osmTimestamp() const;
  QDateTime created() const;
  qint64 chunkCount() const;
  /// the area covered by the data, x = longitude, y = latitude
  QRectF boundingBox() const { return bbox; }

  /**
     @brief The way at a position, as the majority of all ways passing its ~20 m cell

     Each way is sampled every ~10 m, the classification with the most samples in the cell
     wins. Equal counts go to the way with the lowest OSM id.

     @return nothing if no way passes the cell
   */
  std::optional<CSurface::info_t> lookupCell(qreal lat, qreal lon);

  /**
     @brief The way closest to a position
     @param maxDist  the maximum distance in meters
     @return nothing if there is no way within maxDist
   */
  std::optional<CSurface::info_t> lookupNearest(qreal lat, qreal lon, qreal maxDist);

  /// great circle distance in meters, the same formula as the reference implementation
  static qreal distance(qreal lat1, qreal lon1, qreal lat2, qreal lon2);
  /// the cell a position falls into
  static void cell(qreal lat, qreal lon, qint64& row, qint64& col);

 private:
  struct chunk_t {
    qint32 tag;
    QVector<qint32> nodes;  ///< lat, lon, lat, lon, ... in 1e-7 degrees
  };
  bool query(qreal south, qreal north, qreal west, qreal east, QVector<chunk_t>& chunks);

  QString file;
  QString connection;
  bool valid = false;
  QString error;
  QHash<QString, QString> meta;
  QRectF bbox;

  /// classification per tag id
  QHash<qint32, CSurface::info_t> infoByTag;
  /// majority key per tag id: ways with equal classification count together
  QHash<qint32, qint32> keyByTag;
  QVector<CSurface::info_t> infoByKey;

  QSqlQuery queryChunks;
  /// cell -> key index or -1
  QHash<QPair<qint64, qint64>, qint32> cacheCells;
};

#endif  // CSURFACEDB_H
