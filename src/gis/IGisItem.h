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
class IScrOpt;
class IMouse;

class IGisItem : public QTreeWidgetItem
{

    public:
        struct history_event_t
        {
            QDateTime time;
            QString   icon;
            QString   comment;
            QByteArray data;
        };

        struct history_t
        {
            qint32 histIdxInitial;
            qint32 histIdxCurrent;
            QList<history_event_t> events;
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

        enum type_e
        {
              eTypeWpt = 1
            , eTypeTrk = 2
            , eTypeRte = 3
            , eTypeOvl = 4
        };

        enum mark_e
        {
             eMarkNone      = 0
            ,eMarkChanged   = 0x00000001
        };


        IGisItem(QTreeWidgetItem * parent, type_e typ, int idx);
        virtual ~IGisItem();

        /// this mutex has to be locked when ever the item list is accessed.
        static QMutex mutexItems;

        /**
           @brief Update the visual representation of the QTreeWidgetItem
           @param enable
           @param disable
         */
        virtual void updateDecoration(mark_e enable, mark_e disable);

        /**
           @brief Save the item's data into a GPX structure
           @param gpx       the files <gpx> tag to attach the data to
         */
        virtual void save(QDomNode& gpx) = 0;

        /**
           @brief Get key string to identify object
           @return
         */
        const QString& getKey();
        /**
           @brief Get the icon attached to object
           @return
         */
        virtual const QPixmap& getIcon(){return icon;}
        /**
           @brief Get name of this item.
           @return A reference to the internal string object
         */
        virtual const QString& getName() = 0;

        /**
           @brief Get a short string with the items properties to be displayed in tool tips or similar
           @return A string object.
        */
        virtual QString getInfo() = 0;

        /**
            @brief Edit content of item.

            This is quite dependent on the item. The default implemntation does nothing. It has to be
            overwritten and the item has to generate what ever is needed to edit/view it's details.

         */
        virtual void edit(){}

        /**
           @brief Get the dimension of the item

           All coordinates are in Rad. Items with no

           @return
         */
        virtual const QRectF& getBoundingRect(){return boundingRect;}

        /**
           @brief Get screen option object to display and handle actions for this item.
           @param mouse     a pointer to the mouse object initiating the action
           @return A null pointer is returned if no screen option are available
         */
        virtual IScrOpt * getScreenOptions(const QPoint& origin, IMouse * mouse){return 0;}

        /**
           @brief Get a point of the item that is close by the given screen pixel coordinate
           @param point     a point in screen pixels
           @return If no point is found NOPOINTF is returned.
         */
        virtual QPointF getPointCloseBy(const QPoint& point){return NOPOINTF;}

        /**
           @brief Test if the item is close to a given pixel coordinate of the screen

           @param pos       the coordinate on the screen in pixel
           @return If no point can be found NOPOINTF is returned.
        */
        virtual bool isCloseTo(const QPointF& pos) = 0;

        /**
           @brief Query if this item is read only
           @return True if it is read only.
         */
        bool isReadOnly();

        /**
           @brief Query if the item is imported and was changed
           @return True if content was changed.
         */
        bool isTainted();
        /**
           @brief Set the read only mode.

           This is quite dependent on the item. The default implementation will display a
           message box with a warning and ask the user to confirm.

           @param readOnly      set true to make item read only
         */
        virtual void setReadOnlyMode(bool readOnly);

        virtual void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis) = 0;
        virtual void drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis){}
        virtual void drawLabel(QPainter& p, const QRectF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis) = 0;
        virtual void drawHighlight(QPainter& p) = 0;        

        virtual void gainUserFocus(bool yes) = 0;

        /**
           @brief Serialize object out of a QDataStream

           See CGisSerialization.cpp for implementation

           @param stream the binary data stream
           @return The stream object.
        */
        virtual QDataStream& operator<<(QDataStream& stream) = 0;
        /**
           @brief Serialize object into a QDataStream

           See CGisSerialization.cpp for implementation

           @param stream the binary data stream
           @return The stream object.
        */
        virtual QDataStream& operator>>(QDataStream& stream) = 0;

        /**
           @brief Get read access to history of changes

           @return A reference to the history structure.
        */
        const history_t& getHistory() const {return history;}

        /**
           @brief Load a given state of chnage from the history
           @param idx
        */
        void loadHistory(int idx);

        void cutHistory();

        static QString removeHtml(const QString &str);

    protected:
        struct color_t;

        /// set icon of QTreeWidgetItem
        virtual void setSymbol() = 0;
        /// read waypoint data from an XML snippet
        void readWpt(const QDomNode& xml, wpt_t &wpt);
        /// write waypoint data to an XML snippet
        void writeWpt(QDomElement &xml, const wpt_t &wpt);
        /// gnerate a unique key from item's data
        virtual void genKey() = 0;
        /// setup the history structure right after the creation of the item
        void setupHistory();
        /// convert a color string from GPX to a QT color
        QColor str2color(const QString& name);
        /// convertr a QT color to a string to be used in a GPX file
        QString color2str(const QColor &color);
        /// to optimize drawing of large polylines split the line into sections that are visible
        void splitLineToViewport(const QPolygonF& line, const QRectF& extViewport, QList<QPolygonF>& lines);
        /// ditribute arrows over a polyline
        void drawArrows(const QPolygonF &line, const QRectF &extViewport, QPainter& p);
        /// call when ever you make a change to the item's data
        virtual void changed(const QString& what, const QString& icon);

        quint32 flags;
        QString key;
        QString hash;
        QPixmap icon;
        QRectF boundingRect;

        history_t history;


        static const color_t colorMap[];


        struct color_t
        {
            const char * name;
            QColor color;
        };


        enum flags_e
        {
             eFlagCreatedInQms  = 0x00000001
            ,eFlagWriteAllowed  = 0x00000002
            ,eFlagTainted       = 0x00000004
        };



        static inline bool isBlocked(const QRectF& rect, const QList<QRectF> &blockedAreas)
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

