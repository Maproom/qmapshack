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
#include "CMainWindow.h"

#include <QtGui>

CMouseNormal::CMouseNormal(CCanvas *canvas)
    : IMouse(canvas)
    , mapMove(false)
    , stateItemSel(eStateIdle)
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
        // start to block map moving when a previous click
        // has triggered a selection of nay kind
        mapMove     = (stateItemSel < eStateNoMapMovePossible);
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
            /*
                items | ring of items | action
                ------|---------------|------------
                   0  |     no        | -
                   1  |     no        | highight in list and start to show options
                  >1  |     no        | start to show ring of items
                  >1  |     yes       | highight in list and start to show options
            */


            switch(stateItemSel)
            {
                case eStateHooverSingle:
                {
                    stateItemSel = eStateIdle;

                    IGisItem * item = CGisWidget::self().getItemByKey(screenItems.first().key);
                    if(item)
                    {
                        item->treeWidget()->collapseAll();
                        item->treeWidget()->setCurrentItem(item);
                        item->treeWidget()->scrollToItem(item, QAbstractItemView::PositionAtCenter);
                        item->gainUserFocus();
                        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);

                        stateItemSel = eStateShowOptions;
                    }
                    canvas->update();
                    break;
                }
                case eStateHooverMultiple:
                {
                    stateItemSel = eStateUnclutterMultiple;
                    canvas->update();
                    break;
                }
                case eStateUnclutterMultiple:
                {
                    stateItemSel = eStateIdle;

                    foreach(screen_item_t screenItem, screenItems)
                    {
                        if(screenItem.area.contains(point.toPoint()))
                        {
                            screenItems.clear();
                            screenItems << screenItem;

                            stateItemSel = eStateShowOptions;
                            break;
                        }
                    }
                    canvas->update();
                    break;
                }
                case eStateShowOptions:
                {
                    // todo: check click against list of rectangles
                    stateItemSel = eStateIdle;
                    break;
                }
                default:;
            }
        }
        mapMove     = false;
        mapDidMove  = false;
    }
}

const QPoint positions[] =
{
      QPoint(-50,-23)
    , QPoint( 50,-23)
    , QPoint(-50, 23)
    , QPoint( 50, 23)
    , QPoint(-22,-55)
    , QPoint( 22,-55)
    , QPoint(-22, 55)
    , QPoint( 22, 55)

};

void CMouseNormal::draw(QPainter& p, const QRect &rect)
{

    switch(stateItemSel)
    {
        case eStateIdle:
        case eStateHooverSingle:
        case eStateHooverMultiple:
        {
            screenItems.clear();

            QList<IGisItem*> items;
            CGisWidget::self().getItemsByPos(point, items);

            if(items.empty() || items.size() > 8)
            {
                stateItemSel = eStateIdle;
                break;
            }

            foreach(IGisItem * item, items)
            {
                item->drawHighlight(p);

                screen_item_t screenItem;
                screenItem.name = item->getName();
                screenItem.key  = item->getKey();
                screenItem.icon = item->getIcon();
                screenItem.area = screenItem.icon.rect();

                screenItems << screenItem;
            }

            stateItemSel = (screenItems.size() == 1) ? eStateHooverSingle : eStateHooverMultiple;
            break;
        }
        case eStateUnclutterMultiple:
        {
            QFontMetrics fm(CMainWindow::self().getMapFont());
            for(int cnt = 0; cnt < screenItems.size(); cnt++)
            {
                screen_item_t& screenItem = screenItems[cnt];

                if(screenItem.text.isNull())
                {

                    screenItem.area.moveCenter(point.toPoint() + positions[cnt]);
                    screenItem.text = fm.boundingRect(screenItem.name);
                    if(cnt & 0x01)
                    {
                        screenItem.text.moveTopLeft(screenItem.area.topRight() + QPoint( 14, fm.height()/2));
                    }
                    else
                    {
                        screenItem.text.moveTopRight(screenItem.area.topLeft() + QPoint(-14, fm.height()/2));
                    }
                    screenItem.text.adjust(-2, -2, 2, 2);
                }
            }

            foreach(const screen_item_t& screenItem, screenItems)
            {

                p.setPen(Qt::NoPen);
                p.setBrush(QColor(255,255,255,255));
                p.drawEllipse(screenItem.area.center(), 20,20);
                p.drawRoundedRect(screenItem.text, 3, 3);

                p.setPen(QPen(Qt::lightGray,2));
                p.setBrush(Qt::NoBrush);
                p.drawEllipse(screenItem.area.center(), 18,18);

                p.drawPixmap(screenItem.area, screenItem.icon);
                CCanvas::drawText(screenItem.name, p, screenItem.text, Qt::darkBlue);
            }
            break;
        }
        case eStateShowOptions:
        {
            IGisItem * item = CGisWidget::self().getItemByKey(screenItems.first().key);
            if(item == 0)
            {
                break;
            }
            item->drawHighlight(p);
            // todo: draw item otions
            break;
        }
        default:;
    }
}
