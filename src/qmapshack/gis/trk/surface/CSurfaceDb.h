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

#include <QCoreApplication>
#include <QDateTime>
#include <QHash>
#include <QRectF>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <memory>
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
   created in. What is the same for every object of a file, the classified tags and the cells
   looked up so far, is shared between all objects and threads: opening the file again (e.g. for
   each analysis job) is cheap, and lookups already done are not repeated. That data is kept
   until releaseShared() drops it.
 */
class CSurfaceDb {
  Q_DECLARE_TR_FUNCTIONS(CSurfaceDb)

 public:
  static constexpr qint32 kSchemaVersion = 1;
  /** @brief edge length of a lookup cell in degrees latitude (~20 m) */
  static constexpr qreal kCellsPerDegLat = 5500;
  /** @brief edge length of a lookup cell in degrees longitude (~20 m at 50 deg N) */
  static constexpr qreal kCellsPerDegLon = 3500;

  enum mode_e {
    eModeLookup, /**< read everything needed for lookups */
    eModeMeta    /**< read the meta table only: name, area, ... are known, lookups find nothing */
  };

  explicit CSurfaceDb(const QString& filename, mode_e mode = eModeLookup);
  ~CSurfaceDb();

  bool isValid() const { return valid; }
  QString errorString() const { return error; }
  QString filename() const { return file; }
  QString name() const { return meta.value("name"); }
  QString source() const { return meta.value("source"); }
  QDateTime osmTimestamp() const;
  QDateTime created() const;
  qint64 chunkCount() const;
  /** @brief the area covered by the data, x = longitude, y = latitude */
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

  /** @brief a bounding box (x = longitude, y = latitude) as text, e.g. "49.50 N..51.51 N, 2.54 E..6.41 E" */
  static QString areaText(const QRectF& boundingBox);
  /** @brief true if an area (x = longitude, y = latitude) touches a database's bounding box */
  static bool overlaps(const QRectF& boundingBox, const QRectF& area);
  /** @brief great circle distance in meters, the same formula as the reference implementation */
  static qreal distance(qreal lat1, qreal lon1, qreal lat2, qreal lon2);
  /** @brief the cell a position falls into */
  static void cell(qreal lat, qreal lon, qint64& row, qint64& col);

  /** @brief the most cells kept per file before the cache starts over */
  static constexpr qsizetype kMaxCachedCells = 200000;

  /**
     @brief Drop the shared data of all files but @p keep

     Objects still open keep theirs until they are deleted.

     @param keep  absolute file names
   */
  static void releaseShared(const QStringList& keep);
  /** @brief the number of files with shared data */
  static qsizetype sharedCount();

 private:
  struct chunk_t {
    qint32 tag;
    QVector<qint32> nodes; /**< lat, lon, lat, lon, ... in 1e-7 degrees */
  };
  bool query(qreal south, qreal north, qreal west, qreal east, QVector<chunk_t>& chunks);
  bool readTags(QSqlDatabase& db);

  /** @brief use the shared data of the file if it did not change on disk since, else read it */
  bool attachShared(QSqlDatabase& db, const QString& path, const QString& stamp);

  struct shared_t;
  struct cache_t;
  /** @brief the shared data of all files */
  static cache_t& cache();

  QString file;
  QString connection;
  bool valid = false;
  QString error;
  QHash<QString, QString> meta;
  QRectF bbox;

  std::shared_ptr<shared_t> shared;
  QSqlQuery queryChunks;
};

#endif  // CSURFACEDB_H
