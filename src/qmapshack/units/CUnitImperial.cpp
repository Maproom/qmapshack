/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

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

CUnitImperial::CUnitImperial(QObject * parent)
    : IUnit(eTypeImperial, "ft", footPerMeter, "mi/h", meterPerSecToMilePerHour, "ft", footPerMeter, parent)
{
}


void CUnitImperial::meter2distance(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit.clear();
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
        unit = "mi";
    }
    else if(meter < 32000)
    {
        val.sprintf("%1.1f", meter * milePerMeter);
        unit = "mi";
    }
    else
    {
        val.sprintf("%1.0f", meter * milePerMeter);
        unit = "mi";
    }
}

void CUnitImperial::meter2distance(qreal meter, qreal& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = NOFLOAT;
        unit.clear();
    }
    else if(meter < 1600)
    {
        val = meter * footPerMeter;
        unit = "ft";
    }
    else
    {
        val = meter * milePerMeter;
        unit = "mi";
    }
}

void CUnitImperial::meter2area(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit.clear();
    }
    else
    {
        val.sprintf("%1.2f", meter / (1 / milePerMeter * 1 / milePerMeter));
        unit = "mi²";
    }
}

void CUnitImperial::meter2area(qreal meter, qreal& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = NOFLOAT;
        unit.clear();
    }
    else
    {
        val = meter / (1 / milePerMeter * 1 / milePerMeter);
        unit = "mi²";
    }
}

qreal CUnitImperial::elevation2meter(const QString& val) const /* override */
{
    return val.toDouble() / footPerMeter;
}

void CUnitImperial::meter2unit(qreal meter, qreal& scale, QString&  unit) const
{
    if(meter > 1600)
    {
        scale = milePerMeter;
        unit  = "mi";
    }
    else
    {
        scale = footPerMeter;
        unit  = "ft";
    }
}

