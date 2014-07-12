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
#include <math.h>
#include <stdlib.h>
#include <proj_api.h>

#include <QtGui>
#include <QtWidgets>

#define PI M_PI
#define TWOPI (2*PI)

static bool GPS_Math_Deg_To_DegMin(qreal v, qint32 *d, qreal *m)
{
    bool sign = v < 0;
    qint32 deg = abs(v);
    qreal  min = (fabs(v) - deg) * 60.0;

    *d = deg;
    *m = min;

    return sign;
}


static void GPS_Math_DegMin_To_Deg(bool sign, const qint32 d, const qreal m, qreal& deg)
{

    deg = abs(d) + m / 60.0;
    if(sign) {
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
    str.sprintf("%s%02d° %06.3f %s%03d° %06.3f",lat.toUtf8().data(),abs(degN),minN,lng.toUtf8().data(),abs(degE),minE);
}


bool GPS_Math_Str_To_Deg(const QString& str, qreal& lon, qreal& lat)
{
    QRegExp re("^\\s*([N|S]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s+([E|W]){1}\\W*([0-9]+)\\W*([0-9]+\\.[0-9]+)\\s*$");
    if(!re.exactMatch(str)) {
        QMessageBox::warning(0,QObject::tr("Error"),QObject::tr("Bad position format. Must be: [N|S] ddd mm.sss [W|E] ddd mm.sss"),QMessageBox::Abort,QMessageBox::Abort);
        return false;
    }

    bool signLat    = re.cap(1) == "S";
    int degLat      = re.cap(2).toInt();
    qreal minLat   = re.cap(3).toDouble();

    GPS_Math_DegMin_To_Deg(signLat, degLat, minLat, lat);

    bool signLon    = re.cap(4) == "W";
    int degLon      = re.cap(5).toInt();
    qreal minLon   = re.cap(6).toDouble();

    GPS_Math_DegMin_To_Deg(signLon, degLon, minLon, lon);

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

    qreal U1 = atan((1-f) * tan(v1));
    qreal U2 = atan((1-f) * tan(v2));
    qreal sinU1 = sin(U1), cosU1 = cos(U1);
    qreal sinU2 = sin(U2), cosU2 = cos(U2);
    qreal lambda = L, lambdaP = 2*PI;
    unsigned iterLimit = 20;

    while ((fabs(lambda - lambdaP) > 1e-12) && (--iterLimit > 0)) {
        sinLambda = sin(lambda);
        cosLambda = cos(lambda);
        sinSigma = sqrt((cosU2*sinLambda) * (cosU2*sinLambda) + (cosU1*sinU2-sinU1*cosU2*cosLambda) * (cosU1*sinU2-sinU1*cosU2*cosLambda));

        if (sinSigma==0){
            return 0;  // co-incident points
        }

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;

        if (isnan(cos2SigmaM)){
            cos2SigmaM = 0;  // equatorial line: cosSqAlpha=0 (§6)
        }

        qreal C = f/16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1-C) * f * sinAlpha * (sigma + C*sinSigma*(cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    }
    if (iterLimit==0) return FP_NAN;  // formula failed to converge

    qreal uSq = cosSqAlpha * (a*a - b*b) / (b*b);
    qreal A = 1 + uSq/16384*(4096+uSq*(-768+uSq*(320-175*uSq)));
    qreal B = uSq/1024 * (256+uSq*(-128+uSq*(74-47*uSq)));
    qreal deltaSigma = B*sinSigma*(cos2SigmaM+B/4*(cosSigma*(-1+2*cos2SigmaM*cos2SigmaM)-B/6*cos2SigmaM*(-3+4*sinSigma*sinSigma)*(-3+4*cos2SigmaM*cos2SigmaM)));
    qreal s = b*A*(sigma-deltaSigma);

    a1 = atan2(cosU2 * sinLambda, cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * 360 / TWOPI;
    a2 = atan2(cosU1 * sinLambda, -sinU1 * cosU2 + cosU1 * sinU2 * cosLambda) * 360 / TWOPI;
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

    qreal U1 = atan((1-f) * tan(v1));
    qreal U2 = atan((1-f) * tan(v2));
    qreal sinU1 = sin(U1), cosU1 = cos(U1);
    qreal sinU2 = sin(U2), cosU2 = cos(U2);
    qreal lambda = L, lambdaP = 2*PI;
    unsigned iterLimit = 20;

    while ((fabs(lambda - lambdaP) > 1e-12) && (--iterLimit > 0)) {
        sinLambda = sin(lambda);
        cosLambda = cos(lambda);
        sinSigma = sqrt((cosU2*sinLambda) * (cosU2*sinLambda) + (cosU1*sinU2-sinU1*cosU2*cosLambda) * (cosU1*sinU2-sinU1*cosU2*cosLambda));

        if (sinSigma==0){
            return 0;  // co-incident points
        }

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;

        if (isnan(cos2SigmaM)){
            cos2SigmaM = 0;  // equatorial line: cosSqAlpha=0 (§6)
        }

        qreal C = f/16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1-C) * f * sinAlpha * (sigma + C*sinSigma*(cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    }
    if (iterLimit==0) return FP_NAN;  // formula failed to converge

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

    qreal d = 2*asin(sqrt(sin(dV/2) * sin(dV/2) + cos(v1) * cos(v2) * sin(dU/2) * sin(dU/2)));

    return 6371010 * d;
}

void GPS_Math_Wpt_Projection(const qreal lon1, const qreal lat1, const qreal distance, const qreal bearing, qreal& lon2, qreal& lat2)
{
    qreal d    = distance / 6378130.0;
    lat2 = asin(sin(lat1) * cos(d) + cos(lat1) * sin(d) * cos(-bearing));
    lon2 = cos(lat1) == 0 ? lon1 : fmod(lon1 - asin(sin(-bearing) * sin(d) / cos(lat1)) + PI, TWOPI) - PI;
}

double GPS_Math_distPointLine3D(point3D& x1, point3D& x2, point3D& x0)
{

    point3D v1, v2, v3, v1x2;

    double a1x2, a3;

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

    return sqrt(a1x2/a3);
}


struct segment
{
    segment(): idx1(0), idx2(0){}
    segment(qint32 idx1, quint32 idx2) : idx1(idx1), idx2(idx2) {}
    qint32 idx1;
    qint32 idx2;
};

void GPS_Math_DouglasPeucker(QVector<pointDP> &line, double d)
{
    if(line.count() < 3) return;

    QStack<segment> stack;
    stack << segment(0, line.size() - 1);

    while(!stack.isEmpty())
    {
        int idx = -1;
        segment seg = stack.pop();

        pointDP& x1 = line[seg.idx1];
        pointDP& x2 = line[seg.idx2];

        for(int i = seg.idx1 + 1; i < seg.idx2; i++)
        {
            double distance = GPS_Math_distPointLine3D(x1, x2, line[i]);
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
            for(int i = seg.idx1 + 1; i < seg.idx2; i++)
            {
                line[i].used = false;
            }
        }
    }
}

QPointF GPS_Math_Wpt_Projection(const QPointF& pt1, double distance, double bearing)
{
    QPointF pt2;

    double d    = distance / 6378130.0;
    double lon1 = pt1.x();
    double lat1 = pt1.y();

    double lat2 = asin(sin(lat1) * cos(d) + cos(lat1) * sin(d) * cos(-bearing));
    double lon2 = cos(lat1) == 0 ? lon1 : fmod(lon1 - asin(sin(-bearing) * sin(d) / cos(lat1)) + M_PI, (2*M_PI)) - M_PI;

    pt2.rx() = lon2;
    pt2.ry() = lat2;
    return pt2;
}

