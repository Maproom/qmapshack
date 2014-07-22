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

const QPointF NOPOINT(NOFLOAT, NOFLOAT);

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

bool IUnit::parseTimestamp(const QString &time, QDateTime &datetime)
{
    int tzoffset;
    datetime = parseTimestamp(time, tzoffset);

    if (!datetime.isValid())
    {
        return false;
    }


    return true;
}


QDateTime IUnit::parseTimestamp(const QString &timetext, int& tzoffset)
{
    const QRegExp tzRE("[-+]\\d\\d:\\d\\d$");
    int i;

    tzoffset = 0;

    QString format = "yyyy-MM-dd'T'hh:mm:ss";

    i = timetext.indexOf(".");
    if (i != -1)
    {

        if(timetext[i+1] == '0')
        {
            format += ".zzz";
        }
        else
        {
            format += ".z";
        }
    }

    // trailing "Z" explicitly declares the timestamp to be UTC
    if (timetext.indexOf("Z") != -1)
    {
        format += "'Z'";
    }
    else if ((i = tzRE.indexIn(timetext)) != -1)
    {
        // trailing timezone offset [-+]HH:MM present
        // This does not match the original intentions of the GPX
        // file format but appears to be found occasionally in
        // the wild.  Try our best parsing it.

        // add the literal string to the format so fromString()
        // will succeed
        format += "'";
        format += timetext.right(6);
        format += "'";

        // calculate the offset
        int offsetHours(timetext.mid(i + 1, 2).toUInt());
        int offsetMinutes(timetext.mid(i + 4, 2).toUInt());
        if (timetext[i] == '-')
        {
            tzoffset = -(60 * offsetHours + offsetMinutes);
        }
        else
        {
            tzoffset = 60 * offsetHours + offsetMinutes;
        }
        tzoffset *= 60;          // seconds
    }

    QDateTime datetime = QDateTime::fromString(timetext, format);

    return datetime;
}

