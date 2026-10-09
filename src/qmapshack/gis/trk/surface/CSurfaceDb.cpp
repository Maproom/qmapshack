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

#include "gis/trk/surface/CSurfaceDb.h"

#include <QSqlError>
#include <QTimeZone>
#include <QUuid>
#include <QVarLengthArray>
#include <QtEndian>
#include <QtMath>
#include <cmath>

CSurfaceDb::CSurfaceDb(const QString& filename) : file(filename) {
  connection = "surface-" + QUuid::createUuid().toString(QUuid::WithoutBraces);
  {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection);
    db.setDatabaseName(filename);
    db.setConnectOptions("QSQLITE_OPEN_READONLY");
    if (!db.open()) {
      error = db.lastError().text();
      return;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT key, value FROM meta")) {
      error = QString("Not a surface database: %1").arg(query.lastError().text());
      return;
    }
    while (query.next()) {
      meta[query.value(0).toString()] = query.value(1).toString();
    }
    if (meta.value("schema").toInt() != kSchemaVersion) {
      error = QString("Unsupported surface database version %1").arg(meta.value("schema"));
      return;
    }

    bbox = QRectF(QPointF(meta.value("minLon").toDouble(), meta.value("minLat").toDouble()),
                  QPointF(meta.value("maxLon").toDouble(), meta.value("maxLat").toDouble()));

    if (!query.exec("SELECT id, highway, surface, tracktype, footway FROM tags")) {
      error = query.lastError().text();
      return;
    }
    QHash<QString, qint32> keys;
    while (query.next()) {
      const qint32 id = query.value(0).toInt();
      const CSurface::tags_t tags = {query.value(1).toString(), query.value(2).toString(), query.value(3).toString(),
                                     query.value(4).toString()};
      const CSurface::info_t info = CSurface::classify(tags);
      infoByTag[id] = info;

      // unknown highway values stay apart, as in the reference implementation
      const QString key = QString("%1|%2|%3|%4")
                              .arg(info.label)
                              .arg(info.cls)
                              .arg(info.way)
                              .arg(info.way == CSurface::eWayOther ? tags.highway : QString());
      if (!keys.contains(key)) {
        keys[key] = infoByKey.size();
        infoByKey << info;
      }
      keyByTag[id] = keys[key];
    }

    queryChunks = QSqlQuery(db);
    queryChunks.setForwardOnly(true);
    if (!queryChunks.prepare("SELECT c.tag, c.nodes FROM chunk_index AS i JOIN chunks AS c ON c.id = i.id "
                             "WHERE i.maxLat >= ? AND i.minLat <= ? AND i.maxLon >= ? AND i.minLon <= ? "
                             "ORDER BY c.id")) {
      error = queryChunks.lastError().text();
      return;
    }
  }
  valid = true;
}

CSurfaceDb::~CSurfaceDb() {
  queryChunks = QSqlQuery();
  {
    QSqlDatabase db = QSqlDatabase::database(connection, false);
    db.close();
  }
  QSqlDatabase::removeDatabase(connection);
}

QDateTime CSurfaceDb::osmTimestamp() const {
  const qint64 t = meta.value("osmTimestamp").toLongLong();
  return t > 0 ? QDateTime::fromSecsSinceEpoch(t, QTimeZone::UTC) : QDateTime();
}

QDateTime CSurfaceDb::created() const { return QDateTime::fromString(meta.value("created"), Qt::ISODate); }

qint64 CSurfaceDb::chunkCount() const { return meta.value("chunks").toLongLong(); }

qreal CSurfaceDb::distance(qreal lat1, qreal lon1, qreal lat2, qreal lon2) {
  constexpr qreal kDegToRad = M_PI / 180.0;
  const qreal la1 = lat1 * kDegToRad;
  const qreal lo1 = lon1 * kDegToRad;
  const qreal la2 = lat2 * kDegToRad;
  const qreal lo2 = lon2 * kDegToRad;
  const qreal h =
      std::pow(std::sin((la2 - la1) / 2), 2) + std::cos(la1) * std::cos(la2) * std::pow(std::sin((lo2 - lo1) / 2), 2);
  return 2 * 6371000.0 * std::asin(std::sqrt(h));
}

void CSurfaceDb::cell(qreal lat, qreal lon, qint64& row, qint64& col) {
  // nearbyint() rounds half to even, like Python's round()
  row = qint64(std::nearbyint(lat * kCellsPerDegLat));
  col = qint64(std::nearbyint(lon * kCellsPerDegLon));
}

bool CSurfaceDb::query(qreal south, qreal north, qreal west, qreal east, QVector<chunk_t>& chunks) {
  chunks.clear();
  queryChunks.bindValue(0, south);
  queryChunks.bindValue(1, north);
  queryChunks.bindValue(2, west);
  queryChunks.bindValue(3, east);
  if (!queryChunks.exec()) {
    error = queryChunks.lastError().text();
    return false;
  }
  while (queryChunks.next()) {
    chunk_t chunk;
    chunk.tag = queryChunks.value(0).toInt();
    const QByteArray blob = queryChunks.value(1).toByteArray();
    const qsizetype n = blob.size() / sizeof(qint32);
    chunk.nodes.resize(n);
    for (qsizetype i = 0; i < n; i++) {
      chunk.nodes[i] = qFromLittleEndian<qint32>(blob.constData() + i * sizeof(qint32));
    }
    chunks << chunk;
  }
  queryChunks.finish();
  return true;
}

std::optional<CSurface::info_t> CSurfaceDb::lookupCell(qreal lat, qreal lon) {
  if (!valid) {
    return std::nullopt;
  }

  qint64 row, col;
  cell(lat, lon, row, col);
  const QPair<qint64, qint64> id(row, col);
  auto cached = cacheCells.constFind(id);
  if (cached != cacheCells.constEnd()) {
    return *cached < 0 ? std::nullopt : std::optional<CSurface::info_t>(infoByKey[*cached]);
  }

  constexpr qreal kMargin = 1e-6;
  const qreal south = (row - 0.5) / kCellsPerDegLat - kMargin;
  const qreal north = (row + 0.5) / kCellsPerDegLat + kMargin;
  const qreal west = (col - 0.5) / kCellsPerDegLon - kMargin;
  const qreal east = (col + 0.5) / kCellsPerDegLon + kMargin;

  QVector<chunk_t> chunks;
  if (!query(south, north, west, east, chunks)) {
    return std::nullopt;
  }

  // key and sample count, in order of the first sample
  QVarLengthArray<QPair<qint32, qint32>, 8> counts;
  for (const chunk_t& chunk : std::as_const(chunks)) {
    const qint32 key = keyByTag.value(chunk.tag, -1);
    if (key < 0) {
      continue;
    }

    const qsizetype n = chunk.nodes.size() / 2;
    for (qsizetype j = 0; j + 1 < n; j++) {
      const qreal lat1 = chunk.nodes[2 * j] / 1e7;
      const qreal lon1 = chunk.nodes[2 * j + 1] / 1e7;
      const qreal lat2 = chunk.nodes[2 * j + 2] / 1e7;
      const qreal lon2 = chunk.nodes[2 * j + 3] / 1e7;

      if (qMax(lat1, lat2) < south || qMin(lat1, lat2) > north || qMax(lon1, lon2) < west || qMin(lon1, lon2) > east) {
        continue;
      }

      // the reference samples every ~10 m, both end points included
      const qint32 steps = qMax(1, qint32(distance(lat1, lon1, lat2, lon2) / 10));
      for (qint32 i = 0; i <= steps; i++) {
        qint64 r, c;
        cell(lat1 + (lat2 - lat1) * i / steps, lon1 + (lon2 - lon1) * i / steps, r, c);
        if (r != row || c != col) {
          continue;
        }
        bool found = false;
        for (QPair<qint32, qint32>& count : counts) {
          if (count.first == key) {
            count.second++;
            found = true;
            break;
          }
        }
        if (!found) {
          counts.append({key, 1});
        }
      }
    }
  }

  qint32 best = -1;
  qint32 bestCount = 0;
  for (const QPair<qint32, qint32>& count : counts) {
    if (count.second > bestCount) {
      best = count.first;
      bestCount = count.second;
    }
  }

  cacheCells[id] = best;
  return best < 0 ? std::nullopt : std::optional<CSurface::info_t>(infoByKey[best]);
}

std::optional<CSurface::info_t> CSurfaceDb::lookupNearest(qreal lat, qreal lon, qreal maxDist) {
  if (!valid || maxDist <= 0) {
    return std::nullopt;
  }

  // local equirectangular projection in meters around the position
  constexpr qreal kMPerDeg = 6371000.0 * M_PI / 180.0;
  const qreal mPerDegLon = kMPerDeg * qCos(qDegreesToRadians(lat));
  const qreal dLat = maxDist / kMPerDeg * 1.01;
  const qreal dLon = maxDist / mPerDegLon * 1.01;

  QVector<chunk_t> chunks;
  if (!query(lat - dLat, lat + dLat, lon - dLon, lon + dLon, chunks)) {
    return std::nullopt;
  }

  qreal bestDist = maxDist;
  qint32 bestTag = -1;
  for (const chunk_t& chunk : std::as_const(chunks)) {
    const qsizetype n = chunk.nodes.size() / 2;
    for (qsizetype j = 0; j + 1 < n; j++) {
      const qreal x1 = (chunk.nodes[2 * j + 1] / 1e7 - lon) * mPerDegLon;
      const qreal y1 = (chunk.nodes[2 * j] / 1e7 - lat) * kMPerDeg;
      const qreal x2 = (chunk.nodes[2 * j + 3] / 1e7 - lon) * mPerDegLon;
      const qreal y2 = (chunk.nodes[2 * j + 2] / 1e7 - lat) * kMPerDeg;

      const qreal dx = x2 - x1;
      const qreal dy = y2 - y1;
      const qreal len2 = dx * dx + dy * dy;
      qreal t = len2 > 0 ? -(x1 * dx + y1 * dy) / len2 : 0;
      t = qBound(0.0, t, 1.0);
      const qreal d = std::hypot(x1 + t * dx, y1 + t * dy);
      if (d <= bestDist && (bestTag < 0 || d < bestDist)) {
        bestDist = d;
        bestTag = chunk.tag;
      }
    }
  }

  if (bestTag < 0 || !infoByTag.contains(bestTag)) {
    return std::nullopt;
  }
  return infoByTag[bestTag];
}
