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
#include "mouse/line/IMouseEditLine.h"
#include "units/IUnit.h"

#include <QtWidgets>

CLineOpDeletePoint::CLineOpDeletePoint(SGisLine& points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine * parent)
    : ILineOp(points, gis, canvas, parent)
{
    cursor  = QCursor(QPixmap(":/cursors/cursorDelete.png"),0,0);
}

CLineOpDeletePoint::~CLineOpDeletePoint()
{
}

void CLineOpDeletePoint::mouseMoveEventEx(QMouseEvent * e)
{
    idxFocus = isCloseTo(e->pos());
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpDeletePoint::mouseReleaseEventEx(QMouseEvent *e)
{
    if(!mapDidMove && idxFocus != NOIDX)
    {
        if(idxFocus > 0)
        {
            points[idxFocus - 1].subpts.clear();
        }

        points.remove(idxFocus--);
        updateLeadLines(idxFocus);

        slotTimeoutRouting();

        // store to undo/redo history
        parentHandler->storeToHistory(points);
    }
    idxFocus    = NOIDX;
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}


void CLineOpDeletePoint::drawFg(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    const IGisLine::point_t& pt = points[idxFocus];
    drawSinglePoint(pt.pixel, p);
}
