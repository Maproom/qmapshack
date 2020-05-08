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

#include "GeoMath.h"

#define PI M_PI
#define TWOPI (2*PI)


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


