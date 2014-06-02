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
#include <QString>
#include <QPointF>

struct point3D
{
    double x;
    double y;
    double z;
};

struct pointDP : public point3D
{
    pointDP():used(true){}
    bool used;
};

extern void     GPS_Math_Deg_To_Str(const qreal& x, const qreal& y, QString& str);
extern bool     GPS_Math_Str_To_Deg(const QString& str, qreal& lon, qreal& lat);
/// use for long distances
extern qreal    GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2, qreal& a1, qreal& a2);
extern qreal    GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2);
/// use for short distances, much quicker processing
extern qreal    GPS_Math_DistanceQuick(const qreal u1, const qreal v1, const qreal u2, const qreal v2);
extern void     GPS_Math_Wpt_Projection(const qreal lon1, const qreal lat1, const qreal distance, const qreal bearing, qreal& lon2, qreal& lat2);
extern void     GPS_Math_DouglasPeucker(QVector<pointDP>& line, double d);
extern QPointF  GPS_Math_Wpt_Projection(const QPointF& pt1, double distance, double bearing);

#endif                           //GEOMATH_H
