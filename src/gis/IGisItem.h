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
#include <QMutex>
#include <QColor>

#include "units/IUnit.h"

class CGisDraw;


class IGisItem : public QTreeWidgetItem
{
    public:
        IGisItem(QTreeWidgetItem * parent);
        virtual ~IGisItem();

        static QMutex mutexItems;

        const QString& getKey();
        virtual void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis) = 0;
        virtual void drawLabel(QPainter& p, const QRectF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis) = 0;
        virtual void save(QDomNode& gpx) = 0;


    protected:
        struct wpt_t;
        void readWpt(const QDomNode& xml, wpt_t &wpt);
        void writeWpt(QDomElement &xmlWpt, const wpt_t &wpt);
        virtual void genKey() = 0;
        QColor str2color(const QString& name);
        QString color2str(const QColor &color);
        void splitLineToViewport(const QPolygonF& line, const QRectF& extViewport, QList<QPolygonF>& lines);
        void drawArrows(const QPolygonF &line, const QRectF &extViewport, QPainter& p);



        struct color_t
        {
            const char * name;
            QColor color;
        };


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


        QString key;
        static const color_t colorMap[];


        inline void readXml(const QDomNode& xml, const QString& tag, qint32& value)
        {
            if(xml.namedItem(tag).isElement())
            {
                qint32 tmp;
                bool ok = false;
                tmp = xml.namedItem(tag).toElement().text().toInt(&ok);
                if(!ok)
                {
                    tmp = qRound(xml.namedItem(tag).toElement().text().toDouble(&ok));
                }
                if(ok)
                {
                    value = tmp;
                }
            }
        }

        inline void readXml(const QDomNode& xml, const QString& tag, quint32& value)
        {
            if(xml.namedItem(tag).isElement())
            {
                quint32 tmp;
                bool ok = false;
                tmp = xml.namedItem(tag).toElement().text().toUInt(&ok);
                if(ok)
                {
                    value = tmp;
                }
            }
        }

        inline void readXml(const QDomNode& xml, const QString& tag, quint64& value)
        {
            if(xml.namedItem(tag).isElement())
            {
                quint64 tmp;
                bool ok = false;
                tmp = xml.namedItem(tag).toElement().text().toULongLong(&ok);
                if(ok)
                {
                    value = tmp;
                }
            }
        }

        inline void readXml(const QDomNode& xml, const QString& tag, qreal& value)
        {
            if(xml.namedItem(tag).isElement())
            {
                qreal tmp;
                bool ok = false;
                tmp = xml.namedItem(tag).toElement().text().toDouble(&ok);
                if(ok)
                {
                    value = tmp;
                }
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

        inline void readXml(const QDomNode& xml, const QString& tag, QList<link_t>& l)
        {
            if(xml.namedItem(tag).isElement())
            {
                const QDomNodeList& links = xml.toElement().elementsByTagName(tag);
                int N = links.count();
                for(int n = 0; n < N; ++n)
                {
                    const QDomNode& link = links.item(n);

                    link_t tmp;
                    tmp.uri.setUrl(link.attributes().namedItem("href").nodeValue());
                    readXml(link, "text", tmp.text);
                    readXml(link, "type", tmp.type);

                    l << tmp;
                }
            }
        }

        inline void writeXml(QDomNode& xml, const QString& tag, qint32 val)
        {
            if(val != NOINT)
            {
                QDomElement elem = xml.ownerDocument().createElement(tag);
                xml.appendChild(elem);
                QDomText text = xml.ownerDocument().createTextNode(QString::number(val));
                elem.appendChild(text);
            }
        }

        inline void writeXml(QDomNode& xml, const QString& tag, quint32 val)
        {
            if(val != NOINT)
            {
                QDomElement elem = xml.ownerDocument().createElement(tag);
                xml.appendChild(elem);
                QDomText text = xml.ownerDocument().createTextNode(QString::number(val));
                elem.appendChild(text);
            }
        }

        inline void writeXml(QDomNode& xml, const QString& tag, quint64 val)
        {
            if(val != 0)
            {
                QDomElement elem = xml.ownerDocument().createElement(tag);
                xml.appendChild(elem);
                QDomText text = xml.ownerDocument().createTextNode(QString::number(val));
                elem.appendChild(text);
            }
        }

        inline void writeXml(QDomNode& xml, const QString& tag, const QString& val)
        {
            if(!val.isEmpty())
            {
                QDomElement elem = xml.ownerDocument().createElement(tag);
                xml.appendChild(elem);
                QDomText text = xml.ownerDocument().createTextNode(val);
                elem.appendChild(text);
            }
        }

        inline void writeXml(QDomNode& xml, const QString& tag, qreal val)
        {
            if(val != NOFLOAT)
            {
                QDomElement elem = xml.ownerDocument().createElement(tag);
                xml.appendChild(elem);
                QDomText text = xml.ownerDocument().createTextNode(QString("%1").arg(val,0,'f',8));
                elem.appendChild(text);
            }
        }

        inline void writeXmlHtml(QDomNode& xml, const QString& tag, const QString& val)
        {
            if(!val.isEmpty())
            {
                QDomElement elem = xml.ownerDocument().createElement(tag);
                xml.appendChild(elem);
                QDomText text = xml.ownerDocument().createCDATASection(val);
                elem.appendChild(text);
                elem.setAttribute("html","True");

            }
        }

        inline void writeXml(QDomNode& xml, const QString& tag, const QDateTime& time)
        {
            if(time.isValid())
            {
                QDomElement elem = xml.ownerDocument().createElement(tag);
                xml.appendChild(elem);
                QDomText text = xml.ownerDocument().createTextNode(time.toString("yyyy-MM-dd'T'hh:mm:ss'Z'"));
                elem.appendChild(text);
            }
        }


        inline void writeXml(QDomNode& xml, const QString& tag, const QList<link_t>& links)
        {
            if(!links.isEmpty())
            {
                foreach(const link_t& link, links)
                {
                    QDomElement elem = xml.ownerDocument().createElement(tag);
                    xml.appendChild(elem);

                    elem.setAttribute("href", link.uri.toString());
                    writeXml(elem, "text", link.text);
                    writeXml(elem, "type", link.type);
                }
            }
        }

        inline bool isBlocked(const QRectF& rect, const QList<QRectF> &blockedAreas)
        {
            foreach(const QRectF& r, blockedAreas)
            {
                if(rect.intersects(r))
                {
                    return true;
                }
            }
            return false;
        }


};

#endif //IGISITEM_H

