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
#include "mouse/line/CLineOpAddPoint.h"

#include <QtWidgets>

CLineOpAddPoint::CLineOpAddPoint(SGisLine& points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine * parent)
    : ILineOp(points, gis, canvas, parent)
    , idxFocus(NOIDX)
{
    cursor = QCursor(QPixmap(":/cursors/cursorAdd.png"),0,0);
}

CLineOpAddPoint::~CLineOpAddPoint()
{
}

void CLineOpAddPoint::mousePressEvent(QMouseEvent * e)
{
    if(idxFocus == NOIDX)
    {
        ILineOp::mousePressEvent(e);
        return;
    }

    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::mouseMoveEvent(QMouseEvent * e)
{
    if(mapMove)
    {
        ILineOp::mouseMoveEvent(e);
        return;
    }

    idxFocus = isCloseToLine(e->pos());
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::mouseReleaseEvent(QMouseEvent *e)
{
    ILineOp::mouseReleaseEvent(e);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::draw(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    if(idxFocus < (points.size() - 1))
    {
        QPolygonF line;
        const IGisLine::point_t& pt1 = points[idxFocus];
        const IGisLine::point_t& pt2 = points[idxFocus + 1];

        if(pt1.subpts.isEmpty())
        {
            line << pt1.pixel << pt2.pixel;
        }
        else
        {
            foreach(const IGisLine::subpt_t& pt, pt1.subpts)
            {
                line << pt.pixel;
            }
            line << pt2.pixel;
        }

        QRect r(0,0,9,9);

        p.setPen(QPen(Qt::white,4));
        p.setBrush(Qt::red);

        r.moveCenter(pt1.pixel.toPoint());
        p.drawRect(r);
        r.moveCenter(pt2.pixel.toPoint());
        p.drawRect(r);

        p.setPen(QPen(Qt::white, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(line);


        p.setPen(QPen(Qt::red,2));
        p.setBrush(Qt::red);

        r.moveCenter(pt1.pixel.toPoint());
        p.drawRect(r);
        r.moveCenter(pt2.pixel.toPoint());
        p.drawRect(r);

        p.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(line);
    }
}
