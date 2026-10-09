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

#include <QFile>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTest>

#include "gis/trk/surface/CPbfReader.h"
#include "gis/trk/surface/CSurfaceDb.h"
#include "gis/trk/surface/CSurfaceDbBuilder.h"
#include "gis/trk/surface/CSurfaceDbSet.h"

/// PBF reading, database building and lookups on a tiny hand-made extract (data/make_fixture.py).
class test_CSurfaceDb : public QObject {
  Q_OBJECT
 private slots:
  void initTestCase();
  void readerNodesAndWays();
  void readerRejectsGarbage();
  void builderStatistics();
  void builderReportsProgressAndCancels();
  void builderArea();
  void builderNoHighways();
  void metaData();
  void cellLookupMatchesReference_data();
  void cellLookupMatchesReference();
  void chunkingDoesNotMatter();
  void nearestFallback();
  void dbSet();
  void invalidFiles();
  void distanceAndCells();

 private:
  QString build(const QString& name, qint32 chunkSegments = 16, const QRectF& area = QRectF());

  QTemporaryDir tmp;
  QString pbf;
  QString db;
};

// the classification, or "no data" if there is none
static CSurface::info_t infoOf(const std::optional<CSurface::info_t>& info) {
  return info.value_or(CSurface::noData());
}

static QString dutchLabel(const CSurface::info_t& info) {
  static const QHash<int, QString> names = {
      {CSurface::eLabelAsphalt, "asfalt"},
      {CSurface::eLabelSemiPaved, "halfverhard"},
      {CSurface::eLabelUnpavedAssumed, "onverhard (aangenomen)"},
      {CSurface::eLabelPavedAssumed, "verhard (aangenomen)"},
  };
  return names.value(info.label, "?");
}

static QString dutchWay(const CSurface::info_t& info) {
  static const QHash<int, QString> names = {
      {CSurface::eWayStreet, "straat"},       {CSurface::eWayTrack, "bos-/veldweg"}, {CSurface::eWayPath, "pad"},
      {CSurface::eWayBusyRoad, "drukke weg"}, {CSurface::eWayCycleway, "fietspad"},  {CSurface::eWaySteps, "trap"},
  };
  return names.value(info.way, "?");
}

QString test_CSurfaceDb::build(const QString& name, qint32 chunkSegments, const QRectF& area) {
  CSurfaceDbBuilder::options_t opts;
  opts.pbf = pbf;
  opts.target = tmp.filePath(name);
  opts.name = "Fixture";
  opts.chunkSegments = chunkSegments;
  opts.area = area;
  CSurfaceDbBuilder builder(opts);
  if (!builder.build(nullptr)) {
    qWarning() << builder.errorString();
    return QString();
  }
  return opts.target;
}

void test_CSurfaceDb::initTestCase() {
  QVERIFY(tmp.isValid());
  pbf = QFINDTESTDATA("data/fixture.osm.pbf");
  QVERIFY(!pbf.isEmpty());
  db = build("fixture.surface.sqlite");
  QVERIFY(!db.isEmpty());
}

void test_CSurfaceDb::readerNodesAndWays() {
  CPbfReader reader(pbf);
  QVERIFY2(reader.open(), qPrintable(reader.errorString()));
  QVERIFY(reader.header().requiredFeatures.contains("OsmSchema-V0.6"));

  QHash<qint64, QPair<qint32, qint32>> nodes;
  QHash<qint64, QStringList> ways;
  QHash<qint64, QVector<qint64>> refs;
  QVERIFY(reader.readData(
      CPbfReader::eNodes | CPbfReader::eWays, [&](qint64 id, qint32 lat, qint32 lon) { nodes[id] = {lat, lon}; },
      [&](const CPbfReader::way_t& way, const QVector<QByteArray>& strings) {
        QStringList tags;
        for (qsizetype i = 0; i < way.keys.size(); i++) {
          tags << QString::fromUtf8(strings[way.keys[i]] + "=" + strings[way.vals[i]]);
        }
        tags.sort();
        ways[way.id] = tags;
        refs[way.id] = way.refs;
      },
      nullptr));

  QCOMPARE(nodes.size(), 36);
  QCOMPARE(nodes[100], qMakePair(500000000, 50000000));
  QCOMPARE(nodes[210], qMakePair(500020000, 50100000));
  QCOMPARE(nodes[300], qMakePair(499990000, 50050000));
  QVERIFY(!nodes.contains(999));

  QCOMPARE(ways.size(), 7);
  QCOMPARE(ways[12], QStringList({"highway=residential", "name=Rue de Test", "surface=asphalt"}));
  QCOMPARE(ways[13], QStringList({"building=yes"}));
  QCOMPARE(refs[14], QVector<qint64>({500, 999, 501}));
  QCOMPARE(refs[10].size(), 11);
  QCOMPARE(refs[10].last(), 110);

  // only what is asked for
  qint32 cntNodes = 0, cntWays = 0;
  QVERIFY(reader.readData(
      CPbfReader::eWays, [&](qint64, qint32, qint32) { cntNodes++; },
      [&](const CPbfReader::way_t&, const QVector<QByteArray>&) { cntWays++; }, nullptr));
  QCOMPARE(cntNodes, 0);
  QCOMPARE(cntWays, 7);
}

void test_CSurfaceDb::readerRejectsGarbage() {
  QFile file(tmp.filePath("garbage.osm.pbf"));
  QVERIFY(file.open(QIODevice::WriteOnly));
  file.write(QByteArray("\x00\x00\x00\x05hello world, this is no pbf", 32));
  file.close();
  CPbfReader reader(file.fileName());
  QVERIFY(!reader.open());
  QVERIFY(!reader.errorString().isEmpty());

  CPbfReader missing(tmp.filePath("does-not-exist.osm.pbf"));
  QVERIFY(!missing.open());

  // a truncated copy of the fixture fails cleanly
  QFile src(pbf);
  QVERIFY(src.open(QIODevice::ReadOnly));
  const QByteArray data = src.readAll();
  QFile cut(tmp.filePath("cut.osm.pbf"));
  QVERIFY(cut.open(QIODevice::WriteOnly));
  cut.write(data.left(data.size() - 40));
  cut.close();
  CPbfReader truncated(cut.fileName());
  QVERIFY(truncated.open());
  QVERIFY(!truncated.readData(
      CPbfReader::eNodes | CPbfReader::eWays, [](qint64, qint32, qint32) {},
      [](const CPbfReader::way_t&, const QVector<QByteArray>&) {}, nullptr));
  QVERIFY(!truncated.errorString().isEmpty());
}

void test_CSurfaceDb::builderStatistics() {
  CSurfaceDbBuilder::options_t opts;
  opts.pbf = pbf;
  opts.target = tmp.filePath("stats.surface.sqlite");
  opts.chunkSegments = 4;
  CSurfaceDbBuilder builder(opts);
  QVERIFY(builder.build(nullptr));
  // 6 highways (the building is skipped), way 14 keeps 2 of its 3 nodes
  QCOMPARE(builder.statistics().ways, 6);
  QCOMPARE(builder.statistics().nodes, 11 + 11 + 5 + 2 + 2 + 2);
  QCOMPARE(builder.statistics().tags, 6);
  // way 10 and 11: 10 segments -> 3 chunks each, way 12: 4 segments -> 1 chunk, 3 single segments
  QCOMPARE(builder.statistics().chunks, 3 + 3 + 1 + 3);
  QVERIFY(!QFile::exists(opts.target + ".part"));
}

void test_CSurfaceDb::builderReportsProgressAndCancels() {
  CSurfaceDbBuilder::options_t opts;
  opts.pbf = pbf;
  opts.target = tmp.filePath("progress.surface.sqlite");
  CSurfaceDbBuilder builder(opts);
  qreal last = -1;
  bool monotonic = true;
  QVERIFY(builder.build([&](qreal fraction, const QString&) {
    monotonic = monotonic && fraction >= last;
    last = fraction;
  }));
  QVERIFY(monotonic);
  QCOMPARE(last, 1.0);

  std::atomic_bool cancel = true;
  opts.target = tmp.filePath("canceled.surface.sqlite");
  CSurfaceDbBuilder canceled(opts);
  QVERIFY(!canceled.build(nullptr, &cancel));
  QVERIFY(!canceled.errorString().isEmpty());
  QVERIFY(!QFile::exists(opts.target));
  QVERIFY(!QFile::exists(opts.target + ".part"));
}

void test_CSurfaceDb::builderArea() {
  // only way 12 has nodes south of 49.9995
  const QString file = build("area.surface.sqlite", 16, QRectF(QPointF(4.9, 49.99), QPointF(5.1, 49.9995)));
  QVERIFY(!file.isEmpty());
  CSurfaceDb surface(file);
  QVERIFY(surface.isValid());
  QCOMPARE(surface.chunkCount(), 1);
  QVERIFY(surface.lookupCell(50.0, 5.005).has_value());
  QCOMPARE(infoOf(surface.lookupCell(50.0, 5.005)).label, CSurface::eLabelAsphalt);
  QVERIFY(!surface.lookupCell(50.002, 5.001).has_value());
}

void test_CSurfaceDb::builderNoHighways() {
  CSurfaceDbBuilder::options_t opts;
  opts.pbf = pbf;
  opts.target = tmp.filePath("empty.surface.sqlite");
  opts.area = QRectF(QPointF(10, 10), QPointF(11, 11));
  CSurfaceDbBuilder builder(opts);
  QVERIFY(!builder.build(nullptr));
  QVERIFY(builder.errorString().contains("fixture.osm.pbf"));
  QVERIFY(!QFile::exists(opts.target));

  opts.pbf = tmp.filePath("missing.osm.pbf");
  CSurfaceDbBuilder missing(opts);
  QVERIFY(!missing.build(nullptr));
}

void test_CSurfaceDb::metaData() {
  CSurfaceDb surface(db);
  QVERIFY2(surface.isValid(), qPrintable(surface.errorString()));
  QCOMPARE(surface.name(), QString("Fixture"));
  QCOMPARE(surface.source(), QString("fixture.osm.pbf"));
  QCOMPARE(surface.filename(), db);
  QVERIFY(surface.created().isValid());
  QVERIFY(!surface.osmTimestamp().isValid());  // the fixture has none
  QCOMPARE(surface.chunkCount(), 6);
  const QRectF bbox = surface.boundingBox();
  QCOMPARE(bbox.left(), 5.0);
  QCOMPARE(bbox.right(), 5.01);
  QCOMPARE(bbox.top(), 49.999);
  QCOMPARE(bbox.bottom(), 50.01);
}

void test_CSurfaceDb::cellLookupMatchesReference_data() {
  QTest::addColumn<qreal>("lat");
  QTest::addColumn<qreal>("lon");
  QTest::addColumn<QString>("expected");

  QHash<QString, QString> reference;
  QFile file(QFINDTESTDATA("data/fixture_cells.tsv"));
  QVERIFY(file.open(QIODevice::ReadOnly));
  while (!file.atEnd()) {
    const QStringList cols = QString::fromUtf8(file.readLine()).chopped(1).split('\t');
    reference[cols[0] + " " + cols[1]] = cols[2] + "|" + cols[3] + "|" + cols[4];
  }
  QVERIFY(reference.size() > 300);

  // the same grid make_fixture.py queried, empty cells included
  for (qint32 i = -10; i < 141; i++) {
    const qreal lat = 49.9985 + i * 0.0001;
    for (qint32 j = -5; j < 116; j++) {
      const qreal lon = 4.9995 + j * 0.0001;
      const QString key = QString("%1 %2").arg(lat, 0, 'f', 7).arg(lon, 0, 'f', 7);
      QTest::newRow(key.toUtf8()) << lat << lon << reference.value(key, "-");
    }
  }
}

void test_CSurfaceDb::cellLookupMatchesReference() {
  QFETCH(qreal, lat);
  QFETCH(qreal, lon);
  QFETCH(QString, expected);

  static CSurfaceDb* surface = nullptr;
  if (surface == nullptr) {
    surface = new CSurfaceDb(db);
  }
  const std::optional<CSurface::info_t> info = surface->lookupCell(lat, lon);
  const QString actual =
      info ? QString("%1|%2|%3").arg(dutchLabel(infoOf(info))).arg(qint32(infoOf(info).cls)).arg(dutchWay(infoOf(info)))
           : QString("-");
  QCOMPARE(actual, expected);
}

void test_CSurfaceDb::chunkingDoesNotMatter() {
  const QString file = build("chunk2.surface.sqlite", 2);
  CSurfaceDb small(file);
  CSurfaceDb large(db);
  QVERIFY(small.chunkCount() > large.chunkCount());
  for (qint32 i = -10; i < 141; i += 3) {
    for (qint32 j = -5; j < 116; j += 3) {
      const qreal lat = 49.9985 + i * 0.0001;
      const qreal lon = 4.9995 + j * 0.0001;
      const auto a = small.lookupCell(lat, lon);
      const auto b = large.lookupCell(lat, lon);
      QCOMPARE(a.has_value(), b.has_value());
      if (a) {
        QVERIFY(infoOf(a) == infoOf(b));
      }
    }
  }
}

void test_CSurfaceDb::nearestFallback() {
  CSurfaceDb surface(db);
  // 22 m north of the path (way 11 on lat 50.002): another cell row, but within 25 m
  const qreal lat = 50.002 + 22 / 111195.0;
  QVERIFY(!surface.lookupCell(lat, 5.0031).has_value());
  const auto near = surface.lookupNearest(lat, 5.0031, 25);
  QVERIFY(near.has_value());
  QCOMPARE(infoOf(near).way, CSurface::eWayPath);
  QVERIFY(!surface.lookupNearest(lat, 5.0031, 20).has_value());
  QVERIFY(!surface.lookupNearest(lat, 5.0031, 0).has_value());

  // beyond the end of a way the distance is to its end node
  const auto end = surface.lookupNearest(50.0, 5.0101, 25);
  QVERIFY(end.has_value());
  QCOMPARE(infoOf(end).way, CSurface::eWayTrack);
  QVERIFY(!surface.lookupNearest(50.0, 5.0105, 25).has_value());
}

void test_CSurfaceDb::dbSet() {
  CSurfaceDbSet set({db, tmp.filePath("missing.surface.sqlite")});
  QVERIFY(!set.isEmpty());
  QVERIFY(set.covers(QRectF(QPointF(5.004, 50.004), QPointF(5.006, 50.006))));
  QVERIFY(set.covers(QRectF(QPointF(4.0, 49.0), QPointF(6.0, 51.0))));
  QVERIFY(!set.covers(QRectF(QPointF(6.0, 51.0), QPointF(6.1, 51.1))));

  QCOMPARE(infoOf(set.lookup(50.0, 5.0021, 25)).way, CSurface::eWayTrack);
  // cell miss -> nearest within 25 m
  QCOMPARE(infoOf(set.lookup(50.002 + 22 / 111195.0, 5.0031, 25)).way, CSurface::eWayPath);
  QVERIFY(!set.lookup(50.002 + 22 / 111195.0, 5.0031, 0).has_value());
  // outside every database
  QVERIFY(!set.lookup(52.0, 6.0, 25).has_value());

  CSurfaceDbSet empty({});
  QVERIFY(empty.isEmpty());
  QVERIFY(!empty.covers(QRectF(QPointF(4.0, 49.0), QPointF(6.0, 51.0))));
}

void test_CSurfaceDb::invalidFiles() {
  CSurfaceDb missing(tmp.filePath("nothing-here.surface.sqlite"));
  QVERIFY(!missing.isValid());
  QVERIFY(!missing.lookupCell(50, 5).has_value());
  QVERIFY(!missing.lookupNearest(50, 5, 25).has_value());

  // a SQLite file of another kind
  const QString other = tmp.filePath("other.sqlite");
  {
    QSqlDatabase sql = QSqlDatabase::addDatabase("QSQLITE", "other");
    sql.setDatabaseName(other);
    QVERIFY(sql.open());
    QSqlQuery query(sql);
    QVERIFY(query.exec("CREATE TABLE foo (bar INTEGER)"));
    query = QSqlQuery();
    sql.close();
  }
  QSqlDatabase::removeDatabase("other");
  CSurfaceDb wrong(other);
  QVERIFY(!wrong.isValid());
  QVERIFY(!wrong.errorString().isEmpty());

  // a newer schema
  const QString newer = tmp.filePath("newer.surface.sqlite");
  QVERIFY(QFile::copy(db, newer));
  {
    QSqlDatabase sql = QSqlDatabase::addDatabase("QSQLITE", "newer");
    sql.setDatabaseName(newer);
    QVERIFY(sql.open());
    QSqlQuery query(sql);
    QVERIFY(query.exec("UPDATE meta SET value = '99' WHERE key = 'schema'"));
    query = QSqlQuery();
    sql.close();
  }
  QSqlDatabase::removeDatabase("newer");
  CSurfaceDb future(newer);
  QVERIFY(!future.isValid());
  QVERIFY(future.errorString().contains("99"));
}

void test_CSurfaceDb::distanceAndCells() {
  // 0.001 deg latitude is ~111 m
  QVERIFY(qAbs(CSurfaceDb::distance(50.0, 5.0, 50.001, 5.0) - 111.19) < 0.01);
  // not exactly 0 if the compiler contracts to FMA (-march)
  QVERIFY(CSurfaceDb::distance(50.0, 5.0, 50.0, 5.0) < 1e-6);

  qint64 row, col;
  CSurfaceDb::cell(50.0, 5.0, row, col);
  QCOMPARE(row, 275000);
  QCOMPARE(col, 17500);
  // half way rounds to even, like Python
  CSurfaceDb::cell(0.5 / 5500, 1.5 / 3500, row, col);
  QCOMPARE(row, 0);
  QCOMPARE(col, 2);
}

QTEST_GUILESS_MAIN(test_CSurfaceDb)
#include "test_CSurfaceDb.moc"
