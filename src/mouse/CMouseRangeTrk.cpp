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

#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/trk/CGisItemTrk.h"
#include "mouse/CMouseRangeTrk.h"
#include "mouse/CScrOptRangeTrk.h"

#include <QtWidgets>


CMouseRangeTrk::CMouseRangeTrk(CGisItemTrk &trk, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
{
    cursor      = QCursor(QPixmap("://cursors/cursorSelectRange.png"),0,0);
    key         = trk.getKey();

    // switch to full mode to show deleted (hidden) track points, too
    trk.setMode(CGisItemTrk::eModeRange, "CMouseRangeTrk");

    resetState();
}

CMouseRangeTrk::~CMouseRangeTrk()
{
    canvas->reportStatus(key.item, "");

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk)
    {
        trk->setMode(CGisItemTrk::eModeNormal, "CMouseRangeTrk");
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    delete scrOptRange;
}

void CMouseRangeTrk::draw(QPainter& p,  CCanvas::redraw_e, const QRect &)
{
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk)
    {
        trk->drawRange(p);
        if(!mapDidMove && (anchor != NOPOINTF))
        {
            p.setPen(Qt::darkBlue);
            p.setBrush(QColor(255,255,255,200));
            p.drawEllipse(anchor, 6, 6);

            QPixmap bullet("://icons/8x8/bullet_magenta.png");
            p.drawPixmap(anchor.x() - 3, anchor.y() - 3, bullet);
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
    point = e->pos();
    if(e->button() == Qt::RightButton)
    {
        canvas->resetMouse();
        canvas->update();
    }
    else if(e->button() == Qt::LeftButton)
    {
        mapMove     = true;
        firstPoint  = point;
    }
}

void CMouseRangeTrk::mouseMoveEvent(QMouseEvent * e)
{
    point = e->pos();

    if(mapMove)
    {
        if((point - firstPoint).manhattanLength() >= 4)
        {
            QPoint delta = point - lastPoint;
            canvas->moveMap(delta);
            mapDidMove = true;
        }
    }
    else
    {
        switch(state)
        {
        case eStateIdle:
        {
            CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
            if(trk != nullptr)
            {
                anchor = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseMove, "CMouseRangeTrk");
                canvas->update();
            }
            break;
        }

        case eStateSelectRange:
        {
            CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
            if(trk != nullptr)
            {
                anchor = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseMove, "CMouseRangeTrk");
                canvas->update();
            }
            break;
        }

        default:
            ;
        }
    }
    lastPoint = point;
}

void CMouseRangeTrk::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(!mapDidMove)
        {
            switch(state)
            {
            case eStateIdle:
            {
                CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
                if(trk != nullptr && anchor != NOPOINTF)
                {
                    anchor = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseClick, "CMouseRangeTrk");
                    state = eStateSelectRange;
                    canvas->update();
                }
                break;
            }


            case eStateSelectRange:
            {
                CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
                if(trk != nullptr && anchor != NOPOINTF)
                {
                    QPointF pt = trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseClick, "CMouseRangeTrk");
                    scrOptRange = new CScrOptRangeTrk(pt, trk, this);
                    connect(scrOptRange->toolHidePoints, &QToolButton::clicked, this, &CMouseRangeTrk::slotHidePoints);
                    connect(scrOptRange->toolShowPoints, &QToolButton::clicked, this, &CMouseRangeTrk::slotShowPoints);
                    connect(scrOptRange.data(), &CScrOptRangeTrk::activitySelected, this, &CMouseRangeTrk::slotActivity);
                    connect(scrOptRange->toolCopy,       &QToolButton::clicked, this, &CMouseRangeTrk::slotCopy);

                    state = eStateRangeSelected;
                    canvas->update();
                }

                break;
            }

            case eStateRangeSelected:
            {
                resetState();
                break;
            }
            }
        }
        mapDidMove = false;
        mapMove  = false;
    }
}

void CMouseRangeTrk::wheelEvent(QWheelEvent * e)
{
    if(state == eStateRangeSelected)
    {
        resetState();
    }
}

void CMouseRangeTrk::keyPressEvent(QKeyEvent * e)
{
    if(state == eStateRangeSelected)
    {
        resetState();
    }
}

void CMouseRangeTrk::resetState()
{
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != nullptr)
    {
        trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove, "CMouseRangeTrk");
        trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseClick, "CMouseRangeTrk");
    }

    if(!scrOptRange.isNull())
    {
        scrOptRange->deleteLater();
    }
    state  = eStateIdle;
    anchor = NOPOINTF;

    canvas->reportStatus(key.item, tr("<b>Select Range</b><br/>Select first track point with left mouse button. And then a second one. Leave range selection with a click of the right mouse button.<br/>"));
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseRangeTrk::slotHidePoints()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != nullptr)
    {
        trk->hideSelectedPoints();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    resetState();
}

void CMouseRangeTrk::slotShowPoints()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != nullptr)
    {
        trk->showSelectedPoints();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    resetState();
}

void CMouseRangeTrk::slotActivity(quint32 flags)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(nullptr != trk)
    {
        trk->setActivityRange(flags);
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    scrOptRange->deleteLater();
    canvas->resetMouse();
}

void CMouseRangeTrk::slotCopy()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != nullptr)
    {
        trk->copySelectedPoints();
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }
    scrOptRange->deleteLater();
    canvas->resetMouse();
}
