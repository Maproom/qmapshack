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
#include "mouse/line/CLineOpAddPoint.h"
#include "mouse/line/IMouseEditLine.h"

#include <QtWidgets>

CLineOpAddPoint::CLineOpAddPoint(SGisLine& points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine * parent)
    : ILineOp(points, gis, canvas, parent)
    , addPoint(false)
    , isPoint(false)
{
    cursor = QCursor(QPixmap(":/cursors/cursorAdd.png"),0,0);
}

CLineOpAddPoint::~CLineOpAddPoint()
{
}

void CLineOpAddPoint::append()
{
    idxFocus = points.size();
    points.insert(idxFocus, IGisLine::point_t(points.last()));
    addPoint = true;
    isPoint  = true;
    parentHandler->setCanvasPanning(addPoint);
}

void CLineOpAddPoint::mousePressEventEx(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(addPoint)
        {
            slotTimeoutRouting();

            if(isPoint)
            {
                if(idxFocus == (points.size() - 1))
                {
                    idxFocus++;
                }

                QPointF coord = e->pos();
                gis->convertPx2Rad(coord);
                points.insert(idxFocus, IGisLine::point_t(coord));
            }
            else
            {
                addPoint = false;
                idxFocus = NOIDX;
            }
        }
        else if(isPoint)
        {
            if(idxFocus == (points.size() - 1))
            {
                idxFocus++;
            }

            QPointF coord = e->pos();
            gis->convertPx2Rad(coord);
            points.insert(idxFocus, IGisLine::point_t(coord));

            addPoint = true;
        }
        else
        {
            QPointF coord = e->pos();
            gis->convertPx2Rad(coord);

            idxFocus++;
            points.insert(idxFocus, IGisLine::point_t(coord));

            addPoint = true;
        }
    }
    else if(e->button() == Qt::RightButton)
    {
        addPoint = false;

        slotTimeoutRouting();

        if(idxFocus > 0)
        {
            points[idxFocus - 1].subpts.clear();
        }
        points.remove(idxFocus);
        idxFocus--;

        if(idxFocus == (points.size() - 1))
        {
            subLineCoord1.clear();
            subLineCoord2.clear();
        }

        idxFocus = NOIDX;
    }

    parentHandler->setCanvasPanning(addPoint);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::mouseMoveEventEx(QMouseEvent * e)
{
    if(addPoint)
    {
        QPointF coord = e->pos();
        gis->convertPx2Rad(coord);

        IGisLine::point_t& pt = points[idxFocus];

        pt.coord = coord;
        pt.subpts.clear();

        if(idxFocus > 0)
        {
            points[idxFocus - 1].subpts.clear();
        }

        timerRouting->start();
    }
    else
    {
        isPoint  = false;
        idxFocus = isCloseToLine(e->pos());
        if(idxFocus == NOIDX)
        {
            idxFocus = isCloseTo(e->pos());
            isPoint  = true;
        }
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void CLineOpAddPoint::canvasPanned(QPointF pos)
{
    if(addPoint)
    {
        gis->convertPx2Rad(pos);
        points[idxFocus].coord = pos;
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}


void CLineOpAddPoint::drawFg(QPainter& p)
{
    if(idxFocus == NOIDX)
    {
        return;
    }

    if(addPoint)
    {
        const IGisLine::point_t& pt = points[idxFocus];
        drawSinglePoint(pt.pixel, p);
    }
    else if(isPoint)
    {
        const IGisLine::point_t& pt = points[idxFocus];
        drawSinglePoint(pt.pixel, p);
    }
    else if(idxFocus < (points.size() - 1))
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
            line << pt1.pixel;
            foreach(const IGisLine::subpt_t& pt, pt1.subpts)
            {
                line << pt.pixel;
            }
            line << pt2.pixel;
        }

        p.setPen(penBgPoint);
        p.setBrush(brushBgPoint);

        rectPoint.moveCenter(pt1.pixel.toPoint());
        p.drawRect(rectPoint);
        rectPoint.moveCenter(pt2.pixel.toPoint());
        p.drawRect(rectPoint);

        p.setPen(QPen(Qt::white, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(line);


        p.setPen(penFgPoint);
        p.setBrush(brushFgPoint);

        rectPoint.moveCenter(pt1.pixel.toPoint());
        p.drawRect(rectPoint);
        rectPoint.moveCenter(pt2.pixel.toPoint());
        p.drawRect(rectPoint);

        p.setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(line);
    }
}

