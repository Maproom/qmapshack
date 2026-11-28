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

#include "helpers/CFileExt.h"
#include "map/garmin/CGarminImg.h"
#include "map/garmin/CGarminMarine.h"
#include "map/garmin/CGarminPlaces.h"
#include "map/garmin/CGarminPoint.h"
#include "map/garmin/CGarminPolygon.h"
#include "map/garmin/CGarminTyp.h"
#include "map/garmin/CMarineSymbol.h"
#include "map/garmin/CPoiSymbol.h"
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
  void mapFamilies();
  void displayText_data();
  void displayText();
  void shieldCode_data();
  void shieldCode();
  void shortText_data();
  void shortText();
  void shortTextOfLine();
  void splitHeight_data();
  void splitHeight();
  void typStyles();
  void typNoCodePage();
  void typIconSet();
  void marineBytes_data();
  void marineBytes();
  void marine();
  void lightCharacteristic_data();
  void lightCharacteristic();
  void facilities();
  void isHeight();
  void marineTypes();
  void base11_data();
  void base11();
  void poiDetails();
  void exits();
  void copyrights();
  void infoTexts();
  void facilityText_data();
  void facilityText();
  void poiSymbols();
  void typName_data();
  void typName();
  void languageCode_data();
  void languageCode();
  void pointLabelBox_data();
  void pointLabelBox();
  void placesRecords();
  void corruptPlaces();
};

namespace {
/** One object of a Polish-format source, coordinates in [deg] as lat, lon. */
struct mp_object_t {
  QString kind; /**< "POI", "POLYLINE" or "POLYGON" */
  quint32 type = 0;
  QStringList labels; /**< Label, Label2, Label3 */
  QVector<QPointF> coords;
  QMap<QString, QString> attributes; /**< every other key, e.g. Depth */
};

/** One decoded object in the same terms. */
struct img_object_t {
  QString kind;
  quint32 type = 0;
  QStringList labels;
  QVector<QPointF> coords;
  QByteArray extra;
  QMap<QString, QString> details; /**< a POI's address and phone, by their Polish-format keys */
};

/** The Polish-format keys of a POI's address and phone. */
const QStringList& detailKeys() {
  static const QStringList keys = {"HouseNumber", "StreetDesc", "CityName", "RegionName",
                                   "CountryName", "ZipCode",    "Phone"};
  return keys;
}

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
    } else if (current != nullptr && line.contains('=')) {
      const qsizetype eq = line.indexOf('=');
      current->attributes[line.left(eq)] = line.mid(eq + 1);
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

/** Labels named after their offset, "L<offset>", without a file. */
class CFakeStrTbl : public IGarminStrTbl {
 public:
  CFakeStrTbl() : IGarminStrTbl(0, 0, nullptr) {}

 protected:
  void decode(CFileExt& /*file*/, quint32 offset, type_e /*t*/, QStringList& info) override {
    info << QString("L%1").arg(offset);
  }
};

void put(QByteArray& buf, qsizetype at, std::initializer_list<quint8> bytes) {
  if (buf.size() < at + qsizetype(bytes.size())) {
    buf.resize(at + bytes.size(), 0);
  }
  for (const quint8 b : bytes) {
    buf[at++] = char(b);
  }
}

void put3(QByteArray& buf, qsizetype at, quint32 v) { put(buf, at, {quint8(v), quint8(v >> 8), quint8(v >> 16)}); }

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
      const QStringList values = {p.houseNumber, p.street, p.city, p.region, p.country, p.zip, p.phone};
      QMap<QString, QString> details;
      for (qsizetype i = 0; i < detailKeys().size(); ++i) {
        if (!values[i].isEmpty()) {
          details[detailKeys()[i]] = values[i];
        }
      }
      objects.append({"POI", p.type, p.labels, {toDeg(p.pos)}, p.extra, details});
    }
  }
  for (const polytype_t* list : {&polylines, &polygons}) {
    for (const CGarminPolygon& p : *list) {
      img_object_t object{list == &polylines ? "POLYLINE" : "POLYGON", p.type, p.labels, {}, p.extra, {}};
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

void test_CGarminImg::mapFamilies() {
  // families.img: tile A of family 7017 and tile B of family 7018, each with its own TYP (mp/families/). Each tile
  // takes its family from the map list, and so does every object loaded from it.
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  const QString path = QDir(fixtureDir()).filePath("families.img");
  if (!QFileInfo::exists(path)) {
    QSKIP(qPrintable("Test map not built: " + path));
  }

  CGarminImg img(path);
  img.read();
  QList<quint16> families;
  for (const CGarminImg::subfile_desc_t& subfile : img.getSubfiles()) {
    if (subfile.parts.contains("TRE")) {
      families << subfile.family;
    }
  }
  std::sort(families.begin(), families.end());
  QCOMPARE(families, QList<quint16>({7017, 7018}));

  polytype_t polygons, polylines;
  pointtype_t points, pois;
  const QRectF world(-M_PI, M_PI / 2, 2 * M_PI, -M_PI);
  img.load(24, world, CGarminImg::filter_t(), [] { return false; }, polygons, polylines, points, pois);
  QMap<QString, quint16> byLabel;
  for (const CGarminPolygon& line : polylines) {
    byLabel[line.labels.join(" ")] = line.family;
  }
  QCOMPARE(byLabel, (QMap<QString, quint16>{{"A Line", 7017}, {"B Line", 7018}}));
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

void test_CGarminImg::shortText_data() {
  QTest::addColumn<QString>("label");
  QTest::addColumn<QString>("text");

  // As BaseCamp 4.7.5 shows these labels zoomed out.
  QTest::newRow("plain") << "Plain Road" << "Plain Road";
  QTest::newRow("prefix 0x1b") << unescape("Rue d'~[0x1b]Alsace") << "Alsace";
  QTest::newRow("prefix 0x1e") << unescape("Rue de la~[0x1e]Concorde") << "Concorde";
  QTest::newRow("suffix 0x1c") << unescape("Haupt~[0x1c]strasse") << "Haupt";
  QTest::newRow("suffix 0x1f") << unescape("Main~[0x1f]Street") << "Main";
  QTest::newRow("both, blanks") << unescape("West~[0x1e]Lincoln~[0x1f]Road") << "Lincoln";
  QTest::newRow("both") << unescape("Carrer d'~[0x1b]Europa~[0x1c]kalea") << "Europa";
  QTest::newRow("0x1d") << unescape("Alpha~[0x1d]Beta") << "Alpha";
  QTest::newRow("shield") << unescape("~[0x04]A7") << "A7";
  QTest::newRow("empty core") << unescape("~[0x1f]Street") << "Street";
}

void test_CGarminImg::shortText() {
  QFETCH(QString, label);
  QFETCH(QString, text);
  QCOMPARE(IGarminStrTbl::shortText(label), text);
}

void test_CGarminImg::shortTextOfLine() {
  CGarminPolygon line;
  line.type = 0x06;
  QCOMPARE(line.getShortText(), QString());
  line.labels << unescape("West~[0x1e]Lincoln~[0x1f]Road") << "Second";
  QCOMPARE(line.getShortText(), QString("Lincoln"));
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
  // a built-in name the TYP's names replace
  polylines[0x01].strings[0x00] = "built-in";
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
  QCOMPARE(line.firstLanguage, quint8(0x04));
  QVERIFY(line.hasBorder);
  QCOMPARE(line.penBorderDay.width(), 8);

  // A line bitmap keeps its rows in order: the first row is drawn left of the line's direction.
  QVERIFY(polylines.contains(0x03));
  QVERIFY(polylines[0x03].hasPixmap);
  const QImage bitmap = toQImage(polylines[0x03].imgDay);
  QCOMPARE(bitmap.size(), QSize(32, 3));
  QCOMPARE(QColor::fromRgba(bitmap.pixel(31, 0)), QColor(255, 0, 0));
  QCOMPARE(QColor::fromRgba(bitmap.pixel(0, 1)), QColor(255, 0, 0));
  QCOMPARE(qAlpha(bitmap.pixel(4, 1)), 0);
  QCOMPARE(qAlpha(bitmap.pixel(0, 2)), 0);

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

void test_CGarminImg::typIconSet() {
  // typ/iconset.txt: [_icons] sets are not drawn, as in BaseCamp; the points around them still decode.
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  if (!QFileInfo::exists(QDir(fixtureDir()).filePath("iconset.typ"))) {
    QSKIP("Test TYP file not built: iconset.typ");
  }

  CGarminTyp typ;
  QMap<quint32, CGarminTyp::polygon_property> polygons;
  QMap<quint32, CGarminTyp::polyline_property> polylines;
  QList<quint32> drawOrder;
  QMap<quint32, CGarminTyp::point_property> points;
  QVERIFY(readTyp("iconset.typ", typ, polygons, polylines, drawOrder, points));
  QVERIFY(points.contains(0x2f04));
  QCOMPARE(toQImage(points[0x2f04].imgDay).size(), QSize(16, 16));
  QCOMPARE(points[0x2f04].strings.value(0x04), QString("P04 point and set"));
  QVERIFY(points.contains(0x2f06));
  QVERIFY(!points.contains(0x2f05));
}

namespace {
QString distanceText(const CGarminMarine::distance_t& d) {
  return QString("%1 %2").arg(d.value).arg(d.metric ? "m" : "ft");
}

/** All decoded fields of @p m as one line; LBL offsets only if @p withOffsets. */
QString describe(const CGarminMarine& m, bool withOffsets) {
  QStringList parts;
  if (m.depth.valid) {
    parts << "depth " + distanceText(m.depth);
  }
  if (m.position != 0) {
    parts << QString("position %1").arg(m.position);
  }
  if (m.colour != 0) {
    parts << QString("colour %1").arg(CGarminMarine::colourName(m.colour));
  }
  if (m.lineStyle != 0) {
    parts << QString("style 0x%1").arg(m.lineStyle, 0, 16);
  }
  if (m.lightType != 0) {
    parts << QString("type %1").arg(CGarminMarine::lightTypeName(m.lightType));
  }
  if (m.morse != 0) {
    parts << QString("morse %1").arg(QChar(m.morse));
  }
  for (const CGarminMarine::light_t& l : m.lights) {
    parts << QString("light %1 %2 nm at %3").arg(CGarminMarine::lightColourName(l.colour)).arg(l.range).arg(l.angle);
  }
  if (m.period != 0) {
    parts << QString("period %1 s").arg(m.period);
  }
  if (!m.phases.isEmpty()) {
    QStringList phases;
    for (qreal p : m.phases) {
      phases << QString::number(p);
    }
    parts << "phases " + phases.join(',');
  }
  if (m.racon) {
    parts << "racon";
  }
  if (m.heightAboveFoundation.valid) {
    parts << "above foundation " + distanceText(m.heightAboveFoundation);
  }
  if (m.heightAboveDatum.valid) {
    parts << "above datum " + distanceText(m.heightAboveDatum);
  }
  if (m.leadingAngle >= 0) {
    parts << QString("leading %1").arg(m.leadingAngle);
  }
  const QList<QPair<const char*, quint32>> labels = {
      {"note", m.note}, {"local", m.localDesignator}, {"international", m.internationalDesignator}};
  for (const auto& [name, offset] : labels) {
    if (offset != CGarminMarine::kNoLabel) {
      parts << (withOffsets ? QString("%1 0x%2").arg(name).arg(offset, 0, 16) : QString(name));
    }
  }
  if (m.facilities != 0) {
    parts << QString("facilities 0x%1").arg(m.facilities, 0, 16);
  }
  return parts.join("; ");
}

/** Index of @p name in a name list of CGarminMarine, 0 if not found. */
quint8 indexOf(const char* (*names)(quint8), const QString& name) {
  for (quint8 i = 1; i < 32; ++i) {
    if (name.compare(names(i), Qt::CaseInsensitive) == 0) {
      return i;
    }
  }
  return 0;
}

CGarminMarine::distance_t expectedDistance(const QMap<QString, QString>& attributes, const QString& key) {
  CGarminMarine::distance_t d;
  if (attributes.contains(key)) {
    d.valid = true;
    d.value = attributes[key].toDouble();
    d.metric = attributes.value(key + "Unit") != "f";
  }
  return d;
}

/** The extra data an mkgmap source asks for, in CGarminMarine's terms (PolishMapDataSource.makeExtTypeAttributes). */
CGarminMarine expectedMarine(const mp_object_t& object) {
  const QMap<QString, QString>& a = object.attributes;
  const quint32 t = object.type & 0xFFFF;
  CGarminMarine m;

  m.depth = expectedDistance(a, a.contains("Depth") ? "Depth" : "Height");
  if (a.contains("Position")) {
    const QStringList positions = {"unknown", "", "doubtful", "existence doubtful", "approximate", "reported"};
    m.position = positions.indexOf(a["Position"]);
  }
  // Color and Style form mkgmap's style value
  const quint32 style = (a.value("Style", "0").toUInt(nullptr, 0) << 8) | a.value("Color", "0").toUInt(nullptr, 0);
  if (object.kind == "POLYLINE" || (t & 0xFF00) == 0x0500) {
    m.colour = style & 0x0F;
    // mkgmap keeps bits 0-1 and 4-5 of the style byte
    m.lineStyle = (style >> 8) & 0x33;
  }
  if (a.contains("FoundationColor")) {
    m.colour = indexOf(CGarminMarine::colourName, a["FoundationColor"]);
  }

  if (a.contains("LightType")) {
    const QString type = a["LightType"];
    if (type.size() == 1) {
      m.lightType = 11;
      m.morse = type[0].toLatin1();
    } else {
      m.lightType = indexOf(CGarminMarine::lightTypeName, type);
    }
  }
  static const QRegularExpression reLight(R"(\(([a-z]+),([0-9.]+),([0-9.]+)\))");
  QRegularExpressionMatchIterator it = reLight.globalMatch(a.value("Light"));
  const bool isBuoy = (t & 0xFF00) == 0x0200;
  while (it.hasNext()) {
    const QRegularExpressionMatch match = it.next();
    CGarminMarine::light_t light;
    light.colour = indexOf(CGarminMarine::lightColourName, match.captured(1));
    // a buoy keeps the light colour only
    light.range = isBuoy ? 0 : quint8(match.captured(2).toDouble());
    light.angle = isBuoy ? 0 : qint32(match.captured(3).toDouble() * 10) / 10.0;
    m.lights << light;
  }
  if (a.contains("Period")) {
    for (const QString& p : a["Period"].split(',')) {
      m.period += qint32(p.toDouble() * 10) / 10.0;
    }
  }
  m.racon = a.contains("Racon");
  // BaseCamp reads mkgmap's two light heights the other way round
  m.heightAboveDatum = expectedDistance(a, "HeightAboveFoundation");
  m.heightAboveFoundation = expectedDistance(a, "HeightAboveDatum");
  if (a.contains("LeadingAngle")) {
    m.leadingAngle = qint32(a["LeadingAngle"].toDouble() * 10) / 10.0;
  }
  m.note = a.contains("Note") ? 0 : CGarminMarine::kNoLabel;
  m.localDesignator = a.contains("LocalDesignator") ? 0 : CGarminMarine::kNoLabel;
  m.internationalDesignator = a.contains("InternationalDesignator") ? 0 : CGarminMarine::kNoLabel;
  m.facilities = a.value("FacilityPoint", "0").toUInt(nullptr, 0);
  return m;
}

CGarminMarine::Kind kindOf(const QString& kind) {
  return kind == "POI" ? CGarminMarine::Kind::ePoint
                       : (kind == "POLYLINE" ? CGarminMarine::Kind::eLine : CGarminMarine::Kind::ePolygon);
}
}  // namespace

void test_CGarminImg::marineBytes_data() {
  QTest::addColumn<quint32>("type");
  QTest::addColumn<int>("kind");
  QTest::addColumn<QByteArray>("bytes");
  QTest::addColumn<QString>("text");

  const int point = int(CGarminMarine::Kind::ePoint);
  const int line = int(CGarminMarine::Kind::eLine);
  const int polygon = int(CGarminMarine::Kind::ePolygon);

  // Bytes as mkgmap r4924 ExtTypeAttributes writes them.
  QTest::newRow("depth m") << 0x10301u << point << QByteArray::fromHex("900c") << "depth 12 m";
  QTest::newRow("depth tenths") << 0x10301u << point << QByteArray::fromHex("9837") << "depth 5.5 m";
  QTest::newRow("depth 2 bytes") << 0x10301u << point << QByteArray::fromHex("b02c01") << "depth 300 m";
  QTest::newRow("obstruction ft") << 0x10400u << point << QByteArray::fromHex("8403") << "depth 3 ft; position 4";
  QTest::newRow("depth line") << 0x10301u << line << QByteArray::fromHex("900a") << "depth 10 m";
  QTest::newRow("depth area") << 0x10301u << polygon << QByteArray::fromHex("984b") << "depth 7.5 m";
  QTest::newRow("label colour") << 0x10500u << point << QByteArray::fromHex("03") << "colour yellow";
  QTest::newRow("line colour") << 0x10400u << line << QByteArray::fromHex("01") << "colour red";
  QTest::newRow("line style") << 0x10500u << line << QByteArray::fromHex("8201") << "colour green; style 0x1";
  QTest::newRow("line style high") << 0x10600u << line << QByteArray::fromHex("840b") << "colour white; style 0x13";
  QTest::newRow("facilities") << 0x10903u << point << QByteArray::fromHex("a10110") << "facilities 0x10021";
  QTest::newRow("light") << 0x10100u << point << QByteArray::fromHex("e00d800022282a01")
                         << "light red 10 nm at 0; period 4 s";
  // nob counts a single-light byte that is not written with several lights: one trailing 0
  QTest::newRow("light sectors") << 0x10101u << point << QByteArray::fromHex("e0218008260c3200100ab0240860b90c0100")
                                 << "light red 10 nm at 0; light green 8 nm at 120; light white 12 nm at 240; "
                                    "period 5 s; above foundation 12 m";
  QTest::newRow("light morse") << 0x10102u << point << QByteArray::fromHex("e00d8b0122000041")
                               << "type morse code letter; morse A";
  QTest::newRow("light note") << 0x10103u << point << QByteArray::fromHex("e213800022005634120001") << "note 0x123456";
  QTest::newRow("buoy") << 0x10206u << point << QByteArray::fromHex("e00b8100801e01")
                        << "colour red; light green 0 nm at 0; period 3 s";
  QTest::newRow("no extra data type") << 0x10e00u << line << QByteArray::fromHex("01") << "";
}

void test_CGarminImg::marineBytes() {
  QFETCH(quint32, type);
  QFETCH(int, kind);
  QFETCH(QByteArray, bytes);
  QFETCH(QString, text);

  const CGarminMarine m = CGarminMarine::decode(type, CGarminMarine::Kind(kind), bytes);
  QCOMPARE(m.isEmpty(), text.isEmpty());
  QCOMPARE(describe(m, true), text);
}

void test_CGarminImg::marine() {
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  const QString image = QDir(fixtureDir()).filePath("marine.img");
  if (!QFileInfo::exists(image)) {
    QSKIP(qPrintable("Test map not built: " + image));
  }

  QVector<img_object_t> decoded;
  try {
    QVERIFY(readImg(image, decoded) > 0);
  } catch (const CGarminImg::exce_t& e) {
    QFAIL(qPrintable(e.msg));
  }

  const QVector<mp_object_t> sources = readMp(QDir(QMS_IMG_SOURCES).filePath("marine.mp"));
  QVERIFY(!sources.isEmpty());
  for (const mp_object_t& want : sources) {
    const QString what = QString("%1 0x%2 \"%3\"").arg(want.kind).arg(want.type, 0, 16).arg(want.labels.join(", "));
    auto match = std::find_if(decoded.begin(), decoded.end(), [&](const img_object_t& got) {
      return got.kind == want.kind && got.type == want.type && got.labels == want.labels;
    });
    QVERIFY2(match != decoded.end(), qPrintable("Not decoded: " + what));

    const CGarminMarine m = CGarminMarine::decode(match->type, kindOf(match->kind), match->extra);
    QVERIFY2(!m.isEmpty(), qPrintable("No extra data decoded: " + what + ", bytes " + match->extra.toHex()));
    const QString got = describe(m, false);
    const QString expected = describe(expectedMarine(want), false);
    QVERIFY2(got == expected, qPrintable(QString("%1\n   got: %2\n  want: %3").arg(what, got, expected)));
  }
}

void test_CGarminImg::lightCharacteristic_data() {
  QTest::addColumn<quint32>("type");
  QTest::addColumn<QByteArray>("bytes");
  QTest::addColumn<QString>("characteristic");
  QTest::addColumn<QString>("sectors");

  // Chart notation, INT 1 P16: character, colours, period, elevation, range.
  QTest::newRow("single") << 0x10100u << QByteArray::fromHex("e00d800022282a01") << "R.4s10M" << "";
  QTest::newRow("sectors") << 0x10101u << QByteArray::fromHex("e0218008260c3200100ab0240860b90c0100") << "RGW.5s12-8M"
                           << "R 0°-120° 10M; G 120°-240° 8M; W 240°-0° 12M";
  QTest::newRow("morse") << 0x10102u << QByteArray::fromHex("e00d8b0122000041") << "Mo(A)" << "";
  QTest::newRow("buoy") << 0x10206u << QByteArray::fromHex("e00b8100801e01") << "G.3s" << "";
  // white alone is not written; two flashes per group from the phases record
  QTest::newRow("group known") << 0x10100u << QByteArray::fromHex("e01584012232658205050a1e") << "Fl(2).5s5M" << "";
  QTest::newRow("group unknown") << 0x10100u << QByteArray::fromHex("e00d8400220a4501") << "Fl(…)G.1s5M" << "";
  QTest::newRow("no light") << 0x10301u << QByteArray::fromHex("900c") << "" << "";
}

void test_CGarminImg::lightCharacteristic() {
  QFETCH(quint32, type);
  QFETCH(QByteArray, bytes);
  QFETCH(QString, characteristic);
  QFETCH(QString, sectors);

  const CGarminMarine m = CGarminMarine::decode(type, CGarminMarine::Kind::ePoint, bytes);
  QVERIFY(!m.isEmpty());
  QCOMPARE(m.lightCharacteristic(), characteristic);
  QCOMPARE(m.sectorTexts().join("; "), sectors);
}

void test_CGarminImg::facilities() {
  const CGarminMarine m = CGarminMarine::decode(0x10903, CGarminMarine::Kind::ePoint, QByteArray::fromHex("a10110"));
  QCOMPARE(m.facilityNames(), QStringList({"boat ramp", "marina", "handicap accessible"}));
}

void test_CGarminImg::isHeight() {
  // as BaseCamp 4.7.5 labels them
  QVERIFY(!CGarminMarine::isHeight(0x10301, CGarminMarine::Kind::ePoint));
  QVERIFY(CGarminMarine::isHeight(0x10302, CGarminMarine::Kind::ePoint));
  QVERIFY(CGarminMarine::isHeight(0x10400, CGarminMarine::Kind::ePoint));
  QVERIFY(!CGarminMarine::isHeight(0x10301, CGarminMarine::Kind::eLine));
  QVERIFY(CGarminMarine::isHeight(0x10105, CGarminMarine::Kind::eLine));
  QVERIFY(!CGarminMarine::isHeight(0x10301, CGarminMarine::Kind::ePolygon));
}

void test_CGarminImg::marineTypes() {
  // standard types share the low bytes with marine ones: a city 0x0200 is no buoy
  QVERIFY(!CMarineSymbol::hasSymbol(0x0200));
  QVERIFY(!CMarineSymbol::hasSymbol(0x0209));
  QVERIFY(CMarineSymbol::hasSymbol(0x10200));
  QVERIFY(CMarineSymbol::hasSymbol(0x10100));
  QVERIFY(CMarineSymbol::hasSymbol(0x10400));
  QVERIFY(!CMarineSymbol::hasSymbol(0x11100));
  // only the line ranges with marine extra data
  QVERIFY(CGarminMarine::isMarineLine(0x10301));
  QVERIFY(CGarminMarine::isMarineLine(0x10105));
  QVERIFY(CGarminMarine::isMarineLine(0x10600));
  QVERIFY(!CGarminMarine::isMarineLine(0x10e00));
  QVERIFY(!CGarminMarine::isMarineLine(0x0301));
}

void test_CGarminImg::base11_data() {
  QTest::addColumn<QByteArray>("bytes");
  QTest::addColumn<QString>("number");
  QTest::addColumn<qint32>("size");

  // as mkgmap's POIRecord writes them: c1 * 11 + c2, '-' is 10, bit 7 on the first and the last byte
  QTest::newRow("two digits") << QByteArray::fromHex("8df8") << "12" << 2;
  QTest::newRow("phone") << QByteArray::fromHex("8810724e95") << "0815-4711" << 5;
  QTest::newRow("one digit") << QByteArray::fromHex("a0f8") << "2" << 2;
  QTest::newRow("cut off") << QByteArray::fromHex("8810") << "" << 2;
}

void test_CGarminImg::base11() {
  QFETCH(QByteArray, bytes);
  QFETCH(QString, number);
  QFETCH(qint32, size);

  const quint8* p = reinterpret_cast<const quint8*>(bytes.constData());
  const quint8* const start = p;
  QCOMPARE(CGarminPlaces::base11(p, start + bytes.size()), number);
  QCOMPARE(qint32(p - start), size);
}

void test_CGarminImg::poiDetails() {
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  const QString image = QDir(fixtureDir()).filePath("poi.img");
  if (!QFileInfo::exists(image)) {
    QSKIP(qPrintable("Test map not built: " + image));
  }

  QVector<img_object_t> decoded;
  try {
    QVERIFY(readImg(image, decoded) > 0);
  } catch (const CGarminImg::exce_t& e) {
    QFAIL(qPrintable(e.msg));
  }

  const QVector<mp_object_t> sources = readMp(QDir(QMS_IMG_SOURCES).filePath("poi.mp"));
  QVERIFY(!sources.isEmpty());
  for (const mp_object_t& want : sources) {
    // mkgmap writes no POI record for a city point
    if (want.kind != "POI" || want.type < 0x1200) {
      continue;
    }
    const QString what = QString("POI 0x%1 \"%2\"").arg(want.type, 0, 16).arg(want.labels.join(", "));
    auto match = std::find_if(decoded.begin(), decoded.end(), [&](const img_object_t& got) {
      return got.kind == "POI" && got.type == want.type && got.labels == want.labels;
    });
    QVERIFY2(match != decoded.end(), qPrintable("Not decoded: " + what));

    QMap<QString, QString> expected;
    for (const QString& key : detailKeys()) {
      if (want.attributes.contains(key)) {
        expected[key] = want.attributes[key];
      }
    }
    // region and country come with the city
    if (!expected.contains("CityName")) {
      expected.remove("RegionName");
      expected.remove("CountryName");
    }
    const auto text = [](const QMap<QString, QString>& m) {
      QStringList parts;
      for (auto it = m.constBegin(); it != m.constEnd(); ++it) {
        parts << it.key() + "=" + it.value();
      }
      return parts.join(", ");
    };
    QVERIFY2(match->details == expected,
             qPrintable(QString("%1\n   got: %2\n  want: %3").arg(what, text(match->details), text(expected))));
  }
}

void test_CGarminImg::exits() {
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  const QString image = QDir(fixtureDir()).filePath("exits.img");
  if (!QFileInfo::exists(image)) {
    QSKIP(qPrintable("Test map not built: " + image));
  }

  CGarminImg img(image);
  img.read();
  const QVector<CGarminImg::map_level_t>& levels = img.getMapLevels();
  QVERIFY(!levels.isEmpty());
  polytype_t polygons, polylines;
  pointtype_t points, pois;
  const QRectF world(-M_PI, M_PI / 2, 2 * M_PI, -M_PI);
  img.load(levels.last().bits, world, CGarminImg::filter_t(), [] { return false; }, polygons, polylines, points, pois);

  // osm/exits.osm: mkgmap names an exit "<ref> <name>" and takes its description from exit_to
  QMap<QString, const CGarminPoint*> exits;
  for (const pointtype_t* list : {&points, &pois}) {
    for (const CGarminPoint& p : *list) {
      if (p.isExit) {
        exits[IGarminStrTbl::displayText(p.labels)] = &p;
      }
    }
  }
  QCOMPARE(exits.keys(), QStringList({"12 Hilltown North", "13 Rivertown"}));

  const CGarminPoint& north = *exits["12 Hilltown North"];
  QCOMPARE(north.exitHighway, QString("A 1"));
  QCOMPARE(north.exitTo, QString("Hilltown"));
  QVERIFY(!north.exitOvernightParking);
  QCOMPARE(north.exitFacilities.size(), 1);
  // exit:facility="0x02,I,0x47,Truck Stop"
  QCOMPARE(north.exitFacilities[0].description, QString("Truck Stop"));
  QCOMPARE(north.exitFacilities[0].type, quint8(0x02));
  QCOMPARE(north.exitFacilities[0].direction, quint8(4));
  QCOMPARE(north.exitFacilities[0].facilities, quint8(0x47));

  const CGarminPoint& river = *exits["13 Rivertown"];
  QCOMPARE(river.exitHighway, QString("A 1"));
  QVERIFY(river.exitTo.isEmpty());
  QVERIFY(river.exitFacilities.isEmpty());
}

void test_CGarminImg::copyrights() {
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  const QString image = QDir(fixtureDir()).filePath("poi.img");
  if (!QFileInfo::exists(image)) {
    QSKIP(qPrintable("Test map not built: " + image));
  }
  CGarminImg img(image);
  img.read();

  // TRE3: mkgmap's own message and the source's Copyright= in code page 1252; then mkgmap's licence text
  const QStringList copyrights = img.getCopyrights();
  QCOMPARE(copyrights.size(), 3);
  QCOMPARE(copyrights[0], QString("program licenced under GPL v2"));
  QCOMPARE(copyrights[1], QString("QMS test data © Müller"));
  QVERIFY2(copyrights[2].startsWith("Map data (c) OpenStreetMap and its contributors"), qPrintable(copyrights[2]));
  QVERIFY(copyrights[2].endsWith("Program released under the GPL"));
}

void test_CGarminImg::infoTexts() {
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  QMap<QString, QString> address;
  QMap<QString, QString> exit;
  for (const QString& name : {QString("poi.img"), QString("exits.img")}) {
    const QString image = QDir(fixtureDir()).filePath(name);
    if (!QFileInfo::exists(image)) {
      QSKIP(qPrintable("Test map not built: " + image));
    }
    CGarminImg img(image);
    img.read();
    polytype_t polygons, polylines;
    pointtype_t points, pois;
    const QRectF world(-M_PI, M_PI / 2, 2 * M_PI, -M_PI);
    img.load(
        img.getMapLevels().last().bits, world, CGarminImg::filter_t(), [] { return false; }, polygons, polylines,
        points, pois);
    for (const pointtype_t* list : {&points, &pois}) {
      for (const CGarminPoint& p : *list) {
        address[p.getName()] = p.getAddressText();
        exit[p.getName()] = p.getExitText();
      }
    }
  }

  // as BaseCamp 4.7.5 shows them, without the country
  QCOMPARE(address["Restaurant One"], QString("12 Main Street\nHilltown, Upper Valley 87654\n0815-4711"));
  QCOMPARE(address["Bank Two"], QString("221b Baker Street\nRivertown, Lower Valley\n+49 89 1234"));
  QCOMPARE(address["Museum Three"], QString("Main Street"));
  QCOMPARE(address["Peak Four"], QString());
  QCOMPARE(exit["12 Hilltown North"],
           QString("Exit on A 1\nExit to Hilltown\nTruck Stop: Automobile Fuel, inner side of exit"));
  QCOMPARE(exit["13 Rivertown"], QString("Exit on A 1"));
  QCOMPARE(exit["Restaurant One"], QString());
}

void test_CGarminImg::facilityText_data() {
  QTest::addColumn<QString>("description");
  QTest::addColumn<quint8>("type");
  QTest::addColumn<quint8>("direction");
  QTest::addColumn<QString>("text");

  QTest::newRow("all") << "D0" << quint8(0) << quint8(0) << "D0: 24-hour Diesel Fuel With Restaurant, north of exit";
  QTest::newRow("last type") << "D13" << quint8(13) << quint8(6) << "D13: Fast Food, both sides of exit";
  // BaseCamp: "D14: , North of Exit"
  QTest::newRow("type without name") << "D14" << quint8(14) << quint8(0) << "D14: north of exit";
  QTest::newRow("no direction") << "D3" << quint8(3) << quint8(7) << "D3: Restaurant";
  QTest::newRow("no description") << "" << quint8(2) << quint8(7) << "Automobile Fuel";
}

void test_CGarminImg::facilityText() {
  QFETCH(QString, description);
  QFETCH(quint8, type);
  QFETCH(quint8, direction);
  QFETCH(QString, text);

  CGarminPoint::exit_facility_t facility;
  facility.description = description;
  facility.type = type;
  facility.direction = direction;
  QCOMPARE(CGarminPoint::facilityText(facility), text);
}

void test_CGarminImg::poiSymbols() {
  // standard types only: a marine type shares the low bytes, a city has a label only
  QVERIFY(CPoiSymbol::hasSymbol(0x2a00));
  QVERIFY(CPoiSymbol::hasSymbol(0x6616));
  QVERIFY(!CPoiSymbol::hasSymbol(0x12a00));
  QVERIFY(!CPoiSymbol::hasSymbol(0x0800));

  // every type of mkgmap's default style but the cities; each icon loads and is drawn inside the symbol
  qint32 count = 0;
  for (quint32 type = 0; type <= 0xffff; ++type) {
    if (!CPoiSymbol::hasSymbol(type)) {
      continue;
    }
    ++count;
    BLImage img(40, 40, BL_FORMAT_PRGB32);
    BLContext ctx(img);
    ctx.clear_all();
    CPoiSymbol::draw(ctx, QPointF(20, 20), type);
    ctx.end();
    const QImage q = toQImage(img);
    const QRect bounds = CPoiSymbol::bounds(type).translated(20, 20).toAlignedRect();
    // the icon: white on a badge, coloured for a geographic sign
    const bool sign = (type >> 8) >= 0x64;
    bool outside = false;
    qint32 ink = 0;
    for (int y = 0; y < q.height(); ++y) {
      for (int x = 0; x < q.width(); ++x) {
        const QRgb px = q.pixel(x, y);
        if (qAlpha(px) == 0) {
          continue;
        }
        if (!bounds.contains(x, y)) {
          outside = true;
        } else if (bounds.adjusted(3, 3, -3, -3).contains(x, y) && qAlpha(px) == 255) {
          const bool white = qRed(px) > 200 && qGreen(px) > 200 && qBlue(px) > 200;
          ink += sign != white ? 1 : 0;
        }
      }
    }
    const QString what = QString("type 0x%1").arg(type, 0, 16);
    QVERIFY2(!outside, qPrintable(what + ": drawn outside its bounds"));
    QVERIFY2(ink >= 5, qPrintable(what + ": no icon drawn"));
  }
  QCOMPARE(count, 130);

  // at a pixel ratio of 2 the symbol is rendered for it and keeps its logical size
  BLImage img(80, 80, BL_FORMAT_PRGB32);
  BLContext ctx(img);
  ctx.clear_all();
  ctx.scale(2.0);
  CPoiSymbol::draw(ctx, QPointF(20, 20), 0x2a00);
  ctx.end();
  const QImage q = toQImage(img);
  const QRect bounds = CPoiSymbol::bounds(0x2a00).translated(20, 20).toAlignedRect();
  QRect drawn;
  for (int y = 0; y < q.height(); ++y) {
    for (int x = 0; x < q.width(); ++x) {
      if (qAlpha(q.pixel(x, y)) != 0) {
        drawn |= QRect(x, y, 1, 1);
      }
    }
  }
  QCOMPARE(drawn, QRect(bounds.topLeft() * 2, bounds.size() * 2));
}

void test_CGarminImg::typName_data() {
  QTest::addColumn<CGarminTyp::names_t>("strings");
  QTest::addColumn<int>("first");
  QTest::addColumn<QString>("name");

  // as BaseCamp 4.7.5 in German (0x02) picks them; the first name in the TYP comes first in each row
  QTest::newRow("user's language") << CGarminTyp::names_t{{0x00, "default"}, {0x02, "deutsch"}, {0x04, "english"}}
                                   << 0x00 << "deutsch";
  QTest::newRow("user's language, not first")
      << CGarminTyp::names_t{{0x04, "english"}, {0x02, "deutsch"}} << 0x04 << "deutsch";
  QTest::newRow("only one") << CGarminTyp::names_t{{0x04, "english"}} << 0x04 << "english";
  QTest::newRow("unspecified only") << CGarminTyp::names_t{{0x00, "default"}} << 0x00 << "default";
  QTest::newRow("first, lowest code") << CGarminTyp::names_t{{0x01, "french"}, {0x03, "dutch"}} << 0x01 << "french";
  QTest::newRow("first, not English") << CGarminTyp::names_t{{0x01, "french"}, {0x04, "english"}} << 0x01 << "french";
  QTest::newRow("first, not lowest code")
      << CGarminTyp::names_t{{0x04, "english"}, {0x01, "french"}} << 0x04 << "english";
  QTest::newRow("unspecified before first")
      << CGarminTyp::names_t{{0x04, "english"}, {0x00, "default"}} << 0x04 << "default";
  QTest::newRow("none") << CGarminTyp::names_t{} << 0x00 << "";
}

void test_CGarminImg::typName() {
  QFETCH(CGarminTyp::names_t, strings);
  QFETCH(int, first);
  QFETCH(QString, name);

  QCOMPARE(CGarminTyp::name(strings, quint8(first), 0x02), name);
}

void test_CGarminImg::languageCode_data() {
  QTest::addColumn<QString>("iso");
  QTest::addColumn<int>("code");

  QTest::newRow("French") << "fr" << 0x01;
  QTest::newRow("German") << "de" << 0x02;
  QTest::newRow("English") << "en" << 0x04;
  QTest::newRow("Spanish") << "es" << 0x08;
  QTest::newRow("Bokmål") << "nb" << 0x0f;
  QTest::newRow("Bulgarian") << "bg" << 0x22;
  QTest::newRow("no TYP code") << "ja" << 0x00;
  QTest::newRow("empty") << "" << 0x00;
}

void test_CGarminImg::languageCode() {
  QFETCH(QString, iso);
  QFETCH(int, code);

  QCOMPARE(int(CGarminTyp::languageCode(iso)), code);
}

void test_CGarminImg::pointLabelBox_data() {
  QTest::addColumn<QPointF>("pos");
  QTest::addColumn<QRectF>("symbol");
  QTest::addColumn<QRect>("box");

  // icons of a TYP as BaseCamp 4.7.5 draws them: text from 1 px right of the icon, its bottom row above the point
  QTest::newRow("8x8") << QPointF(763, 344) << QRectF(759, 340, 8, 8) << QRect(768, 331, 80, 13);
  QTest::newRow("16x16") << QPointF(763, 411) << QRectF(755, 403, 16, 16) << QRect(772, 398, 80, 13);
  QTest::newRow("24x24") << QPointF(763, 478) << QRectF(751, 466, 24, 24) << QRect(776, 465, 80, 13);
  QTest::newRow("no symbol") << QPointF(100.4, 200.6) << QRectF(100.4, 200.6, 0, 0) << QRect(101, 188, 80, 13);
}

void test_CGarminImg::pointLabelBox() {
  QFETCH(QPointF, pos);
  QFETCH(QRectF, symbol);
  QFETCH(QRect, box);

  QCOMPARE(CGarminPoint::labelBox(pos, symbol, QSize(80, 13)), box);
}

void test_CGarminImg::placesRecords() {
  // tables as mkgmap's PlacesFile writes them; more than 255 cities and zips make their indexes 2 bytes
  QByteArray buf;
  CGarminPlaces::layout_t layout;
  layout.country = {0x000, 2 * 3, 3};
  put3(buf, 0x000, 16);
  put3(buf, 0x003, 17);
  layout.region = {0x010, 5, 5};
  put(buf, 0x010, {1, 0});
  put3(buf, 0x012, 32);
  layout.city = {0x100, 300 * 5, 5};
  put3(buf, 0x100, 48);  // city 1 in region 1
  put(buf, 0x103, {0x01, 0x00});
  put3(buf, 0x100 + 299 * 5, 49);  // city 300 in country 2, no region
  put(buf, 0x100 + 299 * 5 + 3, {0x02, 0x40});
  layout.zip = {0x800, 300 * 3, 3};
  put3(buf, 0x800 + 298 * 3, 64);  // zip 299
  layout.highway = {0xC00, 6, 6};
  put3(buf, 0xC00, 80);
  // two facilities, the second marked last; the third belongs to another exit
  layout.exitFacility = {0xC10, 3 * 5, 5};
  const auto facility = [&](qsizetype at, quint32 label, quint32 type, quint32 direction, bool last) {
    const quint32 word = label | (last ? 0x00800000 : 0) | (type << 24) | (direction << 29);
    put(buf, at, {quint8(word), quint8(word >> 8), quint8(word >> 16), quint8(word >> 24), 0x47});
  };
  facility(0xC10, 96, 2, 4, false);
  facility(0xC15, 97, 3, 6, true);
  facility(0xC1A, 98, 5, 0, true);
  // house number, city, zip and exit set globally
  layout.offsetPoiProps = 0xD00;
  layout.sizePoiProps = 0x100;
  layout.globalFlags = 0x01 | 0x04 | 0x08 | 0x20;
  // all global fields: "12", city 300, zip 299, exit with overnight parking and facility 1
  put3(buf, 0xD00, 1);
  put(buf, 0xD03, {0x8d, 0xf8, 0x2c, 0x01, 0x2b, 0x01});
  put3(buf, 0xD09, 112 | 0x00400000 | 0x00800000);
  put(buf, 0xD0C, {1, 1});
  // own flags: only the city (the second global field), city 1
  put3(buf, 0xD20, 2 | 0x00800000);
  put(buf, 0xD23, {0x80 | 0x02, 0x01, 0x00});
  buf.resize(0xE00);

  const CGarminPlaces::read_t read = [&](CFileExt&, quint32 offset, quint32 size, QByteArray& data) {
    data = quint64(offset) + size <= quint64(buf.size()) ? buf.mid(offset, size) : QByteArray();
  };
  const CGarminPlaces::point_label_t noPoint = [](CFileExt&, quint32, quint32) { return QString(); };
  CFakeStrTbl strtbl;
  CFileExt file(QCoreApplication::applicationFilePath());
  CGarminPlaces places(layout, strtbl, read);

  CGarminPoint all;
  places.readDetails(file, 0x00, noPoint, all);
  QCOMPARE(all.houseNumber, QString("12"));
  QCOMPARE(all.city, QString("L49"));
  QCOMPARE(all.region, QString());
  QCOMPARE(all.country, QString("L17"));
  QCOMPARE(all.zip, QString("L64"));
  QVERIFY(all.isExit);
  QCOMPARE(all.exitTo, QString("L112"));
  QVERIFY(all.exitOvernightParking);
  QCOMPARE(all.exitHighway, QString("L80"));
  QCOMPARE(all.exitFacilities.size(), 2);
  QCOMPARE(all.exitFacilities[0].description, QString("L96"));
  QCOMPARE(all.exitFacilities[0].type, quint8(2));
  QCOMPARE(all.exitFacilities[0].direction, quint8(4));
  QCOMPARE(all.exitFacilities[0].facilities, quint8(0x47));
  QCOMPARE(all.exitFacilities[1].description, QString("L97"));
  QCOMPARE(all.exitFacilities[1].direction, quint8(6));

  // the cached record gives the same
  CGarminPoint again;
  places.readDetails(file, 0x00, noPoint, again);
  QCOMPARE(again.getAddressText(), all.getAddressText());
  QCOMPARE(again.getExitText(), all.getExitText());

  CGarminPoint own;
  places.readDetails(file, 0x20, noPoint, own);
  QCOMPARE(own.houseNumber, QString());
  QCOMPARE(own.city, QString("L48"));
  QCOMPARE(own.region, QString("L32"));
  QCOMPARE(own.country, QString("L16"));
  QCOMPARE(own.zip, QString());
  QVERIFY(!own.isExit);

  // a record past LBL6 and tables past the data read as nothing
  CGarminPoint past;
  places.readDetails(file, 0x100, noPoint, past);
  QVERIFY(past.getAddressText().isEmpty());
  CGarminPlaces::layout_t broken = layout;
  broken.city.offset = 0x100000;
  broken.zip.offset = 0xFFFFFFF0;
  CGarminPlaces brokenPlaces(broken, strtbl, read);
  CGarminPoint none;
  brokenPlaces.readDetails(file, 0x00, noPoint, none);
  QCOMPARE(none.houseNumber, QString("12"));
  QCOMPARE(none.city, QString());
  QCOMPARE(none.zip, QString());
}

void test_CGarminImg::corruptPlaces() {
  if (fixtureDir().isEmpty()) {
    QSKIP("QMS_IMG_FIXTURES not set; build the test maps with make_fixtures.py");
  }
  const QString image = QDir(fixtureDir()).filePath("poi.img");
  if (!QFileInfo::exists(image)) {
    QSKIP(qPrintable("Test map not built: " + image));
  }
  // the city table moved past the end of the file: the map loads and draws, the POIs lose their city
  QFile in(image);
  QVERIFY(in.open(QIODevice::ReadOnly));
  QByteArray bytes = in.readAll();
  const qsizetype hdr = bytes.indexOf("GARMIN LBL") - 2;
  QVERIFY(hdr >= 0);
  const qsizetype lbl4 = hdr + 0x3B;
  for (int i = 0; i < 4; ++i) {
    bytes[lbl4 + i] = char(i == 2 ? 0x10 : 0);
  }
  QTemporaryDir dir;
  const QString broken = dir.filePath("poi.img");
  QFile out(broken);
  QVERIFY(out.open(QIODevice::WriteOnly));
  out.write(bytes);
  out.close();

  QVector<img_object_t> decoded;
  try {
    QVERIFY(readImg(broken, decoded) > 0);
  } catch (const CGarminImg::exce_t& e) {
    QFAIL(qPrintable(e.msg));
  }
  bool restaurant = false;
  for (const img_object_t& o : std::as_const(decoded)) {
    if (o.labels == QStringList{"Restaurant One"}) {
      restaurant = true;
      QCOMPARE(o.details.value("StreetDesc"), QString("Main Street"));
      QCOMPARE(o.details.value("CityName"), QString());
    }
  }
  QVERIFY(restaurant);
}

QTEST_GUILESS_MAIN(test_CGarminImg)
#include "test_CGarminImg.moc"
