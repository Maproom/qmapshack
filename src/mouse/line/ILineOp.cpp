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

#include "CMainWindow.h"
#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
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
{
    timerRouting = new QTimer(this);
    timerRouting->setSingleShot(true);
    timerRouting->setInterval(400);
    connect(timerRouting, &QTimer::timeout, this, &ILineOp::slotTimeoutRouting);
}

ILineOp::~ILineOp()
{
    canvas->reportStatus("Routino", QString());
}

void ILineOp::cancelDelayedRouting()
{
    timerRouting->stop();
}

void ILineOp::startDelayedRouting()
{
    if(parentHandler->useAutoRouting())
    {
        timerRouting->start();
    }
    else if(parentHandler->useVectorRouting())
    {
        slotTimeoutRouting();
    }
}

void ILineOp::slotTimeoutRouting()
{
    timerRouting->stop();
    finalizeOperation(idxFocus);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}


void ILineOp::drawBg(QPainter& p)
{
    drawLeadLine(leadLinePixel1,p);
    drawLeadLine(leadLinePixel2,p);
}

void ILineOp::drawSinglePointSmall(const QPointF& pt, QPainter& p)
{
    QRect r(0,0,3,3);
    r.moveCenter(pt.toPoint());

    p.setPen(QPen(Qt::white, 2));
    p.setBrush(Qt::white);
    p.drawRect(r);

    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawRect(r);
}

void ILineOp::drawSinglePointLarge(const QPointF &pt, QPainter& p)
{
    rectPoint.moveCenter(pt.toPoint());

    p.setPen(penBgPoint);
    p.setBrush(brushBgPoint);
    p.drawRect(rectPoint);

    p.setPen(penFgPoint);
    p.setBrush(brushFgPoint);
    p.drawRect(rectPoint);
}

void ILineOp::drawLeadLine(const QPolygonF& line, QPainter& p) const
{
    p.setPen(QPen(Qt::yellow, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(line);
}

void ILineOp::mousePressEvent(QMouseEvent * e)
{
    if(idxFocus != NOIDX)
    {
        mousePressEventEx(e);
    }


    if(e->button() == Qt::LeftButton)
    {
        lastPos    = e->pos();
        firstPos   = lastPos;
        mapMove    = true;
        mapDidMove = false;
    }

    showRoutingErrorMessage(QString());
}

void ILineOp::mouseMoveEvent(QMouseEvent * e)
{
    const QPoint& pos = e->pos();

    if(mapMove && ((pos - firstPos).manhattanLength() >= 4))
    {
        QPoint delta = pos - lastPos;
        canvas->moveMap(delta);
        mapDidMove  = true;
    }

    updateLeadLines(idxFocus);
    mouseMoveEventEx(e);

    lastPos = pos;
}

void ILineOp::mouseReleaseEvent(QMouseEvent *e)
{
    if(!mapDidMove)
    {
        mouseReleaseEventEx(e);
    }

    mapMove     = false;
    mapDidMove  = false;
}

void ILineOp::updateLeadLines(qint32 idx)
{
    leadLinePixel1.clear();
    leadLinePixel2.clear();
    subLinePixel1.clear();
    subLinePixel2.clear();

    if(parentHandler->useVectorRouting() && (idx != NOIDX))
    {
        leadLineCoord1.clear();
        leadLineCoord2.clear();
        subLineCoord1.clear();
        subLineCoord2.clear();

        if(idx > 0)
        {
            const IGisLine::point_t& pt1 = points[idx - 1];
            const IGisLine::point_t& pt2 = points[idx];
            if(canvas->findPolylineCloseBy(pt2.pixel, pt2.pixel, 10, leadLineCoord1))
            {
                leadLinePixel1 = leadLineCoord1;
                gis->convertRad2Px(leadLinePixel1);

                segment_t result;
                GPS_Math_SubPolyline(pt1.pixel, pt2.pixel, 10, leadLinePixel1, result);
                result.apply(leadLineCoord1, leadLinePixel1, subLineCoord1, subLinePixel1, gis);
            }
        }

        if(idx < points.size() - 1)
        {
            const IGisLine::point_t& pt1 = points[idx];
            const IGisLine::point_t& pt2 = points[idx + 1];
            if(canvas->findPolylineCloseBy(pt1.pixel, pt1.pixel, 10, leadLineCoord2))
            {
                leadLinePixel2 = leadLineCoord2;
                gis->convertRad2Px(leadLinePixel2);

                segment_t result;
                GPS_Math_SubPolyline(pt1.pixel, pt2.pixel, 10, leadLinePixel2, result);
                result.apply(leadLineCoord2, leadLinePixel2, subLineCoord2, subLinePixel2, gis);
            }
        }
    }
}

void ILineOp::showRoutingErrorMessage(const QString &msg) const
{
    if(msg.isEmpty())
    {
        canvas->reportStatus("Routino", QString());
    }
    else
    {
        canvas->reportStatus("Routino", QString("<span style='color: red;'><b>%1</b><br />%2</span>").arg(tr("Routing")).arg(msg));
    }
}

void ILineOp::tryRouting(IGisLine::point_t& pt1, IGisLine::point_t& pt2) const
{
    QPolygonF subs;

    try
    {
        if(CRouterSetup::self().calcRoute(pt1.coord, pt2.coord, subs) >= 0)
        {
            pt1.subpts.clear();
            for(const QPointF &sub : subs)
            {
                pt1.subpts << IGisLine::subpt_t(sub);
            }
        }
        showRoutingErrorMessage(QString());
    }
    catch(const QString &msg)
    {
        showRoutingErrorMessage(msg);
    }
}

void ILineOp::finalizeOperation(qint32 idx)
{
    if(idx == NOIDX)
    {
        return;
    }

    if(parentHandler->useAutoRouting())
    {
        CCanvas::setOverrideCursor(Qt::WaitCursor,"ILineOp::finalizeOperation");
        if(idx > 0)
        {
            tryRouting(points[idx - 1], points[idx]);
        }
        if(idx < (points.size() - 1))
        {
            tryRouting(points[idx], points[idx + 1]);
        }

        CCanvas::restoreOverrideCursor("ILineOp::finalizeOperation");
    }
    else if(parentHandler->useVectorRouting())
    {
        if(idx > 0)
        {
            IGisLine::point_t& pt1 = points[idx - 1];
            pt1.subpts.clear();
            for(const QPointF &pt : subLineCoord1)
            {
                pt1.subpts << IGisLine::subpt_t(pt);
            }
        }

        if(idx < (points.size() - 1))
        {
            IGisLine::point_t& pt1 = points[idx];
            pt1.subpts.clear();
            for(const QPointF &pt : subLineCoord2)
            {
                pt1.subpts << IGisLine::subpt_t(pt);
            }
        }
    }

    parentHandler->updateStatus();
}


qint32 ILineOp::isCloseTo(const QPoint& pos) const
{
    qint32 min = 30;
    qint32 idx = NOIDX;
    const int N = points.size();
    for(int i = 0; i < N; i++)
    {
        const IGisLine::point_t& pt = points[i];

        qint32 d = (pos - pt.pixel).manhattanLength();
        if(d < min)
        {
            min = d;
            idx = i;
        }
    }

    return idx;
}

qint32 ILineOp::isCloseToLine(const QPoint& pos) const
{
    qint32 idx = NOIDX;
    qreal dist = 60;

    for(int i = 0; i < points.size() - 1; i++)
    {
        QPolygonF line;
        const IGisLine::point_t& pt1 = points[i];
        const IGisLine::point_t& pt2 = points[i + 1];

        if(pt1.subpts.isEmpty())
        {
            line << pt1.pixel << pt2.pixel;
        }
        else
        {
            line << pt1.pixel;
            for(const IGisLine::subpt_t& pt : pt1.subpts)
            {
                line << pt.pixel;
            }
            line << pt2.pixel;
        }

        qreal d = GPS_Math_DistPointPolyline(line, pos);
        if(d < dist)
        {
            dist = d;
            idx  = i;
        }
    }

    return idx;
}

