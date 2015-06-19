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
            // update subpoints by triggering the routing, if any.
            slotTimeoutRouting();
            // terminate moving the point
            movePoint = false;
        }
        else
        {
            QPointF coord = e->pos();
            gis->convertPx2Rad(coord);

            // start moving the point
            IGisLine::point_t& pt = points[idxFocus];
            // store original position to restore it on abort
            posOrig     = pt.coord;
            pt.coord    = coord;
            // clear the subpoints from this point to the next
            pt.subpts.clear();


            // clear the subpoints from the previous point to this point
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
            // abort moving the point and restore old position.
            IGisLine::point_t& pt = points[idxFocus];
            pt.coord = posOrig;

            // restore subpoints via routing, if selected.
            updateLeadLines(idxFocus);
            slotTimeoutRouting();
            movePoint = false;
            idxFocus  = NOIDX;
        }
    }

    // switch on map panning if move operation is in progress
    parentHandler->setCanvasPanning(movePoint);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::mouseMoveEventEx(QMouseEvent * e)
{
    if(movePoint)
    {
        QPointF coord = e->pos();
        gis->convertPx2Rad(coord);

        IGisLine::point_t& pt = points[idxFocus];

        // update position of point
        pt.coord = coord;

        // no subpoints while the point is actively moving
        pt.subpts.clear();
        if(idxFocus > 0)
        {
            points[idxFocus - 1].subpts.clear();
        }

        // retrigger delayed routing
        timerRouting->start();
    }
    else
    {
        // no point selected yet, find point to highlight
        idxFocus = isCloseTo(e->pos());
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::canvasPanned(QPointF pos)
{
    // update point position after canvas/map panning
    if(movePoint)
    {
        gis->convertPx2Rad(pos);
        points[idxFocus].coord = pos;
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpMovePoint::drawFg(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    const IGisLine::point_t& pt = points[idxFocus];
    drawSinglePoint(pt.pixel, p);
}


