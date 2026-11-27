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
#include "map/garmin/CGarminTyp.h"
#include "map/garmin/IGarminStrTbl.h"

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
  void levelsAcrossTiles();
  void displayText_data();
  void displayText();
  void shieldCode_data();
  void shieldCode();
  void splitHeight_data();
  void splitHeight();
  void typStyles();
  void typNoCodePage();
};

namespace {
/** One object of a Polish-format source, coordinates in [deg] as lat, lon. */
struct mp_object_t {
  QString kind; /**< "POI", "POLYLINE" or "POLYGON" */
  quint32 type = 0;
  QStringList labels; /**< Label, Label2, Label3 */
  QVector<QPointF> coords;
};

/** One decoded object in the same terms. */
struct img_object_t {
  QString kind;
  quint32 type = 0;
  QStringList labels;
  QVector<QPointF> coords;
};

QString fixtureDir() { return qEnvironmentVariable("QMS_IMG_FIXTURES"); }

/** A Polish-format label with each "~[0xNN]" replaced by that character, as mkgmap reads it. */
QString unescape(const QString& label) {
  static const QRegularExpression reEscape(R"(~\[0x([0-9a-fA-F]{2})\])");
  QString result;
  qsizetype last = 0;
  QRegularExpressionMatchIterator it = reEscape.globalMatch(label);
  while (it.hasNext()) {
    const QRegularExpressionMatch m = it.next();
    result += QStringView(label).mid(last, m.capturedStart() - last);
    result += QChar(m.captured(1).toUShort(nullptr, 16));
    last = m.capturedEnd();
  }
  return result + label.mid(last);
}

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
      current->type = QStringView(line).mid(5).toUInt(nullptr, 16);
    } else if (current != nullptr &&
               (line.startsWith("Label=") || line.startsWith("Label2=") || line.startsWith("Label3="))) {
      current->labels << unescape(line.mid(line.indexOf('=') + 1));
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

/** Decode OUT_DIR/@p name with CGarminTyp; false if the file cannot be read or decoded. */
bool readTyp(const QString& name, CGarminTyp& typ, QMap<quint32, CGarminTyp::polygon_property>& polygons,
             QMap<quint32, CGarminTyp::polyline_property>& polylines, QList<quint32>& drawOrder,
             QMap<quint32, CGarminTyp::point_property>& points) {
  QFile file(QDir(fixtureDir()).filePath(name));
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }
  return typ.decode(file.readAll(), polygons, polylines, drawOrder, points);
}

/** A copy of @p img, unpremultiplied, to read its pixels. */
QImage toQImage(const BLImage& img) {
  BLImageData data;
  if (img.get_data(&data) != BL_SUCCESS || data.pixel_data == nullptr) {
    return QImage();
  }
  const QImage view(static_cast<const uchar*>(data.pixel_data), data.size.w, data.size.h, data.stride,
                    QImage::Format_ARGB32_Premultiplied);
  return view.convertToFormat(QImage::Format_ARGB32);
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
  img.load(level.bits, world, CGarminImg::filter_t(), [] { return false; }, polygons, polylines, points, pois);

  for (const pointtype_t* list : {&points, &pois}) {
    for (const CGarminPoint& p : *list) {
      objects.append({"POI", p.type, p.labels, {toDeg(p.pos)}});
    }
  }
  for (const polytype_t* list : {&polylines, &polygons}) {
    for (const CGarminPolygon& p : *list) {
      img_object_t object{list == &polylines ? "POLYLINE" : "POLYGON", p.type, p.labels, {}};
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
  decoded.removeIf([](const img_object_t& o) { return o.kind == "POLYGON" && o.type == 0x4b; });

  // One unit of the level's resolution, plus rounding.
  const qreal tolerance = 360.0 / (1 << bits) + 1e-9;

  for (const mp_object_t& want : expected) {
    const QString what = QString("%1 0x%2 \"%3\"").arg(want.kind).arg(want.type, 0, 16).arg(want.labels.join("\", \""));
    auto match = std::find_if(decoded.begin(), decoded.end(), [&](const img_object_t& got) {
      return got.kind == want.kind && got.type == want.type && got.labels == want.labels;
    });
    QVERIFY2(match != decoded.end(), qPrintable("Not decoded: " + what));
    QCOMPARE(match->coords.size(), want.coords.size());
    for (qsizetype i = 0; i < want.coords.size(); ++i) {
      const QPointF d = match->coords[i] - want.coords[i];
      QVERIFY2(qAbs(d.x()) <= tolerance && qAbs(d.y()) <= tolerance,
               qPrintable(QString("%1 point %2 is off by %3, %4 deg").arg(what).arg(i).arg(d.x()).arg(d.y())));
    }
    decoded.removeAt(std::distance(decoded.begin(), match));
  }

  for (const img_object_t& extra : decoded) {
    qWarning() << "Not in the source:" << extra.kind << Qt::hex << extra.type << extra.labels;
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

void test_CGarminImg::levelsAcrossTiles() {
  // levels.img: tile A with data at 24 bits, tile B at 24 and 20 bits (mp/levels/). Each tile draws at its own
  // level for every resolution, so both roads are there however far out the map is zoomed.
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  const QString path = QDir(fixtureDir()).filePath("levels.img");
  if (!QFileInfo::exists(path)) {
    QSKIP(qPrintable("Test map not built: " + path));
  }

  CGarminImg img(path);
  img.read();
  const QRectF world(-M_PI, M_PI / 2, 2 * M_PI, -M_PI);
  for (quint8 bits = 16; bits <= 24; ++bits) {
    polytype_t polygons, polylines;
    pointtype_t points, pois;
    img.load(bits, world, CGarminImg::filter_t(), [] { return false; }, polygons, polylines, points, pois);
    QStringList labels;
    for (const CGarminPolygon& line : polylines) {
      labels << line.labels.join(" ");
    }
    labels.sort();
    QCOMPARE(labels, QStringList({"A Road", "B Road"}));
  }
}

void test_CGarminImg::displayText_data() {
  QTest::addColumn<QString>("label");
  QTest::addColumn<QString>("text");

  // As BaseCamp 4.7.5 shows these labels zoomed in.
  QTest::newRow("plain") << "Plain Road" << "Plain Road";
  QTest::newRow("prefix 0x1b") << unescape("Rue d'~[0x1b]Alsace") << "Rue d'Alsace";
  QTest::newRow("prefix 0x1e") << unescape("Rue de la~[0x1e]Concorde") << "Rue de la Concorde";
  QTest::newRow("suffix 0x1c") << unescape("Haupt~[0x1c]strasse") << "Hauptstrasse";
  QTest::newRow("suffix 0x1f") << unescape("Main~[0x1f]Street") << "Main Street";
  QTest::newRow("both") << unescape("Carrer d'~[0x1b]Europa~[0x1c]kalea") << "Carrer d'Europakalea";
  QTest::newRow("0x1d") << unescape("Alpha~[0x1d]Beta") << "Alpha";
  QTest::newRow("shield") << unescape("~[0x04]A7") << "A7";
  QTest::newRow("code inside") << unescape("A~[0x02]7") << "A7";
}

void test_CGarminImg::displayText() {
  QFETCH(QString, label);
  QFETCH(QString, text);
  QCOMPARE(IGarminStrTbl::displayText(label), text);
}

void test_CGarminImg::shieldCode_data() {
  QTest::addColumn<QString>("label");
  QTest::addColumn<int>("code");

  // mkgmap's highway-symbol: interstate, shield, round, hbox, box, oval
  for (int code = 1; code <= 6; ++code) {
    QTest::newRow(qPrintable(QString("code %1").arg(code))) << QString(QChar(code)) + "A7" << code;
  }
  QTest::newRow("none") << "A7" << 0;
  QTest::newRow("empty") << "" << 0;
}

void test_CGarminImg::shieldCode() {
  QFETCH(QString, label);
  QFETCH(int, code);
  QCOMPARE(int(IGarminStrTbl::shieldCode(label)), code);
}

void test_CGarminImg::splitHeight_data() {
  QTest::addColumn<QString>("label");
  QTest::addColumn<bool>("hasHeight");
  QTest::addColumn<QString>("name");
  QTest::addColumn<qreal>("feet");

  QTest::newRow("name and height") << unescape("Gipfel~[0x1f]1234") << true << "Gipfel" << 1234.0;
  QTest::newRow("height only") << unescape("~[0x1f]1234") << true << "" << 1234.0;
  QTest::newRow("suffix") << unescape("Main~[0x1f]Street") << false << "" << 0.0;
  QTest::newRow("no separator") << "Spitze 1234" << false << "" << 0.0;
}

void test_CGarminImg::splitHeight() {
  QFETCH(QString, label);
  QFETCH(bool, hasHeight);
  QFETCH(QString, name);
  QFETCH(qreal, feet);

  QString gotName;
  qreal gotFeet = 0;
  QCOMPARE(IGarminStrTbl::splitHeight(label, gotName, gotFeet), hasHeight);
  QCOMPARE(gotName, name);
  QCOMPARE(gotFeet, feet);
}

void test_CGarminImg::typStyles() {
  // typ/styles.txt, compiled by make_fixtures.py
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  if (!QFileInfo::exists(QDir(fixtureDir()).filePath("styles.typ"))) {
    QSKIP("Test TYP file not built: styles.typ");
  }

  CGarminTyp typ;
  QMap<quint32, CGarminTyp::polygon_property> polygons;
  QMap<quint32, CGarminTyp::polyline_property> polylines;
  QList<quint32> drawOrder;
  QMap<quint32, CGarminTyp::point_property> points;
  QVERIFY(readTyp("styles.typ", typ, polygons, polylines, drawOrder, points));

  QCOMPARE(typ.getFid(), quint16(6357));
  QCOMPARE(typ.getPid(), quint16(2));

  // Three colours and a transparent fourth: line, border, night line; the label follows the widths.
  QVERIFY(polylines.contains(0x01));
  const CGarminTyp::polyline_property& line = polylines[0x01];
  QCOMPARE(line.penLineDay.color(), QColor(255, 0, 0));
  QCOMPARE(line.penBorderDay.color(), QColor(0, 0, 0));
  QCOMPARE(line.penLineNight.color(), QColor(0, 255, 0));
  QCOMPARE(line.penLineDay.width(), 4);
  QCOMPARE(line.strings.value(0x04), QString("Line four colours"));
  QCOMPARE(line.strings.keys(), QList<int>({0x04}));
  QVERIFY(line.hasBorder);
  QCOMPARE(line.penBorderDay.width(), 8);

  // Extended types keep their full number.
  QVERIFY2(polylines.contains(0x18a00), "extended line 0x18a00 missing");
  QCOMPARE(polylines[0x18a00].strings.value(0x04), QString("Line ext"));
  QVERIFY2(polygons.contains(0x18a00), "extended polygon 0x18a00 missing");
  QCOMPARE(polygons[0x18a00].strings.value(0x04), QString("Area ext"));

  // Per-colour transparency: an opaque colour stays fully opaque.
  QVERIFY(points.contains(0x2f06));
  const QImage alpha = toQImage(points[0x2f06].imgDay);
  QCOMPARE(alpha.size(), QSize(4, 2));
  QCOMPARE(QColor::fromRgba(alpha.pixel(0, 0)), QColor(255, 0, 0, 255));
  QVERIFY2(qAbs(qAlpha(alpha.pixel(2, 0)) - 0x88) <= 0x11, qPrintable(QString::number(qAlpha(alpha.pixel(2, 0)))));
  QCOMPARE(qAlpha(alpha.pixel(0, 1)), 0);

  // True colour: one colour per pixel, no colour table.
  QVERIFY(points.contains(0x2f07));
  const QImage rgb = toQImage(points[0x2f07].imgDay);
  QCOMPARE(rgb.size(), QSize(2, 2));
  QCOMPARE(QColor::fromRgba(rgb.pixel(0, 0)), QColor(255, 0, 0));
  QCOMPARE(QColor::fromRgba(rgb.pixel(1, 0)), QColor(0, 255, 0));
  QCOMPARE(QColor::fromRgba(rgb.pixel(0, 1)), QColor(0, 0, 255));
  QCOMPARE(QColor::fromRgba(rgb.pixel(1, 1)), QColor(255, 255, 255));
  QCOMPARE(points[0x2f07].strings.value(0x04), QString("True colour icon"));

  // True colour with a transparent pixel: the transparent colour comes first.
  QVERIFY(points.contains(0x2f08));
  const QImage clear = toQImage(points[0x2f08].imgDay);
  QCOMPARE(clear.size(), QSize(2, 2));
  QCOMPARE(QColor::fromRgba(clear.pixel(0, 1)), QColor(0, 0, 255));
  QCOMPARE(qAlpha(clear.pixel(1, 1)), 0);
  QCOMPARE(points[0x2f08].strings.value(0x04), QString("True colour clear pixel"));

  // True colour with transparency: 4 bit after each pixel.
  QVERIFY(points.contains(0x2f09));
  const QImage half = toQImage(points[0x2f09].imgDay);
  QCOMPARE(half.size(), QSize(2, 2));
  QCOMPARE(QColor::fromRgba(half.pixel(1, 0)), QColor(0, 255, 0));
  QVERIFY2(qAbs(qAlpha(half.pixel(1, 1)) - 0x88) <= 0x11, qPrintable(QString::number(qAlpha(half.pixel(1, 1)))));
  QCOMPARE(points[0x2f09].strings.value(0x04), QString("True colour half pixel"));

  // An odd width: the 28 bit pixels run on across rows without padding.
  QVERIFY(points.contains(0x2f0a));
  const QImage odd = toQImage(points[0x2f0a].imgDay);
  QCOMPARE(odd.size(), QSize(3, 2));
  QCOMPARE(QColor::fromRgba(odd.pixel(2, 0)), QColor(0, 0, 255));
  QCOMPARE(QColor::fromRgba(odd.pixel(0, 1)), QColor(255, 255, 0));
  QVERIFY2(qAbs(qAlpha(odd.pixel(2, 1)) - 0x88) <= 0x11, qPrintable(QString::number(qAlpha(odd.pixel(2, 1)))));
  QCOMPARE(points[0x2f0a].strings.value(0x04), QString("True colour odd width"));
}

void test_CGarminImg::typNoCodePage() {
  // typ/nocodepage.txt: mkgmap writes code page 0 without --code-page.
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  if (!QFileInfo::exists(QDir(fixtureDir()).filePath("nocodepage.typ"))) {
    QSKIP("Test TYP file not built: nocodepage.typ");
  }

  CGarminTyp typ;
  QMap<quint32, CGarminTyp::polygon_property> polygons;
  QMap<quint32, CGarminTyp::polyline_property> polylines;
  QList<quint32> drawOrder;
  QMap<quint32, CGarminTyp::point_property> points;
  QVERIFY(readTyp("nocodepage.typ", typ, polygons, polylines, drawOrder, points));
  QVERIFY(polylines.contains(0x01));
  QCOMPARE(polylines[0x01].strings.value(0x04), QString("Line no code page"));
}

QTEST_GUILESS_MAIN(test_CGarminImg)
#include "test_CGarminImg.moc"
