/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler oliver.eichler@gmx.de

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

**********************************************************************************************/

#include "units/CUnitNautic.h"

CUnitNautic::CUnitNautic()
    : IUnit(eTypeNautic, "nm", 0.00053989, "nm/h", 1.94361780)
{
}


void CUnitNautic::meter2elevation(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.0f", meter);
        unit = "m";
    }
}


void CUnitNautic::meter2distance(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.2f", meter * basefactor);
        unit = baseunit;
    }
}


void CUnitNautic::meter2speed(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.2f",meter * speedfactor);
        unit = speedunit;
    }
}

void CUnitNautic::meter2area(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.2f", meter / (1852 * 1852));
        unit = "nm²";
    }
}


qreal CUnitNautic::elevation2meter(const QString& val) const /* override */
{
    return val.toDouble();
}
