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
#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/IGisLine.h"
#include "gis/rte/router/CRouterSetup.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "mouse/line/CLineOpAddPoint.h"
#include "mouse/line/CLineOpDeletePoint.h"
#include "mouse/line/CLineOpMovePoint.h"
#include "mouse/line/CLineOpSelectRange.h"
#include "mouse/line/CScrOptEditLine.h"
#include "mouse/line/IMouseEditLine.h"
#include "units/IUnit.h"

#include <QtWidgets>

IMouseEditLine::IMouseEditLine(const IGisItem::key_t &key, const QPointF& point, bool enableStatus, const QString &type, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent)
    , key(key)
    , enableStatus(enableStatus)
    , type(type)
{
    commonSetup();
    scrOptEditLine->pushSaveOrig->hide(); // hide as there is no original

    points << IGisLine::point_t(point);
    points.updatePixel(gis);

    storeToHistory(points);
}

IMouseEditLine::IMouseEditLine(const IGisItem::key_t &key, IGisLine &src, bool enableStatus, const QString &type, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , idxHistory(NOIDX)
    , key(key)
    , doCanvasPanning(false)
    , lineOp(0)
    , enableStatus(enableStatus)
    , type(type)
{
    commonSetup();

    src.getPolylineFromData(points);
    points.updatePixel(gis);

    storeToHistory(points);
}


IMouseEditLine::~IMouseEditLine()
{
    canvas->reportStatus("IMouseEditLine","");
    canvas->reportStatus(key.item,"");

    int mode = 0;
    if(scrOptEditLine->toolNoRoute->isChecked())
    {
        mode = 0;
    }
    else if(scrOptEditLine->toolAutoRoute->isChecked())
    {
        mode = 1;
    }
    else if(scrOptEditLine->toolVectorRoute->isChecked())
    {
        mode = 2;
    }

    SETTINGS;
    cfg.setValue("Route/drawMode", mode);

    delete scrOptEditLine;
}

void IMouseEditLine::commonSetup()
{
    // create permanent line edit on screen options
    scrOptEditLine = new CScrOptEditLine(this);
    connect(scrOptEditLine->pushSaveOrig,    SIGNAL(clicked()), this, SLOT(slotCopyToOrig()   ));
    connect(scrOptEditLine->pushSaveNew,     SIGNAL(clicked()), this, SLOT(slotCopyToNew()    ));
    connect(scrOptEditLine->pushAbort,       SIGNAL(clicked()), this, SLOT(slotAbort()        ));

    connect(scrOptEditLine->toolMovePoint,   SIGNAL(clicked()), this, SLOT(slotMovePoint()    ));
    connect(scrOptEditLine->toolSelectRange, SIGNAL(clicked()), this, SLOT(slotSelectRange()  ));
    connect(scrOptEditLine->toolAddPoint,    SIGNAL(clicked()), this, SLOT(slotAddPoint()     ));
    connect(scrOptEditLine->toolDeletePoint, SIGNAL(clicked()), this, SLOT(slotDeletePoint()  ));

    connect(scrOptEditLine->toolNoRoute,     SIGNAL(clicked()), this, SLOT(slotNoRouting()    ));
    connect(scrOptEditLine->toolAutoRoute,   SIGNAL(clicked()), this, SLOT(slotAutoRouting()  ));
    connect(scrOptEditLine->toolVectorRoute, SIGNAL(clicked()), this, SLOT(slotVectorRouting()));

    connect(scrOptEditLine->toolUndo,        SIGNAL(clicked()), this, SLOT(slotUndo()         ));
    connect(scrOptEditLine->toolRedo,        SIGNAL(clicked()), this, SLOT(slotRedo()         ));

    SETTINGS;
    int mode = cfg.value("Route/drawMode",0).toInt();
    switch(mode)
    {
    case 0:
        scrOptEditLine->toolNoRoute->setChecked(true);
        break;

    case 1:
        scrOptEditLine->toolAutoRoute->setChecked(true);
        break;

    case 2:
        scrOptEditLine->toolVectorRoute->setChecked(true);
        break;
    }

    slotMovePoint();
}

void IMouseEditLine::abortStep()
{
    // at first try to abort a step within the current operation (px. stop adding a new waypoint)
    if(!lineOp->abortStep())
    {
        // if within operation nothing can be aborted, then abort the whole operation
        // this equals clicking the `abort` button
        slotAbortEx(true);
    }
}

bool IMouseEditLine::useAutoRouting()
{
    return scrOptEditLine->toolAutoRoute->isChecked();
}

bool IMouseEditLine::useVectorRouting()
{
    return scrOptEditLine->toolVectorRoute->isChecked();
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

    if(pixelPts.isEmpty())
    {
        return;
    }

    lineOp->drawBg(p);

    drawLine(pixelLine, Qt::white, 7, p);

    // draw magenta arrows (with white background)
    p.setBrush(Qt::magenta);
    CDraw::arrows(pixelLine, QRectF(), p, 10, 80);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    QRect r1(0,0,9,9);
    foreach(const QPointF &pt, pixelPts)
    {
        r1.moveCenter(pt.toPoint());
        p.drawRect(r1);
    }

    drawLine(pixelLine, Qt::magenta, 5, p);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    QRect r2(0,0,7,7);
    foreach(const QPointF &pt, pixelPts)
    {
        r2.moveCenter(pt.toPoint());
        p.drawRect(r2);
    }

    foreach(const QPointF &pt, pixelSubs)
    {
        p.drawEllipse(pt, 2, 2);
    }

    QRect r3(0,0,9,9);
    p.setBrush(Qt::NoBrush);

    p.setPen(QPen(Qt::yellow,2));
    r3.moveCenter(pixelPts.first().toPoint());
    p.drawRect(r3);

    p.setPen(QPen(Qt::green,2));
    r3.moveCenter(pixelPts.last().toPoint());
    p.drawRect(r3);

    lineOp->drawFg(p);
}


void IMouseEditLine::startNewLine(const QPointF& point)
{
    scrOptEditLine->toolAddPoint->setChecked(true);
    slotAddPoint();

    CLineOpAddPoint * lineOpAddPoint = dynamic_cast<CLineOpAddPoint*>(lineOp);
    if(lineOpAddPoint)
    {
        lineOpAddPoint->append();
    }

    canvas->reportStatus(key.item, tr("<b>New Line</b><br/>Move the mouse and use the left mouse button to drop points. When done use the right mouse button to stop.<br/>"));
}

void IMouseEditLine::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    lineOp->mousePressEvent(e);
}

void IMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();

    if(doCanvasPanning)
    {
        panCanvas(e->pos());
    }

    lineOp->mouseMoveEvent(e);
}

void IMouseEditLine::mouseReleaseEvent(QMouseEvent *e)
{
    point  = e->pos();
    lineOp->mouseReleaseEvent(e);
}

void IMouseEditLine::wheelEvent(QWheelEvent * e)
{
    canvas->update();
    lineOp->wheelEvent(e);
}

void IMouseEditLine::keyPressEvent(QKeyEvent * e)
{
    canvas->update();
    lineOp->keyPressEvent(e);
}

void IMouseEditLine::slotPanCanvas()
{
    IMouse::slotPanCanvas();
    lineOp->canvasPanned(point);
}

void IMouseEditLine::slotDeletePoint()
{
    canvas->reportStatus(key.item, tr("<b>Delete Point</b><br/>Move the mouse close to a point and press the left button to delete it.<br/>"));
    delete lineOp;
    lineOp = new CLineOpDeletePoint(points, gis, canvas, this);
    changeCursor();
    doCanvasPanning = false;
}

void IMouseEditLine::slotSelectRange()
{
    canvas->reportStatus(key.item, tr("<b>Select Range of Points</b><br/>Left click on first point to start selection. Left click second point to complete selection and choose from options. Use the right mouse button to cancel.<br/>"));
    delete lineOp;
    lineOp = new CLineOpSelectRange(points, gis, canvas, this);
    changeCursor();
    doCanvasPanning = false;
}

void IMouseEditLine::slotMovePoint()
{
    canvas->reportStatus(key.item, tr("<b>Move Point</b><br/>Move the mouse close to a point and press the left button to make it stick to the cursor. Move the mouse to move the point. Drop the point by a left click. Use the right mouse button to cancel.<br/>"));
    delete lineOp;
    lineOp = new CLineOpMovePoint(points, gis, canvas, this);
    changeCursor();
    doCanvasPanning = false;
}

void IMouseEditLine::slotAddPoint()
{
    canvas->reportStatus(key.item, tr("<b>Add Point</b><br/>Move the mouse close to a line segment and press the left button to add a point. The point will stick to the cursor and you can move it.  Drop the point by a left click. Use the right mouse button to cancel.<br/>"));
    delete lineOp;
    lineOp = new CLineOpAddPoint(points, gis, canvas, this);
    changeCursor();
    doCanvasPanning = false;
}

void IMouseEditLine::slotNoRouting()
{
    canvas->reportStatus(key.item, tr("<b>No Routing</b><br/>All points will be connected with a straight line.<br/>"));
}

void IMouseEditLine::slotAutoRouting()
{
    canvas->reportStatus(key.item, tr("<b>Auto Routing</b><br/>The current router setup is used to derive a route between points. <b>Note:</b> The selected router must be able to route on-the-fly. Offline routers usually can do, online routers can't.<br/>"));
}

void IMouseEditLine::slotVectorRouting()
{
    canvas->reportStatus(key.item, tr("<b>Vector Routing</b><br/>Connect points with a line from a loaded vector map if possible.<br/>"));
}


void IMouseEditLine::changeCursor()
{
    cursor = lineOp->getCursor();
    if(QApplication::overrideCursor() != 0)
    {
        CCanvas::changeOverrideCursor(cursor,"IMouseEditLine::changeCursor");
    }
}

void IMouseEditLine::slotAbortEx(bool showMB)
{
    // ask if the current operation should be aborted (only in case there are things to be saved)
    bool doAbort = ( idxHistory == 0 ) || !showMB;
    if(!doAbort)
    {
        doAbort = (QMessageBox::Yes == QMessageBox::question(NULL, "Abort", "Do you really want to abort?\nAny modifications done will be discarded.", QMessageBox::Yes | QMessageBox::No));
    }

    if(doAbort)
    {
        canvas->resetMouse();
        canvas->update();
    }
}

void IMouseEditLine::slotCopyToOrig()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    IGisLine * l = getGisLine();
    if(l != 0)
    {
        CMainWindow::self().getEelevationAt(points);
        l->setDataFromPolyline(points);
    }


    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void IMouseEditLine::restoreFromHistory(SGisLine& line)
{
    line = history[idxHistory];
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);

    updateStatus();
}

void IMouseEditLine::storeToHistory(const SGisLine& line)
{
    // crop history if necessary
    if(idxHistory != NOIDX)
    {
        while(history.size() > (idxHistory + 1))
        {
            history.pop_back();
        }
    }

    history << line;
    idxHistory = history.size() - 1;

    scrOptEditLine->toolRedo->setEnabled(false);
    scrOptEditLine->toolUndo->setEnabled(idxHistory > 0);

    updateStatus();
}

void IMouseEditLine::slotUndo()
{
    if(idxHistory > 0)
    {
        idxHistory--;
    }

    points = history[idxHistory];

    scrOptEditLine->toolRedo->setEnabled(true);
    scrOptEditLine->toolUndo->setEnabled(idxHistory > 0);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void IMouseEditLine::slotRedo()
{
    if(idxHistory < (history.size() - 1))
    {
        idxHistory++;
    }


    points = history[idxHistory];

    scrOptEditLine->toolRedo->setEnabled(idxHistory < (history.size() - 1));
    scrOptEditLine->toolUndo->setEnabled(true);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);
}

void IMouseEditLine::updateStatus()
{
    if(!enableStatus || points.isEmpty())
    {
        canvas->reportStatus("IMouseEditLine","");
        return;
    }

    canvas->getElevationAt(points);

    qreal asc   = 0;
    qreal dsc   = 0;
    qreal dist  = 0;

    qreal lastEle = points[0].ele;
    qreal absDelta;
    qreal delta;


    QPointF lastPos = points[0].coord;

    for(int i = 0; i < points.size(); i++)
    {
        const IGisLine::point_t& pt1 = points[i];

        delta       = pt1.ele - lastEle;
        absDelta    = qAbs(delta);
        if(absDelta > ASCEND_THRESHOLD)
        {
            if(delta > 0)
            {
                asc += delta;
            }
            if(delta < 0)
            {
                dsc -= delta;
            }
            lastEle = pt1.ele;
        }

        dist += GPS_Math_Distance(lastPos.x(), lastPos.y(), pt1.coord.x(), pt1.coord.y());
        lastPos = pt1.coord;

        foreach(const IGisLine::subpt_t& pt, pt1.subpts)
        {
            delta       = pt.ele - lastEle;
            absDelta    = qAbs(delta);
            if(absDelta > ASCEND_THRESHOLD)
            {
                if(delta > 0)
                {
                    asc += delta;
                }
                if(delta < 0)
                {
                    dsc -= delta;
                }
                lastEle = pt.ele;
            }

            dist += GPS_Math_Distance(lastPos.x(), lastPos.y(), pt.coord.x(), pt.coord.y());
            lastPos = pt.coord;
        }
    }

    QString msg, val, unit;

    msg += tr("<b>%1 Metrics</b>").arg(type);
    msg += "<table>";
    IUnit::self().meter2distance(dist, val, unit);
    msg += "<tr><td>" + tr("Distance:") + "</td><td>" + QString("&nbsp;%1 %2").arg(val).arg(unit) + "</td></tr>";
    IUnit::self().meter2elevation(asc, val, unit);
    msg += "<tr><td>" + tr("Ascend:") + "</td><td>" + QString("&nbsp;%1 %2").arg(val).arg(unit) + "</td></tr>";
    IUnit::self().meter2elevation(dsc, val, unit);
    msg += "<tr><td>" + tr("Descend:") + "</td><td>" + QString("&nbsp;%1 %2").arg(val).arg(unit) + "</td></tr>";
    msg += "</table>";

    canvas->reportStatus("IMouseEditLine",msg);
}
