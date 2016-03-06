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

#include "units/CUnitImperial.h"

CUnitImperial::CUnitImperial()
    : IUnit(eTypeImperial, "ft", footPerMeter, "ml/h", meterPerSecToMilePerHour)
{
}


void CUnitImperial::meter2elevation(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.0f", meter * footPerMeter);
        unit = "ft";
    }
}


void CUnitImperial::meter2distance(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else if(meter < 10)
    {
        val.sprintf("%1.1f", meter * footPerMeter);
        unit = "ft";
    }
    else if(meter < 1600)
    {
        val.sprintf("%1.0f", meter * footPerMeter);
        unit = "ft";
    }
    else if(meter < 16000)
    {
        val.sprintf("%1.2f", meter * milePerMeter);
        unit = "ml";
    }
    else if(meter < 32000)
    {
        val.sprintf("%1.1f", meter * milePerMeter);
        unit = "ml";
    }
    else
    {
        val.sprintf("%1.0f", meter * milePerMeter);
        unit = "ml";
    }
}

void CUnitImperial::meter2area(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.2f", meter / (1/milePerMeter * 1/milePerMeter));
        unit = "ml²";
    }
}

qreal CUnitImperial::elevation2meter(const QString& val) const /* override */
{
    return val.toDouble() / footPerMeter;
}
