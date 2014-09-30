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

#ifndef CGISITEMOVLAREA_H
#define CGISITEMOVLAREA_H

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"

#include <QPen>

class CGisProject;

#define OVL_N_COLORS 17

class CGisItemOvlArea : public IGisItem, public IGisLine
{
    public:
        CGisItemOvlArea(const QPolygonF& line, const QString &name, CGisProject * project, int idx);
        CGisItemOvlArea(const QDomNode &xml, CGisProject *project);
        virtual ~CGisItemOvlArea();

        void setName(const QString& str);
        void setColor(int idx);
        void setData(const QPolygonF& line);

        const QString& getName();
        QString getInfo();
        void getData(QPolygonF& line);


        void save(QDomNode& gpx);

        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
        void drawHighlight(QPainter& p);

        void gainUserFocus(bool yes);


        bool isCloseTo(const QPointF& pos);

        static const QColor  lineColors[OVL_N_COLORS];
        static const QString bulletColors[OVL_N_COLORS];
    protected:
        void genKey();


    public:

        struct pt_t : public wpt_t
        {
            pt_t(){}
        };

        struct area_t
        {
            area_t() : number(0) {}
            // -- all gpx tags - start
            QString name;
            QString cmt;
            QString desc;
            QString src;
            QList<link_t> links;
            quint64 number;
            QString type;
            QVector<pt_t> pts;
            // -- all gpx tags - stop

            QString color;
        };

    private:
        void readArea(const QDomNode& xml, area_t& area);
        void setColor(const QColor& c);
        void setIcon(const QString& c);
        void readLine(const QPolygonF &line);
        void deriveSecondaryData();

        area_t area;

        static QString keyUserFocus;
        static const QPen penBackground;

        QPen penForeground;

        /// the track line color
        QColor  color;
        /// the trakpoint bullet icon
        QPixmap bullet;
        /// the track line color by index
        unsigned colorIdx;

        QPolygonF line;



};

#endif //CGISITEMOVLAREA_H

