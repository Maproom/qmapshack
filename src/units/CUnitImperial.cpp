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

CUnitImperial::CUnitImperial(QObject * parent)
    : IUnit(eTypeImperial, "ft", 3.28084, "ml/h", 2.23693164, parent)
{
}


CUnitImperial::~CUnitImperial()
{
}


/* virtual */ void CUnitImperial::meter2elevation(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.0f", meter * 3.28084);
        unit = "ft";
    }
}


/* virtual */ void CUnitImperial::meter2distance(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else if(meter < 10)
    {
        val.sprintf("%1.1f", meter * 3.28084);
        unit = "ft";
    }
    else if(meter < 1600)
    {
        val.sprintf("%1.0f", meter * 3.28084);
        unit = "ft";
    }
    else if(meter < 16000)
    {
        val.sprintf("%1.2f", meter * 0.6213699E-3);
        unit = "ml";
    }
    else if(meter < 32000)
    {
        val.sprintf("%1.1f", meter * 0.6213699E-3);
        unit = "ml";
    }
    else
    {
        val.sprintf("%1.0f", meter * 0.6213699E-3);
        unit = "ml";
    }
}

/* virtual */ void CUnitImperial::meter2area(qreal meter, QString& val, QString& unit) const /* override */
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.2f", meter / (1/0.6213699E-3 * 1/0.6213699E-3));
        unit = "ml²";
    }
}



/* virtual */ qreal CUnitImperial::elevation2meter(const QString& val) const /* override */
{
    return val.toDouble() / 3.28084;
}
