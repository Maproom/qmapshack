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
#include "CUnitMetric.h"

CUnitMetric::CUnitMetric(QObject * parent)
: IUnit(eTypeMetric, "m", 1.0, "km/h", 3.6, parent)
{

}


CUnitMetric::~CUnitMetric()
{

}


void CUnitMetric::meter2elevation(qreal meter, QString& val, QString& unit)
{
    if(meter == NOFLOAT || meter == NOINT)
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


void CUnitMetric::meter2distance(qreal meter, QString& val, QString& unit)
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else if(meter < 10)
    {
        val.sprintf("%1.1f", meter);
        unit = "m";
    }
    else if(meter < 1000)
    {
        val.sprintf("%1.0f", meter);
        unit = "m";
    }
    else if(meter < 10000)
    {
        val.sprintf("%1.2f", meter / 1000);
        unit = "km";
    }
    else if(meter < 20000)
    {
        val.sprintf("%1.1f", meter / 1000);
        unit = "km";
    }
    else
    {
        val.sprintf("%1.0f", meter / 1000);
        unit = "km";
    }
}


void CUnitMetric::meter2speed(qreal meter, QString& val, QString& unit)
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else if (meter < 10.0)
    {
        val.sprintf("%1.1f",meter * speedfactor);
    }
    else
    {
        val.sprintf("%1.0f",meter * speedfactor);
    }
    unit = speedunit;
}

void CUnitMetric::meter2area(qreal meter, QString& val, QString& unit)
{
    if(meter == NOFLOAT)
    {
        val  = "-";
        unit = "";
    }
    else
    {
        val.sprintf("%1.2f", meter / 1000000);
        unit = "km²";
    }

}

qreal CUnitMetric::elevation2meter(const QString& val)
{
    return val.toDouble();
}
