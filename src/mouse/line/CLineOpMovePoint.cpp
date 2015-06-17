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

    timerRouting = new QTimer(this);
    timerRouting->setSingleShot(true);
    timerRouting->setInterval(400);
    connect(timerRouting, SIGNAL(timeout()), this, SLOT(slotTimeoutRouting()));
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

    if(e->button() == Qt::LeftButton)
    {

        if(movePoint)
        {
            timerRouting->stop();
            slotTimeoutRouting();
            movePoint = false;
        }
        else
        {
            QPointF pos = e->pos();
            gis->convertPx2Rad(pos);

            IGisLine::point_t& pt = points[idxFocus];
            pt.subpts.clear();

            posOrig     = pt.coord;
            pt.coord    = pos;

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

            timerRouting->stop();
            slotTimeoutRouting();
            movePoint = false;
            idxFocus  = NOIDX;
        }
    }
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

        timerRouting->start();
    }
    else
    {
        idxFocus = isCloseTo(e->pos());
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

    QRect r(0,0,9,9);
    r.moveCenter(pt.pixel.toPoint());

    p.setPen(QPen(Qt::white,4));
    p.setBrush(Qt::red);
    p.drawRect(r);

    p.setPen(QPen(Qt::red,2));
    p.setBrush(Qt::red);
    p.drawRect(r);
}


void CLineOpMovePoint::slotTimeoutRouting()
{
    finalizeOperation(idxFocus);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}
