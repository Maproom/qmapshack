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

#include <QPointer>

class IGisProject;
class QDomNode;
class CScrOptWpt;
class QSqlDatabase;
class CQlgtWpt;

class CGisItemWpt : public IGisItem
{
    public:
        enum geocacheservice_e {eGC, eOC, eTC};

        struct geocachelog_t
        {
            geocachelog_t() : id(0){}
            quint32 id;
            QDateTime date;
            QString type;
            QString finderId;
            QString finder;
            bool    textIsHtml;
            QString text;
        };

        struct geocache_t
        {
            geocache_t() : service(eOC), hasData(false), id(0), available(true), archived(false), difficulty(0), terrain(0), shortDescIsHtml(false), longDescIsHtml(false){}
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
            bool    shortDescIsHtml;
            QString shortDesc;
            bool    longDescIsHtml;
            QString longDesc;
            QString hint;
            QString country;
            QString state;
            QString locale;
            QList<geocachelog_t> logs;
        };

        struct image_t
        {
            QImage  pixmap;
            qreal   direction;
            QString info;
            QString filePath;
            QString fileName;
        };

        /**
           @brief Create a completely new waypoint
           @param pos       the waypoint's position [°]
           @param name      the waypoint's name
           @param icon      the waypoint's icon
           @param project   the project the waypoint is added to
         */
        CGisItemWpt(const QPointF& pos, const QString& name, const QString& icon, IGisProject * project);
        /**
           @brief Create a copy of an existing waypoint with a new position
           @param pos       the waypoint's new position [°]
           @param parentWpt the waypoint to copy
           @param project   the project the waypoint is added to
         */
        CGisItemWpt(const QPointF& pos, const CGisItemWpt &parentWpt, IGisProject *project);
        /**
           @brief Create a 1:1 copy of an existing waypoint (with new key)
           @param parentWpt the waypoint to copy
           @param project   the project the waypoint is added to
           @param idx       the index to insert the item. If -1 the item will be appended to it's group
         */
        CGisItemWpt(const CGisItemWpt &parentWpt, IGisProject *project, int idx, bool clone);
        /**
           @brief Create item from GPX.
           @param xml       the GPX section containing the item
           @param project   the project to append with item
         */
        CGisItemWpt(const QDomNode& xml, IGisProject * project);

        /**
           @brief Create item from list of changes
           @param hist      the change history
           @param project   the project to append with item
        */
        CGisItemWpt(const history_t& hist, IGisProject * project);

        /**
           @brief Read item from database by it's database ID
           @param id        the item's ID in the database
           @param db        the database itself
           @param project   the project to append with item
        */
        CGisItemWpt(quint64 id, QSqlDatabase& db, IGisProject * project);

        CGisItemWpt(const CQlgtWpt& wpt1);

        virtual ~CGisItemWpt();

        void save(QDomNode& gpx);
        QDataStream& operator<<(QDataStream& stream);
        QDataStream& operator>>(QDataStream& stream);

        void setName(const QString& str);
        void setPosition(const QPointF& pos);
        void setElevation(qint32 val);
        void setProximity(qreal val);
        void setIcon(const QString& name);
        void setComment(const QString& str);
        void setDescription(const QString& str);
        void setLinks(const QList<link_t>& links);

        const QString& getName() const {return wpt.name;}
        QString getInfo() const;
        QPointF getPosition(){return QPointF(wpt.lon, wpt.lat);}
        qint32 getElevation(){return wpt.ele;}
        qreal getProximity(){return proximity;}
        const QDateTime& getTime(){return wpt.time;}
        const QString& getIconName(){return wpt.sym;}
        const QString& getComment(){return wpt.cmt;}
        const QString& getDescription(){return wpt.desc;}
        const geocache_t& getGeoCache(){return geocache;}
        const QList<link_t>& getLinks(){return wpt.links;}

        IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
        QPointF getPointCloseBy(const QPoint& ){return posScreen;}

        void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
        void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
        void drawHighlight(QPainter& p);        
        bool isCloseTo(const QPointF& pos);
        bool isGeocache(){return geocache.hasData;}
        void gainUserFocus(bool yes);

        void edit();

        static const QString &getNewName();
        static const QString &getNewIcon();
        static void getNewPosition(QPointF &pos);

    private:
        void setSymbol();
        void readGpx(const QDomNode& xml);
        void setIcon();
        void readGcExt(const QDomNode& xmlCache);
        void writeGcExt(QDomNode& xmlCache);

        static key_t keyUserFocus;
        static QString lastName;
        static QString lastIcon;


        // --- start all waypoint data ----
        wpt_t wpt;
        qreal proximity;
        geocache_t geocache;
        QList<image_t> images;

        QPointF focus;
        QPointF posScreen;

        // additonal data, common to all IGisItems, is found in IItem //

        // --- stop all waypoint data ----

        QPointer<CScrOptWpt> scrOpt;

};

#endif //CGISITEMWPT_H

