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
#include "gis/CGisDraw.h"
#include "mouse/line/CLineOpMovePoint.h"
#include "units/IUnit.h"

#include <QtWidgets>

CLineOpMovePoint::CLineOpMovePoint(SGisLine &points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine *parent)
    : ILineOp(points, gis, canvas, parent)
    , idxFocus(NOIDX)
    , movePoint(false)
{
    cursor = QCursor(QPixmap(":/cursors/cursorMovePoint.png"),0,0);
}

CLineOpMovePoint::~CLineOpMovePoint()
{
}

void CLineOpMovePoint::mousePressEvent(QMouseEvent * e)
{
    if(idxFocus == NOIDX)
    {
        ILineOp::mousePressEvent(e);
        return;
    }

    QPointF pos = e->pos();
    gis->convertPx2Rad(pos);

    IGisLine::point_t& pt = points[idxFocus];
    pt.subpts.clear();
    pt.coord = pos;

    if(idxFocus != 0)
    {
        points[idxFocus - 1].subpts.clear();
    }

    movePoint = true;
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::mouseMoveEvent(QMouseEvent * e)
{
    if(mapMove)
    {
        ILineOp::mouseMoveEvent(e);
        return;
    }

    if(movePoint)
    {
        QPointF pos = e->pos();
        gis->convertPx2Rad(pos);

        points[idxFocus].coord = pos;
    }
    else
    {
        idxFocus = isCloseTo(e->pos());
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::mouseReleaseEvent(QMouseEvent *e)
{
    ILineOp::mouseReleaseEvent(e);

    finalizeOperation(idxFocus);

    idxFocus    = NOIDX;
    movePoint   = false;
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::draw(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    const IGisLine::point_t& pt = points[idxFocus];

    QRect r(0,0,9,9);
    r.moveCenter(pt.pixel.toPoint());

    p.setPen(QPen(Qt::red,2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(r);
}

qint32 CLineOpMovePoint::isCloseTo(const QPoint& pos)
{
    qint32 min = NOINT;
    qint32 idx = NOIDX;
    const int N = points.size();
    for(int i = 0; i < N; i++)
    {
        const IGisLine::point_t& pt = points[i];

        qint32 d = (pos - pt.pixel).manhattanLength();
        if((d < 20) && (d < min))
        {
            min = d;
            idx = i;
        }
    }

    return idx;
}
