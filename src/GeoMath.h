/**********************************************************************************************
    Copyright (C) 2009 Oliver Eichler oliver.eichler@gmx.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#ifndef GEOMATH_H
#define GEOMATH_H
#include <QPointF>
#include <QRectF>
#include <QString>

#include <stdint.h>

class QPolygonF;
class IDrawContext;

struct point3D
{
    qreal x = 0;
    qreal y = 0;
    qreal z = 0;
};

struct pointDP : public point3D
{
    pointDP();
    pointDP(const qreal &x, const qreal &y, const qreal &z);
    bool used;
    qint32 idx;
};

struct segment_t
{
    segment_t();

    void apply(const QPolygonF& coords, const QPolygonF& pixel, QPolygonF& segCoord, QPolygonF& segPixel, IDrawContext * context);

    qint32 idx11;
    qint32 idx12;
    qint32 idx21;

    QPointF px1;
    QPointF px2;
};

void    GPS_Math_DegMin_To_Deg(bool sign, const qint32 d, const qreal m, qreal& deg);
void    GPS_Math_DegMinSec_To_Deg(bool sign, const qint32 d, const qint32 m, const qreal s, qreal& deg);
bool    GPS_Math_Deg_To_DegMin(qreal v, qint32 *deg, qreal *min);
/// use for long distances
qreal   GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2, qreal& a1, qreal& a2);
qreal   GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2);
/// use for short distances, much quicker processing
qreal   GPS_Math_DistanceQuick(const qreal u1, const qreal v1, const qreal u2, const qreal v2);
void    GPS_Math_Wpt_Projection(const qreal lon1, const qreal lat1, const qreal distance, const qreal bearing, qreal& lon2, qreal& lat2);
void    GPS_Math_DouglasPeucker(QVector<pointDP>& line, qreal d);
QPointF GPS_Math_Wpt_Projection(const QPointF& pt1, qreal distance, qreal bearing);
bool    GPS_Math_LineCrossesRect(const QPointF& p1, const QPointF& p2, const QRectF& rect);
void    GPS_Math_SubPolyline(const QPointF& pt1, const QPointF& pt2, qint32 threshold, const QPolygonF& pixel, segment_t& result);
qreal   GPS_Math_DistPointPolyline(const QPolygonF &points, const QPointF &q);

#endif                           //GEOMATH_H

