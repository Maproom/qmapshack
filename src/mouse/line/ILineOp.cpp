/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "canvas/CCanvas.h"
#include "gis/rte/router/CRouterSetup.h"
#include "mouse/line/ILineOp.h"
#include "mouse/line/IMouseEditLine.h"

#include <QtWidgets>

ILineOp::ILineOp(SGisLine& points, CGisDraw *gis, CCanvas *canvas, IMouseEditLine *parent)
    : QObject(parent)
    , parentHandler(parent)
    , points(points)
    , canvas(canvas)
    , gis(gis)
    , mapMove(false)
    , mapDidMove(false)
{
}

ILineOp::~ILineOp()
{
}

void ILineOp::mousePressEvent(QMouseEvent * e)
{
    const QPoint& pos = e->pos();

    if(e->button() == Qt::LeftButton)
    {
        lastPos     = pos;
        mapMove     = true;
        mapDidMove  = false;
    }
}

void ILineOp::mouseMoveEvent(QMouseEvent * e)
{
    const QPoint& pos = e->pos();

    if(mapMove)
    {
        if(pos != lastPos)
        {
            QPoint delta = pos - lastPos;
            canvas->moveMap(delta);
            mapDidMove  = true;
        }
    }

    lastPos = pos;
}

void ILineOp::mouseReleaseEvent(QMouseEvent *e)
{
    mapMove     = false;
    mapDidMove  = false;
}

void ILineOp::finalizeOperation(qint32 idx)
{
    if(!parentHandler->useAutoRouting())
    {
        return;
    }

    if(idx != NOIDX)
    {
        if(idx > 0)
        {
            QPolygonF subs;
            IGisLine::point_t& pt1 = points[idx - 1];
            IGisLine::point_t& pt2 = points[idx];
            CRouterSetup::self().calcRoute(pt1.coord, pt2.coord, subs);

            pt1.subpts.clear();
            foreach(const QPointF &sub, subs)
            {
                pt1.subpts << IGisLine::subpt_t(sub);
            }
        }

        if(idx < (points.size() - 1))
        {
            QPolygonF subs;
            IGisLine::point_t& pt1 = points[idx];
            IGisLine::point_t& pt2 = points[idx + 1];
            CRouterSetup::self().calcRoute(pt1.coord, pt2.coord, subs);

            pt1.subpts.clear();
            foreach(const QPointF &sub, subs)
            {
                pt1.subpts << IGisLine::subpt_t(sub);
            }
        }
    }
}
