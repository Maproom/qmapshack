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

        const QString& getName(){return trk.name;}
        QString getInfo();
        IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
        QPointF getPointCloseBy(const QPoint& screenPos);
        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
        void drawHighlight(QPainter& p);
        void save(QDomNode& gpx);
        bool isCloseTo(const QPointF& pos);
        void gainUserFocus();

    private:
        struct trk_t;
        static const QColor  lineColors[];
        static const QString bulletColors[];

        void genKey();
        void readTrk(const QDomNode& xml, trk_t& trk);
        void setColor(const QColor& c);
        void setIcon(const QString& c);
        void deriveSecondaryData();

        static QString keyUserFocus;

        struct trkpt_t : public wpt_t
        {
            trkpt_t()
                : flags(0)
            {reset();}

            void reset()
            {
                deltaDistance   = NOFLOAT;
                distance        = NOFLOAT;
                ascend          = NOFLOAT;
                descend         = NOFLOAT;
                elapsedSeconds  = NOFLOAT;
                elapsedSecondsMoving = NOFLOAT;
                slope           = NOFLOAT;
                speed           = NOFLOAT;
            }

            enum flag_e
            {
                 eSelected  = 0x00000001  ///< selected by track info view
//                ,eCursor    = 0x00000002  ///< selected by cursor
                ,eDeleted   = 0x00000004  ///< mark point as deleted
//                ,eFocus     = 0x00000008  ///< mark current point of user focus
                ,eAllowEdit = 0x80000000
            };

            quint32 flags;

            /// the distance to the last point
            qreal deltaDistance;
            /// the distance from the start of the track
            qreal distance;
            /// the ascend from the start of the track
            qreal ascend;
            /// the descend from the start of the track
            qreal descend;

            qreal slope;

            qreal speed;

            /// the seconds since the start of the track
            qreal elapsedSeconds;

            qreal elapsedSecondsMoving;
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

        quint32     cntTotalPoints;
        quint32     cntVisiblePoints;
        QDateTime   timeStart;
        QDateTime   timeEnd;
        qreal       totalDistance;
        qreal       totalAscend;
        qreal       totalDescend;
        qreal       totalElapsedSeconds;
        qreal       totalElapsedSecondsMoving;

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

