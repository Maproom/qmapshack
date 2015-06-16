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
#include "mouse/line/CLineOpDeletePoint.h"
#include "units/IUnit.h"

#include <QtWidgets>

CLineOpDeletePoint::CLineOpDeletePoint(SGisLine& points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine * parent)
    : ILineOp(points, gis, canvas, parent)
    , idxFocus(NOIDX)
{
    cursor  = QCursor(QPixmap(":/cursors/cursorDelete.png"),0,0);
}

CLineOpDeletePoint::~CLineOpDeletePoint()
{
}

void CLineOpDeletePoint::mouseMoveEvent(QMouseEvent * e)
{
    if(mapMove)
    {
        ILineOp::mouseMoveEvent(e);
        return;
    }

    idxFocus = isCloseTo(e->pos());
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpDeletePoint::mouseReleaseEvent(QMouseEvent *e)
{
    if(!mapDidMove && idxFocus != NOIDX)
    {
        points.remove(idxFocus);

        finalizeOperation(idxFocus - 1);
    }
    idxFocus    = NOIDX;

    ILineOp::mouseReleaseEvent(e);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}


void CLineOpDeletePoint::draw(QPainter& p)
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
