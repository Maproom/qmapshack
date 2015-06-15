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

#include "CMainWindow.h"
#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/IGisLine.h"
#include "gis/rte/router/CRouterSetup.h"
#include "mouse/CScrOptPoint.h"
#include "mouse/CScrOptRange.h"
#include "mouse/line/CLineOpAddPoint.h"
#include "mouse/line/CLineOpDeletePoint.h"
#include "mouse/line/CLineOpMovePoint.h"
#include "mouse/line/CLineOpSelectRange.h"
#include "mouse/line/CScrOptEditLine.h"
#include "mouse/line/IMouseEditLine.h"
#include "units/IUnit.h"


#include <QtWidgets>

IMouseEditLine::IMouseEditLine(quint32 features, const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent)
    , features(features)
    , lineOp(0)
{
    commonSetup();
    scrOptEditLine->pushSaveOrig->hide(); // hide as there is no original

    points << IGisLine::point_t(point);
    points.updatePixel(gis);
}

IMouseEditLine::IMouseEditLine(quint32 features, IGisLine &src, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , features(features)
    , lineOp(0)
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

    slotMovePoint();
}

void IMouseEditLine::drawLine(const QPolygonF &l, const QColor color, int width, QPainter& p)
{
    p.setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(l);
}


void IMouseEditLine::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
    if(needsRedraw & (CCanvas::eRedrawMouse|CCanvas::eRedrawGis))
    {
        points.updatePixel(gis);

        pixelLine.clear();
        pixelPts.clear();
        pixelSubs.clear();

        for(int i = 0; i < points.size(); i++)
        {
            IGisLine::point_t& pt = points[i];
            pixelLine << pt.pixel;
            pixelPts << pt.pixel;

            for(int n = 0; n < pt.subpts.size(); n++)
            {
                IGisLine::subpt_t& sub = pt.subpts[n];
                pixelLine << sub.pixel;
                pixelSubs << sub.pixel;
            }
        }
    }

    drawLine(pixelLine, Qt::white, 5, p);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    QRect r1(0,0,9,9);
    foreach(const QPointF &pt, pixelPts)
    {
        r1.moveCenter(pt.toPoint());
        p.drawRect(r1);
    }

    drawLine(pixelLine, Qt::magenta, 3, p);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    QRect r2(0,0,7,7);
    foreach(const QPointF &pt, pixelPts)
    {
        r2.moveCenter(pt.toPoint());
        p.drawRect(r2);
    }

    lineOp->draw(p);
}

void IMouseEditLine::mousePressEvent(QMouseEvent * e)
{
    lineOp->mousePressEvent(e);
}

void IMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{
    lineOp->mouseMoveEvent(e);
}

void IMouseEditLine::mouseReleaseEvent(QMouseEvent *e)
{
    lineOp->mouseReleaseEvent(e);
}

void IMouseEditLine::wheelEvent(QWheelEvent * e)
{
    canvas->update();
}

void IMouseEditLine::slotDeletePoint()
{
    delete lineOp;
    lineOp = new CLineOpDeletePoint(points, gis, canvas, this);
    changeCursor();
}

void IMouseEditLine::slotSelectRange()
{
    delete lineOp;
    lineOp = new CLineOpSelectRange(points, gis, canvas, this);
    changeCursor();
}

void IMouseEditLine::slotMovePoint()
{
    delete lineOp;
    lineOp = new CLineOpMovePoint(points, gis, canvas, this);
    changeCursor();
}

void IMouseEditLine::slotAddPoint()
{
    delete lineOp;
    lineOp = new CLineOpAddPoint(points, gis, canvas, this);
    changeCursor();
}

void IMouseEditLine::changeCursor()
{
    cursor = lineOp->getCursor();
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(cursor);
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



