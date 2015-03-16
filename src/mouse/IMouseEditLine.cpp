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
#include "mouse/CScrOptEditLine.h"
#include "mouse/CScrOptPoint.h"
#include "mouse/CScrOptRange.h"
#include "mouse/IMouseEditLine.h"
#include "units/IUnit.h"


#include <QtWidgets>

IMouseEditLine::IMouseEditLine(const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouse(gis, parent)
    , state(eStateAddPointFwd)
    , idxFocus(0)
    , idxStart(-1)
    , idxStop(-1)
{
    // create a single point line
    coords1 << point;
    line << point;
    gis->convertRad2Px(line);

    // start with adding trackpints imediately
    slotAddPoint2();

    // create permanent line edit on screen options
    scrOptEditLine = new CScrOptEditLine(canvas);
    scrOptEditLine->pushSaveOrig->hide(); // hide as there is no original
    connect(scrOptEditLine->pushSaveNew, SIGNAL(clicked()), this, SLOT(slotCopyToNew()));
    connect(scrOptEditLine->pushAbort, SIGNAL(clicked()), this, SLOT(slotAbort()));
}

IMouseEditLine::IMouseEditLine(IGisLine &src, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , state(eStateIdle)
    , idxFocus(-1)
    , idxStart(-1)
    , idxStop(-1)
{
    src.getPolylineFromData(coords1);
    // calculate a pixel polyline from track coordinates
    line = coords1;
    gis->convertRad2Px(line);

    // create permanent line edit on screen options
    scrOptEditLine = new CScrOptEditLine(canvas);
    connect(scrOptEditLine->pushSaveOrig, SIGNAL(clicked()), this, SLOT(slotCopyToOrig()));
    connect(scrOptEditLine->pushSaveNew, SIGNAL(clicked()), this, SLOT(slotCopyToNew()));
    connect(scrOptEditLine->pushAbort, SIGNAL(clicked()), this, SLOT(slotAbort()));
}

IMouseEditLine::~IMouseEditLine()
{
    delete scrOptPoint;
    delete scrOptEditLine;
    delete scrOptRange;
}

void IMouseEditLine::drawLine(const QPolygonF &l, QPainter& p)
{
    p.setPen(QPen(Qt::magenta, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(l);
}

void IMouseEditLine::drawPointOfFocus(QPainter& p)
{
    if(idxFocus < 0)
    {
        return;
    }

    const QPointF& pt = line[idxFocus];

    p.setPen(Qt::darkBlue);
    p.setBrush(QColor(255,255,255,200));
    p.drawEllipse(pt, 6, 6);

    QPixmap bullet("://icons/8x8/bullet_magenta.png");
    p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
}

void IMouseEditLine::drawBullets(const QPolygonF &l, QPainter& p)
{
    QPixmap bullet("://icons/8x8/bullet_magenta.png");
    foreach(const QPointF &pt, l)
    {
        p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
    }
}

void IMouseEditLine::drawHighlight1(QPainter& p)
{
    if(idxStart < 0 || idxFocus < 0)
    {
        return;
    }

    int pos = idxFocus < idxStart ? idxFocus : idxStart;
    int len = qAbs(idxFocus - idxStart) + 1;

    QPolygonF highlight = line.mid(pos,len);

    p.setPen(QPen(Qt::red, 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(highlight);
}

void IMouseEditLine::drawHighlight2(QPainter& p)
{
    if(idxStart < 0 || idxStop < 0)
    {
        return;
    }

    int pos = idxStop < idxStart ? idxStop : idxStart;
    int len = qAbs(idxStop - idxStart) + 1;

    QPolygonF highlight = line.mid(pos,len);

    p.setPen(QPen(Qt::red, 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(highlight);
}

void IMouseEditLine::drawArrows(const QPolygonF &l, QPainter& p)
{
    QPointF arrow[4] =
    {
        QPointF( 24.0, 9.0),     //front
        QPointF( 0.0, 0.0),      //upper tail
        QPointF( 5.0, 9.0),      //mid tail
        QPointF( 0.0, 19.0)      //lower tail
    };

    QPointF pt, pt1, ptt;

    // draw direction arrows
    bool start = true;
    qreal heading;

    //generate arrow pic on-the-fly
    QImage arrow_pic(25,20, QImage::Format_ARGB32);
    arrow_pic.fill( qRgba(0,0,0,0));
    QPainter t_paint(&arrow_pic);
    USE_ANTI_ALIASING(t_paint, true);
    t_paint.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    t_paint.setBrush(Qt::magenta);
    t_paint.drawPolygon(arrow, 4);
    t_paint.end();

    foreach(pt,l)
    {
        if(start)                // no arrow on  the first loop
        {
            start = false;
        }
        else
        {
            if((qAbs(pt.x() - pt1.x()) + qAbs(pt.y() - pt1.y())) < 7)
            {
                pt1 = pt;
                continue;
            }
            // keep distance
            if((qAbs(pt.x() - ptt.x()) + qAbs(pt.y() - ptt.y())) > 100)
            {
                if(0 != pt.x() - pt1.x() && (pt.y() - pt1.y()))
                {
                    heading = ( qAtan2((qreal)(pt.y() - pt1.y()), (qreal)(pt.x() - pt1.x())) * 180.) / M_PI;

                    p.save();
                    // draw arrow between bullets
                    p.translate((pt.x() + pt1.x())/2,(pt.y() + pt1.y())/2);
                    p.rotate(heading);
                    p.drawImage(-13, -9, arrow_pic);
                    p.restore();
                    //remember last point
                    ptt = pt;
                }
            }
        }
        pt1 = pt;
    }
}

void IMouseEditLine::drawLeadLine(const QPolygonF &l, QPainter& p)
{
    p.setPen(QPen(QColor(255,255,0,100), 9, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(l);
}

void IMouseEditLine::draw(QPainter& p, bool needsRedraw, const QRect &rect)
{
    if(needsRedraw)
    {
        line = coords1;
        gis->convertRad2Px(line);
        newLine = newCoords;
        gis->convertRad2Px(newLine);
    }

    switch(state)
    {
    case eStateMoveMap:
    case eStateIdle:
    case eStatePointSelected:
        drawArrows(line, p);
        drawLine(line, p);
        drawBullets(line, p);
        drawPointOfFocus(p);
        break;

    case eStateSelectRange:
        drawArrows(line, p);
        drawLine(line, p);
        drawHighlight1(p);
        drawBullets(line, p);
        drawPointOfFocus(p);
        break;

    case eStateRangeSelected:
        drawArrows(line, p);
        drawLine(line, p);
        drawHighlight2(p);
        drawBullets(line, p);
        drawPointOfFocus(p);
        break;

    case eStateMovePoint:
        drawArrows(line, p);
        drawLine(line, p);
        drawBullets(line, p);
        drawPointOfFocus(p);
        break;

    case eStateAddPointFwd:
    {
        QPolygonF l;
        if(subLinePixel.isEmpty())
        {
            l = line.mid(0, idxStart) + newLine + line.mid(idxStop, -1);
        }
        else
        {
            l = line.mid(0, idxStart) + newLine.mid(0, newLine.size() - 2) + subLinePixel + line.mid(idxStop, -1);
        }
        drawLeadLine(leadLinePixel, p);
        drawArrows(l, p);
        drawLine(l, p);
        drawBullets(l, p);
        break;
    }

    case eStateAddPointBwd:
    {
        QPolygonF l;
        if(subLinePixel.isEmpty())
        {
            l = line.mid(0, idxStart + 1) + newLine + line.mid(idxStop + 1, -1);
        }
        else
        {
            l = line.mid(0, idxStart + 1) + subLinePixel + newLine.mid(2, -1) + line.mid(idxStop + 1, -1);
        }
        drawLeadLine(leadLinePixel, p);
        drawArrows(l, p);
        drawLine(l, p);
        drawBullets(l, p);
        break;
    }

    default:;
    }


    if(!scrOptPoint.isNull())
    {
        scrOptPoint->draw(p);
    }
    if(!scrOptRange.isNull())
    {
        scrOptRange->draw(p);
    }
}

void IMouseEditLine::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        switch(state)
        {
        case eStateSelectRange:
            cursor  = cursor1;
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(cursor);
        //break; no break fall thru

        case eStateRangeSelected:
            delete scrOptRange;

            state       = eStateIdle;
            idxFocus    = -1;
            idxStart    = -1;
            idxStop     = -1;
            break;

        case eStateMovePoint:
            state       = eStateIdle;
            idxFocus    = -1;
            coords1     = save;
            line        = coords1;
            gis->convertRad2Px(line);
            break;

        case eStateAddPointBwd:
        case eStateAddPointFwd:
            if(QMessageBox::question(canvas, tr("Add points?"), tr("Add points to temporary line?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
            {
                if(state == eStateAddPointBwd)
                {
                    // remove first point, which is the current moving point
                    newCoords.pop_front();
                    save = coords1.mid(0, idxStart + 1) + newCoords + coords1.mid(idxStop + 1, -1);
                }

                if(state == eStateAddPointFwd)
                {
                    // remove last point, which is the current moving point
                    newCoords.pop_back();
                    save = coords1.mid(0, idxStart) + newCoords + coords1.mid(idxStop, -1);
                }


                coords1 = save;
                line    = coords1;
                gis->convertRad2Px(line);
            }

            state       = eStateIdle;
            idxFocus    = -1;
            idxStart    = -1;
            idxStop     = -1;

            cursor  = cursor1;
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(cursor);
            break;

        default:
            delete scrOptPoint;
            delete scrOptRange;

            state       = eStateIdle;
            idxFocus    = -1;
            idxStart    = -1;
            idxStop     = -1;
        }
    }
    else if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
        case eStateIdle:
        {
            if(idxFocus >= 0)
            {
                scrOptPoint = new CScrOptPoint(line[idxFocus], canvas);

                // set icon for add points towards start (backward/eStateAddPointBwd)
                if(idxFocus == 0)
                {
                    // point is first of line
                    qreal a1, a2;
                    if(coords1.size() > 1)
                    {
                        const QPointF& pt1 = coords1[idxFocus];
                        const QPointF& pt2 = coords1[idxFocus + 1];
                        GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y(), a1, a2);
                    }
                    else
                    {
                        // only one point in the line
                        a1 = 90;
                    }

                    QPixmap pix("://icons/32x32/ToTop.png");
                    QTransform trans;
                    trans.rotate(a1 + 180);

                    pix = pix.transformed(trans, Qt::SmoothTransformation);

                    scrOptPoint->toolAdd1->setIcon(pix);
                }
                else
                {
                    // point in the middle of the line
                    qreal a1, a2;
                    const QPointF& pt1 = coords1[idxFocus];
                    const QPointF& pt2 = coords1[idxFocus - 1];
                    GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y(), a1, a2);

                    QPixmap pix("://icons/32x32/Up.png");
                    QTransform trans;
                    trans.rotate(a1);

                    pix = pix.transformed(trans, Qt::SmoothTransformation);

                    scrOptPoint->toolAdd1->setIcon(pix);
                }

                // set icon for add points towards end (forward/eStateAddPointFwd)
                if(idxFocus == (line.size() - 1))
                {
                    // point is last of line
                    qreal a1, a2;
                    if(coords1.size() > 1)
                    {
                        const QPointF& pt1 = coords1[idxFocus];
                        const QPointF& pt2 = coords1[idxFocus - 1];
                        GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y(), a1, a2);
                    }
                    else
                    {
                        // only one point in the line
                        a1 = -90;
                    }

                    QPixmap pix("://icons/32x32/ToTop.png");
                    QTransform trans;
                    trans.rotate(a1 + 180);

                    pix = pix.transformed(trans, Qt::SmoothTransformation);

                    scrOptPoint->toolAdd2->setIcon(pix);
                }
                else
                {
                    // point in the middle of the line
                    qreal a1, a2;
                    const QPointF& pt1 = coords1[idxFocus];
                    const QPointF& pt2 = coords1[idxFocus + 1];
                    GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y(), a1, a2);

                    QPixmap pix("://icons/32x32/Up.png");
                    QTransform trans;
                    trans.rotate(a1);

                    pix = pix.transformed(trans, Qt::SmoothTransformation);

                    scrOptPoint->toolAdd2->setIcon(pix);
                }

                connect(scrOptPoint->toolDelete, SIGNAL(clicked()), this, SLOT(slotDeletePoint()));
                connect(scrOptPoint->toolSelectRange, SIGNAL(clicked()), this, SLOT(slotSelectRange()));
                connect(scrOptPoint->toolMove, SIGNAL(clicked()), this, SLOT(slotMovePoint()));
                connect(scrOptPoint->toolAdd1, SIGNAL(clicked()), this, SLOT(slotAddPoint1()));
                connect(scrOptPoint->toolAdd2, SIGNAL(clicked()), this, SLOT(slotAddPoint2()));

                state = eStatePointSelected;
            }
            else
            {
                state = eStateMoveMap;
            }
            break;
        }

        case eStateSelectRange:
        {
            state   = eStateRangeSelected;
            idxStop = idxFocus;

            scrOptRange = new CScrOptRange(line[idxStop], canvas);
            connect(scrOptRange->toolDelete, SIGNAL(clicked()), this, SLOT(slotDeleteRange()));

            cursor  = cursor1;
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(cursor);
            break;
        }

        case eStateAddPointFwd:
        {
            if(!subLineCoord.isEmpty())
            {
                newLine.pop_back();
                newLine.pop_back();
                newLine     = newLine + subLinePixel;
                newCoords.pop_back();
                newCoords.pop_back();
                newCoords   = newCoords + subLineCoord;

                subLineCoord.clear();
                subLinePixel.clear();
            }
            newLine.append(newLine.last());
            newCoords.append(newCoords.last());
            idxFocus = newLine.size() - 1;
            break;
        }

        case eStateAddPointBwd:
        {
            if(!subLineCoord.isEmpty())
            {
                newLine.pop_front();
                newLine.pop_front();
                newLine = subLinePixel + newLine;
                newCoords.pop_front();
                newCoords.pop_front();
                newCoords = subLineCoord + newCoords;

                subLineCoord.clear();
                subLinePixel.clear();
            }

            newLine.prepend(newLine.first());
            newCoords.prepend(newCoords.first());
            idxFocus = 0;
            break;
        }

        default:
        {
            delete scrOptPoint;
            delete scrOptRange;

            state       = eStateIdle;
            idxFocus    = -1;
            idxStart    = -1;
            idxStop     = -1;
        }
        }
    }
    canvas->update();
}

void IMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();

    switch(state)
    {
    case eStateIdle:
    {
        int old = idxFocus;
        idxFocus = getPointCloseBy(point);
        if(old != idxFocus)
        {
            canvas->update();
        }
        break;
    }

    case eStateSelectRange:
    {
        if(!scrOptEditLine->rect().contains(point))
        {
            panCanvas(point);
        }

        int old = idxFocus;
        idxFocus = getPointCloseBy(point);

        if(idxFocus < 0)
        {
            idxFocus = old;
        }

        if(old != idxFocus)
        {
            canvas->update();
        }
        break;
    }

    case eStateMovePoint:
    {
        panCanvas(point);

        QPointF pt      = point;
        line[idxFocus]  = pt;
        gis->convertPx2Rad(pt);
        coords1[idxFocus]  = pt;

        canvas->update();
        break;
    }

    case eStateAddPointBwd:
    case eStateAddPointFwd:
    {
        panCanvas(point);

        QPointF pt          = point;
        newLine[idxFocus]   = pt;
        gis->convertPx2Rad(pt);
        newCoords[idxFocus] = pt;


        leadLineCoord.clear();
        leadLinePixel.clear();
        subLinePixel.clear();
        subLineCoord.clear();


        // find polyline to snap
        QPointF px1;
        QPointF px2;
        if(newLine.size() > 1)
        {
            px1 = state == eStateAddPointFwd ? newLine[idxFocus - 1] : newLine[1];
            px2 = newLine[idxFocus];
        }

        if(canvas->findPolylineCloseBy(px2, px2, 10, leadLineCoord))
        {
            leadLinePixel = leadLineCoord;
            gis->convertRad2Px(leadLinePixel);

            segment_t result;
            GPS_Math_SubPolyline(px1, px2, 10, leadLinePixel, result);
            result.apply(leadLineCoord, leadLinePixel, subLineCoord, subLinePixel, gis);

            if(state == eStateAddPointBwd)
            {
                QPolygonF tmp1;
                QPolygonF tmp2;
                for(int i = 0; i < subLineCoord.size(); i++)
                {
                    tmp1.push_front(subLineCoord[i]);
                    tmp2.push_front(subLinePixel[i]);
                }
                subLineCoord = tmp1;
                subLinePixel = tmp2;
            }
        }


        canvas->update();
        break;
    }

    case eStateMoveMap:
    {
        if(point != lastPoint)
        {
            QPoint delta = point - lastPoint;
            canvas->moveMap(delta);
        }
        break;
    }

    default:;
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


int IMouseEditLine::getPointCloseBy(const QPoint& screenPos)
{
    qint32 i    = 0;
    qint32 idx  = -1;
    qint32 d   = NOINT;
    foreach(const QPointF &point, line)
    {
        int tmp = (screenPos - point).manhattanLength();
        if(tmp < d)
        {
            idx = i;
            d   = tmp;
        }
        i++;
    }

    if(d > 40)
    {
        idx = -1;
    }

    return idx;
}

void IMouseEditLine::slotDeletePoint()
{
    if(idxFocus < 0)
    {
        return;
    }
    scrOptPoint->deleteLater();

    coords1.remove(idxFocus);
    line.remove(idxFocus);

    idxFocus  = -1;
    state       = eStateIdle;

    canvas->update();
}

void IMouseEditLine::slotSelectRange()
{
    if(idxFocus < 0)
    {
        return;
    }
    scrOptPoint->deleteLater();

    idxStart    = idxFocus;
    state       = eStateSelectRange;

    cursor  = QCursor(QPixmap(":/cursors/cursorSelectRange.png"),0,0);
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(cursor);
}

void IMouseEditLine::slotDeleteRange()
{
    if(idxStart < 0 || idxStop < 0)
    {
        return;
    }
    scrOptRange->deleteLater();

    int len = qAbs(idxStop - idxStart);
    int idx = idxStart < idxStop ? idxStart : idxStop;

    coords1.remove(idx+1,len-1);
    line = coords1;
    gis->convertRad2Px(line);

    state = eStateIdle;
    idxFocus  = -1;
    idxStart    = -1;
    idxStop     = -1;

    canvas->update();
}

void IMouseEditLine::slotMovePoint()
{
    if(idxFocus < 0)
    {
        return;
    }
    scrOptPoint->deleteLater();

    save    = coords1;
    state   = eStateMovePoint;

    canvas->update();
}

void IMouseEditLine::slotAddPoint1()
{
    if(idxFocus < 0)
    {
        return;
    }
    scrOptPoint->deleteLater();

    newCoords.clear();
    newLine.clear();
    /*
        Adding points will work completely on newLine and newCoords until the
        line segment is taken over or dumped. The current point of focus is
        added twice. The first time as fixed starting point of the tmeporary
        newLine. And the second time as initial point to be moved by the mouse.

     */
    newCoords << coords1[idxFocus]  << coords1[idxFocus];
    newLine << line[idxFocus]     << line[idxFocus];

    // mark gap to insert points
    idxStart = idxFocus - 1;
    idxStop  = idxFocus;

    // set focus to first point in newLine
    idxFocus = 0;

    state   = eStateAddPointBwd;
    cursor  = QCursor(QPixmap(":/cursors/cursorAdd.png"),0,0);
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(cursor);

    canvas->update();
}

void IMouseEditLine::slotAddPoint2()
{
    if(idxFocus < 0)
    {
        return;
    }

    if(!scrOptPoint.isNull())
    {
        scrOptPoint->deleteLater();
    }


    newCoords.clear();
    newLine.clear();
    /*
        Adding points will work completely on newLine and newCoords until the
        line segment is taken over or dumped. The current point of focus is
        added twice. The first time as fixed starting point of the tmeporary
        newLine. And the second time as initial point to be moved by the mouse.

     */
    newCoords << coords1[idxFocus]  << coords1[idxFocus];
    newLine << line[idxFocus]     << line[idxFocus];

    // mark gap to insert points
    idxStart = idxFocus;
    idxStop  = idxFocus + 1;

    // set focus to first point in newLine
    idxFocus = 1;

    state   = eStateAddPointFwd;
    cursor  = QCursor(QPixmap(":/cursors/cursorAdd.png"),0,0);
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(cursor);

    canvas->update();
}


void IMouseEditLine::slotAbort()
{
    canvas->resetMouse();
    canvas->update();
}

void IMouseEditLine::slotCopyToOrig()
{
    if(coords1.size() < 2)
    {
        return;
    }

    int res = QMessageBox::warning(canvas, tr("Warning!"), tr("This will replace all data of the orignal by a simple line of coordinates. All other data will be lost permanently."), QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);

    if(res != QMessageBox::Ok)
    {
        return;
    }

    IGisLine * l = getGisLine();
    if(l != 0)
    {
        l->setDataFromPolyline(coords1);
    }
    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}


