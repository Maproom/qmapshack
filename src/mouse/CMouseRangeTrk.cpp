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

#include "mouse/CMouseRangeTrk.h"
#include "mouse/CScrOptRangeTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>


CMouseRangeTrk::CMouseRangeTrk(CGisItemTrk &trk, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , state(eStateIdle)
    , anchor(NOPOINTF)
{
    cursor      = QCursor(QPixmap("://cursors/cursorSelectRange.png"),0,0);
    key         = trk.getKey();

    // reset user focus if the track has it
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove);
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseClick);
    if(trk.hasUserFocus())
    {
        trk.gainUserFocus(false);
    }

    // switch to full mode to show deleted (hidden) track points, too
    trk.setDrawMode(CGisItemTrk::eDrawRange);

    canvas->reportStatus(key.item, tr("<b>Select Range</b><br/>Select first track point. And then a second one.<br/>"));
    /*
        trigger complete update of GIS components to make sure all changes to
        the originating object are reflected on the canvas
    */
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

CMouseRangeTrk::~CMouseRangeTrk()
{
    canvas->reportStatus(key.item, "");

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk)
    {
        trk->setDrawMode(CGisItemTrk::eDrawNormal);
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    delete scrOptRange;
}

void CMouseRangeTrk::draw(QPainter& p,  bool needsRedraw, const QRect &rect)
{
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk)
    {
        switch(state)
        {
            case eStateIdle:
            case eStateMoveMap:
                if(anchor != NOPOINTF)
                {
                    p.setPen(Qt::darkBlue);
                    p.setBrush(QColor(255,255,255,200));
                    p.drawEllipse(anchor, 6, 6);

                    QPixmap bullet("://icons/8x8/bullet_magenta.png");
                    p.drawPixmap(anchor.x() - 3, anchor.y() - 3, bullet);
                }
                break;

            case eStateSelectRange:
            case eStateRangeSelected:
                trk->drawRange(p);
                if(anchor != NOPOINTF)
                {
                    p.setPen(Qt::darkBlue);
                    p.setBrush(QColor(255,255,255,200));
                    p.drawEllipse(anchor, 6, 6);

                    QPixmap bullet("://icons/8x8/bullet_magenta.png");
                    p.drawPixmap(anchor.x() - 3, anchor.y() - 3, bullet);
                }

                break;
        }
    }

    if(!scrOptRange.isNull())
    {
        scrOptRange->draw(p);
    }
}

void CMouseRangeTrk::mousePressEvent(QMouseEvent * e)
{
    canvas->reportStatus(key.item, "");
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        canvas->resetMouse();
        canvas->update();
    }
    else if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
            case eStateIdle:
            {
                CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
                if(trk != 0 && anchor != NOPOINTF)
                {
                    anchor = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseClick);
                    state = eStateSelectRange;
                    canvas->update();
                }
                else
                {
                    state = eStateMoveMap;
                }
                break;
            }

            case eStateSelectRange:
            {
                CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
                if(trk != 0 && anchor != NOPOINTF)
                {
                    QPointF pt = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseMove);
                    scrOptRange = new CScrOptRangeTrk(pt, trk, canvas);
                    connect(scrOptRange->toolHidePoints, SIGNAL(clicked()), this, SLOT(slotHidePoints()));
                    connect(scrOptRange->toolShowPoints, SIGNAL(clicked()), this, SLOT(slotShowPoints()));
                    connect(scrOptRange->toolCopy, SIGNAL(clicked()), this, SLOT(slotCopy()));

                    state = eStateRangeSelected;
                    canvas->update();
                }

                break;
            }
            case eStateRangeSelected:
            {
                scrOptRange->deleteLater();
                CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
                if(trk != 0)
                {
                    trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove);
                    trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseClick);
                }
                state = eStateIdle;
                canvas->update();
                break;
            }
            default:;
        }

    }
}

void CMouseRangeTrk::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();

    switch(state)
    {
        case eStateIdle:
        {
            CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
            if(trk != 0)
            {
                anchor = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseClick);
                canvas->update();
            }
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

        case eStateSelectRange:
        {

            CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
            if(trk != 0)
            {
                anchor = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseMove);
                canvas->update();
            }
            panCanvas(point);
            break;
        }
        default:;
    }

    lastPoint = point;
}

void CMouseRangeTrk::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(state == eStateMoveMap)
        {
            state = eStateIdle;
        }
    }
}

void CMouseRangeTrk::wheelEvent(QWheelEvent * e)
{

}

void CMouseRangeTrk::slotHidePoints()
{
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != 0)
    {
        trk->hideSelectedPoints();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    scrOptRange->deleteLater();
    canvas->resetMouse();
}

void CMouseRangeTrk::slotShowPoints()
{
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != 0)
    {
        trk->showSelectedPoints();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    scrOptRange->deleteLater();
    canvas->resetMouse();
}

void CMouseRangeTrk::slotCopy()
{
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != 0)
    {
        trk->copySelectedPoints();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
    scrOptRange->deleteLater();
    canvas->resetMouse();
}
