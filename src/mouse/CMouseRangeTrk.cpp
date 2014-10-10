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
{
    cursor  = QCursor(QPixmap("://cursors/cursorSelectRange.png"),0,0);
    key = trk.getKey();


    // reset user focus if the track has it
    if(trk.hasUserFocus())
    {
        trk.gainUserFocus(false);
    }

    // switch to full mode to show deleted (hidden) track points, too
    trk.setDrawMode(CGisItemTrk::eDrawRange);

    /*
        trigger complete update of GIS components to make sure all changes to
        the originating object are reflected on the canvas
    */
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

CMouseRangeTrk::~CMouseRangeTrk()
{
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
        trk->drawRange(p);
    }

    if(!scrOptRange.isNull())
    {
        scrOptRange->draw(p);
    }
}

void CMouseRangeTrk::mousePressEvent(QMouseEvent * e)
{
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
                if(trk != 0)
                {
                    QPointF pt = trk->getPointCloseBy(point);
                    scrOptRange = new CScrOptRangeTrk(pt, canvas);
                    state = eStateRangeSelected;
                    canvas->update();
                }
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
                trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseMove);
                canvas->update();
            }
            panCanvas(point);
            break;
        }
        default:;
    }
}

void CMouseRangeTrk::mouseReleaseEvent(QMouseEvent *e)
{

}

void CMouseRangeTrk::wheelEvent(QWheelEvent * e)
{

}
