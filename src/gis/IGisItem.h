/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#ifndef IGISITEM_H
#define IGISITEM_H

#include <QTreeWidgetItem>

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QUrl>
#include <QDomNode>

#include "units/IUnit.h"



class IGisItem : public QTreeWidgetItem
{
    public:
        IGisItem(QTreeWidgetItem * parent);
        virtual ~IGisItem();

        const QString& getKey();

    protected:

        struct link_t
        {
            QUrl    uri;
            QString text;
            QString type;
        };

        struct wpt_t
        {
            wpt_t() :
                lat(NOFLOAT),
                lon(NOFLOAT),
                ele(NOINT),
                magvar(NOINT),
                geoidheight(NOINT),
                sat(NOINT),
                hdop(NOINT),
                vdop(NOINT),
                pdop(NOINT),
                ageofdgpsdata(NOINT),
                dgpsid(NOINT)
            {}
            // -- all gpx tags - start
            qreal lat;
            qreal lon;
            qint32 ele;
            QDateTime time;
            qint32 magvar;
            qint32 geoidheight;
            QString name;
            QString cmt;
            QString desc;
            QString src;
            QList<link_t> links;
            QString sym;
            QString type;
            QString fix;
            qint32 sat;
            qint32 hdop;
            qint32 vdop;
            qint32 pdop;
            qint32 ageofdgpsdata;
            qint32 dgpsid;
            // -- all gpx tags - stop
            QMap<QString, QVariant> extensions;
        };

        void readWpt(const QDomNode& xml, wpt_t &wpt);
        virtual void genKey() = 0;


        QString key;

};

inline void readXml(const QDomNode& xml, const QString& tag, qint32& value)
{
    if(xml.namedItem(tag).isElement())
    {
        value = xml.namedItem(tag).toElement().text().toInt();
    }
}

inline void readXml(const QDomNode& xml, const QString& tag, quint64& value)
{
    if(xml.namedItem(tag).isElement())
    {
        value = xml.namedItem(tag).toElement().text().toULongLong();
    }
}

inline void readXml(const QDomNode& xml, const QString& tag, qreal& value)
{
    if(xml.namedItem(tag).isElement())
    {
        value = xml.namedItem(tag).toElement().text().toDouble();
    }
}

inline void readXml(const QDomNode& xml, const QString& tag, QString& value)
{
    if(xml.namedItem(tag).isElement())
    {
        value = xml.namedItem(tag).toElement().text();
    }
}

inline void readXml(const QDomNode& xml, const QString& tag, QDateTime& value)
{
    if(xml.namedItem(tag).isElement())
    {
        QString time = xml.namedItem(tag).toElement().text();
        IUnit::parseTimestamp(time, value);

    }
}


#endif //IGISITEM_H

