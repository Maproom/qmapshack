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

#include "gis/trk/CGisItemTrk.h"
#include "GeoMath.h"

#include <proj_api.h>

void CGisItemTrk::filterReducePoints(qreal dist)
{

    QVector<pointDP> line;

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

        pt2.x = pt1.x + cos(a1 * DEG_TO_RAD) * d;
        pt2.y = pt1.y + sin(a1 * DEG_TO_RAD) * d;
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
                pt.flags |=  trkpt_t::eHidden;
            }

            cnt++;
        }
    }

    QString val, unit;
    IUnit::self().meter2distance(dist, val, unit);

    deriveSecondaryData();
    changed(QObject::tr("Reduced points by Douglas Peuker algorithm (%1%2)").arg(val).arg(unit), "://icons/48x48/PointHide.png");
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
