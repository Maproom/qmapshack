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

#include <QTest>

#include "gis/trk/surface/CSurfaceAnalyzer.h"
#include "gis/trk/surface/CSurfaceDb.h"

/// Sampling, smoothing and aggregation, with a synthetic map instead of a database.
class test_CSurfaceAnalyzer : public QObject {
  Q_OBJECT
 private slots:
  void emptyTrack();
  void singlePoint();
  void straightLineSampling();
  void segmentsAreNotConnected();
  void noDataCountsAsUnknown();
  void smoothingRemovesSingleBlips();
  void smoothingKeepsLongerParts();
  void referenceModeUsesStartPoint();
  void cancel();
};

namespace {
const CSurface::info_t asphaltRoad = {CSurface::eLabelAsphalt, CSurface::eClassPaved, CSurface::eWayRoad};
const CSurface::info_t gravelTrack = {CSurface::eLabelGravel, CSurface::eClassSemiPaved, CSurface::eWayTrack};
const CSurface::info_t groundPath = {CSurface::eLabelGround, CSurface::eClassUnpaved, CSurface::eWayPath};

// 0.001 deg latitude ~ 111.2 m
QVector<CSurfaceAnalyzer::point_t> northwards(qreal lat0, qint32 count, qreal step = 0.001) {
  QVector<CSurfaceAnalyzer::point_t> pts;
  for (qint32 i = 0; i < count; i++) {
    pts << CSurfaceAnalyzer::point_t{lat0 + i * step, 5.0, false};
  }
  return pts;
}

qreal sum(const auto& array) {
  qreal total = 0;
  for (qreal v : array) {
    total += v;
  }
  return total;
}
}  // namespace

void test_CSurfaceAnalyzer::emptyTrack() {
  const auto result = CSurfaceAnalyzer::analyze({}, [](qreal, qreal) { return asphaltRoad; }, {});
  QVERIFY(result.valid);
  QCOMPARE(result.total, 0.0);
  QCOMPARE(result.samples, 0);
  QCOMPARE(result.percent(CSurface::eClassPaved), 0.0);
}

void test_CSurfaceAnalyzer::singlePoint() {
  const auto result = CSurfaceAnalyzer::analyze(northwards(50, 1), [](qreal, qreal) { return gravelTrack; }, {});
  QVERIFY(result.valid);
  QCOMPARE(result.perPoint.size(), 1);
  QVERIFY(result.perPoint[0] == gravelTrack);
  QCOMPARE(result.total, 0.0);
}

void test_CSurfaceAnalyzer::straightLineSampling() {
  // 1 km north; asphalt south of 50.0034, gravel north of it
  const auto pts = northwards(50.0, 10, 0.001);
  qint32 calls = 0;
  const auto result = CSurfaceAnalyzer::analyze(pts,
                                                [&](qreal lat, qreal) {
                                                  calls++;
                                                  return lat < 50.0034 ? asphaltRoad : gravelTrack;
                                                },
                                                {});

  const qreal length = CSurfaceDb::distance(50.0, 5.0, 50.009, 5.0);
  QVERIFY(qAbs(result.total - length) < 1e-6);
  QVERIFY(qAbs(sum(result.byClass) - length) < 1e-6);
  QVERIFY(qAbs(sum(result.byWay) - length) < 1e-6);
  QVERIFY(qAbs(sum(result.byLabel) - length) < 1e-6);

  // 111 m per segment -> 6 samples of 18.5 m each, plus the end point
  QCOMPARE(calls, 9 * 6 + 1);
  QCOMPARE(result.samples, calls);
  QCOMPARE(result.samplesWithData, calls);

  // three segments and half of the fourth
  QVERIFY(qAbs(result.byClass[CSurface::eClassPaved] - 3.5 * length / 9) < 1e-6);
  QVERIFY(qAbs(result.byWay[CSurface::eWayTrack] - 5.5 * length / 9) < 1e-6);
  QVERIFY(qAbs(result.byLabel[CSurface::eLabelGravel] - 5.5 * length / 9) < 1e-6);
  QVERIFY(qAbs(result.percent(CSurface::eClassPaved) - 350.0 / 9) < 1e-6);

  QCOMPARE(result.perPoint.size(), 10);
  QVERIFY(result.perPoint[3] == asphaltRoad);
  QVERIFY(result.perPoint[4] == gravelTrack);
  QVERIFY(result.perPoint[9] == gravelTrack);
}

void test_CSurfaceAnalyzer::segmentsAreNotConnected() {
  auto pts = northwards(50.0, 2);
  auto second = northwards(50.010, 2);
  second[0].newSegment = true;
  pts += second;

  const auto result = CSurfaceAnalyzer::analyze(pts, [](qreal, qreal) { return groundPath; }, {});
  const qreal length = 2 * CSurfaceDb::distance(50.0, 5.0, 50.001, 5.0);
  QVERIFY(qAbs(result.total - length) < 1e-6);
  QCOMPARE(result.perPoint.size(), 4);
  for (const auto& info : result.perPoint) {
    QVERIFY(info == groundPath);
  }
}

void test_CSurfaceAnalyzer::noDataCountsAsUnknown() {
  const auto result = CSurfaceAnalyzer::analyze(
      northwards(50.0, 5),
      [](qreal lat, qreal) { return lat < 50.0019 ? std::optional<CSurface::info_t>(asphaltRoad) : std::nullopt; }, {});
  QVERIFY(result.samplesWithData < result.samples);
  QVERIFY(qAbs(result.percent(CSurface::eClassPaved) - 50) < 1e-6);
  QVERIFY(qAbs(result.percent(CSurface::eClassUnknown) - 50) < 1e-6);
  QVERIFY(result.byWay[CSurface::eWayNoData] > 0);
  QVERIFY(result.perPoint[4] == CSurface::noData());
}

void test_CSurfaceAnalyzer::smoothingRemovesSingleBlips() {
  // a crossing road at 50.0021..50.0022 hits exactly one 18.5 m sample
  auto lookup = [](qreal lat, qreal) { return (lat > 50.0021 && lat < 50.0022) ? asphaltRoad : groundPath; };
  const auto pts = northwards(50.0, 5);

  CSurfaceAnalyzer::options_t raw;
  raw.smooth = false;
  const auto blip = CSurfaceAnalyzer::analyze(pts, lookup, raw);
  QVERIFY(blip.byClass[CSurface::eClassPaved] > 0);

  const auto smooth = CSurfaceAnalyzer::analyze(pts, lookup, {});
  QCOMPARE(smooth.byClass[CSurface::eClassPaved], 0.0);
  QVERIFY(qAbs(smooth.total - blip.total) < 1e-6);
}

void test_CSurfaceAnalyzer::smoothingKeepsLongerParts() {
  // three samples on the road are real
  auto lookup = [](qreal lat, qreal) { return (lat > 50.0021 && lat < 50.0026) ? asphaltRoad : groundPath; };
  const auto result = CSurfaceAnalyzer::analyze(northwards(50.0, 5), lookup, {});
  QVERIFY(result.byClass[CSurface::eClassPaved] > 30);
}

void test_CSurfaceAnalyzer::referenceModeUsesStartPoint() {
  // sampleDistance 0: each track segment counts for the way at its start point
  CSurfaceAnalyzer::options_t opts;
  opts.sampleDistance = 0;
  opts.smooth = false;
  qint32 calls = 0;
  const auto result = CSurfaceAnalyzer::analyze(
      northwards(50.0, 3),
      [&](qreal lat, qreal) {
        calls++;
        return lat < 50.0005 ? asphaltRoad : gravelTrack;
      },
      opts);
  QCOMPARE(calls, 3);
  QVERIFY(qAbs(result.percent(CSurface::eClassPaved) - 50) < 1e-6);
}

void test_CSurfaceAnalyzer::cancel() {
  std::atomic_bool cancel = true;
  const auto result =
      CSurfaceAnalyzer::analyze(northwards(50.0, 5), [](qreal, qreal) { return asphaltRoad; }, {}, &cancel);
  QVERIFY(!result.valid);
}

QTEST_GUILESS_MAIN(test_CSurfaceAnalyzer)
#include "test_CSurfaceAnalyzer.moc"
