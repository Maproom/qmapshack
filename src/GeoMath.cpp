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

#include "GeoMath.h"
#include "canvas/IDrawContext.h"
#include <proj_api.h>
#include <stdlib.h>

#include <QtGui>
#include <QtWidgets>

#define PI M_PI
#define TWOPI (2*PI)

void GPS_Math_DegMinSec_To_Deg(bool sign, const int32_t d, const int32_t m, const int32_t s, qreal &deg)
{
    deg = qAbs(d) + qreal(m) / 60.0 + qreal(s) / 3600;
    if(sign)
    {
        deg = -deg;
    }

    return;
}


bool GPS_Math_Deg_To_DegMin(qreal v, qint32 *d, qreal *m)
{
    bool sign = v < 0;
    qint32 deg = qAbs(v);
    qreal min = (qAbs(v) - deg) * 60.0;

    *d = deg;
    *m = min;

    return sign;
}


void GPS_Math_DegMin_To_Deg(bool sign, const qint32 d, const qreal m, qreal& deg)
{
    deg = qAbs(d) + m / 60.0;
    if(sign)
    {
        deg = -deg;
    }

    return;
}

void GPS_Math_Deg_To_Str(const qreal& x, const qreal& y, QString& str)
{
    qint32 degN,degE;
    qreal minN,minE;

    bool signLat = GPS_Math_Deg_To_DegMin(y, &degN, &minN);

    bool signLon = GPS_Math_Deg_To_DegMin(x, &degE, &minE);

    QString lat,lng;
    lat = signLat ? "S" : "N";
    lng = signLon ? "W" : "E";
    str.sprintf("%s%02d° %06.3f %s%03d° %06.3f",lat.toUtf8().data(),qAbs(degN),minN,lng.toUtf8().data(),qAbs(degE),minE);
}


static QRegExp reCoord1("^\\s*([N|S]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s+([E|W|O]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s*$");

static QRegExp reCoord2("^\\s*([N|S]){1}\\s*([0-9]+\\.[0-9]+)\\W*\\s+([E|W|O]){1}\\s*([0-9]+\\.[0-9]+)\\W*\\s*$");

static QRegExp reCoord3("^\\s*([-0-9]+\\.[0-9]+)\\s+([-0-9]+\\.[0-9]+)\\s*$");

static QRegExp reCoord4("^\\s*([N|S]){1}\\s*([0-9]+)\\W+([0-9]+)\\W+([0-9]+)\\W*([E|W|O]){1}\\W*([0-9]+)\\W+([0-9]+)\\W+([0-9]+)\\W*\\s*$");

static QRegExp reCoord5("^\\s*([-0-9]+\\.[0-9]+)([N|S])\\s+([-0-9]+\\.[0-9]+)([W|E])\\s*$");


bool GPS_Math_Str_To_Deg(const QString& str, qreal& lon, qreal& lat)
{
    if(reCoord2.exactMatch(str))
    {
        bool signLat    = reCoord2.cap(1) == "S";
        float absLat    = reCoord2.cap(2).toDouble();
        lat = signLat ? -absLat : absLat;

        bool signLon    = reCoord2.cap(3) == "W";
        float absLon    = reCoord2.cap(4).toDouble();
        lon = signLon ? -absLon : absLon;
    }
    else if(reCoord1.exactMatch(str))
    {
        bool signLat    = reCoord1.cap(1) == "S";
        int degLat      = reCoord1.cap(2).toInt();
        float minLat    = reCoord1.cap(3).toDouble();

        GPS_Math_DegMin_To_Deg(signLat, degLat, minLat, lat);

        bool signLon    = reCoord1.cap(4) == "W";
        int degLon      = reCoord1.cap(5).toInt();
        float minLon    = reCoord1.cap(6).toDouble();

        GPS_Math_DegMin_To_Deg(signLon, degLon, minLon, lon);
    }
    else if(reCoord3.exactMatch(str))
    {
        lat             = reCoord3.cap(1).toDouble();
        lon             = reCoord3.cap(2).toDouble();
    }
    else if(reCoord4.exactMatch(str))
    {
        bool signLat    = reCoord4.cap(1) == "S";
        int degLat    = reCoord4.cap(2).toInt();
        int minLat    = reCoord4.cap(3).toInt();
        int secLat    = reCoord4.cap(4).toInt();

        GPS_Math_DegMinSec_To_Deg(signLat, degLat, minLat, secLat, lat);

        bool signLon    = reCoord4.cap(5) == "W";
        int degLon    = reCoord4.cap(6).toInt();
        int minLon    = reCoord4.cap(7).toInt();
        int secLon    = reCoord4.cap(8).toInt();

        GPS_Math_DegMinSec_To_Deg(signLon, degLon, minLon, secLon, lon);
    }
    else if(reCoord5.exactMatch(str))
    {
        bool signLon    = reCoord4.cap(4) == "W";
        bool signLat    = reCoord4.cap(2) == "S";
        lat             = reCoord5.cap(1).toDouble();
        lon             = reCoord5.cap(3).toDouble();

        if(signLon)
        {
            lon = -lon;
        }
        if(signLat)
        {
            lat = -lat;
        }
    }
    else
    {
        QMessageBox::warning(0,QObject::tr("Error"),QObject::tr("Bad position format. Must be: \"[N|S] ddd mm.sss [W|E] ddd mm.sss\" or \"[N|S] ddd.ddd [W|E] ddd.ddd\""),QMessageBox::Ok,QMessageBox::NoButton);
        return false;
    }

    if(fabs(lon) > 180.0 || fabs(lat) > 90.0)
    {
        QMessageBox::warning(0,QObject::tr("Error"),QObject::tr("Position values out of bounds. "),QMessageBox::Ok,QMessageBox::NoButton);
        return false;
    }

    return true;
}

// from http://www.movable-type.co.uk/scripts/LatLongVincenty.html
qreal GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2, qreal& a1, qreal& a2)
{
    qreal cosSigma = 0.0;
    qreal sigma = 0.0;
    qreal sinAlpha = 0.0;
    qreal cosSqAlpha = 0.0;
    qreal cos2SigmaM = 0.0;
    qreal sinSigma = 0.0;
    qreal sinLambda = 0.0;
    qreal cosLambda = 0.0;

    qreal a = 6378137.0, b = 6356752.3142,  f = 1.0/298.257223563;  // WGS-84 ellipsiod
    qreal L = u2 - u1;

    qreal U1 = qAtan((1-f) * qTan(v1));
    qreal U2 = qAtan((1-f) * qTan(v2));
    qreal sinU1 = qSin(U1), cosU1 = qCos(U1);
    qreal sinU2 = qSin(U2), cosU2 = qCos(U2);
    qreal lambda = L, lambdaP = 2*PI;
    unsigned iterLimit = 20;

    while ((qAbs(lambda - lambdaP) > 1e-12) && (--iterLimit > 0))
    {
        sinLambda = qSin(lambda);
        cosLambda = qCos(lambda);
        sinSigma = qSqrt((cosU2*sinLambda) * (cosU2*sinLambda) + (cosU1*sinU2-sinU1*cosU2*cosLambda) * (cosU1*sinU2-sinU1*cosU2*cosLambda));

        if (sinSigma==0)
        {
            return 0;   // co-incident points
        }

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = qAtan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;

        if (qIsNaN(cos2SigmaM))
        {
            cos2SigmaM = 0;  // equatorial line: cosSqAlpha=0 (§6)
        }

        qreal C = f/16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1-C) * f * sinAlpha * (sigma + C*sinSigma*(cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    }
    if (iterLimit==0)
    {
        return FP_NAN;                // formula failed to converge
    }
    qreal uSq = cosSqAlpha * (a*a - b*b) / (b*b);
    qreal A = 1 + uSq/16384*(4096+uSq*(-768+uSq*(320-175*uSq)));
    qreal B = uSq/1024 * (256+uSq*(-128+uSq*(74-47*uSq)));
    qreal deltaSigma = B*sinSigma*(cos2SigmaM+B/4*(cosSigma*(-1+2*cos2SigmaM*cos2SigmaM)-B/6*cos2SigmaM*(-3+4*sinSigma*sinSigma)*(-3+4*cos2SigmaM*cos2SigmaM)));
    qreal s = b*A*(sigma-deltaSigma);

    a1 = qAtan2(cosU2 * sinLambda, cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * 360 / TWOPI;
    a2 = qAtan2(cosU1 * sinLambda, -sinU1 * cosU2 + cosU1 * sinU2 * cosLambda) * 360 / TWOPI;
    return s;
}

qreal GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2)
{
    qreal cosSigma = 0.0;
    qreal sigma = 0.0;
    qreal sinAlpha = 0.0;
    qreal cosSqAlpha = 0.0;
    qreal cos2SigmaM = 0.0;
    qreal sinSigma = 0.0;
    qreal sinLambda = 0.0;
    qreal cosLambda = 0.0;

    qreal a = 6378137.0, b = 6356752.3142,  f = 1.0/298.257223563;  // WGS-84 ellipsiod
    qreal L = u2 - u1;

    qreal U1 = qAtan((1-f) * qTan(v1));
    qreal U2 = qAtan((1-f) * qTan(v2));
    qreal sinU1 = qSin(U1), cosU1 = qCos(U1);
    qreal sinU2 = qSin(U2), cosU2 = qCos(U2);
    qreal lambda = L, lambdaP = 2*PI;
    unsigned iterLimit = 20;

    while ((qAbs(lambda - lambdaP) > 1e-12) && (--iterLimit > 0))
    {
        sinLambda = qSin(lambda);
        cosLambda = qCos(lambda);
        sinSigma = qSqrt((cosU2*sinLambda) * (cosU2*sinLambda) + (cosU1*sinU2-sinU1*cosU2*cosLambda) * (cosU1*sinU2-sinU1*cosU2*cosLambda));

        if (sinSigma==0)
        {
            return 0;   // co-incident points
        }

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = qAtan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;

        if (qIsNaN(cos2SigmaM))
        {
            cos2SigmaM = 0;  // equatorial line: cosSqAlpha=0 (§6)
        }

        qreal C = f/16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1-C) * f * sinAlpha * (sigma + C*sinSigma*(cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    }
    if (iterLimit==0)
    {
        return FP_NAN;                // formula failed to converge
    }
    qreal uSq = cosSqAlpha * (a*a - b*b) / (b*b);
    qreal A = 1 + uSq/16384*(4096+uSq*(-768+uSq*(320-175*uSq)));
    qreal B = uSq/1024 * (256+uSq*(-128+uSq*(74-47*uSq)));
    qreal deltaSigma = B*sinSigma*(cos2SigmaM+B/4*(cosSigma*(-1+2*cos2SigmaM*cos2SigmaM)-B/6*cos2SigmaM*(-3+4*sinSigma*sinSigma)*(-3+4*cos2SigmaM*cos2SigmaM)));
    qreal s = b*A*(sigma-deltaSigma);

    return s;
}

qreal GPS_Math_DistanceQuick(const qreal u1, const qreal v1, const qreal u2, const qreal v2)
{
    qreal dU = u2 - u1; // lambda
    qreal dV = v2 - v1; // roh

    qreal d = 2*qAsin(qSqrt(qSin(dV/2) * qSin(dV/2) + qCos(v1) * qCos(v2) * qSin(dU/2) * qSin(dU/2)));

    return 6371010 * d;
}

void GPS_Math_Wpt_Projection(const qreal lon1, const qreal lat1, const qreal distance, const qreal bearing, qreal& lon2, qreal& lat2)
{
    qreal d    = distance / 6378130.0;
    lat2 = qAsin(qSin(lat1) * qCos(d) + qCos(lat1) * qSin(d) * qCos(-bearing));
    lon2 = qCos(lat1) == 0 ? lon1 : fmod(lon1 - qAsin(qSin(-bearing) * qSin(d) / qCos(lat1)) + PI, TWOPI) - PI;
}

qreal GPS_Math_distPointLine3D(point3D& x1, point3D& x2, point3D& x0)
{
    point3D v1, v2, v3, v1x2;

    qreal a1x2, a3;

    // (x0 - x1)
    v1.x    = x0.x - x1.x;
    v1.y    = x0.y - x1.y;
    v1.z    = x0.z - x1.z;

    // (x0 - x2)
    v2.x    = x0.x - x2.x;
    v2.y    = x0.y - x2.y;
    v2.z    = x0.z - x2.z;

    // (x2 - x1)
    v3.x    = x2.x - x1.x;
    v3.y    = x2.y - x1.y;
    v3.z    = x2.z - x1.z;

    // (x0 - x1)x(x0 - x2)
    v1x2.x  = v1.y * v2.z - v1.z * v2.y;
    v1x2.y  = v1.z * v2.x - v1.x * v2.z;
    v1x2.z  = v1.x * v2.y - v1.y * v2.x;

    // |(x0 - x1)x(x0 - x2)|
    a1x2    = v1x2.x*v1x2.x + v1x2.y*v1x2.y + v1x2.z*v1x2.z;
    // |(x2 - x1)|
    a3      = v3.x*v3.x + v3.y*v3.y + v3.z*v3.z;

    return qSqrt(a1x2/a3);
}

static inline qreal sqr(qreal a)
{
    return a*a;
}
static inline qreal sqrlen(const QPointF &a)
{
    return sqr(a.x()) + sqr(a.y());
}


qreal GPS_Math_DistPointPolyline(const QPolygonF &points, const QPointF &q)
{
    const qint32 count = points.size();

    QPointF b = points[0];
    QPointF dbq = b - q;
    qreal dist = sqrlen(dbq);

    for (qint32 i = 1; i<count; ++i)
    {
        const QPointF a = b;
        const QPointF daq = dbq;
        b = points[i];
        dbq = b - q;

        const QPointF dab = a - b;

        const qreal inv_sqrlen = 1./sqrlen(dab);
        const qreal t = (dab.x()*daq.x() + dab.y()*daq.y())*inv_sqrlen;
        if (t < 0.)
        {
            continue;
        }
        qreal current_dist;
        if (t<=1.)
        {
            current_dist = sqr(dab.x()*dbq.y() - dab.y()*dbq.x())*inv_sqrlen;
        }
        else//t>1.
        {
            current_dist = sqrlen(dbq);
        }
        if (current_dist<dist)
        {
            dist = current_dist;
        }
    }
    return dist;
}



struct segment
{
    segment() : idx1(0), idx2(0)
    {
    }
    segment(qint32 idx1, quint32 idx2) : idx1(idx1), idx2(idx2)
    {
    }
    qint32 idx1;
    qint32 idx2;
};

void GPS_Math_DouglasPeucker(QVector<pointDP> &line, qreal d)
{
    if(line.count() < 3)
    {
        return;
    }

    QStack<segment> stack;
    stack << segment(0, line.size() - 1);

    while(!stack.isEmpty())
    {
        qint32 idx = NOIDX;
        segment seg = stack.pop();

        pointDP& x1 = line[seg.idx1];
        pointDP& x2 = line[seg.idx2];

        for(qint32 i = seg.idx1 + 1; i < seg.idx2; i++)
        {
            qreal distance = GPS_Math_distPointLine3D(x1, x2, line[i]);
            if(distance > d)
            {
                idx = i;
                break;
            }
        }

        if(idx > 0)
        {
            stack << segment(seg.idx1, idx);
            stack << segment(idx, seg.idx2);
        }
        else
        {
            for(qint32 i = seg.idx1 + 1; i < seg.idx2; i++)
            {
                line[i].used = false;
            }
        }
    }
}

QPointF GPS_Math_Wpt_Projection(const QPointF& pt1, qreal distance, qreal bearing)
{
    QPointF pt2;

    qreal d    = distance / 6378130.0;
    qreal lon1 = pt1.x();
    qreal lat1 = pt1.y();

    qreal lat2 = qAsin(qSin(lat1) * qCos(d) + qCos(lat1) * qSin(d) * qCos(-bearing));
    qreal lon2 = qCos(lat1) == 0 ? lon1 : fmod(lon1 - qAsin(qSin(-bearing) * qSin(d) / qCos(lat1)) + M_PI, (2*M_PI)) - M_PI;

    pt2.rx() = lon2;
    pt2.ry() = lat2;
    return pt2;
}


bool GPS_Math_LineCrossesRect(const QPointF &p1, const QPointF &p2, const QRectF &rect)
{
    // the trival case
    if(rect.contains(p1) || rect.contains(p2))
    {
        return true;
    }

    qreal slope    = qreal(p2.y() - p1.y()) / (p2.x() - p1.x());
    qreal offset   = p1.y() - slope * p1.x();
    qreal y1       = offset + slope * rect.left();
    qreal y2       = offset + slope * rect.right();

    if((y1 < rect.top()) && (y2 < rect.top()))
    {
        return false;
    }
    else if((y1 > rect.bottom()) && (y2 > rect.bottom()))
    {
        return false;
    }

    return true;
}


void GPS_Math_SubPolyline(const QPointF& pt1, const QPointF& pt2, qint32 threshold, const QPolygonF& pixel, segment_t &result)
{
    qint32 i, len;
    projXY p1, p2;
    qreal dx,dy;                // delta x and y defined by p1 and p2
    qreal d_p1_p2;              // distance between p1 and p2
    qreal u;                    // ratio u the tangent point will divide d_p1_p2
    qreal x,y;                  // coord. (x,y) of the point on line defined by [p1,p2] close to pt
    qreal distance;             // the distance to the polyline
    qreal shortest1 = threshold;
    qreal shortest2 = threshold;
    qint32 idx11 = NOIDX, idx21 = NOIDX, idx12 = NOIDX;

    QPointF pt11;
    QPointF pt21;

    len = pixel.size();

    // find points on line closest to pt1 and pt2
    for(i=1; i<len; ++i)
    {
        p1.u = pixel[i - 1].x();
        p1.v = pixel[i - 1].y();
        p2.u = pixel[i].x();
        p2.v = pixel[i].y();

        dx = p2.u - p1.u;
        dy = p2.v - p1.v;
        d_p1_p2 = qSqrt(dx * dx + dy * dy);

        // find point on line closest to pt1
        u = ((pt1.x() - p1.u) * dx + (pt1.y() - p1.v) * dy) / (d_p1_p2 * d_p1_p2);

        if(u >= 0.0 && u <= 1.0)
        {
            x = p1.u + u * dx;
            y = p1.v + u * dy;

            distance = qSqrt((x - pt1.x())*(x - pt1.x()) + (y - pt1.y())*(y - pt1.y()));

            if(distance < shortest1)
            {
                idx11 = i - 1;
                idx12 = i;
                pt11.setX(x);
                pt11.setY(y);
                shortest1 = distance;
            }
        }

        // find point on line closest to pt2
        u = ((pt2.x() - p1.u) * dx + (pt2.y() - p1.v) * dy) / (d_p1_p2 * d_p1_p2);

        if(u >= 0.0 && u <= 1.0)
        {
            x = p1.u + u * dx;
            y = p1.v + u * dy;

            distance = qSqrt((x - pt2.x())*(x - pt2.x()) + (y - pt2.y())*(y - pt2.y()));

            if(distance < shortest2)
            {
                idx21 = i - 1;
                pt21.setX(x);
                pt21.setY(y);
                shortest2 = distance;
            }
        }
    }

    // if 1st point can't be found test for distance to both ends
    if(idx11 == NOIDX)
    {
        QPointF px = pixel.first();
        distance = qSqrt((qreal)((px.x() - pt1.x())*(px.x() - pt1.x()) + (px.y() - pt1.y())*(px.y() - pt1.y())));
        if(distance < (threshold<<1))
        {
            idx11 = 0;
            idx12 = 1;
            pt11 = px;
        }
        else
        {
            px = pixel.last();
            distance = qSqrt((qreal)((px.x() - pt1.x())*(px.x() - pt1.x()) + (px.y() - pt1.y())*(px.y() - pt1.y())));
            if(distance < (threshold<<1))
            {
                idx11 = pixel.size() - 2;
                idx12 = pixel.size() - 1;
                pt11 = px;
            }
        }
    }

    // if 2nd point can't be found test for distance to both ends
    if(idx21 == NOIDX)
    {
        QPointF px = pixel.first();
        distance = qSqrt((qreal)((px.x() - pt2.x())*(px.x() - pt2.x()) + (px.y() - pt2.y())*(px.y() - pt2.y())));

        if(distance < (threshold<<1))
        {
            idx21 = 0;
            pt21 = px;
        }
        else
        {
            px = pixel.last();
            distance = qSqrt((qreal)((px.x() - pt2.x())*(px.x() - pt2.x()) + (px.y() - pt2.y())*(px.y() - pt2.y())));
            if(distance < (threshold<<1))
            {
                idx21 = pixel.size() - 2;
                pt21 = px;
            }
        }
    }

    //    qDebug() << pixel.size() << idx11 << idx12 << idx21 << pt1 << pt2 << pt11 << pt21;

    result.idx11 = idx11;
    result.idx12 = idx12;
    result.idx21 = idx21;
    result.px1   = pt11;
    result.px2   = pt21;
}

void segment_t::apply(const QPolygonF& coords, const QPolygonF& pixel, QPolygonF& segCoord, QPolygonF& segPixel, IDrawContext * context)
{
    QPointF pt1 = px1;
    QPointF pt2 = px2;

    context->convertPx2Rad(pt1);
    context->convertPx2Rad(pt2);

    if(idx11 != NOIDX && idx21 != NOIDX)
    {
        if(idx11 == idx21)
        {
//            segPixel.push_back(px1);
//            segPixel.push_back(px2);

//            segCoord.push_back(pt1);
//            segCoord.push_back(pt2);
        }
        else if(idx12 == idx21)
        {
//            segPixel.push_back(px1);
            segPixel.push_back(pixel[idx12]);
//            segPixel.push_back(px2);

//            segCoord.push_back(pt1);
            segCoord.push_back(coords[idx12]);
//            segCoord.push_back(pt2);
        }
        else if(idx11 < idx21)
        {
//            segPixel.push_back(px1);
//            segCoord.push_back(pt1);
            for(int i = idx12; i <= idx21; i++)
            {
                segPixel.push_back(pixel[i]);
                segCoord.push_back(coords[i]);
            }
//            segPixel.push_back(px2);
//            segCoord.push_back(pt2);
        }
        else if(idx11 > idx21)
        {
//            segPixel.push_back(px1);
//            segCoord.push_back(pt1);
            for(int i = idx11; i > idx21; i--)
            {
                segPixel.push_back(pixel[i]);
                segCoord.push_back(coords[i]);
            }
//            segPixel.push_back(px2);
//            segCoord.push_back(pt2);
        }
    }
}
