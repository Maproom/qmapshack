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

#ifndef CGISITEMRTE_H
#define CGISITEMRTE_H

#include "gis/IGisItem.h"

#include <QPen>

class QDomNode;
class CGisProject;

class CGisItemRte : public IGisItem
{
    public:
        CGisItemRte(const QDomNode &xml, CGisProject *parent);
        virtual ~CGisItemRte();

        const QString& getName(){return rte.name;}
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
        struct rte_t;
        void genKey();
        void readRte(const QDomNode& xml, rte_t& rte);

        static QString keyUserFocus;

        struct rtept_t : public wpt_t
        {
            QPixmap icon;
            QPointF focus;
        };

        struct rte_t
        {
            rte_t() : number(0) {}
            // -- all gpx tags - start
            QString name;
            QString cmt;
            QString desc;
            QString src;
            QList<link_t> links;
            quint64 number;
            QString type;
            QVector<rtept_t> pts;
            // -- all gpx tags - stop
            QMap<QString, QVariant> extensions;
        };

        static const QPen penBackground;
        QPen penForeground;

        rte_t rte;
        QRectF boundingRect;

        QPolygonF line;

};

#endif //CGISITEMRTE_H

