/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/IGisLine.h"
#include "gis/rte/router/CRouterSetup.h"
#include "mouse/CScrOptEditLine.h"
#include "mouse/CScrOptPoint.h"
#include "mouse/CScrOptRange.h"
#include "mouse/IMouseEditLine.h"
#include "units/IUnit.h"
#include "CMainWindow.h"


#include <QtWidgets>

IMouseEditLine::IMouseEditLine(quint32 features, const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent)
    , features(features)
    , state(eStateIdle)
    , idxFocus(NOIDX)
{
    commonSetup();
    scrOptEditLine->pushSaveOrig->hide(); // hide as there is no original

    points << IGisLine::point_t(point);
    points.updatePixel(gis);
}

IMouseEditLine::IMouseEditLine(quint32 features, IGisLine &src, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , features(features)
    , state(eStateIdle)
    , idxFocus(NOIDX)
{
    commonSetup();

    src.getPolylineFromData(points);
    points.updatePixel(gis);
}


IMouseEditLine::~IMouseEditLine()
{
    delete scrOptEditLine;
}

void IMouseEditLine::commonSetup()
{
    // create permanent line edit on screen options
    scrOptEditLine = new CScrOptEditLine(canvas);
    connect(scrOptEditLine->pushSaveOrig, SIGNAL(clicked()), this, SLOT(slotCopyToOrig()));
    connect(scrOptEditLine->pushSaveNew, SIGNAL(clicked()), this, SLOT(slotCopyToNew()));
    connect(scrOptEditLine->pushAbort, SIGNAL(clicked()), this, SLOT(slotAbort()));

    connect(scrOptEditLine->toolMovePoint, SIGNAL(clicked()), this, SLOT(slotMovePoint()));
    connect(scrOptEditLine->toolSelectRange, SIGNAL(clicked()), this, SLOT(slotSelectRange()));
    connect(scrOptEditLine->toolAddPoint, SIGNAL(clicked()), this, SLOT(slotAddPoint()));
    connect(scrOptEditLine->toolDeletePoint, SIGNAL(clicked()), this, SLOT(slotDeletePoint()));
}

void IMouseEditLine::drawLine(const QPolygonF &l, QPainter& p)
{
}


void IMouseEditLine::draw(QPainter& p, bool needsRedraw, const QRect &rect)
{
    if(needsRedraw)
    {
        points.updatePixel(gis);
    }

    QPolygonF line;
    QPolygonF pts;
    QPolygonF subs;

    for(int i = 0; i < points.size(); i++)
    {
        IGisLine::point_t& pt = points[i];
        line << pt.pixel;
        pts  << pt.pixel;

        for(int n = 0; n < pt.subpts.size(); n++)
        {
            IGisLine::subpt_t& sub = pt.subpts[n];
            line << sub.pixel;
            subs << sub.pixel;
        }
    }

    p.setPen(QPen(Qt::white, 7));
    p.drawPolyline(line);

    p.setPen(QPen(Qt::magenta, 5));
    p.drawPolyline(line);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::red);
    foreach(const QPointF& pt, pts)
    {
        p.drawEllipse(pt, 3, 3);
    }

}

void IMouseEditLine::mousePressEvent(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
        case eStateIdle:
        {
            if(idxFocus == NOIDX)
            {
                state = eStateMoveMap;
            }
            break;
        }
        }
    }
}

void IMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{
    QPoint point = e->pos();

    switch(state)
    {
    case eStateIdle:
        break;

    case eStateMoveMap:
    {
        if(point != lastPoint)
        {
            QPoint delta = point - lastPoint;
            canvas->moveMap(delta);
        }
        break;
    }
    }

    lastPoint = point;
}

void IMouseEditLine::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(state == eStateMoveMap)
        {
            state = eStateIdle;
        }
    }
}

void IMouseEditLine::wheelEvent(QWheelEvent * e)
{
    canvas->update();
}

void IMouseEditLine::slotDeletePoint()
{

}

void IMouseEditLine::slotSelectRange()
{

}

void IMouseEditLine::slotMovePoint()
{

}

void IMouseEditLine::slotAddPoint()
{

}

void IMouseEditLine::slotAbort()
{
    canvas->resetMouse();
    canvas->update();
}

void IMouseEditLine::slotCopyToOrig()
{      
    IGisLine * l = getGisLine();
    if(l != 0)
    {
        CMainWindow::self().getEelevationAt(points);
        l->setDataFromPolyline(points);
    }


    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}


