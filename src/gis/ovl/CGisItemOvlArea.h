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
#include <QPointer>

class IGisProject;
class CScrOptOvlArea;

#define OVL_N_COLORS 17
#define OVL_N_WIDTHS 4
#define OVL_N_STYLES 8

class CGisItemOvlArea : public IGisItem, public IGisLine
{
    public:
        CGisItemOvlArea(const QPolygonF& line, const QString &name, IGisProject * project, int idx);
        CGisItemOvlArea(const CGisItemOvlArea &parentArea, IGisProject * project, int idx);
        CGisItemOvlArea(const QDomNode &xml, IGisProject *project);
        CGisItemOvlArea(const history_t& hist, IGisProject * project);
        virtual ~CGisItemOvlArea();

        QDataStream& operator<<(QDataStream& stream);
        QDataStream& operator>>(QDataStream& stream);

        const QString& getName();
        int getColorIdx(){return colorIdx;}
        QString getInfo();
        void getData(QPolygonF& line);
        const QString& getComment(){return area.cmt;}
        const QString& getDescription(){return area.desc;}
        const QList<link_t>& getLinks(){return area.links;}
        qint32 getWidth(){return area.width;}
        qint32 getStyle(){return area.style;}
        bool getOpacity(){return area.opacity;}


        void setName(const QString& str);
        void setColor(int idx);
        void setData(const QPolygonF& line);
        void setWidth(qint32 w);
        void setStyle(qint32 s);
        void setOpacity(bool yes);
        void setComment(const QString& str);
        void setDescription(const QString& str);


        void save(QDomNode& gpx);
        void edit();

        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
        void drawHighlight(QPainter& p);

        IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
        QPointF getPointCloseBy(const QPoint& screenPos);
        bool isCloseTo(const QPointF& pos);

        void gainUserFocus(bool yes);

        struct width_t
        {
            int width;
            QString string;
        };

        static const QColor  lineColors[OVL_N_COLORS];
        static const QString bulletColors[OVL_N_COLORS];
        static const width_t lineWidths[OVL_N_WIDTHS];
        static const Qt::BrushStyle brushStyles[OVL_N_STYLES];
    protected:
        void genKey();


    public:

        struct pt_t : public wpt_t
        {
            pt_t(){}
        };

        struct area_t
        {
            area_t() : number(0), width(5), style(Qt::BDiagPattern), opacity(false) {}
            // -- all gpx tags - start
            QString name;
            QString cmt;
            QString desc;
            QString src;
            QList<link_t> links;
            quint64 number;
            QString type;
            QVector<pt_t> pts;
            QString color;
            qint32 width;
            qint32 style;
            bool opacity;
        };

        const area_t& getAreaData() const{return area;}

    private:
        void readArea(const QDomNode& xml, area_t& area);
        void setColor(const QColor& c);
        void setIcon(const QString& c);
        void readLine(const QPolygonF &line);
        void deriveSecondaryData();
        QPointF getPolygonCentroid(const QPolygonF& polygon);

        area_t area;

        static QString keyUserFocus;

        QPen penForeground;
        QPen penBackground;

        /// the track line color
        QColor  color;
        /// the trakpoint bullet icon
        QPixmap bullet;
        /// the track line color by index
        unsigned colorIdx;

        QPolygonF line;

        QPointer<CScrOptOvlArea>  scrOpt;

};

#endif //CGISITEMOVLAREA_H

