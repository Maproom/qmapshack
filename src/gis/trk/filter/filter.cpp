/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/CGisWidget.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CPropertyTrk.h"

#include <QtMath>
#include <proj_api.h>

void CGisItemTrk::filterReducePoints(qreal dist)
{
    QVector<pointDP> line;
    bool nothingDone = true;

    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt, seg.pts)
        {
            pointDP dp;
            dp.x = pt.lon * DEG_TO_RAD;
            dp.y = pt.lat * DEG_TO_RAD;
            dp.z = pt.ele;
            dp.used = !(pt.flags & CGisItemTrk::trkpt_t::eHidden);

            line << dp;
        }
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
        qreal d, a1, a2;
        pointDP& pt1 = line[i - 1];
        pointDP& pt2 = line[i];

        d = GPS_Math_Distance(pt0.x, pt0.y, pt2.x, pt2.y, a1, a2);

        pt0 = pt2;

        pt2.x = pt1.x + qCos(a1 * DEG_TO_RAD) * d;
        pt2.y = pt1.y + qSin(a1 * DEG_TO_RAD) * d;
    }

    GPS_Math_DouglasPeucker(line, dist);

    int cnt = 0;

    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];

            if(line[cnt].used)
            {
                pt.flags &= ~trkpt_t::eHidden;
            }
            else
            {
                if((pt.flags & trkpt_t::eHidden) == 0)
                {
                    nothingDone = false;
                    pt.flags |=  trkpt_t::eHidden;
                }
            }

            cnt++;
        }
    }

    if(nothingDone)
    {
        return;
    }

    deriveSecondaryData();
    QString val, unit;
    IUnit::self().meter2distance(dist, val, unit);
    changed(QObject::tr("Hide points by Douglas Peuker algorithm (%1%2)").arg(val).arg(unit), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterRemoveNullPoints()
{
    bool nothingDone    = true;
    bool done           = false;

    for(int i = 0; i < trk.segs.size() && !done; i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size() && !done; n++)
        {
            trkpt_t& pt = seg.pts[n];

            if( (NOFLOAT == pt.lat || 0. == pt.lat)
                && (NOFLOAT == pt.lon || 0. == pt.lon) )
            {
                pt.flags |= trkpt_t::eHidden;
                nothingDone = false;
            }
            else
            {
                done = true;
            }
        }
    }

    if(nothingDone)
    {
        return;
    }

    deriveSecondaryData();
    changed(QObject::tr("Hide points with invalid coordinates at the beginning of the track"), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterReset()
{
    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];
            pt.flags &= ~trkpt_t::eHidden;
        }
    }
    deriveSecondaryData();
    changed(QObject::tr("Reset all hidden track points to visible"), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterDelete()
{
    bool nothingDone = true;

    for(int i = 0; i < trk.segs.size(); i++)
    {
        QVector<trkpt_t> pts;
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];

            if(pt.flags & trkpt_t::eHidden)
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
    changed(QObject::tr("Permanently removed all hidden track points"), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::filterSmoothProfile(int points)
{
    QVector<int> window(points, 0);
    QVector<int> ele1, ele2;

    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];

            ele1 << pt.ele;
            ele2 << pt.ele;
        }
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
    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];
            pt.ele = ele2[cnt++];
        }
    }
    deriveSecondaryData();
    changed(QObject::tr("Smoothed profile with a Median filter of size %1").arg(points), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterReplaceElevation()
{
    QPolygonF line;

    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];

            line << QPointF(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD);
        }
    }

    QPolygonF ele(line.size());
    CMainWindow::self().getElevationAt(line, ele);

    int cnt = 0;
    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];

            pt.ele = ele[cnt++].y();
        }
    }

    deriveSecondaryData();
    changed(QObject::tr("Replaced elevation data with data from DEM files."), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterOffsetElevation(int offset)
{
    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];

            if(pt.ele != NOINT)
            {
                pt.ele += offset;
            }
        }
    }

    QString val, unit;
    IUnit::self().meter2elevation(offset, val, unit);
    deriveSecondaryData();
    changed(QObject::tr("Offset elevation data by %1%2.").arg(val).arg(unit), "://icons/48x48/SetEle.png");
}

void CGisItemTrk::filterNewDate(const QDateTime& date)
{
    qint64 delta = qint64(date.toTime_t()) - qint64(timeStart.toUTC().toTime_t());

    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];
            pt.time = pt.time.addSecs(delta);
        }
    }

    deriveSecondaryData();
    changed(QObject::tr("Changed start of track to %1.").arg(date.toString()), "://icons/48x48/Time.png");
}

void CGisItemTrk::filterObscureDate(int delta)
{
    if(delta == 0)
    {
        for(int i = 0; i < trk.segs.size(); i++)
        {
            trkseg_t& seg = trk.segs[i];

            for(int n = 0; n < seg.pts.size(); n++)
            {
                trkpt_t& pt = seg.pts[n];
                pt.time = QDateTime();
            }
        }

        deriveSecondaryData();
        changed(QObject::tr("Remove timestamps."), "://icons/48x48/Time.png");
    }
    else
    {
        QDateTime timestamp = timeStart;
        if(!timestamp.isValid())
        {
            timestamp = QDateTime::currentDateTime().toUTC();
        }

        for(int i = 0; i < trk.segs.size(); i++)
        {
            trkseg_t& seg = trk.segs[i];

            for(int n = 0; n < seg.pts.size(); n++)
            {
                trkpt_t& pt = seg.pts[n];
                pt.time = timestamp;
                timestamp = timestamp.addSecs(delta);
            }
        }

        deriveSecondaryData();
        changed(QObject::tr("Set artificial timestamps with delta of %1 sec.").arg(delta), "://icons/48x48/Time.png");
    }
}

void CGisItemTrk::filterSpeed(qreal speed)
{
    QDateTime timestamp = timeStart;
    if(!timestamp.isValid())
    {
        timestamp = QDateTime::currentDateTime().toUTC();
    }

    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            trkpt_t& pt = seg.pts[n];
            if(pt.flags & trkpt_t::eHidden)
            {
                continue;
            }

            qreal dmsec = 1000 * pt.deltaDistance/speed;
            timestamp   = timestamp.addMSecs(qRound(dmsec));
            pt.time     = timestamp;
        }
    }

    deriveSecondaryData();
    QString val, unit;
    IUnit::self().meter2speed(speed, val, unit);
    changed(QObject::tr("Changed speed to %1%2.").arg(val).arg(unit), "://icons/48x48/Time.png");
}

void CGisItemTrk::filterSplitSegment()
{
    IGisProject * project = CGisWidget::self().selectProject();
    if(nullptr == project)
    {
        return;
    }

    int part = 0;
    foreach(const trkseg_t &seg, trk.segs)
    {
        if(0 < seg.pts.count())
        {
            qint32 idx1 = seg.pts[                  0].idxTotal;
            qint32 idx2 = seg.pts[seg.pts.count() - 1].idxTotal;

            new CGisItemTrk(QObject::tr("%1 (Segment %2)").arg(trk.name).arg(part), idx1, idx2, trk, project);
            part++;
        }
    }
}

void CGisItemTrk::filterDeleteExtension(const QString &extStr)
{
    for(int i = 0; i < trk.segs.size(); i++)
    {
        trkseg_t& seg = trk.segs[i];

        for(int n = 0; n < seg.pts.size(); n++)
        {
            seg.pts[n].extensions.remove(extStr);
        }
    }

    extrema.remove(extStr);
    existingExtensions.remove(extStr);
    propHandler->setupData();

    const CKnownExtension &ext = CKnownExtension::get(extStr);
    changed(QObject::tr("Removed extension %1 from all Track Points").arg(ext.name), "://icons/48x48/FilterModifyExtension.png");
    updateVisuals(eVisualAll, "filterDeleteExtension()");
}
