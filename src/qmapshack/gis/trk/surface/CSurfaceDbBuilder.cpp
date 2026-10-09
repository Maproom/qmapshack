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

#include "gis/trk/surface/CSurfaceDbBuilder.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QtEndian>
#include <algorithm>
#include <limits>
#include <vector>

#include "gis/trk/surface/CPbfReader.h"
#include "gis/trk/surface/CSurfaceDb.h"

CSurfaceDbBuilder::CSurfaceDbBuilder(const options_t& options) : opts(options) {}

bool CSurfaceDbBuilder::build(const fProgress& progress, const std::atomic_bool* cancel) {
  stats = stats_t();
  error.clear();

  auto report = [&](qreal fraction, const QString& step) {
    if (progress) {
      progress(fraction, step);
    }
  };
  auto canceled = [&]() { return cancel != nullptr && cancel->load(); };

  CPbfReader reader(opts.pbf);
  if (!reader.open()) {
    error = reader.errorString();
    return false;
  }
  const qreal size = qMax<qreal>(1, reader.size());

  // ---- pass 1: ways with a highway tag ----
  QHash<QString, qint32> tagIds;
  QVector<QStringList> tags;
  std::vector<qint32> wayTag;
  std::vector<qint64> wayId;
  std::vector<qint64> wayStart;  // index into refs
  std::vector<qint64> refs;

  const QString step1 = tr("Reading ways...");
  report(0, step1);
  bool ok = reader.readData(
      CPbfReader::eWays, nullptr,
      [&](const CPbfReader::way_t& way, const QVector<QByteArray>& strings) {
        QString values[4];
        bool isHighway = false;
        for (qsizetype i = 0; i < way.keys.size(); i++) {
          const QByteArray& key = strings[way.keys[i]];
          qint32 idx = -1;
          if (key == "highway") {
            idx = 0;
            isHighway = true;
          } else if (key == "surface") {
            idx = 1;
          } else if (key == "tracktype") {
            idx = 2;
          } else if (key == "footway") {
            idx = 3;
          }
          if (idx >= 0) {
            values[idx] = QString::fromUtf8(strings[way.vals[i]]);
          }
        }
        if (!isHighway || way.refs.size() < 2) {
          return;
        }

        const QString key = values[0] + '\x1f' + values[1] + '\x1f' + values[2] + '\x1f' + values[3];
        auto it = tagIds.constFind(key);
        qint32 tagId;
        if (it == tagIds.constEnd()) {
          tagId = tags.size();
          tagIds.insert(key, tagId);
          tags << QStringList{values[0], values[1], values[2], values[3]};
        } else {
          tagId = *it;
        }

        wayId.push_back(way.id);
        wayTag.push_back(tagId);
        wayStart.push_back(qint64(refs.size()));
        refs.insert(refs.end(), way.refs.begin(), way.refs.end());
      },
      [&](qint64 pos) {
        report(0.4 * pos / size, step1);
        return !canceled();
      });
  if (!ok) {
    error = canceled() ? tr("Canceled.") : reader.errorString();
    return false;
  }
  wayStart.push_back(qint64(refs.size()));

  // ---- pass 2: locations of the nodes used by those ways ----
  const QString step2 = tr("Reading node locations...");
  report(0.4, step2);
  std::vector<qint64> needed(refs);
  std::sort(needed.begin(), needed.end());
  needed.erase(std::unique(needed.begin(), needed.end()), needed.end());
  constexpr qint32 kInvalid = std::numeric_limits<qint32>::min();
  std::vector<qint32> lats(needed.size(), kInvalid);
  std::vector<qint32> lons(needed.size(), kInvalid);

  size_t cursor = 0;
  ok = reader.readData(
      CPbfReader::eNodes,
      [&](qint64 id, qint32 lat, qint32 lon) {
        // nodes come sorted by id, so usually the cursor just moves on
        if (cursor >= needed.size() || needed[cursor] > id) {
          auto it = std::lower_bound(needed.begin(), needed.end(), id);
          cursor = size_t(it - needed.begin());
        } else if (needed[cursor] < id) {
          auto it = std::lower_bound(needed.begin() + cursor, needed.end(), id);
          cursor = size_t(it - needed.begin());
        }
        if (cursor < needed.size() && needed[cursor] == id) {
          lats[cursor] = lat;
          lons[cursor] = lon;
        }
      },
      nullptr,
      [&](qint64 pos) {
        report(0.4 + 0.4 * pos / size, step2);
        return !canceled();
      });
  if (!ok) {
    error = canceled() ? tr("Canceled.") : reader.errorString();
    return false;
  }

  // ---- write the database ----
  const QString step3 = tr("Writing database...");
  report(0.8, step3);

  const QString tmpFile = opts.target + ".part";
  QFile::remove(tmpFile);
  const QString connection = "surface-build-" + QUuid::createUuid().toString(QUuid::WithoutBraces);
  {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection);
    db.setDatabaseName(tmpFile);
    if (!db.open()) {
      error = db.lastError().text();
    } else {
      auto exec = [&](QSqlQuery& query, const QString& sql = QString()) {
        if (error.isEmpty() && !(sql.isEmpty() ? query.exec() : query.exec(sql))) {
          error = query.lastError().text();
        }
        return error.isEmpty();
      };

      QSqlQuery query(db);
      exec(query, "PRAGMA journal_mode = OFF");
      exec(query, "PRAGMA synchronous = OFF");
      exec(query, "PRAGMA page_size = 4096");
      exec(query, "CREATE TABLE meta (key TEXT PRIMARY KEY, value TEXT)");
      exec(query,
           "CREATE TABLE tags (id INTEGER PRIMARY KEY, highway TEXT, surface TEXT, tracktype TEXT, footway TEXT)");
      exec(query, "CREATE TABLE chunks (id INTEGER PRIMARY KEY, way INTEGER, tag INTEGER, nodes BLOB)");
      exec(query, "CREATE VIRTUAL TABLE chunk_index USING rtree(id, minLat, maxLat, minLon, maxLon)");
      db.transaction();

      QSqlQuery insertTag(db);
      insertTag.prepare("INSERT INTO tags (id, highway, surface, tracktype, footway) VALUES (?, ?, ?, ?, ?)");
      for (qint32 i = 0; i < tags.size() && error.isEmpty(); i++) {
        insertTag.bindValue(0, i);
        for (qint32 j = 0; j < 4; j++) {
          insertTag.bindValue(j + 1, tags[i][j]);
        }
        exec(insertTag);
      }
      stats.tags = tags.size();

      QSqlQuery insertChunk(db);
      insertChunk.prepare("INSERT INTO chunks (id, way, tag, nodes) VALUES (?, ?, ?, ?)");
      QSqlQuery insertIndex(db);
      insertIndex.prepare("INSERT INTO chunk_index (id, minLat, maxLat, minLon, maxLon) VALUES (?, ?, ?, ?, ?)");

      const bool filter = opts.area.isValid();
      qint32 minLat = std::numeric_limits<qint32>::max(), maxLat = kInvalid;
      qint32 minLon = std::numeric_limits<qint32>::max(), maxLon = kInvalid;
      std::vector<qint32> nodes;
      const size_t ways = wayId.size();
      for (size_t w = 0; w < ways && error.isEmpty(); w++) {
        if ((w & 0x3FFF) == 0) {
          if (canceled()) {
            error = tr("Canceled.");
            break;
          }
          report(0.8 + 0.2 * w / qMax<size_t>(1, ways), step3);
        }

        nodes.clear();
        bool inside = !filter;
        for (qint64 r = wayStart[w]; r < wayStart[w + 1]; r++) {
          const size_t idx = size_t(std::lower_bound(needed.begin(), needed.end(), refs[r]) - needed.begin());
          if (lats[idx] == kInvalid) {
            continue;  // node missing in the extract
          }
          nodes.push_back(lats[idx]);
          nodes.push_back(lons[idx]);
          if (!inside && opts.area.contains(QPointF(lons[idx] / 1e7, lats[idx] / 1e7))) {
            inside = true;
          }
        }
        const qint64 n = qint64(nodes.size() / 2);
        if (n < 2 || !inside) {
          continue;
        }
        stats.ways++;
        stats.nodes += n;

        // chunks share their boundary node, so sampling them one by one equals sampling the way
        for (qint64 start = 0; start < n - 1 && error.isEmpty(); start += opts.chunkSegments) {
          const qint64 end = qMin<qint64>(start + opts.chunkSegments, n - 1);
          QByteArray blob((end - start + 1) * 2 * sizeof(qint32), Qt::Uninitialized);
          qint32 cMinLat = std::numeric_limits<qint32>::max(), cMaxLat = kInvalid;
          qint32 cMinLon = std::numeric_limits<qint32>::max(), cMaxLon = kInvalid;
          for (qint64 k = start; k <= end; k++) {
            const qint32 lat = nodes[2 * k];
            const qint32 lon = nodes[2 * k + 1];
            qToLittleEndian<qint32>(lat, blob.data() + (k - start) * 2 * sizeof(qint32));
            qToLittleEndian<qint32>(lon, blob.data() + ((k - start) * 2 + 1) * sizeof(qint32));
            cMinLat = qMin(cMinLat, lat);
            cMaxLat = qMax(cMaxLat, lat);
            cMinLon = qMin(cMinLon, lon);
            cMaxLon = qMax(cMaxLon, lon);
          }
          minLat = qMin(minLat, cMinLat);
          maxLat = qMax(maxLat, cMaxLat);
          minLon = qMin(minLon, cMinLon);
          maxLon = qMax(maxLon, cMaxLon);

          // ids follow the OSM way order: the lookup breaks ties by them
          const qint64 id = ++stats.chunks;
          insertChunk.bindValue(0, id);
          insertChunk.bindValue(1, wayId[w]);
          insertChunk.bindValue(2, wayTag[w]);
          insertChunk.bindValue(3, blob);
          exec(insertChunk);

          insertIndex.bindValue(0, id);
          insertIndex.bindValue(1, cMinLat / 1e7);
          insertIndex.bindValue(2, cMaxLat / 1e7);
          insertIndex.bindValue(3, cMinLon / 1e7);
          insertIndex.bindValue(4, cMaxLon / 1e7);
          exec(insertIndex);
        }
      }

      if (error.isEmpty() && stats.chunks == 0) {
        error = tr("No roads or paths found in %1.").arg(QFileInfo(opts.pbf).fileName());
      }

      const CPbfReader::header_t& hdr = reader.header();
      QSqlQuery insertMeta(db);
      insertMeta.prepare("INSERT INTO meta (key, value) VALUES (?, ?)");
      const QList<QPair<QString, QString>> meta = {
          {"schema", QString::number(CSurfaceDb::kSchemaVersion)},
          {"name", opts.name},
          {"source", QFileInfo(opts.pbf).fileName()},
          {"osmTimestamp", QString::number(hdr.timestamp)},
          {"created", QDateTime::currentDateTimeUtc().toString(Qt::ISODate)},
          {"minLat", QString::number(minLat / 1e7, 'f', 7)},
          {"maxLat", QString::number(maxLat / 1e7, 'f', 7)},
          {"minLon", QString::number(minLon / 1e7, 'f', 7)},
          {"maxLon", QString::number(maxLon / 1e7, 'f', 7)},
          {"ways", QString::number(stats.ways)},
          {"chunks", QString::number(stats.chunks)},
      };
      for (const QPair<QString, QString>& item : meta) {
        insertMeta.bindValue(0, item.first);
        insertMeta.bindValue(1, item.second);
        exec(insertMeta);
      }

      if (error.isEmpty()) {
        db.commit();
      } else {
        db.rollback();
      }
      insertTag = QSqlQuery();
      insertChunk = QSqlQuery();
      insertIndex = QSqlQuery();
      insertMeta = QSqlQuery();
      query = QSqlQuery();
      db.close();
    }
  }
  QSqlDatabase::removeDatabase(connection);

  if (!error.isEmpty()) {
    QFile::remove(tmpFile);
    return false;
  }

  QFile::remove(opts.target);
  if (!QFile::rename(tmpFile, opts.target)) {
    error = tr("Failed to write %1.").arg(opts.target);
    QFile::remove(tmpFile);
    return false;
  }

  report(1.0, tr("Done."));
  return true;
}
