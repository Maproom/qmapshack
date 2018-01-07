/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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

#include "CMainWindow.h"
#include "GeoMath.h"
#include "gis/CGisWorkspace.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CPropertyTrk.h"

#include <QtMath>
#include <proj_api.h>

void CGisItemTrk::filterReducePoints(qreal dist)
{
    QVector<pointDP> line;
    bool nothingDone = true;

    for(const CTrackData::trkpt_t &pt : trk)
    {
        pointDP dp(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD, pt.ele);
        dp.used = !pt.isHidden();

        line << dp;
    }

    if(line.size() < 3)
    {
        return;
    }

    point3D pt0 = line[0];

    line[0].x = 0;
    line[0].y = 0;
    for(int i = 1; i < line.size(); i++)
    {
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

    for(CTrackData::trkpt_t& pt : trk)
    {
        if(line[cnt].used)
        {
            pt.unsetFlag(CTrackData::trkpt_t::eHidden);
        }
        else
        {
            if(!pt.isHidden())
            {
                nothingDone = false;
                pt.setFlag(CTrackData::trkpt_t::eHidden);
            }
        }

        cnt++;
    }

    if(nothingDone)
    {
        return;
    }

    deriveSecondaryData();
    QString val, unit;
    IUnit::self().meter2distance(dist, val, unit);
    changed(tr("Hide points by Douglas Peuker algorithm (%1%2)").arg(val).arg(unit), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterRemoveInvalidPoints()
{
    bool nothingDone = true;

    // use all valid flags as invalid mask. By that only
    // invalid flags for properties with valid points count
    quint32 invalidMask = (getAllValidFlags() & CTrackData::trkpt_t::eValidMask) << 16;

    for(CTrackData::trkpt_t& pt : trk)
    {
        if(pt.isInvalid(CTrackData::trkpt_t::invalid_e(invalidMask)))
        {
            pt.setFlag(CTrackData::trkpt_t::eHidden);
            nothingDone = false;
        }
    }

    if(nothingDone)
    {
        return;
    }

    deriveSecondaryData();
    changed(tr("Hide points with invalid data."), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterReset()
{
    for(CTrackData::trkpt_t& pt : trk)
    {
        pt.unsetFlag(CTrackData::trkpt_t::eHidden);
    }
    deriveSecondaryData();
    changed(tr("Reset all hidden track points to visible"), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterDelete()
{
    bool nothingDone = true;

    for(CTrackData::trkseg_t& seg : trk.segs)
    {
        QVector<CTrackData::trkpt_t> pts;
        for(const CTrackData::trkpt_t &pt : seg.pts)
        {
            if(pt.isHidden())
            {
                nothingDone = false;
                continue;
            }

            pts << pt;
        }

        seg.pts = pts;
    }

    if(nothingDone)
    {
        return;
    }

    deriveSecondaryData();
    changed(tr("Permanently removed all hidden track points"), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterSmoothProfile(int points)
{
    QVector<int> window(points, 0);
    QVector<int> ele1, ele2;

    for(const CTrackData::trkpt_t &pt : trk)
    {
        ele1 << pt.ele;
        ele2 << pt.ele;
    }

    if(ele1.size() < (points + 1))
    {
        return;
    }

    int d = points >> 1;
    for(int i = d; i < ele1.size() - d; i++)
    {
        for(int n = i - d, m = 0; m < points; n++, m++)
        {
            window[m] = ele1[n];
        }

        qSort(window);
        ele2[i] = window[d];
    }

    int cnt = 0;
    for(CTrackData::trkpt_t& pt : trk)
    {
        pt.ele = ele2[cnt++];
    }
    deriveSecondaryData();
    changed(tr("Smoothed profile with a Median filter of size %1").arg(points), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterTerrainSlope()
{
    QPolygonF line;
    for(const CTrackData::trkpt_t &pt : trk)
    {
        line << pt.radPoint();
    }

    QPolygonF slope(line.size());
    CMainWindow::self().getSlopeAt(line, slope);

    int cnt = 0;
    for(CTrackData::trkpt_t& pt : trk)
    {
        pt.extensions[CKnownExtension::internalTerrainSlope] = slope[cnt].ry();
        ++cnt;
    }

    deriveSecondaryData();
    changed(tr("Added terrain slope from DEM file."), "://icons/48x48/CSrcSlope.png");
}

void CGisItemTrk::filterReplaceElevation()
{
    QPolygonF line;
    for(const CTrackData::trkpt_t &pt : trk)
    {
        line << pt.radPoint();
    }

    QPolygonF ele(line.size());
    CMainWindow::self().getElevationAt(line, ele);

    int cnt = 0;
    for(CTrackData::trkpt_t& pt : trk)
    {
        pt.ele = (ele[cnt].y() == NOFLOAT) ? NOINT : ele[cnt].y();
        ++cnt;
    }

    deriveSecondaryData();
    changed(tr("Replaced elevation data with data from DEM files."), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterInterpolateElevation()
{
    if(!interp.valid)
    {
        return;
    }

    for(CTrackData::trkpt_t& pt : trk)
    {
        qreal ele = getElevationInterpolated(pt.distance);
        pt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);
    }

    interp.valid = false;
    deriveSecondaryData();
    changed(tr("Replaced elevation data with interpolated values. (M=%1, RMSErr=%2)").arg(interp.m).arg(interp.rep.rmserror), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterOffsetElevation(int offset)
{
    for(CTrackData::trkpt_t& pt : trk)
    {
        if(pt.ele != NOINT)
        {
            pt.ele += offset;
        }
    }

    QString val, unit;
    IUnit::self().meter2elevation(offset, val, unit);
    deriveSecondaryData();
    changed(tr("Offset elevation data by %1%2.").arg(val).arg(unit), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterNewDate(const QDateTime& date)
{
    qint64 delta = qint64(date.toTime_t()) - qint64(timeStart.toUTC().toTime_t());

    for(CTrackData::trkpt_t& pt : trk)
    {
        pt.time = pt.time.addSecs(delta);
    }

    deriveSecondaryData();
    changed(tr("Changed start of track to %1.").arg(date.toString()), "://icons/48x48/Time.png");
}

void CGisItemTrk::filterObscureDate(int delta)
{
    if(delta == 0)
    {
        for(CTrackData::trkpt_t& pt : trk)
        {
            pt.time = QDateTime();
        }

        deriveSecondaryData();
        changed(tr("Remove timestamps."), "://icons/48x48/Time.png");
    }
    else
    {
        QDateTime timestamp = timeStart;
        if(!timestamp.isValid())
        {
            timestamp = QDateTime::currentDateTime().toUTC();
        }

        for(CTrackData::trkpt_t& pt : trk)
        {
            pt.time = timestamp;
            timestamp = timestamp.addSecs(delta);
        }

        deriveSecondaryData();
        changed(tr("Set artificial timestamps with delta of %1 sec.").arg(delta), "://icons/48x48/Time.png");
    }
}

void CGisItemTrk::filterSpeed(qreal linearSpeed,
                              qreal minSpeed, qreal slopeAtMinSpeed, QEasingCurve::Type upHillCurveType,
                              qreal maxSpeed, qreal slopeAtMaxSpeed, QEasingCurve::Type downHillCurveType)
{
   QDateTime timestamp = timeStart;
    if(!timestamp.isValid())
    {
        timestamp = QDateTime::currentDateTime().toUTC();
    }

    qreal averageSpeed = 0, speed;
    qint32 noOfPoints = 0;

    QEasingCurve upHillCurve(upHillCurveType);
    QEasingCurve downHillCurve(downHillCurveType);

    for(CTrackData::trkpt_t& pt : trk)
    {
        if(pt.isHidden())
        {
            continue;
        }

        if(pt.slope2 < slopeAtMaxSpeed)
        {
           speed = maxSpeed;
        }
        else if(pt.slope2 < 0 && pt.slope2 >= slopeAtMaxSpeed)
        {
            // -(P2Y-P0Y)/POTENZ(P2X;2)*POTENZ(slope-P2X;2)+P2Y
//            speed = -(maxSpeed - linearSpeed) / pow(slopeAtMaxSpeed, 2) * pow(pt.slope2 - slopeAtMaxSpeed, 2) + maxSpeed;
            speed = linearSpeed + (maxSpeed - linearSpeed) * downHillCurve.valueForProgress(pt.slope2 / slopeAtMaxSpeed);
        }
        else if(pt.slope2 == 0)
        {
           speed = linearSpeed;
        }
        else if(pt.slope2 > 0 && pt.slope2 <= slopeAtMinSpeed)
        {
            // (P0Y-P1Y)/POTENZ(P1X;2)*POTENZ(P1X-slope;2)+P1Y
//            speed = (linearSpeed - minSpeed) / pow(slopeAtMinSpeed , 2) * pow(slopeAtMinSpeed - pt.slope2, 2) + minSpeed;
            speed = linearSpeed + (minSpeed - linearSpeed) * upHillCurve.valueForProgress(pt.slope2 / slopeAtMinSpeed);
        }
        else if(pt.slope2 > slopeAtMinSpeed)
        {
            speed = minSpeed;
        }
//        qDebug() << "pt.slope: " << pt.slope2 << " speed: " << speed*3.6 << "\n";

        timestamp = speed == 0 ? QDateTime() : timestamp.addMSecs(qRound(1000 * pt.deltaDistance/speed));
        pt.time   = timestamp;

        averageSpeed += speed;
        ++noOfPoints;
    }

    speed = averageSpeed / noOfPoints;

    deriveSecondaryData();
    QString val, unit;
    IUnit::self().meter2speed(speed, val, unit);
    changed(tr("Changed average speed depending on slope to %1%2.").arg(val).arg(unit), "://icons/48x48/Time.png");
}

void CGisItemTrk::filterSpeed(qreal speed)
{
    QDateTime timestamp = timeStart;
    if(!timestamp.isValid())
    {
        timestamp = QDateTime::currentDateTime().toUTC();
    }

    for(CTrackData::trkpt_t& pt : trk)
    {
        if(pt.isHidden())
        {
            continue;
        }

        timestamp = speed == 0 ? QDateTime() : timestamp.addMSecs(qRound(1000 * pt.deltaDistance/speed));
        pt.time   = timestamp;
    }

    deriveSecondaryData();
    QString val, unit;
    IUnit::self().meter2speed(speed, val, unit);
    changed(tr("Changed speed to %1%2.").arg(val).arg(unit), "://icons/48x48/Time.png");
}

void CGisItemTrk::filterSplitSegment()
{
    IGisProject * project = CGisWorkspace::self().selectProject();
    if(nullptr == project)
    {
        return;
    }

    int part = 0;
    for(const CTrackData::trkseg_t &seg : trk.segs)
    {
        if(0 < seg.pts.count())
        {
            qint32 idx1 = seg.pts[                  0].idxTotal;
            qint32 idx2 = seg.pts[seg.pts.count() - 1].idxTotal;

            new CGisItemTrk(tr("%1 (Segment %2)").arg(trk.name).arg(part), idx1, idx2, trk, project);
            ++part;
        }
    }
}

void CGisItemTrk::filterDeleteExtension(const QString &extStr)
{
    for(CTrackData::trkpt_t& pt : trk)
    {
        pt.extensions.remove(extStr);
    }

    extrema.remove(extStr);
    existingExtensions.remove(extStr);
    propHandler->setupData();

    const CKnownExtension &ext = CKnownExtension::get(extStr);
    changed(tr("Removed extension %1 from all Track Points").arg(ext.nameLongText), "://icons/48x48/FilterModifyExtension.png");
}

void CGisItemTrk::filterSubPt2Pt()
{
    for(CTrackData::trkpt_t& pt : trk)
    {
        pt.unsetFlag(CTrackData::trkpt_t::eSubpt);
    }
    propHandler->setupData();

    changed(tr("Converted subpoints from routing to track points"), "://icons/48x48/FilterSubPt2Pt.png");
}
