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

#ifndef CGISITEMTRK_H
#define CGISITEMTRK_H

#include "gis/IGisItem.h"

#include <QPen>

class QDomNode;
class CGisProject;

class CGisItemTrk : public IGisItem
{
    public:
        CGisItemTrk(const QDomNode &xml, CGisProject *parent);
        virtual ~CGisItemTrk();

        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);

        void save(QDomNode& gpx);

    private:
        struct trk_t;
        static const QColor  lineColors[];
        static const QString bulletColors[];


        void genKey();
        void readTrk(const QDomNode& xml, trk_t& trk);
        void setColor(const QColor& c);
        void setIcon(const QString& c);

        struct trkpt_t : public wpt_t
        {
            trkpt_t() : flags(0) {}
            enum flag_e
            {
                 eSelected  = 1  ///< selected by track info view
                ,eCursor    = 2  ///< selected by cursor
                ,eDeleted   = 4  ///< mark point as deleted
                ,eFocus     = 8  ///< mark current point of user focus
            };

            quint32 flags;
        };

        struct trkseg_t
        {
            QVector<trkpt_t> pts;
            QMap<QString, QVariant> extensions;
        };

        struct trk_t
        {
            trk_t() : number(0) {}
            // -- all gpx tags - start
            QString name;
            QString cmt;
            QString desc;
            QString src;
            QList<link_t> links;
            quint64 number;
            QString type;
            QVector<trkseg_t> segs;
            // -- all gpx tags - stop
            QMap<QString, QVariant> extensions;
        };

        static const QPen penBackground;
        QPen penForeground;

        trk_t trk;

        QRectF boundingRect;
        /// the track line color
        QColor  color;
        /// the trakpoint bullet icon
        QPixmap bullet;
        /// the track line color by index
        unsigned colorIdx;
        ///
        QPolygonF line;


};

#endif //CGISITEMTRK_H

