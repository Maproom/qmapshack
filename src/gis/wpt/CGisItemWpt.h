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

#ifndef CGISITEMWPT_H
#define CGISITEMWPT_H

#include "gis/IGisItem.h"

class CGisProject;
class QDomNode;

class CGisItemWpt : public IGisItem
{
    public:
        CGisItemWpt(const QDomNode& xml, CGisProject * parent);
        virtual ~CGisItemWpt();

        void setName(const QString& str);
        void setPosition(const QPointF& pos);
        void setElevation(qint32 val);
        void setProximity(qreal val);
        void setIcon(const QString& name);
        void setComment(const QString& str);
        void setDescription(const QString& str);

        const QString& getName(){return wpt.name;}
        QString getInfo();
        QPointF getPosition(){return QPointF(wpt.lon, wpt.lat);}
        qint32 getElevation(){return wpt.ele;}
        qreal getProximity(){return proximity;}
        const QString& getIconName(){return wpt.sym;}
        const QString& getComment(){return wpt.cmt;}
        const QString& getDescription(){return wpt.desc;}

        IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
        QPointF getPointCloseBy(const QPoint& ){return posScreen;}

        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
        void drawHighlight(QPainter& p);
        void save(QDomNode& gpx);
        bool isCloseTo(const QPointF& pos);
        void gainUserFocus();

        void edit();

    private:
        void genKey();
        void setIcon();
        void readGcExt(const QDomNode& xmlCache);
        void writeGcExt(QDomNode& xmlCache);

        static QString keyUserFocus;

        enum geocacheservice_e {eGC, eOC, eTC};

        struct geocachelog_t
        {
            geocachelog_t() : id(0){}
            quint32 id;
            QDateTime date;
            QString type;
            QString finderId;
            QString finder;
            QString text;
        };

        struct geocache_t
        {
            geocache_t() : service(eOC), hasData(false), id(0), available(true), archived(false), difficulty(0), terrain(0){}
            geocacheservice_e service;
            bool hasData;
            quint32 id;
            bool available;
            bool archived;
            qreal difficulty;
            qreal terrain;
            QString status;
            QString name;
            QString owner;
            QString ownerId;
            QString type;
            QString container;
            QString shortDesc;
            QString longDesc;
            QString hint;
            QString country;
            QString state;
            QString locale;
            QList<geocachelog_t> logs;
        };


        wpt_t wpt;
        qreal proximity;
        geocache_t geocache;

        QPointF focus;
        QPointF posScreen;

};

#endif //CGISITEMWPT_H

