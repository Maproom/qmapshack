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
#include <QScopeGuard>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTest>
#include <QTranslator>
#include <QtEndian>

#include "gis/trk/surface/CPbfReader.h"
#include "gis/trk/surface/CSurfaceDb.h"
#include "gis/trk/surface/CSurfaceDbBuilder.h"
#include "gis/trk/surface/CSurfaceDbSet.h"

/** @brief PBF reading, database building and lookups on a tiny hand-made extract (data/make_fixture.py). */
class test_CSurfaceDb : public QObject {
  Q_OBJECT
 private slots:
  void initTestCase();
  void readerNodesAndWays();
  void readerRejectsGarbage();
  void readerRejectsUnsupportedFeatures_data();
  void readerRejectsUnsupportedFeatures();
  void readerEmptyFile();
  void readerInflate();
  void readerRejectsBadValues_data();
  void readerRejectsBadValues();
  void readerRejectsBrokenBlocks_data();
  void readerRejectsBrokenBlocks();
  void readerHeaderBlock();
  void readerStopsAfterNodes_data();
  void readerStopsAfterNodes();
  void readerRejectsBrokenFiles_data();
  void readerRejectsBrokenFiles();
  void areaText();
  void builderStatistics();
  void builderReportsProgressAndCancels();
  void builderArea();
  void builderSplitsAtMissingNode();
  void builderNoHighways();
  void builderUnsortedWays();
  void builderReplacesDatabase();
  void replaceFile();
  void fileName_data();
  void fileName();
  void nameForSource_data();
  void nameForSource();
  void rebuiltFileGetsNewCache();
  void metaData();
  void metaOnly();
  void releaseShared();
  void cellLookupMatchesReference_data();
  void cellLookupMatchesReference();
  void chunkingDoesNotMatter();
  void nearestFallback();
  void dbSet();
  void analyzeWithSet();
  void overlaps();
  void invalidFiles();
  void errorsAreTranslated();
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
  static const QHash<qint32, QString> names = {
      {CSurface::eLabelAsphalt, "asfalt"},
      {CSurface::eLabelSemiPaved, "halfverhard"},
      {CSurface::eLabelUnpavedAssumed, "onverhard (aangenomen)"},
      {CSurface::eLabelPavedAssumed, "verhard (aangenomen)"},
  };
  return names.value(info.label, "?");
}

static QString dutchWay(const CSurface::info_t& info) {
  static const QHash<qint32, QString> names = {
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

  QCOMPARE(nodes.size(), 38);
  QCOMPARE(nodes[100], qMakePair(500000000, 50000000));
  QCOMPARE(nodes[210], qMakePair(500020000, 50100000));
  QCOMPARE(nodes[300], qMakePair(499990000, 50050000));
  QVERIFY(!nodes.contains(999));

  QCOMPARE(ways.size(), 7);
  QCOMPARE(ways[12], QStringList({"highway=residential", "name=Rue de Test", "surface=asphalt"}));
  QCOMPARE(ways[13], QStringList({"building=yes"}));
  QCOMPARE(refs[14], QVector<qint64>({500, 501, 999, 502, 503}));
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

// protocol buffer encoding, just enough for a header block
static QByteArray varint(quint64 value) {
  QByteArray out;
  do {
    quint8 byte = value & 0x7F;
    value >>= 7;
    if (value != 0) {
      byte |= 0x80;
    }
    out.append(char(byte));
  } while (value != 0);
  return out;
}

static QByteArray bytesField(quint32 field, const QByteArray& data) {
  return varint((field << 3) | 2) + varint(data.size()) + data;
}

/** @brief a PBF file with an uncompressed header block with these features and data blocks */
static void writePbf(const QString& filename, const QStringList& requiredFeatures,
                     const QList<QByteArray>& dataBlocks = {}, const QStringList& optionalFeatures = {}) {
  QByteArray header;
  for (const QString& feature : requiredFeatures) {
    header += bytesField(4, feature.toUtf8());
  }
  for (const QString& feature : optionalFeatures) {
    header += bytesField(5, feature.toUtf8());
  }

  QFile file(filename);
  QVERIFY(file.open(QIODevice::WriteOnly));
  auto writeBlob = [&](const QByteArray& type, const QByteArray& data) {
    const QByteArray blob = bytesField(1, data);
    const QByteArray blobHeader = bytesField(1, type) + varint(3 << 3) + varint(blob.size());
    QByteArray len(4, 0);
    qToBigEndian<quint32>(blobHeader.size(), len.data());
    file.write(len + blobHeader + blob);
  };
  writeBlob("OSMHeader", header);
  for (const QByteArray& data : dataBlocks) {
    writeBlob("OSMData", data);
  }
}

static void writeHeaderOnlyPbf(const QString& filename, const QStringList& requiredFeatures) {
  writePbf(filename, requiredFeatures);
}

void test_CSurfaceDb::readerRejectsUnsupportedFeatures_data() {
  QTest::addColumn<QStringList>("features");
  QTest::addColumn<bool>("ok");
  QTest::addColumn<QString>("error");

  QTest::newRow("regular") << QStringList({"OsmSchema-V0.6", "DenseNodes"}) << true << QString();
  QTest::newRow("history") << QStringList({"OsmSchema-V0.6", "DenseNodes", "HistoricalInformation"}) << false
                           << QString(".osh.pbf");
  QTest::newRow("locations on ways") << QStringList({"OsmSchema-V0.6", "DenseNodes", "LocationsOnWays"}) << false
                                     << QString("LocationsOnWays");
}

void test_CSurfaceDb::readerRejectsUnsupportedFeatures() {
  QFETCH(QStringList, features);
  QFETCH(bool, ok);
  QFETCH(QString, error);

  const QString file = tmp.filePath("features.osm.pbf");
  writeHeaderOnlyPbf(file, features);
  CPbfReader reader(file);
  QCOMPARE(reader.open(), ok);
  QVERIFY2(reader.errorString().contains(error), qPrintable(reader.errorString()));
  if (ok) {
    QCOMPARE(reader.header().requiredFeatures, features);
  }

  // the builder passes the reason on
  CSurfaceDbBuilder::options_t opts;
  opts.pbf = file;
  opts.target = tmp.filePath("features.surface.sqlite");
  CSurfaceDbBuilder builder(opts);
  QVERIFY(!builder.build(nullptr));
  if (!ok) {
    QVERIFY2(builder.errorString().contains(error), qPrintable(builder.errorString()));
  }
  QVERIFY(!QFile::exists(opts.target));
}

void test_CSurfaceDb::readerEmptyFile() {
  const QString file = tmp.filePath("empty.osm.pbf");
  QFile empty(file);
  QVERIFY(empty.open(QIODevice::WriteOnly));
  empty.close();
  CPbfReader reader(file);
  QVERIFY(!reader.open());
  QVERIFY2(reader.errorString().contains("empty"), qPrintable(reader.errorString()));
}

void test_CSurfaceDb::readerInflate() {
  const QByteArray raw(100000, 'x');
  // qCompress() puts the size in front of the zlib stream
  const QByteArray zlib = qCompress(raw).mid(4);

  QByteArray out;
  QVERIFY(CPbfReader::inflate(zlib, raw.size(), out));
  QCOMPARE(out, raw);

  // more data than announced is an error, not a bigger buffer
  QVERIFY(!CPbfReader::inflate(zlib, 100, out));
  QVERIFY(out.isEmpty());
  // and so is less
  QVERIFY(!CPbfReader::inflate(zlib, raw.size() + 1, out));
  QVERIFY(!CPbfReader::inflate("no zlib stream", 10, out));
}

static QByteArray varintField(quint32 field, quint64 value) { return varint(field << 3) + varint(value); }

static quint64 zigzag(qint64 value) { return (quint64(value) << 1) ^ quint64(value >> 63); }

/** @brief a PrimitiveBlock with one node and one way using string 1 as key and value */
static QByteArray primitiveBlock(qint64 lat, qint64 lon, quint64 granularity, quint64 keyIndex) {
  const QByteArray strings = bytesField(1, bytesField(1, "") + bytesField(1, "highway"));
  const QByteArray node = varintField(1, zigzag(7)) + varintField(8, zigzag(lat)) + varintField(9, zigzag(lon));
  const QByteArray way = varintField(1, 8) + bytesField(2, varint(keyIndex)) + bytesField(3, varint(1)) +
                         bytesField(8, varint(zigzag(7)) + varint(zigzag(0)));
  return strings + bytesField(2, bytesField(1, node)) + bytesField(2, bytesField(3, way)) +
         varintField(17, granularity);
}

void test_CSurfaceDb::readerRejectsBadValues_data() {
  QTest::addColumn<qint64>("lat");
  QTest::addColumn<qint64>("lon");
  QTest::addColumn<quint64>("granularity");
  QTest::addColumn<quint64>("keyIndex");
  QTest::addColumn<QString>("error");

  QTest::newRow("valid") << qint64(500000000) << qint64(50000000) << quint64(100) << quint64(1) << QString();
  QTest::newRow("edge of the world") << qint64(-900000000) << qint64(1800000000) << quint64(100) << quint64(1)
                                     << QString();
  QTest::newRow("latitude beyond the pole")
      << qint64(900000001) << qint64(0) << quint64(100) << quint64(1) << QString("coordinate out of range");
  QTest::newRow("longitude beyond 180") << qint64(0) << qint64(-1800000001) << quint64(100) << quint64(1)
                                        << QString("coordinate out of range");
  // granularity * value overflows 64 bits
  QTest::newRow("overflow") << qint64(1) << qint64(1LL << 40) << quint64(1ULL << 40) << quint64(1)
                            << QString("coordinate out of range");
  // 2^32 + 1 cut to 32 bits would be the valid index 1
  QTest::newRow("string index") << qint64(0) << qint64(0) << quint64(100) << quint64((1ULL << 32) + 1)
                                << QString("value out of range");
  QTest::newRow("string index past the table")
      << qint64(0) << qint64(0) << quint64(100) << quint64(2) << QString("string index out of range");
}

void test_CSurfaceDb::readerRejectsBadValues() {
  QFETCH(qint64, lat);
  QFETCH(qint64, lon);
  QFETCH(quint64, granularity);
  QFETCH(quint64, keyIndex);
  QFETCH(QString, error);

  const QByteArray block = primitiveBlock(lat, lon, granularity, keyIndex);
  QString what;
  qint32 cntNodes = 0, cntWays = 0;
  try {
    CPbfReader::parsePrimitiveBlock(
        block, CPbfReader::eNodes | CPbfReader::eWays,
        [&](qint64 id, qint32 latE7, qint32 lonE7) {
          QCOMPARE(id, 7);
          QCOMPARE(qint64(latE7), lat * qint64(granularity) / 100);
          QCOMPARE(qint64(lonE7), lon * qint64(granularity) / 100);
          cntNodes++;
        },
        [&](const CPbfReader::way_t& way, const QVector<QByteArray>& strings) {
          QCOMPARE(way.id, 8);
          QCOMPARE(strings[way.keys[0]], QByteArray("highway"));
          QCOMPARE(way.refs, QVector<qint64>({7, 7}));
          cntWays++;
        });
  } catch (const std::exception& e) {
    what = e.what();
  }
  QCOMPARE(what, error);
  if (error.isEmpty()) {
    QCOMPARE(cntNodes, 1);
    QCOMPARE(cntWays, 1);
  }
}

void test_CSurfaceDb::readerRejectsBrokenBlocks_data() {
  QTest::addColumn<QByteArray>("block");
  QTest::addColumn<QString>("error");
  QTest::addColumn<qint32>("ways");

  const QByteArray strings = bytesField(1, bytesField(1, "") + bytesField(1, "highway") + bytesField(1, "x"));
  auto group = [](quint32 field, const QByteArray& data) { return bytesField(2, bytesField(field, data)); };
  const QByteArray refs = bytesField(8, varint(zigzag(1)) + varint(zigzag(1)));

  QTest::newRow("truncated varint") << QByteArray("\x80", 1) << "truncated varint" << 0;
  QTest::newRow("varint too long") << QByteArray(11, '\xff') << "varint too long" << 0;
  QTest::newRow("truncated field") << (varint(2 << 3 | 2) + varint(10) + "abc") << "truncated field" << 0;
  QTest::newRow("truncated fixed64") << (varint(99 << 3 | 1) + "1234") << "truncated field" << 0;
  QTest::newRow("unsupported wire type") << varint(99 << 3 | 3) << "unsupported wire type" << 0;
  // unknown fields of every wire type are skipped
  QTest::newRow("unknown fields") << (varint(99 << 3 | 1) + "12345678" + varint(98 << 3 | 5) + "1234" +
                                      varintField(97, 5) + bytesField(96, "skipped") + varintField(19, 0) +
                                      varintField(20, 0))
                                  << QString() << 0;
  QTest::newRow("dense nodes without lat")
      << group(2, bytesField(1, varint(zigzag(1))) + bytesField(9, varint(zigzag(1)))) << "inconsistent dense nodes"
      << 0;
  QTest::newRow("way with a key but no value")
      << (strings + group(3, varintField(1, 1) + bytesField(2, varint(1)) + refs)) << "inconsistent way tags" << 0;
  QTest::newRow("value index past the table")
      << (strings + group(3, varintField(1, 1) + bytesField(2, varint(1)) + bytesField(3, varint(3)) + refs))
      << "string index out of range" << 0;
  // keys and values one by one instead of packed, an unknown field in the string table
  QTest::newRow("unpacked tags") << (bytesField(1, bytesField(1, "") + bytesField(1, "highway") + bytesField(1, "x") +
                                                       varintField(2, 0)) +
                                     group(3, varintField(1, 1) + varintField(2, 1) + varintField(3, 2) + refs))
                                 << QString() << 1;
}

void test_CSurfaceDb::readerRejectsBrokenBlocks() {
  QFETCH(QByteArray, block);
  QFETCH(QString, error);
  QFETCH(qint32, ways);

  QString what;
  qint32 cntWays = 0;
  try {
    CPbfReader::parsePrimitiveBlock(
        block, CPbfReader::eNodes | CPbfReader::eWays, [](qint64, qint32, qint32) {},
        [&](const CPbfReader::way_t& way, const QVector<QByteArray>& strings) {
          QCOMPARE(strings[way.keys[0]], QByteArray("highway"));
          QCOMPARE(strings[way.vals[0]], QByteArray("x"));
          cntWays++;
        });
  } catch (const std::exception& e) {
    what = e.what();
  }
  QCOMPARE(what, error);
  QCOMPARE(cntWays, ways);
}

void test_CSurfaceDb::readerHeaderBlock() {
  // bbox in nanodegrees: left, right, top, bottom, and a field of another wire type
  const QByteArray bbox = varintField(1, zigzag(-1500000000)) + varintField(2, zigzag(6400000000)) +
                          varintField(3, zigzag(51500000000)) + varintField(4, zigzag(49500000000)) +
                          varintField(5, 1) + bytesField(6, "x");
  const QByteArray data = bytesField(1, bbox) + bytesField(4, "OsmSchema-V0.6") + bytesField(5, "Sort.Type_then_ID") +
                          bytesField(16, "osmium/1.16") + bytesField(17, "https://www.openstreetmap.org/api/0.6") +
                          varintField(32, 1760000000) + varintField(33, 1);
  CPbfReader::header_t hdr;
  CPbfReader::parseHeaderBlock(data, hdr);
  QVERIFY(hdr.hasBBox);
  QCOMPARE(hdr.left, -1.5);
  QCOMPARE(hdr.right, 6.4);
  QCOMPARE(hdr.top, 51.5);
  QCOMPARE(hdr.bottom, 49.5);
  QCOMPARE(hdr.requiredFeatures, QStringList({"OsmSchema-V0.6"}));
  QCOMPARE(hdr.optionalFeatures, QStringList({"Sort.Type_then_ID"}));
  QCOMPARE(hdr.writingProgram, QString("osmium/1.16"));
  QCOMPARE(hdr.source, QString("https://www.openstreetmap.org/api/0.6"));
  QCOMPARE(hdr.timestamp, 1760000000);
}

void test_CSurfaceDb::readerStopsAfterNodes_data() {
  QTest::addColumn<QStringList>("required");
  QTest::addColumn<QStringList>("optional");
  QTest::addColumn<QList<qint64>>("nodes");

  QTest::newRow("sorted") << QStringList() << QStringList({"Sort.Type_then_ID"}) << QList<qint64>({1, 2});
  QTest::newRow("sorted, required") << QStringList({"Sort.Type_then_ID"}) << QStringList() << QList<qint64>({1, 2});
  QTest::newRow("not sorted") << QStringList() << QStringList() << QList<qint64>({1, 2, 4});
}

void test_CSurfaceDb::readerStopsAfterNodes() {
  QFETCH(QStringList, required);
  QFETCH(QStringList, optional);
  QFETCH(QList<qint64>, nodes);

  const QByteArray strings = bytesField(1, bytesField(1, "") + bytesField(1, "highway"));
  auto node = [](qint64 id) {
    return bytesField(2, bytesField(1, varintField(1, zigzag(id)) + varintField(8, 0) + varintField(9, 0)));
  };
  const QByteArray way = bytesField(2, bytesField(3, varintField(1, 3) + bytesField(8, varint(zigzag(1)))));
  const QByteArray relation = bytesField(2, bytesField(4, varintField(1, 5)));
  // block 2 ends the nodes, also if it starts with some; the stray node 4 breaks the sort order
  const QList<QByteArray> blocks = {strings + node(1), strings + node(2) + way, strings + relation, strings + node(4)};
  const QString file = tmp.filePath("sorted.osm.pbf");
  writePbf(file, QStringList({"OsmSchema-V0.6"}) + required, blocks, optional);

  CPbfReader reader(file);
  QVERIFY2(reader.open(), qPrintable(reader.errorString()));
  QList<qint64> ids;
  qint32 cntBlocks = 0;
  QVERIFY(reader.readData(
      CPbfReader::eNodes, [&](qint64 id, qint32, qint32) { ids << id; }, nullptr,
      [&](qint64) {
        cntBlocks++;
        return true;
      }));
  QCOMPARE(ids, nodes);
  QCOMPARE(cntBlocks, nodes.size() == 2 ? 2 : 4);

  // ways are read to the end
  ids.clear();
  qint32 cntWays = 0;
  QVERIFY(reader.readData(
      CPbfReader::eNodes | CPbfReader::eWays, [&](qint64 id, qint32, qint32) { ids << id; },
      [&](const CPbfReader::way_t&, const QVector<QByteArray>&) { cntWays++; }, nullptr));
  QCOMPARE(ids, QList<qint64>({1, 2, 4}));
  QCOMPARE(cntWays, 1);
}

/** @brief a block as in the file: its length, the BlobHeader and the Blob */
static QByteArray fileBlock(const QByteArray& type, const QByteArray& blob, quint64 announced = 0) {
  const QByteArray blobHeader = bytesField(1, type) + varintField(3, announced ? announced : quint64(blob.size()));
  QByteArray len(4, 0);
  qToBigEndian<quint32>(blobHeader.size(), len.data());
  return len + blobHeader + blob;
}

void test_CSurfaceDb::readerRejectsBrokenFiles_data() {
  QTest::addColumn<QByteArray>("data");
  QTest::addColumn<bool>("opens");
  QTest::addColumn<QString>("error");

  const QByteArray header = fileBlock("OSMHeader", bytesField(1, bytesField(4, "OsmSchema-V0.6")));
  const QByteArray raw(1000, 'x');
  const QByteArray zlib = qCompress(raw).mid(4);

  QTest::newRow("data first") << fileBlock("OSMData", bytesField(1, "")) << false << "first block is 'OSMData'";
  QTest::newRow("broken header") << fileBlock("OSMHeader", bytesField(1, "\x80")) << false << "Broken OSM header";
  QTest::newRow("cut length") << QByteArray("\x00\x00", 2) << false << "truncated block header";
  QTest::newRow("huge block header") << QByteArray("\x00\x10\x00\x00", 4) << false << "block header too large";
  QTest::newRow("cut block header") << QByteArray("\x00\x00\x00\x10\x0a", 5) << false << "truncated block header";
  QTest::newRow("huge block") << fileBlock("OSMHeader", QByteArray(), 1ULL << 30) << false << "block too large";
  QTest::newRow("cut block") << fileBlock("OSMHeader", QByteArray(), 100) << false << "truncated block";
  QTest::newRow("other compression") << fileBlock("OSMHeader", varintField(2, 10) + bytesField(4, "lzma")) << false
                                     << "unsupported compression";
  QTest::newRow("huge raw size") << fileBlock("OSMHeader", varintField(2, 1ULL << 30) + bytesField(3, zlib)) << false
                                 << "block too large";
  QTest::newRow("wrong raw size") << fileBlock("OSMHeader", varintField(2, 10) + bytesField(3, zlib)) << false
                                  << "zlib decompression failed";
  QTest::newRow("broken data block") << (header + fileBlock("OSMData", bytesField(1, "\x80"))) << true
                                     << "Broken PBF data block";
  // zlib compressed blocks and blocks of other types, which are skipped
  const QByteArray features = bytesField(4, "OsmSchema-V0.6");
  QTest::newRow("compressed") << (fileBlock("OSMHeader", varintField(2, features.size()) +
                                                             bytesField(3, qCompress(features).mid(4))) +
                                  fileBlock("Other", bytesField(1, "ignored")) +
                                  fileBlock("OSMData", bytesField(1, QByteArray())))
                              << true << QString();
}

void test_CSurfaceDb::readerRejectsBrokenFiles() {
  QFETCH(QByteArray, data);
  QFETCH(bool, opens);
  QFETCH(QString, error);

  const QString file = tmp.filePath("broken.osm.pbf");
  QFile out(file);
  QVERIFY(out.open(QIODevice::WriteOnly));
  out.write(data);
  out.close();

  CPbfReader reader(file);
  // reading before opening
  QVERIFY(!reader.readData(CPbfReader::eWays, nullptr, nullptr, nullptr));
  QVERIFY(reader.errorString().contains("not open"));

  CPbfReader broken(file);
  QCOMPARE(broken.open(), opens);
  if (opens) {
    QCOMPARE(broken.readData(
                 CPbfReader::eNodes | CPbfReader::eWays, [](qint64, qint32, qint32) {},
                 [](const CPbfReader::way_t&, const QVector<QByteArray>&) {}, nullptr),
             error.isEmpty());
  }
  QVERIFY2(broken.errorString().contains(error), qPrintable(broken.errorString()));
}

void test_CSurfaceDb::areaText() {
  QCOMPARE(CSurfaceDb::areaText(QRectF(QPointF(2.54, 49.5), QPointF(6.41, 51.51))),
           QString("49.50 N..51.51 N, 2.54 E..6.41 E"));
  QCOMPARE(CSurfaceDb::areaText(QRectF(QPointF(-73.99, -55.98), QPointF(-53.64, -21.78))),
           QString("55.98 S..21.78 S, 73.99 W..53.64 W"));
  QCOMPARE(CSurfaceDb::areaText(QRectF(QPointF(-0.5, -1), QPointF(0.5, 1))), QString("1.00 S..1.00 N, 0.50 W..0.50 E"));
}

void test_CSurfaceDb::builderStatistics() {
  CSurfaceDbBuilder::options_t opts;
  opts.pbf = pbf;
  opts.target = tmp.filePath("stats.surface.sqlite");
  opts.chunkSegments = 4;
  CSurfaceDbBuilder builder(opts);
  QVERIFY(builder.build(nullptr));
  // 6 highways (the building is skipped), way 14 keeps 4 of its 5 nodes
  QCOMPARE(builder.statistics().ways, 6);
  QCOMPARE(builder.statistics().nodes, 11 + 11 + 5 + 4 + 2 + 2);
  QCOMPARE(builder.statistics().tags, 6);
  // way 10 and 11: 10 segments -> 3 chunks each, way 12: 4 segments -> 1 chunk, 4 single segments
  QCOMPARE(builder.statistics().chunks, 3 + 3 + 1 + 4);
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

void test_CSurfaceDb::builderSplitsAtMissingNode() {
  // way 14: nodes at lon 5.000, 5.001, missing, 5.003, 5.004
  CSurfaceDb surface(db);
  QCOMPARE(infoOf(surface.lookupCell(50.007, 5.0005)).way, CSurface::eWayBusyRoad);
  QCOMPARE(infoOf(surface.lookupCell(50.007, 5.0035)).way, CSurface::eWayBusyRoad);
  QVERIFY(!surface.lookupCell(50.007, 5.002).has_value());
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

void test_CSurfaceDb::builderUnsortedWays() {
  // two ways on the same nodes, the one with the higher id first in the file
  const QByteArray strings =
      bytesField(1, bytesField(1, "") + bytesField(1, "highway") + bytesField(1, "residential") +
                        bytesField(1, "surface") + bytesField(1, "asphalt") + bytesField(1, "track"));
  auto node = [](qint64 id, qint64 lat, qint64 lon) {
    return bytesField(1, varintField(1, zigzag(id)) + varintField(8, zigzag(lat)) + varintField(9, zigzag(lon)));
  };
  auto way = [](quint64 id, const QByteArray& keys, const QByteArray& vals) {
    return bytesField(3, varintField(1, id) + bytesField(2, keys) + bytesField(3, vals) +
                             bytesField(8, varint(zigzag(1)) + varint(zigzag(1))));
  };
  const QByteArray nodes = bytesField(2, node(1, 500000000, 50000000) + node(2, 500000000, 50010000));
  const QByteArray ways =
      bytesField(2, way(20, varint(1) + varint(3), varint(2) + varint(4)) + way(10, varint(1), varint(5)));
  const QString file = tmp.filePath("unsorted.osm.pbf");
  writePbf(file, {"OsmSchema-V0.6"}, {strings + nodes + ways});

  CSurfaceDbBuilder::options_t opts;
  opts.pbf = file;
  opts.target = tmp.filePath("unsorted.surface.sqlite");
  CSurfaceDbBuilder builder(opts);
  QVERIFY2(builder.build(nullptr), qPrintable(builder.errorString()));
  QCOMPARE(builder.statistics().ways, 2);

  // a tie goes to the lowest OSM id, wherever the way is in the file
  CSurfaceDb surface(opts.target);
  QVERIFY(surface.isValid());
  QCOMPARE(infoOf(surface.lookupCell(50.0, 5.0005)).way, CSurface::eWayTrack);
}

void test_CSurfaceDb::builderReplacesDatabase() {
  const QString file = build("replace.surface.sqlite", 16, QRectF(QPointF(4.9, 49.99), QPointF(5.1, 49.9995)));
  QVERIFY(!file.isEmpty());
  QCOMPARE(CSurfaceDb(file).chunkCount(), 1);
  QCOMPARE(build("replace.surface.sqlite"), file);
  QVERIFY(CSurfaceDb(file).chunkCount() > 1);
  QVERIFY(!QFile::exists(file + ".part"));
  QVERIFY(!QFile::exists(file + ".old"));
}

static void writeFile(const QString& filename, const QByteArray& data) {
  QFile file(filename);
  QVERIFY(file.open(QIODevice::WriteOnly));
  file.write(data);
}

static QByteArray readFile(const QString& filename) {
  QFile file(filename);
  return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}

void test_CSurfaceDb::replaceFile() {
  const QString target = tmp.filePath("replace.bin");
  const QString part = tmp.filePath("replace.bin.part");
  QString error;

  writeFile(part, "first");
  QVERIFY(CSurfaceDbBuilder::replaceFile(part, target, error));
  QCOMPARE(readFile(target), QByteArray("first"));
  QVERIFY(!QFile::exists(part));

  writeFile(part, "second");
  QVERIFY(CSurfaceDbBuilder::replaceFile(part, target, error));
  QCOMPARE(readFile(target), QByteArray("second"));
  QVERIFY(!QFile::exists(part));
  QVERIFY(!QFile::exists(target + ".old"));

  // the new file can't be moved into place: the old one must survive
  QVERIFY(!CSurfaceDbBuilder::replaceFile(tmp.filePath("missing.part"), target, error));
  QVERIFY(!error.isEmpty());
  QCOMPARE(readFile(target), QByteArray("second"));
  QVERIFY(!QFile::exists(target + ".old"));
}

void test_CSurfaceDb::fileName_data() {
  QTest::addColumn<QString>("name");
  QTest::addColumn<QString>("file");
  QTest::newRow("ascii") << "Belgium" << "Belgium.surface.sqlite";
  QTest::newRow("spaces and quotes") << " Côte d'Or " << "Côte_d_Or.surface.sqlite";
  QTest::newRow("chinese") << "北京" << "北京.surface.sqlite";
  QTest::newRow("japanese") << "東京" << "東京.surface.sqlite";
  // "Liège" with the accent as a combining character
  QTest::newRow("decomposed") << QString("Lie") + QChar(0x0300) + "ge"
                              << QString("Li") + QChar(0xe8) + "ge.surface.sqlite";
  QTest::newRow("path") << "../a/b" << ".._a_b.surface.sqlite";
  QTest::newRow("empty") << "  " << QString();
}

void test_CSurfaceDb::fileName() {
  QFETCH(QString, name);
  QFETCH(QString, file);
  QCOMPARE(CSurfaceDbBuilder::fileName(name), file);
}

void test_CSurfaceDb::nameForSource_data() {
  QTest::addColumn<QString>("pbf");
  QTest::addColumn<QString>("previous");
  QTest::addColumn<QString>("name");
  QTest::addColumn<QString>("result");
  QTest::newRow("first pick") << "/data/belgium-latest.osm.pbf" << QString() << QString() << "Belgium";
  QTest::newRow("other region") << "/data/france-latest.osm.pbf" << "/data/belgium-latest.osm.pbf" << "Belgium"
                                << "France";
  QTest::newRow("same file keeps own name") << "/data/belgium-latest.osm.pbf" << "/data/belgium-latest.osm.pbf"
                                            << "Belgique" << "Belgique";
  QTest::newRow("same file, no name yet") << "/data/belgium-latest.osm.pbf" << "/data/belgium-latest.osm.pbf"
                                          << " " << "Belgium";
  QTest::newRow("no -latest") << "/data/liege.osm.pbf" << QString() << QString() << "Liege";
  QTest::newRow("no name") << "/data/.osm.pbf" << QString() << QString() << QString();
}

void test_CSurfaceDb::nameForSource() {
  QFETCH(QString, pbf);
  QFETCH(QString, previous);
  QFETCH(QString, name);
  QFETCH(QString, result);
  QCOMPARE(CSurfaceDbBuilder::nameForSource(pbf, previous, name), result);
}

void test_CSurfaceDb::rebuiltFileGetsNewCache() {
  // the classified tags and the cell cache are shared by all objects of a file, but not
  // with a file built again under the same name
  const QString file = build("rebuilt.surface.sqlite");
  QVERIFY(!file.isEmpty());
  CSurfaceDb before(file);
  QVERIFY(before.isValid());
  QCOMPARE(infoOf(before.lookupCell(50.002, 5.001)).way, CSurface::eWayPath);
  // a second object gets the cell from the cache
  CSurfaceDb again(file);
  QCOMPARE(infoOf(again.lookupCell(50.002, 5.001)).way, CSurface::eWayPath);

  // only way 12
  QCOMPARE(build("rebuilt.surface.sqlite", 16, QRectF(QPointF(4.9, 49.99), QPointF(5.1, 49.9995))), file);
  CSurfaceDb after(file);
  QVERIFY(after.isValid());
  QCOMPARE(after.chunkCount(), 1);
  QVERIFY(!after.lookupCell(50.002, 5.001).has_value());
  QCOMPARE(infoOf(after.lookupCell(50.0, 5.005)).label, CSurface::eLabelAsphalt);
}

void test_CSurfaceDb::metaData() {
  CSurfaceDb surface(db);
  QVERIFY2(surface.isValid(), qPrintable(surface.errorString()));
  QCOMPARE(surface.name(), QString("Fixture"));
  QCOMPARE(surface.source(), QString("fixture.osm.pbf"));
  QCOMPARE(surface.filename(), db);
  QVERIFY(surface.created().isValid());
  QVERIFY(!surface.osmTimestamp().isValid());  // the fixture has none
  QCOMPARE(surface.chunkCount(), 7);           // way 14 is two stretches
  const QRectF bbox = surface.boundingBox();
  QCOMPARE(bbox.left(), 5.0);
  QCOMPARE(bbox.right(), 5.01);
  QCOMPARE(bbox.top(), 49.999);
  QCOMPARE(bbox.bottom(), 50.01);
}

void test_CSurfaceDb::metaOnly() {
  // without the tags table the file can't be used for lookups, but its meta data can be read
  const QString file = tmp.filePath("meta-only.surface.sqlite");
  QVERIFY(QFile::copy(db, file));
  {
    QSqlDatabase sql = QSqlDatabase::addDatabase("QSQLITE", "meta-only");
    sql.setDatabaseName(file);
    QVERIFY(sql.open());
    QSqlQuery query(sql);
    QVERIFY(query.exec("DROP TABLE tags"));
    query = QSqlQuery();
    sql.close();
  }
  QSqlDatabase::removeDatabase("meta-only");

  CSurfaceDb::releaseShared({});
  CSurfaceDb meta(file, CSurfaceDb::eModeMeta);
  QVERIFY2(meta.isValid(), qPrintable(meta.errorString()));
  QCOMPARE(meta.name(), QString("Fixture"));
  QCOMPARE(meta.chunkCount(), 7);
  QCOMPARE(meta.boundingBox().right(), 5.01);
  QVERIFY(!meta.lookupCell(50.002, 5.001).has_value());
  QVERIFY(!meta.lookupNearest(50.002, 5.001, 25).has_value());
  QCOMPARE(CSurfaceDb::sharedCount(), 0);

  CSurfaceDb full(file);
  QVERIFY(!full.isValid());
}

void test_CSurfaceDb::releaseShared() {
  CSurfaceDb::releaseShared({});
  {
    CSurfaceDb surface(db);
    QVERIFY(surface.isValid());
  }
  QCOMPARE(CSurfaceDb::sharedCount(), 1);
  CSurfaceDb::releaseShared({db});
  QCOMPARE(CSurfaceDb::sharedCount(), 1);
  CSurfaceDb::releaseShared({tmp.filePath("other.surface.sqlite")});
  QCOMPARE(CSurfaceDb::sharedCount(), 0);

  // an object still open keeps working
  CSurfaceDb open(db);
  CSurfaceDb::releaseShared({});
  QCOMPARE(infoOf(open.lookupCell(50.002, 5.001)).way, CSurface::eWayPath);
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
  // the missing file is left out, the other one is used
  CSurfaceDbSet set({tmp.filePath("missing.surface.sqlite"), db});

  QCOMPARE(infoOf(set.lookup(50.0, 5.0021, 25)).way, CSurface::eWayTrack);
  // cell miss -> nearest within 25 m
  QCOMPARE(infoOf(set.lookup(50.002 + 22 / 111195.0, 5.0031, 25)).way, CSurface::eWayPath);
  QVERIFY(!set.lookup(50.002 + 22 / 111195.0, 5.0031, 0).has_value());
  // outside every database
  QVERIFY(!set.lookup(52.0, 6.0, 25).has_value());

  CSurfaceDbSet empty({});
  QVERIFY(!empty.lookup(50.0, 5.0021, 25).has_value());
}

void test_CSurfaceDb::analyzeWithSet() {
  const QVector<CSurfaceAnalyzer::point_t> pts = {{50.0, 5.0, true}, {50.0, 5.003, false}};

  // canceled before it starts: the files are not even opened
  CSurfaceDb::releaseShared({});
  std::atomic_bool cancel = true;
  QVERIFY(!CSurfaceDbSet::analyze({db}, pts, &cancel).valid);
  QCOMPARE(CSurfaceDb::sharedCount(), 0);

  cancel = false;
  const CSurfaceAnalyzer::result_t result = CSurfaceDbSet::analyze({db}, pts, &cancel);
  QVERIFY(result.valid);
  QCOMPARE(CSurfaceDb::sharedCount(), 1);
  QVERIFY(result.samplesWithData > 0);
  QCOMPARE(result.perPoint.size(), 2);

  // without a usable file every sample is "no data"
  const CSurfaceAnalyzer::result_t none =
      CSurfaceDbSet::analyze({tmp.filePath("nothing.surface.sqlite")}, pts, nullptr);
  QVERIFY(none.valid);
  QCOMPARE(none.samplesWithData, 0);
}

void test_CSurfaceDb::overlaps() {
  const QRectF box(QPointF(5.0, 50.0), QPointF(5.1, 50.1));
  QVERIFY(CSurfaceDb::overlaps(box, QRectF(5.05, 50.05, 1e-9, 1e-9)));
  QVERIFY(CSurfaceDb::overlaps(box, QRectF(QPointF(4.9, 50.05), QPointF(5.01, 50.06))));
  QVERIFY(CSurfaceDb::overlaps(box, QRectF(QPointF(4.0, 49.0), QPointF(6.0, 51.0))));
  // a box of one point: its center lies in it although it intersects nothing
  QVERIFY(CSurfaceDb::overlaps(box, QRectF(5.05, 50.05, 0, 0)));
  QVERIFY(!CSurfaceDb::overlaps(box, QRectF(QPointF(5.2, 50.05), QPointF(5.3, 50.06))));
  QVERIFY(!CSurfaceDb::overlaps(box, QRectF(5.2, 50.2, 0, 0)));
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

/** @brief marks every text of the surface code it translates */
class CMarkingTranslator : public QTranslator {
 public:
  bool isEmpty() const override { return false; }
  QString translate(const char* context, const char* sourceText, const char* disambiguation, qint32 n) const override {
    Q_UNUSED(disambiguation);
    Q_UNUSED(n);
    const QByteArray ctx(context);
    return ctx == "CPbfReader" || ctx == "CSurfaceDb" ? "<" + QString::fromUtf8(sourceText) + ">" : QString();
  }
};

void test_CSurfaceDb::errorsAreTranslated() {
  CMarkingTranslator translator;
  QVERIFY(QCoreApplication::installTranslator(&translator));
  auto cleanup = qScopeGuard([&]() { QCoreApplication::removeTranslator(&translator); });

  const QString empty = tmp.filePath("translated-empty.osm.pbf");
  writeFile(empty, QByteArray());
  CPbfReader readerEmpty(empty);
  QVERIFY(!readerEmpty.open());
  QCOMPARE(readerEmpty.errorString(), QString("<The file is empty.>"));

  // the text of the cause, too
  const QString cut = tmp.filePath("translated-cut.osm.pbf");
  writeFile(cut, QByteArray("\x00\x00", 2));
  CPbfReader readerCut(cut);
  QVERIFY(!readerCut.open());
  QVERIFY2(readerCut.errorString().startsWith("<Broken PBF file at offset ") &&
               readerCut.errorString().endsWith(": <truncated block header>>"),
           qPrintable(readerCut.errorString()));

  const QString newer = tmp.filePath("translated-newer.surface.sqlite");
  QVERIFY(QFile::copy(db, newer));
  {
    QSqlDatabase sql = QSqlDatabase::addDatabase("QSQLITE", "translated");
    sql.setDatabaseName(newer);
    QVERIFY(sql.open());
    QSqlQuery query(sql);
    QVERIFY(query.exec("UPDATE meta SET value = '99' WHERE key = 'schema'"));
    query = QSqlQuery();
    sql.close();
  }
  QSqlDatabase::removeDatabase("translated");
  CSurfaceDb future(newer);
  QCOMPARE(future.errorString(), QString("<Unsupported surface database version 99>"));
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
