/**********************************************************************************************
    Copyright (C) 2009 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/IDrawContext.h"
#include "gis/GeoMath.h"
#include "units/IUnit.h"
#include <stdlib.h>

#include <QtGui>
#include <QtWidgets>

#define PI M_PI
#define TWOPI (2 * PI)

pointDP::pointDP() : used(true), idx(NOIDX)
{
}

pointDP::pointDP(const qreal &x, const qreal &y, const qreal &z)
    : point3D(x, y, z), used(true), idx(NOIDX)
{
}

static inline qreal distance(const QPointF &pa, const QPointF &pb)
{
    const qreal &dx = pa.x() - pb.x();
    const qreal &dy = pa.y() - pb.y();
    return qSqrt(dx * dx + dy * dy);
}

void GPS_Math_DegMinSec_To_Deg(bool sign, const qint32 d, const qint32 m, const qreal s, qreal &deg)
{
    deg = qAbs(d) + qreal(m) / 60.0 + s / 3600;
    if(sign)
    {
        deg = -deg;
    }
}


bool GPS_Math_Deg_To_DegMin(qreal v, qint32 *deg, qreal *min)
{
    *deg = qAbs(v);
    *min = (qAbs(v) - *deg) * 60.0;

    return v < 0;
}


void GPS_Math_DegMin_To_Deg(bool sign, const qint32 d, const qreal m, qreal& deg)
{
    deg = qAbs(d) + m / 60.0;
    if(sign)
    {
        deg = -deg;
    }
}


// from http://www.movable-type.co.uk/scripts/LatLongVincenty.html
qreal GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2, qreal& a1, qreal& a2)
{
    qreal cosSigma   = 0.0;
    qreal sigma      = 0.0;
    qreal sinAlpha   = 0.0;
    qreal cosSqAlpha = 0.0;
    qreal cos2SigmaM = 0.0;
    qreal sinSigma   = 0.0;
    qreal sinLambda  = 0.0;
    qreal cosLambda  = 0.0;

    qreal a = 6378137.0, b = 6356752.3142,  f = 1.0 / 298.257223563;  // WGS-84 ellipsiod
    qreal L = u2 - u1;

    qreal U1 = qAtan((1 - f) * qTan(v1));
    qreal U2 = qAtan((1 - f) * qTan(v2));
    qreal sinU1 = qSin(U1), cosU1 = qCos(U1);
    qreal sinU2 = qSin(U2), cosU2 = qCos(U2);
    qreal lambda = L, lambdaP = 2 * PI;
    unsigned iterLimit = 20;

    while ((qAbs(lambda - lambdaP) > 1e-12) && (--iterLimit > 0))
    {
        sinLambda = qSin(lambda);
        cosLambda = qCos(lambda);
        sinSigma = qSqrt((cosU2 * sinLambda) * (cosU2 * sinLambda) + (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));

        if (sinSigma == 0)
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

        qreal C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1 - C) * f * sinAlpha * (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    }
    if (iterLimit == 0)
    {
        return FP_NAN;                // formula failed to converge
    }
    qreal uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    qreal A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    qreal B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    qreal deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) - B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) * (-3 + 4 * cos2SigmaM * cos2SigmaM)));
    qreal s = b * A * (sigma - deltaSigma);

    a1 = qAtan2(cosU2 * sinLambda, cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * 360 / TWOPI;
    a2 = qAtan2(cosU1 * sinLambda, -sinU1 * cosU2 + cosU1 * sinU2 * cosLambda) * 360 / TWOPI;
    return s;
}

qreal GPS_Math_Distance(const qreal u1, const qreal v1, const qreal u2, const qreal v2)
{
    qreal cosSigma   = 0.0;
    qreal sigma      = 0.0;
    qreal sinAlpha   = 0.0;
    qreal cosSqAlpha = 0.0;
    qreal cos2SigmaM = 0.0;
    qreal sinSigma   = 0.0;
    qreal sinLambda  = 0.0;
    qreal cosLambda  = 0.0;

    qreal a = 6378137.0, b = 6356752.3142,  f = 1.0 / 298.257223563;  // WGS-84 ellipsiod
    qreal L = u2 - u1;

    qreal U1 = qAtan((1 - f) * qTan(v1));
    qreal U2 = qAtan((1 - f) * qTan(v2));
    qreal sinU1 = qSin(U1), cosU1 = qCos(U1);
    qreal sinU2 = qSin(U2), cosU2 = qCos(U2);
    qreal lambda = L, lambdaP = 2 * PI;
    unsigned iterLimit = 20;

    while ((qAbs(lambda - lambdaP) > 1e-12) && (--iterLimit > 0))
    {
        sinLambda = qSin(lambda);
        cosLambda = qCos(lambda);
        sinSigma = qSqrt((cosU2 * sinLambda) * (cosU2 * sinLambda) + (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));

        if (sinSigma == 0)
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

        qreal C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1 - C) * f * sinAlpha * (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    }
    if (iterLimit == 0)
    {
        return FP_NAN;                // formula failed to converge
    }
    qreal uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    qreal A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    qreal B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    qreal deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) - B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) * (-3 + 4 * cos2SigmaM * cos2SigmaM)));
    qreal s = b * A * (sigma - deltaSigma);

    return s;
}

qreal GPS_Math_DistanceQuick(const qreal u1, const qreal v1, const qreal u2, const qreal v2)
{
    qreal dU = u2 - u1; // lambda
    qreal dV = v2 - v1; // roh

    qreal d = 2 * qAsin(qSqrt(qSin(dV / 2) * qSin(dV / 2) + qCos(v1) * qCos(v2) * qSin(dU / 2) * qSin(dU / 2)));

    return 6371010 * d;
}


static qreal GPS_Math_distPointLine3D(const point3D &x1, const point3D &x2, const point3D &x0)
{
    point3D v1, v2, v3, v1x2;

    // (x0 - x1)
    v1.x = x0.x - x1.x;
    v1.y = x0.y - x1.y;
    v1.z = x0.z - x1.z;

    // (x0 - x2)
    v2.x = x0.x - x2.x;
    v2.y = x0.y - x2.y;
    v2.z = x0.z - x2.z;

    // (x2 - x1)
    v3.x = x2.x - x1.x;
    v3.y = x2.y - x1.y;
    v3.z = x2.z - x1.z;

    // (x0 - x1)x(x0 - x2)
    v1x2.x  = v1.y * v2.z - v1.z * v2.y;
    v1x2.y  = v1.z * v2.x - v1.x * v2.z;
    v1x2.z  = v1.x * v2.y - v1.y * v2.x;

    // |(x0 - x1)x(x0 - x2)|
    qreal a1x2    = v1x2.x * v1x2.x + v1x2.y * v1x2.y + v1x2.z * v1x2.z;
    // |(x2 - x1)|
    qreal a3      = v3.x * v3.x + v3.y * v3.y + v3.z * v3.z;

    return qSqrt(a1x2 / a3);
}

static inline qreal sqr(qreal a)
{
    return a * a;
}
static inline qreal sqrlen(const QPointF &a)
{
    return sqr(a.x()) + sqr(a.y());
}


qreal GPS_Math_DistPointPolyline(const QPolygonF &points, const QPointF &q)
{
    const qint32 count = points.size();

    if(count == 0)
    {
        return NOFLOAT;
    }

    QPointF b   = points[0];
    QPointF dbq = b - q;
    qreal dist  = sqrlen(dbq);

    for (qint32 i = 1; i < count; ++i)
    {
        const QPointF a = b;
        const QPointF daq = dbq;
        b = points[i];
        dbq = b - q;

        const QPointF dab = a - b;

        const qreal inv_sqrlen = 1. / sqrlen(dab);
        const qreal t = (dab.x() * daq.x() + dab.y() * daq.y()) * inv_sqrlen;
        if (t < 0.)
        {
            continue;
        }
        qreal current_dist;
        if (t <= 1.)
        {
            current_dist = sqr(dab.x() * dbq.y() - dab.y() * dbq.x()) * inv_sqrlen;
        }
        else//t>1.
        {
            current_dist = sqrlen(dbq);
        }
        if (current_dist < dist)
        {
            dist = current_dist;
        }
    }
    return dist;
}

qreal GPS_Math_DistPointPolyline(const QPolygonF& line, const QPointF& pt, qreal threshold)
{
    qreal d = threshold + 1;

    const int len = line.size();
    // see http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
    for(int i = 1; i < len; ++i)
    {
        const QPointF &p1 = line[i - 1];
        const QPointF &p2 = line[i];

        qreal dx = p2.x() - p1.x();
        qreal dy = p2.y() - p1.y();

        // distance between p1 and p2
        qreal d_p1_p2 = qSqrt(dx * dx + dy * dy);

        // ratio u the tangent point will divide d_p1_p2
        qreal u = ((pt.x() - p1.x()) * dx + (pt.y() - p1.y()) * dy) / (d_p1_p2 * d_p1_p2);

        if(u < 0.0 || u > 1.0)
        {
            continue;
        }

        // coord. (x,y) of the point on line defined by [p1,p2] close to pt
        qreal x = p1.x() + u * dx;
        qreal y = p1.y() + u * dy;

        qreal distance = qSqrt((x - pt.x()) * (x - pt.x()) + (y - pt.y()) * (y - pt.y()));
        if(distance < threshold)
        {
            d = threshold = distance;
        }
    }

    return d;
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

        qreal dmax = d;
        for(qint32 i = seg.idx1 + 1; i < seg.idx2; i++)
        {
            qreal distance = GPS_Math_distPointLine3D(x1, x2, line[i]);
            if(distance > dmax)
            {
                idx  = i;
                dmax = distance;
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


bool GPS_Math_LineCrossesRect(const QPointF &p1, const QPointF &p2, const QRectF &rect)
{
    // the trivial case
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

QPointF GPS_Math_Wpt_Projection(const QPointF& p1, qreal distance, qreal bearing)
{
    qreal Phi1 = p1.y();
    qreal Lambda1 = p1.x();
    qreal Alpha1 = bearing;
    qreal s = distance;

    const qreal a = 6378137.0;
    const qreal b = 6356752.3142;
    const qreal f = 1.0 / 298.257223563;

    qreal sinAlpha1 = qSin(Alpha1);
    qreal cosAlpha1 = qCos(Alpha1);

    qreal tanU1 = (1 - f) * qTan(Phi1);
    qreal cosU1 = 1 / qSqrt((1 + tanU1 * tanU1));
    qreal sinU1 = tanU1 * cosU1;
    qreal Sigma1 = qAtan2(tanU1, cosAlpha1);
    qreal sinAlpha = cosU1 * sinAlpha1;
    qreal cosSqAlpha = 1 - sinAlpha * sinAlpha;
    qreal uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    qreal A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    qreal B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));

    qreal cos2SigmaM;
    qreal sinSigma;
    qreal cosSigma;
    qreal deltaSigma;

    qreal Sigma = s / (b * A);
    qreal Sigma_;
    int iterations = 0;
    do
    {
        cos2SigmaM = qCos(2 * Sigma1 + Sigma);
        sinSigma = qSin(Sigma);
        cosSigma = qCos(Sigma);
        deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) -
                                                           B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) * (-3 + 4 * cos2SigmaM * cos2SigmaM)));
        Sigma_ = Sigma;
        Sigma = s / (b * A) + deltaSigma;
    }
    while (qAbs(Sigma - Sigma_) > 1e-12 && ++iterations < 100);
    if (iterations >= 100)
    {
        return NOPOINTF;
    }
    qreal x = sinU1 * sinSigma - cosU1 * cosSigma * cosAlpha1;
    qreal Phi2 = qAtan2(sinU1 * cosSigma + cosU1 * sinSigma * cosAlpha1, (1 - f) * qSqrt(sinAlpha * sinAlpha + x * x));
    qreal Lambda = qAtan2(sinSigma * sinAlpha1, cosU1 * cosSigma - sinU1 * sinSigma * cosAlpha1);
    qreal C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
    qreal L = Lambda - (1 - C) * f * sinAlpha *
              (Sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));

    qreal Lambda2 = Lambda1 + L;
    if(Lambda2 > PI)
    {
        Lambda2 = -Lambda2 + PI;
    }

//    qreal Alpha2 = qAtan2(sinAlpha, -x);
//    Alpha2 = (Alpha2 + 2*PI) % (2*PI); // normalise to 0..360

    return QPointF(Lambda2, Phi2);
}
