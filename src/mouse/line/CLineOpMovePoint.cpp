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
#include "mouse/line/IMouseEditLine.h"
#include "units/IUnit.h"

#include <QtWidgets>

CLineOpMovePoint::CLineOpMovePoint(SGisLine &points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine *parent)
    : ILineOp(points, gis, canvas, parent)
    , movePoint(false)
{
    cursor = QCursor(QPixmap(":/cursors/cursorMovePoint.png"),0,0);
}

CLineOpMovePoint::~CLineOpMovePoint()
{
}

void CLineOpMovePoint::mousePressEventEx(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(movePoint)
        {
            slotTimeoutRouting();
            movePoint = false;
        }
        else
        {
            QPointF coord = e->pos();
            gis->convertPx2Rad(coord);

            IGisLine::point_t& pt = points[idxFocus];
            pt.subpts.clear();

            posOrig     = pt.coord;
            pt.coord    = coord;

            if(idxFocus != 0)
            {
                points[idxFocus - 1].subpts.clear();
            }

            movePoint = true;
        }
    }
    else if(e->button() == Qt::RightButton)
    {
        if(movePoint)
        {
            IGisLine::point_t& pt = points[idxFocus];
            pt.coord = posOrig;

            slotTimeoutRouting();
            movePoint = false;
            idxFocus  = NOIDX;
        }
    }
    parentHandler->setCanvasPanning(movePoint);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::mouseMoveEventEx(QMouseEvent * e)
{
    if(movePoint)
    {
        QPointF coord = e->pos();
        gis->convertPx2Rad(coord);
        points[idxFocus].coord = coord;

        timerRouting->start();
    }
    else
    {
        idxFocus = isCloseTo(e->pos());
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::canvasPanned(QPointF pos)
{
    if(movePoint)
    {
        gis->convertPx2Rad(pos);
        points[idxFocus].coord = pos;
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::draw(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    const IGisLine::point_t& pt = points[idxFocus];
    drawSinglePoint(pt.pixel, p);
}


