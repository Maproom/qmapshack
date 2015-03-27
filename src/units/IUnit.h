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
#ifndef IUNIT_H
#define IUNIT_H
#include <QObject>
#include <QTimeZone>


#define NOFLOAT 1000000000000.0
#define NOINT   0x7FFFFFFF
#define NOTIME  0xFFFFFFFF
#define NOIDX   (-1)

extern const QPointF NOPOINTF;
extern const QPoint NOPOINT;

class IUnit : public QObject
{
    Q_OBJECT
public:
    virtual ~IUnit();

    static IUnit& self()
    {
        return *m_self;
    }
    /// convert meter of elevation into a value and unit string
    virtual void meter2elevation(qreal meter, QString& val, QString& unit) = 0;
    /// convert meter of distance into a value and unit string
    virtual void meter2distance(qreal meter, QString& val, QString& unit) = 0;
    /// convert meter per second to a speed value string and unit label
    virtual void meter2speed(qreal meter, QString& val, QString& unit);
    virtual void meter2area(qreal meter, QString& val, QString& unit) = 0;
    virtual void seconds2time(quint32 ttime, QString& val, QString& unit);
    virtual qreal elevation2meter(const QString& val) = 0;


    enum type_e {eTypeMetric, eTypeImperial, eTypeNautic};
    static void setUnitType(type_e t, QObject *parent);

    static bool parseTimestamp(const QString &time, QDateTime &datetime);

    /**
       @brief Convert date time object to string using the current timezone configuration


       @param time          the date/time object
       @param shortDate     set true to get a short ISO time string
       @param pos           optional a position attached to the date/time object [rad]
       @return              A time string.
     */
    static QString datetime2string(const QDateTime& time, bool shortDate, const QPointF& pos = NOPOINTF);

    static QByteArray pos2timezone(const QPointF& pos);

    const type_e type;
    const QString baseunit;
    const qreal basefactor;
    const QString speedunit;
    const qreal speedfactor;
    static const char *  tblTimezone[];

    enum tz_mode_e
    {
        eTZUtc
        ,eTZLocal
        ,eTZAuto
        ,eTZSelected
    };

    static void getTimeZoneSetup(tz_mode_e& mode, QByteArray& zone)
    {
        mode = timeZoneMode; zone = timeZone;
    }
    static void setTimeZoneSetup(tz_mode_e mode, const QByteArray& zone)
    {
        timeZoneMode = mode; timeZone = zone;
    }

protected:
    IUnit(const type_e& type, const QString& baseunit, const qreal basefactor, const QString& speedunit, const qreal speedfactor, QObject * parent);

    static QDateTime parseTimestamp(const QString &timetext, int& tzoffset);

    static tz_mode_e timeZoneMode;
    static QByteArray timeZone;


private:
    static IUnit * m_self;
};
#endif                           //IUNIT_H
