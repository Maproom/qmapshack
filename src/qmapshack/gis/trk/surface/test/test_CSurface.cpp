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
#include <QSet>
#include <QTest>

#include "gis/trk/surface/CSurface.h"

/// Classification must match the reference implementation (osmsurface.py) case by case.
class test_CSurface : public QObject {
  Q_OBJECT
 private slots:
  void referenceTable_data();
  void referenceTable();
  void assumptions();
  void wayTypes();
  void namesAndColors();
};

static QHash<QString, CSurface::label_e> labelByDutch() {
  return {
      {"asfalt", CSurface::eLabelAsphalt},
      {"verhard", CSurface::eLabelPaved},
      {"beton", CSurface::eLabelConcrete},
      {"betonsporen", CSurface::eLabelConcreteLanes},
      {"betonplaten", CSurface::eLabelConcretePlates},
      {"klinkers", CSurface::eLabelPavingStones},
      {"kasseien", CSurface::eLabelCobblestone},
      {"metaal", CSurface::eLabelMetal},
      {"hout", CSurface::eLabelWood},
      {"verdicht steenslag", CSurface::eLabelCompacted},
      {"fijn grind", CSurface::eLabelFineGravel},
      {"grind", CSurface::eLabelGravel},
      {"kiezel", CSurface::eLabelPebblestone},
      {"grasdallen", CSurface::eLabelGrassPaver},
      {"halfverhard", CSurface::eLabelSemiPaved},
      {"onverhard", CSurface::eLabelUnpaved},
      {"aarde", CSurface::eLabelDirt},
      {"natuurlijke bodem", CSurface::eLabelGround},
      {"gras", CSurface::eLabelGrass},
      {"modder", CSurface::eLabelMud},
      {"zand", CSurface::eLabelSand},
      {"houtsnippers", CSurface::eLabelWoodchips},
      {"rots", CSurface::eLabelRock},
      {"stapstenen", CSurface::eLabelSteppingStones},
      {"verhard (aangenomen)", CSurface::eLabelPavedAssumed},
      {"onverhard (aangenomen)", CSurface::eLabelUnpavedAssumed},
      {"onbekend", CSurface::eLabelUnknown},
  };
}

static QHash<QString, CSurface::waytype_e> wayByDutch() {
  return {
      {"pad", CSurface::eWayPath},
      {"bos-/veldweg", CSurface::eWayTrack},
      {"fietspad", CSurface::eWayCycleway},
      {"trap", CSurface::eWaySteps},
      {"straat", CSurface::eWayStreet},
      {"weg", CSurface::eWayRoad},
      {"drukke weg", CSurface::eWayBusyRoad},
  };
}

void test_CSurface::referenceTable_data() {
  QTest::addColumn<QString>("highway");
  QTest::addColumn<QString>("surface");
  QTest::addColumn<QString>("tracktype");
  QTest::addColumn<QString>("footway");
  QTest::addColumn<QString>("label");
  QTest::addColumn<qint32>("cls");
  QTest::addColumn<QString>("way");

  QFile file(QFINDTESTDATA("data/classify_cases.tsv"));
  QVERIFY(file.open(QIODevice::ReadOnly));
  qint32 rows = 0;
  while (!file.atEnd()) {
    const QStringList cols = QString::fromUtf8(file.readLine()).chopped(1).split('\t');
    QCOMPARE(cols.size(), 7);
    const QString name = QString("%1|%2|%3|%4").arg(cols[0], cols[1], cols[2], cols[3]);
    QTest::newRow(name.toUtf8()) << cols[0] << cols[1] << cols[2] << cols[3] << cols[4] << cols[5].toInt() << cols[6];
    rows++;
  }
  QVERIFY(rows > 900);
}

void test_CSurface::referenceTable() {
  QFETCH(QString, highway);
  QFETCH(QString, surface);
  QFETCH(QString, tracktype);
  QFETCH(QString, footway);
  QFETCH(QString, label);
  QFETCH(int, cls);
  QFETCH(QString, way);

  const CSurface::info_t info = CSurface::classify({highway, surface, tracktype, footway});

  QVERIFY2(labelByDutch().contains(label), qPrintable(label));
  QCOMPARE(info.label, labelByDutch().value(label));
  QCOMPARE(qint32(info.cls), cls);
  QCOMPARE(info.way, wayByDutch().value(way, CSurface::eWayOther));
}

void test_CSurface::assumptions() {
  // the cases a walker meets most, spelled out
  QCOMPARE(CSurface::classify({"track", "", "", ""}).cls, CSurface::eClassUnknown);
  QCOMPARE(CSurface::classify({"track", "", "grade2", ""}).cls, CSurface::eClassSemiPaved);
  QCOMPARE(CSurface::classify({"path", "", "", ""}).label, CSurface::eLabelUnpavedAssumed);
  QCOMPARE(CSurface::classify({"footway", "", "", "sidewalk"}).label, CSurface::eLabelPavedAssumed);
  QCOMPARE(CSurface::classify({"footway", "", "", ""}).label, CSurface::eLabelUnpavedAssumed);
  QCOMPARE(CSurface::classify({"residential", "", "", ""}).label, CSurface::eLabelPavedAssumed);
  // a surface tag always wins over the tracktype
  QCOMPARE(CSurface::classify({"track", "asphalt", "grade5", ""}).cls, CSurface::eClassPaved);
  QCOMPARE(CSurface::classify({"primary", "gravel", "", ""}).cls, CSurface::eClassSemiPaved);
}

void test_CSurface::wayTypes() {
  QCOMPARE(CSurface::wayType("bridleway"), CSurface::eWayPath);
  QCOMPARE(CSurface::wayType("trunk"), CSurface::eWayBusyRoad);
  QCOMPARE(CSurface::wayType("motorway"), CSurface::eWayOther);
  QCOMPARE(CSurface::wayType(""), CSurface::eWayOther);
  QCOMPARE(CSurface::noData().way, CSurface::eWayNoData);
  QCOMPARE(CSurface::noData().cls, CSurface::eClassUnknown);
}

void test_CSurface::namesAndColors() {
  QSet<QString> names;
  QSet<QRgb> colors;
  for (qint32 i = 0; i < CSurface::eClassCount; i++) {
    names << CSurface::className(CSurface::class_e(i));
    colors << CSurface::classColor(CSurface::class_e(i)).rgb();
  }
  QCOMPARE(names.size(), qint32(CSurface::eClassCount));
  QCOMPARE(colors.size(), qint32(CSurface::eClassCount));

  names.clear();
  colors.clear();
  for (qint32 i = 0; i < CSurface::eWayCount; i++) {
    names << CSurface::wayTypeName(CSurface::waytype_e(i));
    colors << CSurface::wayTypeColor(CSurface::waytype_e(i)).rgb();
  }
  QCOMPARE(names.size(), qint32(CSurface::eWayCount));
  QCOMPARE(colors.size(), qint32(CSurface::eWayCount));

  names.clear();
  for (qint32 i = 0; i < CSurface::eLabelCount; i++) {
    const QString name = CSurface::labelName(CSurface::label_e(i));
    QVERIFY(!name.isEmpty());
    names << name;
  }
  QCOMPARE(names.size(), qint32(CSurface::eLabelCount));
}

QTEST_GUILESS_MAIN(test_CSurface)
#include "test_CSurface.moc"
