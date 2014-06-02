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
#include "CUnitImperial.h"
#include "CUnitNautic.h"

#include <QtCore>
IUnit * IUnit::m_self = 0;

IUnit::IUnit(const type_e &type, const QString& baseunit, const qreal basefactor, const QString& speedunit, const qreal speedfactor, QObject * parent)
: QObject(parent)
, type(type)
, baseunit(baseunit)
, basefactor(basefactor)
, speedunit(speedunit)
, speedfactor(speedfactor)
{
    //there can be only one...
    if(m_self) delete m_self;
    m_self = this;
}


IUnit::~IUnit()
{

}

void IUnit::setUnitType(type_e t, QObject * parent)
{
    switch(t)
    {
    case eTypeMetric:
        new CUnitMetric(parent);
        break;
    case eTypeImperial:
        new CUnitImperial(parent);
        break;
    case eTypeNautic:
        new CUnitNautic(parent);
        break;
    }

    QSettings cfg;
    cfg.setValue("main/units/type",t);
}

void IUnit::meter2speed(qreal meter, QString& val, QString& unit)
{
    val.sprintf("%1.2f",meter * speedfactor);
    unit = speedunit;
}

void IUnit::seconds2time(quint32 ttime, QString& val, QString& unit)
{
    QTime time(0,0,0);
    quint32 days  = ttime / 86400;

    time = time.addSecs(ttime);

    if(days)
    {
        val = QString("%1:").arg(days) + time.toString("HH:mm:ss");
    }
    else
    {
        val = time.toString("HH:mm:ss");
    }
    unit.clear();
}

