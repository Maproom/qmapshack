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
#include "gis/CGisDraw.h"

#include <QtWidgets>

CLineOpAddPoint::CLineOpAddPoint(SGisLine& points, CGisDraw *gis, CCanvas * canvas, IMouseEditLine * parent)
    : ILineOp(points, gis, canvas, parent)
    , idxFocus(NOIDX)
    , addPoint(false)
{
    cursor = QCursor(QPixmap(":/cursors/cursorAdd.png"),0,0);

    timerRouting = new QTimer(this);
    timerRouting->setSingleShot(true);
    timerRouting->setInterval(400);
    connect(timerRouting, SIGNAL(timeout()), this, SLOT(slotTimeoutRouting()));
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

    if(e->button() == Qt::LeftButton)
    {
        if(addPoint)
        {
            timerRouting->stop();
            slotTimeoutRouting();
            addPoint = false;
            idxFocus = NOIDX;
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
        points.remove(idxFocus);
        addPoint = false;
        idxFocus = NOIDX;
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

    if(addPoint)
    {
        QPointF coord = e->pos();
        gis->convertPx2Rad(coord);

        points[idxFocus].coord = coord;

        timerRouting->start();
    }
    else
    {
        idxFocus = isCloseToLine(e->pos());
    }
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

    if(addPoint)
    {
        const IGisLine::point_t& pt1 = points[idxFocus];
        QRect r(0,0,9,9);

        p.setPen(QPen(Qt::white,4));
        p.setBrush(Qt::red);

        r.moveCenter(pt1.pixel.toPoint());
        p.drawRect(r);

        p.setPen(QPen(Qt::red,2));
        p.setBrush(Qt::red);

        r.moveCenter(pt1.pixel.toPoint());
        p.drawRect(r);
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
            foreach(const IGisLine::subpt_t& pt, pt1.subpts)
            {
                line << pt.pixel;
            }
            line << pt2.pixel;
        }

        QRect r(0,0,9,9);

        p.setPen(QPen(Qt::white,4));
        p.setBrush(Qt::white);

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

void CLineOpAddPoint::slotTimeoutRouting()
{
    finalizeOperation(idxFocus);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}
