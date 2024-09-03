/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2016 Christian Eichler <code@christian-eichler.de>

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

#include <QLineF>
#include <QtMath>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWorkspace.h"
#include "gis/GeoMath.h"
#include "gis/proj_x.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CPropertyTrk.h"

void CGisItemTrk::filterReducePoints(qreal dist) {
  QVector<pointDP> line;
  bool nothingDone = true;

  for (const CTrackData::trkpt_t& pt : trk) {
    pointDP dp(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD, pt.ele);
    dp.used = !pt.isHidden();

    line << dp;
  }

  if (line.size() < 3) {
    return;
  }

  point3D pt0 = line[0];

  line[0].x = 0;
  line[0].y = 0;
  for (int i = 1; i < line.size(); i++) {
    pointDP& pt1 = line[i - 1];
    pointDP& pt2 = line[i];

    qreal a1, a2;
    qreal d = GPS_Math_Distance(pt0.x, pt0.y, pt2.x, pt2.y, a1, a2);

    pt0 = pt2;

    pt2.x = pt1.x + qCos(a1 * DEG_TO_RAD) * d;
    pt2.y = pt1.y + qSin(a1 * DEG_TO_RAD) * d;
  }

  GPS_Math_DouglasPeucker(line, dist);

  int cnt = 0;

  for (CTrackData::trkpt_t& pt : trk) {
    if (line[cnt].used) {
      pt.unsetFlag(CTrackData::trkpt_t::eFlagHidden);
    } else {
      if (!pt.isHidden() && pt.desc.isEmpty() && pt.keyWpt.item.isEmpty()) {
        nothingDone = false;
        pt.setFlag(CTrackData::trkpt_t::eFlagHidden);
      }
    }

    cnt++;
  }

  if (nothingDone) {
    return;
  }

  deriveSecondaryData();
  QString val, unit;
  IUnit::self().meter2distance(dist, val, unit);
  changed(tr("Hide points by Douglas Peuker algorithm (%1%2)").arg(val, unit), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterRemoveInvalidPoints() {
  bool nothingDone = true;

  // use all valid flags as invalid mask. By that only
  // invalid flags for properties with valid points count
  quint32 invalidMask = (getAllValidFlags() & CTrackData::trkpt_t::eValidMask) << 16;

  for (CTrackData::trkseg_t& seg : trk.segs) {
    QVector<CTrackData::trkpt_t> pts;
    for (const CTrackData::trkpt_t& pt : std::as_const(seg.pts)) {
      if (pt.isInvalid(CTrackData::trkpt_t::invalid_e(invalidMask))) {
        nothingDone = false;
        continue;
      }

      pts << pt;
    }

    seg.pts = pts;
  }

  if (nothingDone) {
    return;
  }

  deriveSecondaryData();
  changed(tr("Permanently removed points with invalid data."), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterReset() {
  for (CTrackData::trkpt_t& pt : trk) {
    pt.unsetFlag(CTrackData::trkpt_t::eFlagHidden);
  }
  deriveSecondaryData();
  changed(tr("Reset all hidden track points to visible"), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterDelete() {
  bool nothingDone = true;

  for (CTrackData::trkseg_t& seg : trk.segs) {
    QVector<CTrackData::trkpt_t> pts;
    for (const CTrackData::trkpt_t& pt : std::as_const(seg.pts)) {
      if (pt.isHidden()) {
        nothingDone = false;
        continue;
      }

      pts << pt;
    }

    seg.pts = pts;
  }

  if (nothingDone) {
    return;
  }

  deriveSecondaryData();
  changed(tr("Permanently removed all hidden track points"), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterSmoothProfile(int points) {
  QVector<int> window(points, 0);
  QVector<int> ele1, ele2;

  for (const CTrackData::trkpt_t& pt : trk) {
    ele1 << pt.ele;
    ele2 << pt.ele;
  }

  if (ele1.size() < (points + 1)) {
    return;
  }

  int d = points >> 1;
  for (int i = d; i < ele1.size() - d; i++) {
    for (int n = i - d, m = 0; m < points; n++, m++) {
      window[m] = ele1[n];
    }

    std::sort(window.begin(), window.end());
    ele2[i] = window[d];
  }

  int cnt = 0;
  for (CTrackData::trkpt_t& pt : trk) {
    pt.ele = ele2[cnt++];
  }
  deriveSecondaryData();
  changed(tr("Smoothed profile with a Median filter of size %1").arg(points), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterTerrainSlope() {
  QPolygonF line;
  for (const CTrackData::trkpt_t& pt : trk) {
    line << pt.radPoint();
  }

  QPolygonF slope(line.size());
  CMainWindow::self().getSlopeAt(line, slope);

  int cnt = 0;
  for (CTrackData::trkpt_t& pt : trk) {
    pt.extensions[CKnownExtension::internalTerrainSlope] = slope[cnt].ry();
    ++cnt;
  }

  deriveSecondaryData();
  changed(tr("Added terrain slope from DEM file."), "://icons/48x48/CSrcSlope.png");
}

void CGisItemTrk::filterReplaceElevation(CCanvas* canvas) {
  QPolygonF line;
  for (const CTrackData::trkpt_t& pt : trk) {
    line << pt.radPoint();
  }

  QPolygonF ele(line.size());
  if (canvas != nullptr) {
    canvas->getElevationAt(line, ele);
  } else {
    CMainWindow::self().getElevationAt(line, ele);
  }

  int cnt = 0;
  for (CTrackData::trkpt_t& pt : trk) {
    pt.ele = (ele[cnt].y() == NOFLOAT) ? NOINT : ele[cnt].y();
    ++cnt;
  }

  deriveSecondaryData();
  changed(tr("Replaced elevation data with data from DEM files."), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterInterpolateElevation() {
  if (!interp.valid) {
    return;
  }

  for (CTrackData::trkpt_t& pt : trk) {
    qreal ele = getElevationInterpolated(pt.distance);
    pt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);
  }

  interp.valid = false;
  deriveSecondaryData();
  changed(
      tr("Replaced elevation data with interpolated values. (M=%1, RMSErr=%2)").arg(interp.m).arg(interp.rep.rmserror),
      "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterOffsetElevation(int offset) {
  for (CTrackData::trkpt_t& pt : trk) {
    if (pt.ele != NOINT) {
      pt.ele += offset;
    }
  }

  QString val, unit;
  IUnit::self().meter2elevation(offset, val, unit);
  deriveSecondaryData();
  changed(tr("Offset elevation data by %1%2.").arg(val, unit), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterNewDate(const QDateTime& date) {
  qint64 delta = qint64(date.toSecsSinceEpoch()) - qint64(timeStart.toUTC().toSecsSinceEpoch());

  for (CTrackData::trkpt_t& pt : trk) {
    pt.time = pt.time.addSecs(delta);
  }

  deriveSecondaryData();
  changed(tr("Changed start of track to %1.").arg(date.toString()), "://icons/48x48/Time.png");
}

void CGisItemTrk::filterObscureDate(int delta) {
  if (delta == 0) {
    for (CTrackData::trkpt_t& pt : trk) {
      pt.time = QDateTime();
    }

    deriveSecondaryData();
    changed(tr("Remove timestamps."), "://icons/48x48/Time.png");
  } else {
    QDateTime timestamp = timeStart;
    if (!timestamp.isValid()) {
      timestamp = QDateTime::currentDateTimeUtc();
    }

    for (CTrackData::trkpt_t& pt : trk) {
      pt.time = timestamp;
      timestamp = timestamp.addSecs(delta);
    }

    deriveSecondaryData();
    changed(tr("Set artificial timestamps with delta of %1 sec.").arg(delta), "://icons/48x48/Time.png");
  }
}

void CGisItemTrk::filterSpeed(qreal speed)  // Constant speed
{
  QDateTime timestamp = timeStart;
  if (!timestamp.isValid()) {
    timestamp = QDateTime::currentDateTimeUtc();
  }

  for (CTrackData::trkpt_t& pt : trk) {
    if (pt.isHidden()) {
      continue;
    }

    timestamp = speed == 0 ? QDateTime() : timestamp.addMSecs(qRound(1000 * pt.deltaDistance / speed));
    pt.time = timestamp;
  }

  deriveSecondaryData();
  QString val, unit;
  IUnit::self().meter2speed(speed, val, unit);
  changed(tr("Changed speed to %1%2.").arg(val, unit), "://icons/48x48/Time.png");
}

void CGisItemTrk::filterSpeed(const CFilterSpeedCycle::cycling_type_t& cyclingType) {
  qreal plainSpeed = cyclingType.plainSpeed / IUnit::self().speedFactor;
  qreal minSpeed = cyclingType.minSpeed / IUnit::self().speedFactor;
  qreal slopeAtMinSpeed = cyclingType.slopeAtMinSpeed;
  qreal maxSpeed = cyclingType.maxSpeed / IUnit::self().speedFactor;
  qreal slopeAtMaxSpeed = cyclingType.slopeAtMaxSpeed;

  QDateTime timestamp = timeStart;
  if (!timestamp.isValid()) {
    timestamp = QDateTime::currentDateTimeUtc();
  }

  qreal speed = 0;

  QEasingCurve::Type upHillType = (minSpeed < plainSpeed) ? QEasingCurve::OutQuad : QEasingCurve::InQuad;
  QEasingCurve::Type downHillType = (maxSpeed < plainSpeed) ? QEasingCurve::OutQuad : QEasingCurve::InQuad;
  QEasingCurve upHillCurve(upHillType);
  QEasingCurve downHillCurve(downHillType);

  for (CTrackData::trkpt_t& pt : trk) {
    if (pt.isHidden()) {
      continue;
    }

    // calculation based on slope2 (Percent)
    qreal slope = pt.slope2;

    if (slope < slopeAtMaxSpeed) {
      speed = maxSpeed;
    } else if (slope < 0 && slope >= slopeAtMaxSpeed) {
      speed = plainSpeed + (maxSpeed - plainSpeed) * downHillCurve.valueForProgress(slope / slopeAtMaxSpeed);
    } else if (slope == 0) {
      speed = plainSpeed;
    } else if (slope > 0 && slope <= slopeAtMinSpeed) {
      speed = plainSpeed + (minSpeed - plainSpeed) * upHillCurve.valueForProgress(slope / slopeAtMinSpeed);
    } else if (slope > slopeAtMinSpeed) {
      speed = minSpeed;
    }

    timestamp = speed == 0 ? timestamp : timestamp.addMSecs(qRound(1000 * pt.deltaDistance / speed));
    pt.time = timestamp;
  }

  deriveSecondaryData();
  QString val, unit;
  IUnit::self().meter2speed(totalDistance / totalElapsedSecondsMoving, val, unit);
  changed(tr("Changed average moving cycling speed with profile '%3' to %1%2.").arg(val, unit, cyclingType.name),
          "://icons/48x48/Time.png");
}

void CGisItemTrk::filterSpeed(const CFilterSpeedHike::hiking_type_t& hikingType) {
  if (!hikingType.isValid()) {
    return;
  }

  QDateTime timestamp = timeStart;
  if (!timestamp.isValid()) {
    timestamp = QDateTime::currentDateTimeUtc();
  }

  // Curve algorithm based on carloscoi curves
  // variable names according to carloscoi xls cell names, A9, B4, B5, B6
  // for better interpretation of formula
  qreal B4 = hikingType.plainSpeed / IUnit::self().speedFactor * 3.6;  // Transform from set format to km/h
  qreal B5 = hikingType.ascending;
  qreal B6 = hikingType.descending;

  //    qDebug() << "KKA: plainSpeed m/s=" << B4 / 3.6 << "plainSpeed km/h=" << B4 << "ascending=" << B5 <<
  //    "descending=" << B6; qint32 i = 0; // Debug only

  for (CTrackData::trkpt_t& pt : trk) {
    if (pt.isHidden()) {
      continue;
    }

    // calculation based on slope2 in percent transformed to tangens
    qreal A9 = pt.slope2 / 100;

    // The carloscoi curve formula is based on 7 terms.
    // For the given four input vaulues: slope, plainSpeed, ascending and descending
    // the max value of these 7 terms will be used for the speed at the trkPnt
    QVector<qreal> formulaTerms(7);
    formulaTerms[0] = 60 / B4;
    formulaTerms[1] = A9 * 60000 / B5;
    formulaTerms[2] = (A9 - 0) * ((60 / B4 + 0.5 * (0.2 * 60000 / B5 - 60 / B4)) - (60 / B4)) / (0.2 - 0) + (60 / B4);
    formulaTerms[3] =
        (A9 - 0.2) * ((60 / B4) - (0.2 * 60000 / B5)) / (((B5 / (B4 * 1000) - 0) * 0.5) - 0.2) + (0.2 * 60000 / B5);
    formulaTerms[4] = -A9 * 60000 / B6;
    formulaTerms[5] =
        (A9 + 0.05) * ((60 / B4 + 0.5 * (0.25 * 60000 / B6 - 60 / B4)) - (60 / B4)) / (-0.25 + 0.05) + (60 / B4);
    formulaTerms[6] =
        (A9 + 0.25) * ((60 / B4) - (0.25 * 60000 / B6)) / ((-0.05 - 0.5 * (B6 / (B4 * 1000) - 0.05)) + 0.25) +
        (0.25 * 60000 / B6);

    std::stable_sort(formulaTerms.begin(), formulaTerms.end(),
                     std::greater<qreal>());  // Sort for max term, formulaTerms[0] is max in min/km

    if (!formulaTerms[0])  // To avoid devide by zero
    {
      continue;
    }
    qreal speed = 1 / formulaTerms[0] / 60 * 1000;  // Transform from min/km to m/s

    timestamp = speed == 0 ? QDateTime() : timestamp.addMSecs(qRound(1000 * pt.deltaDistance / speed));
    pt.time = timestamp;
  }

  deriveSecondaryData();
  QString val, unit;
  IUnit::self().meter2speed(totalDistance / totalElapsedSecondsMoving, val, unit);
  changed(tr("Changed average moving hiking speed with profile '%3' to %1%2.").arg(val, unit, hikingType.name),
          "://icons/48x48/Time.png");
}

void CGisItemTrk::filterGetSlopeLimits(qreal& minSlope, qreal& maxSlope) const {
  const limits_t& limit = extrema["ql:slope"];
  minSlope = limit.min;
  maxSlope = limit.max;
}

void CGisItemTrk::filterSplitSegment() {
  IGisProject* project = CGisWorkspace::self().selectProject(false);
  if (nullptr == project) {
    return;
  }

  int part = 0;
  for (const CTrackData::trkseg_t& seg : std::as_const(trk.segs)) {
    if (0 < seg.pts.count()) {
      qint32 idx1 = seg.pts[0].idxTotal;
      qint32 idx2 = seg.pts[seg.pts.count() - 1].idxTotal;

      new CGisItemTrk(tr("%1 (Segment %2)").arg(trk.name).arg(part), idx1, idx2, trk, project);
      ++part;
    }
  }
}

void CGisItemTrk::filterDeleteExtension(const QString& extStr) {
  for (CTrackData::trkpt_t& pt : trk) {
    pt.extensions.remove(extStr);
  }

  extrema.remove(extStr);
  existingExtensions.remove(extStr);
  propHandler->setupData();

  const CKnownExtension& ext = CKnownExtension::get(extStr);
  changed(tr("Removed extension %1 from all Track Points").arg(ext.nameLongText),
          "://icons/48x48/FilterModifyExtension.png");
}

void CGisItemTrk::filterSubPt2Pt() {
  for (CTrackData::trkpt_t& pt : trk) {
    pt.unsetFlag(CTrackData::trkpt_t::eFlagSubpt);
  }
  propHandler->setupData();

  changed(tr("Converted subpoints from routing to track points"), "://icons/48x48/FilterSubPt2Pt.png");
}

void CGisItemTrk::filterChangeStartPoint(qint32 idxNewStartPoint, const QString& wptName) {
  if ((idxNewStartPoint == 0) || (idxNewStartPoint >= cntVisiblePoints)) {
    return;
  }

  QVector<CTrackData::trkpt_t> pts;
  for (CTrackData::trkpt_t& pt : trk) {
    if (pt.isHidden()) {
      continue;
    }
    pt.keyWpt.clear();
    pts << pt;
  }

  QDateTime oldTimeStart = getTimeStart();
  QDateTime oldTimeEnd = getTimeEnd();
  QDateTime newTimeStart = pts[idxNewStartPoint].time;
  QDateTime newTimeEnd = pts[idxNewStartPoint - 1].time;

  qint64 deltaStart = qint64(oldTimeStart.toUTC().toSecsSinceEpoch()) - qint64(newTimeStart.toUTC().toSecsSinceEpoch());
  qint64 deltaEnd = qint64(oldTimeEnd.toUTC().toSecsSinceEpoch()) - qint64(newTimeEnd.toUTC().toSecsSinceEpoch());

  qint32 i;
  for (i = 0; i < idxNewStartPoint; ++i) {
    pts[i].time = pts[i].time.addSecs(deltaEnd);  // Adjust old Start to new Start
  }

  for (; i < pts.size(); ++i) {
    pts[i].time = pts[i].time.addSecs(deltaStart);  // Adjust new Start to End
  }

  const QVector<CTrackData::trkpt_t>& part1 = pts.mid(0, idxNewStartPoint);
  const QVector<CTrackData::trkpt_t>& part2 = pts.mid(idxNewStartPoint, -1);

  trk.readFrom(part2 + part1);

  deriveSecondaryData();

  changed(tr("Start Point moved to: ") + wptName.toLatin1(), "://icons/48x48/FilterChangeStartPoint.png");
}

void CGisItemTrk::filterLoopsCut(qreal minLoopLength) {
  IGisProject* project = CGisWorkspace::self().selectProject(false);
  if (nullptr == project) {
    return;
  }

  int part = 1;
  QVector<CTrackData::trkpt_t> pts;

  for (const CTrackData::trkpt_t& headPt : trk) {
    if (headPt.isHidden()) {
      continue;
    }

    pts << headPt;

    if (pts.size() >= 4) {
      const QLineF headLine = QLineF(headPt.lon, headPt.lat, pts[pts.size() - 2].lon, pts[pts.size() - 2].lat);

      bool firstCycle = true;
      CTrackData::trkpt_t prevScannedPt;
      for (const CTrackData::trkpt_t& scannedPt : std::as_const(pts)) {
        if (scannedPt.idxTotal == pts[pts.size() - 2].idxTotal) {
          break;
        }

        if (firstCycle) {
          prevScannedPt = scannedPt;
          firstCycle = false;
          continue;
        }

        const QLineF scannedLine = QLineF(scannedPt.lon, scannedPt.lat, prevScannedPt.lon, prevScannedPt.lat);
        QPointF intersectionPoint;

        if ((headLine.intersects(scannedLine, &intersectionPoint) == QLineF::BoundedIntersection) &&
            (pts[pts.size() - 2].distance - scannedPt.distance) >
                minLoopLength)  // loop is long enough to cut the track)
        {
          new CGisItemTrk(tr("%1 (Part %2)").arg(trk.name).arg(part), pts.first().idxTotal,
                          pts[pts.size() - 2].idxTotal, trk, project);
          part++;
          pts.remove(0, pts.size() - 2);

          break;
        }

        prevScannedPt = scannedPt;
      }
    }
  }

  // last part : no loop detected but this last part should be copied, too
  new CGisItemTrk(tr("%1 (Part %2)").arg(trk.name).arg(part), pts.first().idxTotal, pts.last().idxTotal, trk, project);
}

void CGisItemTrk::filterSplitTrack(qint8 nTracks) {
  if (cntTotalPoints <= nTracks)  // Can't split into tracks with length 1 or smaller
  {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("The filter can not be applied"),
                         tr("The number of points in the track must be higher"
                            " than the number of tracks to split into."),
                         QMessageBox::Abort, QMessageBox::Abort);
    return;
  }
  IGisProject* project = CGisWorkspace::self().selectProject(false);
  if (nullptr == project) {
    return;
  }
  const qint32 cntTotalPointsNew = cntTotalPoints + nTracks - 1;
  const qint32 segNodes = cntTotalPointsNew / nTracks;
  qint32 nOver = cntTotalPointsNew % nTracks;  // Reaminder of nodes to distribute over splitted tracks

  qint32 segStartIdx = 0;
  qint32 segEndIdx = segNodes - 1 + (nOver > 0);

  qint8 part = 0;
  while (part < nTracks) {
    new CGisItemTrk(tr("%1 (Part %2)").arg(trk.name).arg(part), segStartIdx, segEndIdx, trk, project);
    // for next segment. If this was last, never used again
    nOver--;
    segStartIdx = segEndIdx;
    segEndIdx = segStartIdx + segNodes - 1 + (nOver > 0);

    ++part;
  }
}

void CGisItemTrk::filterZeroSpeedDriftCleaner(qreal distance, qreal ratio) {
  qint32 knotPtsCount = 0;
  bool knotStarted = false;
  qreal knotLength = 0.0;
  qreal firstKnotEndSegmentFoundLength = 0.0;
  pointDP knotStartPreviousPt, knotEndPt;
  bool firstKnotEndSegmentFound = false;
  QVector<bool> trackPoints;

  for (CTrackData::trkpt_t& pt : trk) {
    if (pt.isHidden()) {
      trackPoints << false;  // it is already invisible, then it is not necessary to hide it again
      continue;
    }

    if (pt.idxVisible == 0)  // first visible point
    {
      trackPoints << false;  // first visible point will never be hidden
    } else {
      const CTrackData::trkpt_t* prevVisiblePt = trk.getTrkPtByVisibleIndex(pt.idxVisible - 1);
      qreal d = pt.distance - prevVisiblePt->distance;  // "distance" field of trackpoints includes visible points only

      if (d < distance)  // "distance" defines the threshold at which knots are detected: knot starts when
                         // distance between 2 consecutive points is shorter than this
      {
        firstKnotEndSegmentFound = false;

        if (!knotStarted) {
          knotStartPreviousPt.x = prevVisiblePt->lon * DEG_TO_RAD;
          knotStartPreviousPt.y = prevVisiblePt->lat * DEG_TO_RAD;
          knotStarted = true;
        }

        knotPtsCount++;
        knotLength += d;
      } else {
        if (!knotStarted) {
          trackPoints << false;  // not part of a knot : point will not be hidden
        }

        if (knotStarted && !firstKnotEndSegmentFound)  // when computed position solution change (when a satellite
                                                       // (dis-)appears), a position jump occurs and a single "long
                                                       // segment" is recorded: this is not the end of the knot
        {
          firstKnotEndSegmentFound = true;

          knotEndPt.x = prevVisiblePt->lon * DEG_TO_RAD;
          knotEndPt.y = prevVisiblePt->lat * DEG_TO_RAD;

          knotPtsCount++;
          knotLength += d;
          firstKnotEndSegmentFoundLength = d;

          continue;
        }

        if (knotStarted && firstKnotEndSegmentFound)  // now we have a second long segment in a row: this is not a
                                                      // solution change and this may be the end of the knot
        {
          knotPtsCount--;                                // to remove point corresponding to 1st long segment found
          knotLength -= firstKnotEndSegmentFoundLength;  // to remove length corresponding to 1st long segment found
          qreal straightDistance =
              GPS_Math_DistanceQuick(knotStartPreviousPt.x, knotStartPreviousPt.y, knotEndPt.x, knotEndPt.y);

          if (knotLength > ratio * straightDistance)  // Ratio is used when track has straight parts at low speed:
                                                      // these are not knots and must not be removed.
                                                      // Knots are twisty, slow speed parts are not.
                                                      // The filter will detect a knot if the length of the knot is
                                                      // greater than the ratio multiplied by the distance on a straight
                                                      // line between the beginning of the knot and its end.
                                                      // The default value of the ratio parameter is 2.

          {  // true knot
            for (qint32 i = 0; i < knotPtsCount; i++) {
              trackPoints << true;
            }
          } else {  // low speed part, not a knot
            for (qint32 i = 0; i < knotPtsCount; i++) {
              trackPoints << false;
            }
          }
          knotLength = 0;
          knotPtsCount = 0;
          knotStarted = false;

          trackPoints << false;  // second point of the first long segment found
          trackPoints << false;  // second point of the second long segment found
        }
      }
    }
  }

  if (knotPtsCount > 0)  // if knot is at the end of the track, these points must be hidden, too
  {
    for (qint32 i = 0; i < knotPtsCount; i++) {
      trackPoints << true;
    }
  }

  Q_ASSERT_X(trackPoints.size() == trk.segs.last().pts.last().idxTotal + 1, "just before visibility change",
             "trackPoints size is different from trk number of points");
  for (CTrackData::trkpt_t& pt : trk) {
    bool toBeHidden = trackPoints.takeFirst();
    if (toBeHidden && pt.desc.isEmpty() && pt.keyWpt.item.isEmpty()) {
      pt.setFlag(CTrackData::trkpt_t::eFlagHidden);
    }
  }

  deriveSecondaryData();
  QString val, unit;
  IUnit::self().meter2distance(distance, val, unit);
  changed(
      tr("Hide zero speed drift knots with a distance criteria of (%1%2) and ratio of (%3)").arg(val, unit).arg(ratio),
      "://icons/48x48/FilterZeroSpeedDriftCleaner.png");
}
