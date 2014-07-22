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

#include "mouse/CMouseNormal.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWidget.h"
#include "gis/IGisItem.h"
#include "gis/CGisWidget.h"

#include <QtGui>

CMouseNormal::CMouseNormal(CCanvas *canvas)
    : IMouse(canvas)
    , mapMove(false)
{
    cursor = QCursor(QPixmap(":/cursors/cursorMoveMap.png"),0,0);
}

CMouseNormal::~CMouseNormal()
{

}

void CMouseNormal::mousePressEvent(QMouseEvent * e)
{
    point = e->pos();
    if(e->button() == Qt::LeftButton)
    {
        lastPos     = e->pos();
        mapMove     = true;
        mapDidMove  = false;
    }
}

void CMouseNormal::mouseMoveEvent(QMouseEvent * e)
{
    point = e->pos();
    const QPoint pos = e->pos();

    if(mapMove)
    {
        if(pos != lastPos)
        {
            QPoint delta = pos - lastPos;
            canvas->moveMap(delta);
            lastPos     = pos;
            mapDidMove  = true;
        }
    }
    else
    {
        canvas->displayInfo(pos);
        canvas->update();
    }
}

void CMouseNormal::mouseReleaseEvent(QMouseEvent *e)
{
    point = e->pos();
    if(e->button() == Qt::LeftButton)
    {      
        if(!mapDidMove)
        {
            QList<IGisItem*> items;
            CGisWidget::self().getItemByPos(point, items);

            if(items.size() == 1)
            {
                IGisItem * item = items.first();
                item->treeWidget()->collapseAll();
                item->treeWidget()->setCurrentItem(item);
                item->treeWidget()->scrollToItem(item, QAbstractItemView::PositionAtCenter);
                item->gainUserFocus();
                canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
            }

        }

        mapMove     = false;
        mapDidMove  = false;
    }
}

void CMouseNormal::draw(QPainter& p, const QRect &rect)
{
    QList<IGisItem*> items;
    CGisWidget::self().getItemByPos(point, items);

    if(items.size() > 8)
    {
        return;
    }

    foreach(IGisItem * item, items)
    {
        item->drawHighlight(p);
    }
}
