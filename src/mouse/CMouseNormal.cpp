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
#include "mouse/CScrOptUnclutter.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWidget.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/CGisWidget.h"
#include "gis/CGisDraw.h"
#include "gis/CGisProject.h"

#include <QtWidgets>

CMouseNormal::CMouseNormal(CGisDraw *gis, CCanvas *canvas)
    : IMouse(gis, canvas)
    , mapMove(false)
    , stateItemSel(eStateIdle)
{
    cursor = QCursor(QPixmap(":/cursors/cursorMoveMap.png"),0,0);
    screenUnclutter = new CScrOptUnclutter(canvas);

    menu = new QMenu(canvas);
    menu->addAction(QIcon("://icons/32x32/AddWpt.png"), tr("Add Waypoint"), this, SLOT(slotAddWpt()));
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
        // has triggered a selection of any kind
        mapMove     = (stateItemSel < eStateNoMapMovePossible);
        mapDidMove  = false;
    }
    else if(e->button() == Qt::RightButton)
    {
        QPoint p = canvas->mapToGlobal(point);
        menu->exec(p);

    }
}

void CMouseNormal::mouseMoveEvent(QMouseEvent * e)
{
    screenUnclutter->mouseMoveEvent(e);
    if(!screenItemOption.isNull())
    {
        screenItemOption->mouseMoveEvent(e);
    }

    point = e->pos();    

    if(mapMove)
    {
        if(point != lastPos)
        {
            QPoint delta = point - lastPos;
            canvas->moveMap(delta);
            lastPos     = point;
            mapDidMove  = true;
        }
    }
    else
    {
        canvas->displayInfo(point);
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
            switch(stateItemSel)
            {
                case eStateHooverSingle:
                {
                    stateItemSel = eStateIdle;

                    IGisItem * item = CGisWidget::self().getItemByKey(screenUnclutter->getItemKey());
                    if(item)
                    {
                        item->treeWidget()->collapseAll();
                        item->treeWidget()->setCurrentItem(item);
                        item->treeWidget()->scrollToItem(item, QAbstractItemView::PositionAtCenter);

                        delete screenItemOption;
                        screenItemOption = item->getScreenOptions(point, this);

                        if(!screenItemOption.isNull())
                        {
                            stateItemSel = eStateShowItemOptions;

                        }
                    }
                    break;
                }
                case eStateHooverMultiple:
                {
                    screenUnclutter->setOrigin(e->pos());
                    stateItemSel = eStateUnclutterMultiple;
                    break;
                }
                case eStateUnclutterMultiple:
                {
                    const CScrOptUnclutter::item_t * scrOpt = screenUnclutter->selectItem(point);
                    if(scrOpt != 0)
                    {
                        QString key = scrOpt->key;
                        screenUnclutter->clear();

                        IGisItem * item = CGisWidget::self().getItemByKey(key);
                        if(item)
                        {
                            item->treeWidget()->collapseAll();
                            item->treeWidget()->setCurrentItem(item);
                            item->treeWidget()->scrollToItem(item, QAbstractItemView::PositionAtCenter);

                            delete screenItemOption;
                            screenItemOption = item->getScreenOptions(screenUnclutter->getOrigin(), this);

                            if(!screenItemOption.isNull())
                            {
                                stateItemSel = eStateShowItemOptions;
                                break;
                            }
                        }
                    }
                    screenUnclutter->clear();
                    stateItemSel = eStateIdle;
                    break;
                }
                case eStateShowItemOptions:
                {
                    delete screenItemOption;
                    stateItemSel = eStateIdle;
                    break;
                }
                default:;
            }
            canvas->update();
        }
        mapMove     = false;
        mapDidMove  = false;
    }
}

void CMouseNormal::wheelEvent(QWheelEvent * e)
{
    screenUnclutter->clear();
    delete screenItemOption;
    stateItemSel = eStateIdle;
}


void CMouseNormal::draw(QPainter& p, const QRect &rect)
{
    // no mouse interaction while gis thread is running
    if(gis->isRunning())
    {
        return;
    }

    switch(stateItemSel)
    {
        case eStateIdle:
        case eStateHooverSingle:
        case eStateHooverMultiple:
        {
            screenUnclutter->clear();

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
                screenUnclutter->addItem(item);
            }

            stateItemSel = (screenUnclutter->size() == 1) ? eStateHooverSingle : eStateHooverMultiple;
            break;
        }
        case eStateUnclutterMultiple:
        {
            screenUnclutter->draw(p);
            break;
        }
        case eStateShowItemOptions:
        {
            if(screenItemOption.isNull())
            {
                stateItemSel = eStateIdle;
                break;
            }

            screenItemOption->draw(p);
            break;
        }
        default:;
    }
}


void CMouseNormal::slotAddWpt()
{

    QPointF pt = point;
    gis->convertPx2Rad(pt);

    QString name = CGisItemWpt::getNewName();
    if(name.isEmpty())
    {
        return;
    }

    QString icon = CGisItemWpt::getNewIcon();
    if(icon.isEmpty())
    {
        return;
    }

    CGisProject * project = CGisWidget::self().selectProject();
    if(project == 0)
    {
        return;
    }

    new CGisItemWpt(pt*RAD_TO_DEG, name, icon, project);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);

}
