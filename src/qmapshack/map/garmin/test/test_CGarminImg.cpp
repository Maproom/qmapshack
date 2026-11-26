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

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTest>
#include <QTextCodec>
#include <QtMath>

#include "map/garmin/CGarminImg.h"
#include "map/garmin/CGarminPoint.h"
#include "map/garmin/CGarminPolygon.h"

/**
   @brief The reader decodes every object of an mkgmap-built test map as written in its Polish-format source.

   The test maps come from make_fixtures.py; QMS_IMG_FIXTURES names their folder. Their search indexes
   (*_mdr.img) hold no map tile and must be refused. Any other IMG file there, e.g. mkgmap's own test
   tiles 63240001-3.img from https://www.mkgmap.org.uk/testinput/img/, only has to load without error.
 */
class test_CGarminImg : public QObject {
  Q_OBJECT
 private slots:
  void decode_data();
  void decode();
  void load_data();
  void load();
  void refuseIndex_data();
  void refuseIndex();
  void refuseLarger4GB();
  void cutOffExtended();
};

namespace {
/** One object of a Polish-format source, coordinates in [deg] as lat, lon. */
struct mp_object_t {
  QString kind; /**< "POI", "POLYLINE" or "POLYGON" */
  quint32 type = 0;
  QString label;
  QVector<QPointF> coords;
};

/** One decoded object in the same terms. */
struct img_object_t {
  QString kind;
  quint32 type = 0;
  QString label;
  QVector<QPointF> coords;
};

QString fixtureDir() { return qEnvironmentVariable("QMS_IMG_FIXTURES"); }

QVector<mp_object_t> readMp(const QString& path) {
  QVector<mp_object_t> objects;
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    return objects;
  }
  const QByteArray data = file.readAll();

  // Labels are in the source's CodePage, as the map's labels are.
  static const QRegularExpression reCodePage(R"(^CodePage=(\d+))", QRegularExpression::MultilineOption);
  const QRegularExpressionMatch cp = reCodePage.match(QString::fromLatin1(data));
  const QByteArray codecName = !cp.hasMatch()              ? QByteArray("ISO-8859-1")
                               : cp.captured(1) == "65001" ? QByteArray("UTF-8")
                                                           : "windows-" + cp.captured(1).toLatin1();
  QTextCodec* codec = QTextCodec::codecForName(codecName);
  if (codec == nullptr) {
    qWarning() << "No codec" << codecName << "for" << path;
    return objects;
  }

  static const QRegularExpression reCoord(R"(\(\s*([-0-9.]+)\s*,\s*([-0-9.]+)\s*\))");
  mp_object_t* current = nullptr;
  const QStringList lines = codec->toUnicode(data).split('\n');
  for (const QString& raw : lines) {
    const QString line = raw.trimmed();
    if (line == "[POI]" || line == "[POLYLINE]" || line == "[POLYGON]") {
      objects.append(mp_object_t());
      current = &objects.last();
      current->kind = line.mid(1, line.size() - 2);
    } else if (line == "[END]") {
      current = nullptr;
    } else if (current != nullptr && line.startsWith("Type=")) {
      current->type = line.mid(5).toUInt(nullptr, 16);
    } else if (current != nullptr && line.startsWith("Label=")) {
      current->label = line.mid(6);
    } else if (current != nullptr && line.startsWith("Data0=")) {
      QRegularExpressionMatchIterator it = reCoord.globalMatch(line);
      while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        current->coords << QPointF(m.captured(1).toDouble(), m.captured(2).toDouble());
      }
    }
  }

  // The IMG format stores a polygon without its closing point.
  for (mp_object_t& object : objects) {
    if (object.kind == "POLYGON" && object.coords.size() > 1 && object.coords.first() == object.coords.last()) {
      object.coords.removeLast();
    }
  }
  return objects;
}

QPointF toDeg(const QPointF& rad) { return QPointF(qRadiansToDegrees(rad.y()), qRadiansToDegrees(rad.x())); }

/** Decode the most detailed level of @p path; returns that level's bits. */
quint8 readImg(const QString& path, QVector<img_object_t>& objects) {
  CGarminImg img(path);
  img.read();

  const QVector<CGarminImg::map_level_t>& levels = img.getMapLevels();
  if (levels.isEmpty()) {
    return 0;
  }
  const CGarminImg::map_level_t& level = levels.last();

  polytype_t polygons, polylines;
  pointtype_t points, pois;
  const QRectF world(-M_PI, M_PI / 2, 2 * M_PI, -M_PI);
  img.load(level.level, world, CGarminImg::filter_t(), [] { return false; }, polygons, polylines, points, pois);

  for (const pointtype_t* list : {&points, &pois}) {
    for (const CGarminPoint& p : *list) {
      objects.append({"POI", p.type, p.labels.join(" "), {toDeg(p.pos)}});
    }
  }
  for (const polytype_t* list : {&polylines, &polygons}) {
    for (const CGarminPolygon& p : *list) {
      img_object_t object{list == &polylines ? "POLYLINE" : "POLYGON", p.type, p.labels.join(" "), {}};
      for (const QPointF& c : p.coords) {
        object.coords << toDeg(c);
      }
      objects.append(object);
    }
  }
  return level.bits;
}
}  // namespace

void test_CGarminImg::decode_data() {
  QTest::addColumn<QString>("source");
  QTest::addColumn<QString>("image");

  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }

  const QDir sources(QMS_IMG_SOURCES);
  for (const QString& name : sources.entryList({"*.mp"}, QDir::Files, QDir::Name)) {
    const QString stem = QFileInfo(name).completeBaseName();
    QTest::newRow(qPrintable(stem)) << sources.filePath(name) << QDir(fixtureDir()).filePath(stem + ".img");
  }
}

void test_CGarminImg::decode() {
  QFETCH(QString, source);
  QFETCH(QString, image);

  if (!QFileInfo::exists(image)) {
    QSKIP(qPrintable("Test map not built: " + image));
  }

  const QVector<mp_object_t> expected = readMp(source);
  QVERIFY2(!expected.isEmpty(), qPrintable("No objects in " + source));

  QVector<img_object_t> decoded;
  quint8 bits = 0;
  try {
    bits = readImg(image, decoded);
  } catch (const CGarminImg::exce_t& e) {
    QFAIL(qPrintable(e.msg));
  }
  QVERIFY(bits > 0);

  // mkgmap adds a background polygon to every tile.
  decoded.erase(std::remove_if(decoded.begin(), decoded.end(),
                               [](const img_object_t& o) { return o.kind == "POLYGON" && o.type == 0x4b; }),
                decoded.end());

  // One unit of the level's resolution, plus rounding.
  const qreal tolerance = 360.0 / (1 << bits) + 1e-9;

  for (const mp_object_t& want : expected) {
    const QString what = QString("%1 0x%2 \"%3\"").arg(want.kind).arg(want.type, 0, 16).arg(want.label);
    auto match = std::find_if(decoded.begin(), decoded.end(), [&](const img_object_t& got) {
      return got.kind == want.kind && got.type == want.type && got.label == want.label;
    });
    QVERIFY2(match != decoded.end(), qPrintable("Not decoded: " + what));
    QCOMPARE(match->coords.size(), want.coords.size());
    for (qsizetype i = 0; i < want.coords.size(); ++i) {
      const QPointF d = match->coords[i] - want.coords[i];
      QVERIFY2(qAbs(d.x()) <= tolerance && qAbs(d.y()) <= tolerance,
               qPrintable(QString("%1 point %2 is off by %3, %4 deg").arg(what).arg(i).arg(d.x()).arg(d.y())));
    }
    decoded.erase(match);
  }

  for (const img_object_t& extra : decoded) {
    qWarning() << "Not in the source:" << extra.kind << Qt::hex << extra.type << extra.label;
  }
  QVERIFY2(decoded.isEmpty(), "Decoded objects that are not in the source");
}

void test_CGarminImg::load_data() {
  QTest::addColumn<QString>("image");

  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }

  const QDir sources(QMS_IMG_SOURCES);
  const QDir images(fixtureDir());
  qint32 rows = 0;
  for (const QString& name : images.entryList({"*.img"}, QDir::Files, QDir::Name)) {
    if (!sources.exists(QFileInfo(name).completeBaseName() + ".mp") && !name.endsWith("_mdr.img")) {
      QTest::newRow(qPrintable(name)) << images.filePath(name);
      ++rows;
    }
  }
  // QFETCH without a row is a fatal error in QtTest.
  if (rows == 0) {
    QSKIP("No IMG file without a source in QMS_IMG_FIXTURES");
  }
}

void test_CGarminImg::load() {
  QFETCH(QString, image);

  QVector<img_object_t> decoded;
  try {
    QVERIFY(readImg(image, decoded) > 0);
  } catch (const CGarminImg::exce_t& e) {
    QFAIL(qPrintable(e.msg));
  }
  QVERIFY(!decoded.isEmpty());
}

void test_CGarminImg::refuseIndex_data() {
  QTest::addColumn<QString>("image");

  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }

  const QDir images(fixtureDir());
  const QStringList names = images.entryList({"*_mdr.img"}, QDir::Files, QDir::Name);
  for (const QString& name : names) {
    QTest::newRow(qPrintable(name)) << images.filePath(name);
  }
  // QFETCH without a row is a fatal error in QtTest.
  if (names.isEmpty()) {
    QSKIP("No search index *_mdr.img in QMS_IMG_FIXTURES; build the test maps with make_fixtures.py");
  }
}

void test_CGarminImg::refuseIndex() {
  QFETCH(QString, image);

  CGarminImg img(image);
  try {
    img.read();
  } catch (const CGarminImg::exce_t& e) {
    QCOMPARE(e.err, CGarminImg::errFormat);
    QVERIFY2(e.msg.startsWith("The file holds no map tile"), qPrintable(e.msg));
    return;
  }
  QFAIL("A file without map tiles was read");
}

void test_CGarminImg::refuseLarger4GB() {
  // A sparse file: its size is all the check reads.
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QFile file(dir.filePath("large.img"));
  QVERIFY(file.open(QIODevice::WriteOnly));
  QVERIFY(file.resize(qint64(std::numeric_limits<quint32>::max()) + 1));
  file.close();

  CGarminImg img(file.fileName());
  try {
    img.read();
  } catch (const CGarminImg::exce_t& e) {
    QCOMPARE(e.err, CGarminImg::errFormat);
    QVERIFY2(e.msg.startsWith("The file is larger than 4 GB"), qPrintable(e.msg));
    return;
  }
  QFAIL("A file larger than 4 GB was read");
}

void test_CGarminImg::cutOffExtended() {
  // An extended point and line with label and one byte of extra data, as mkgmap writes them.
  const quint8 point[] = {0x11, 0xA1, 0x10, 0x00, 0x20, 0x00, 0x01, 0x00, 0x00, 0x05};
  const quint8 line[] = {0x10, 0xA0, 0x10, 0x00, 0x20, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05};

  for (qsizetype size = 0; size <= qsizetype(sizeof(point)); ++size) {
    CGarminPoint p;
    QCOMPARE(p.decode2(0, 0, 0, point, point + size), quint32(size));
    // Cut before the end of the label: nothing decoded. Only the extra data cut: the point itself is whole.
    QCOMPARE(p.type, size < 9 ? quint32(0) : quint32(0x11101));
  }

  for (qsizetype size = 0; size <= qsizetype(sizeof(line)); ++size) {
    CGarminPolygon p;
    QCOMPARE(p.decode2(0, 0, 0, true, line, line + size), quint32(size));
    QCOMPARE(p.pixel.isEmpty(), size < 13);
  }
}

QTEST_GUILESS_MAIN(test_CGarminImg)
#include "test_CGarminImg.moc"
