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

#ifndef IGISLINE_H
#define IGISLINE_H

#include <QVector>
#include <QPointF>

class QPolygonF;
class CGisDraw;
class CDemDraw;
struct SGisLine;

class IGisLine
{
public:
    IGisLine();
    virtual ~IGisLine();

    struct subpt_t
    {
        subpt_t();
        subpt_t(const QPointF& pt);
        QPointF coord;
        QPointF pixel;
        qint32 ele;
    };

    struct point_t : public subpt_t
    {
        point_t(){}
        point_t(const QPointF &pt);
        void resetElevation();
        QVector<subpt_t> subpts;
    };

    virtual void setDataFromPolyline(const SGisLine& line) = 0;
    virtual void getPolylineFromData(SGisLine& line) = 0;
};

struct SGisLine : public QVector<IGisLine::point_t>
{
    void updateElevation(CDemDraw * dem);
    void updatePixel(CGisDraw * gis);
};


#endif //IGISLINE_H

