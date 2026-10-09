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
#include <QTemporaryDir>
#include <QTest>
#include <QXmlStreamReader>

#include "gis/trk/surface/CSurfaceAnalyzer.h"
#include "gis/trk/surface/CSurfaceDb.h"
#include "gis/trk/surface/CSurfaceDbBuilder.h"
#include "gis/trk/surface/CSurfaceDbSet.h"

/**
   Real-world check against the reference implementation (osmsurface.py / surface.py) on a
   27.6 km hike near Durbuy, Belgium, with the Geofabrik extract of Belgium.

   Needs data outside the repository, skipped if it is missing:
     QMS_SURFACE_GPX  the track Durbuy-Tohogne_2.gpx
     QMS_SURFACE_PBF  belgium-latest.osm.pbf of the same day
     QMS_SURFACE_DB   optional: an existing surface database to use instead of building one

   Reference result for the extract of 2026-10-08 (verhard / halfverhard / onverhard / onbekend):
   29.12 % / 20.46 % / 44.63 % / 5.79 % of 28.03 km.

   With the reference's method (track points only) the port must give the same numbers. What
   QMapShack shows samples every 20 m instead, which moves about 2 points from paved to unpaved:
   the reference counts a whole track segment for the way at its start point, and at a junction
   that is mostly the through road, also when the segment itself follows a path (e.g. the 229 m
   path from 50.38738 N 5.47368 E).
 */
class test_SurfaceDurbuy : public QObject {
  Q_OBJECT
 private slots:
  void initTestCase();
  void referenceMode();
  void defaultMode();

 private:
  void compare(const CSurfaceAnalyzer::result_t& result, qreal tolerance);

  QTemporaryDir tmp;
  QString db;
  QVector<CSurfaceAnalyzer::point_t> points;
};

static constexpr qreal kReference[CSurface::eClassCount] = {29.117401, 20.463965, 44.626584, 5.792051};
static constexpr qreal kToleranceReferenceMode = 0.05;  // percentage points
static constexpr qreal kToleranceDefaultMode = 2.5;

void test_SurfaceDurbuy::initTestCase() {
  const QString gpx = qEnvironmentVariable("QMS_SURFACE_GPX");
  const QString pbf = qEnvironmentVariable("QMS_SURFACE_PBF");
  db = qEnvironmentVariable("QMS_SURFACE_DB");

  if (gpx.isEmpty()) {
    QSKIP("QMS_SURFACE_GPX not set");
  }
  QFile file(gpx);
  if (!file.open(QIODevice::ReadOnly)) {
    QSKIP(qPrintable("Track not found: " + gpx));
  }

  // track points only, route points are no part of the track
  QXmlStreamReader xml(&file);
  bool newSegment = false;
  while (!xml.atEnd()) {
    xml.readNext();
    if (!xml.isStartElement()) {
      continue;
    }
    if (xml.name() == QLatin1String("trkseg")) {
      newSegment = true;
    } else if (xml.name() == QLatin1String("trkpt")) {
      points << CSurfaceAnalyzer::point_t{xml.attributes().value("lat").toDouble(),
                                          xml.attributes().value("lon").toDouble(), newSegment};
      newSegment = false;
    }
  }
  QCOMPARE(points.size(), 480);

  if (db.isEmpty()) {
    if (!QFile::exists(pbf)) {
      QSKIP(qPrintable("OSM extract not found: " + pbf));
    }
    QVERIFY(tmp.isValid());
    QRectF area;
    for (const auto& pt : std::as_const(points)) {
      area |= QRectF(pt.lon, pt.lat, 1e-9, 1e-9);
    }
    CSurfaceDbBuilder::options_t opts;
    opts.pbf = pbf;
    opts.target = tmp.filePath("durbuy.surface.sqlite");
    opts.name = "Durbuy";
    opts.area = area.adjusted(-0.02, -0.02, 0.02, 0.02);
    CSurfaceDbBuilder builder(opts);
    QVERIFY2(builder.build(nullptr), qPrintable(builder.errorString()));
    db = opts.target;
  }
}

void test_SurfaceDurbuy::compare(const CSurfaceAnalyzer::result_t& result, qreal tolerance) {
  QVERIFY(result.valid);
  QVERIFY2(qAbs(result.total - 28031.2) < 1, qPrintable(QString::number(result.total)));
  for (qint32 i = 0; i < CSurface::eClassCount; i++) {
    qInfo("%-12s %6.2f %% (reference %6.2f %%)", qPrintable(CSurface::className(CSurface::class_e(i))),
          result.percent(CSurface::class_e(i)), kReference[i]);
  }
  for (qint32 i = 0; i < CSurface::eClassCount; i++) {
    const qreal percent = result.percent(CSurface::class_e(i));
    QVERIFY2(qAbs(percent - kReference[i]) <= tolerance,
             qPrintable(QString("class %1: %2 % vs %3 %").arg(i).arg(percent).arg(kReference[i])));
  }
}

void test_SurfaceDurbuy::referenceMode() {
  // the reference's method: track points only, no fallback, no smoothing
  CSurfaceDbSet set({db});
  QVERIFY(set.covers(QRectF(QPointF(5.45, 50.34), QPointF(5.51, 50.39))));
  CSurfaceAnalyzer::options_t opts;
  opts.sampleDistance = 0;
  opts.smooth = false;
  const auto result =
      CSurfaceAnalyzer::analyze(points, [&](qreal lat, qreal lon) { return set.lookup(lat, lon, 0); }, opts);
  compare(result, kToleranceReferenceMode);
}

void test_SurfaceDurbuy::defaultMode() {
  // what QMapShack shows: 20 m samples, nearest way within 25 m, smoothing
  CSurfaceDbSet set({db});
  const CSurfaceAnalyzer::options_t opts;
  const auto result = CSurfaceAnalyzer::analyze(
      points, [&](qreal lat, qreal lon) { return set.lookup(lat, lon, opts.fallbackDistance); }, opts);
  compare(result, kToleranceDefaultMode);
  QVERIFY(result.samplesWithData == result.samples);
}

QTEST_GUILESS_MAIN(test_SurfaceDurbuy)
#include "test_SurfaceDurbuy.moc"
