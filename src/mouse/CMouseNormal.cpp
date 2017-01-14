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
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "mouse/CMouseNormal.h"
#include "mouse/CScrOptUnclutter.h"
#include "widgets/CFadingIcon.h"

#include <QtWidgets>

CMouseNormal::CMouseNormal(CGisDraw *gis, CCanvas *canvas)
    : IMouse(gis, canvas)
{
    cursor = QCursor(QPixmap(":/cursors/cursorMoveMap.png"),0,0);
    screenUnclutter = new CScrOptUnclutter(this);

    menu = new QMenu(canvas);
    menu->addAction(QIcon("://icons/32x32/AddWpt.png"),  tr("Add Waypoint"), this, SLOT(slotAddWpt()));
    menu->addAction(QIcon("://icons/32x32/AddTrk.png"),  tr("Add Track"),    this, SLOT(slotAddTrk()));
    menu->addAction(QIcon("://icons/32x32/AddRte.png"),  tr("Add Route"),    this, SLOT(slotAddRte()));
    menu->addAction(QIcon("://icons/32x32/AddArea.png"), tr("Add Area"),     this, SLOT(slotAddArea()));
    menu->addSeparator();
    menu->addAction(QIcon("://icons/32x32/SelectArea.png"), tr("Select Items On Map"), this, SLOT(slotSelectArea()));
    menu->addSeparator();
    menu->addAction(QIcon("://icons/32x32/Copy.png"), tr("Copy position"), this, SLOT(slotCopyPosition()));
    menu->addAction(QIcon("://icons/32x32/Copy.png"), tr("Copy position (Grid)"), this, SLOT(slotCopyPositionGrid()));
}

CMouseNormal::~CMouseNormal()
{
}

void CMouseNormal::stopTracking() const
{
    const IGisItem::key_t& key = CGisItemTrk::getKeyUserFocus();
    if(!key.item.isEmpty())
    {
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
        if(trk != nullptr)
        {
            trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove, "CMouseNormal");
        }
    }
}

void CMouseNormal::mousePressEvent(QMouseEvent * e)
{
    point = e->pos();

    if(e->button() == Qt::LeftButton)
    {
        lastPos     = e->pos();
        firstPos    = lastPos;
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
        if((point - firstPos).manhattanLength() >= 4)
        {
            QPoint delta = point - lastPos;
            canvas->moveMap(delta);
            lastPos    = point;
            mapDidMove = true;
        }
    }
    else
    {
        switch(stateItemSel)
        {
        case eStateIdle:
            CGisWidget::self().mouseMove(point);

        //break; skip break intentionally
        case eStateHooverSingle:
        case eStateHooverMultiple:
        {
            const IGisItem::key_t& keyTrk = CGisItemTrk::getKeyUserFocus();
            if(!keyTrk.item.isEmpty())
            {
                CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(keyTrk));
                if(trk != nullptr)
                {
                    trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseMove, "CMouseNormal");
                }
            }

            const IGisItem::key_t& keyRte = CGisItemRte::getKeyUserFocus();
            if(!keyRte.item.isEmpty())
            {
                CGisItemRte * rte = dynamic_cast<CGisItemRte*>(CGisWidget::self().getItemByKey(keyRte));
                if(rte != nullptr)
                {
                    rte->setMouseFocusByPoint(point, CGisItemRte::eFocusMouseMove, "CMouseNormal");
                }
            }
            break;
        }

        default:
            ;
        }

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
                if(nullptr != item)
                {
                    item->treeWidget()->collapseAll();
                    item->treeWidget()->setCurrentItem(item);
                    item->treeWidget()->scrollToItem(item, QAbstractItemView::PositionAtCenter);

                    if(setScreenOption(point, item))
                    {
                        stateItemSel = eStateShowItemOptions;
                    }
                    stopTracking();
                }
                break;
            }

            case eStateHooverMultiple:
            {
                screenUnclutter->setOrigin(e->pos());
                stateItemSel = eStateUnclutterMultiple;
                stopTracking();
                break;
            }

            case eStateUnclutterMultiple:
            {
                const CScrOptUnclutter::item_t * scrOpt = screenUnclutter->selectItem(point);
                if(scrOpt != nullptr)
                {
                    IGisItem * item = CGisWidget::self().getItemByKey(scrOpt->key);
                    screenUnclutter->clear(); // CAUTION!! this will delete the object scrOpt is pointing to.
                    scrOpt = nullptr;
                    if(item)
                    {
                        item->treeWidget()->collapseAll();
                        item->treeWidget()->setCurrentItem(item);
                        item->treeWidget()->scrollToItem(item, QAbstractItemView::PositionAtCenter);

                        if(setScreenOption(screenUnclutter->getOrigin(), item))
                        {
                            stateItemSel = eStateShowItemOptions;
                            break;
                        }
                    }
                }
                resetState();
                break;
            }

            case eStateShowItemOptions:
            {
                resetState();
                break;
            }
            }

            canvas->update();
        }
        mapMove     = false;
        mapDidMove  = false;
    }
}

void CMouseNormal::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(stateItemSel == eStateIdle)
    {
        const IGisItem::key_t& keyTrk = CGisItemTrk::getKeyUserFocus();
        CGisWidget::self().focusTrkByKey(false, keyTrk);

        const IGisItem::key_t& keyRte = CGisItemRte::getKeyUserFocus();
        CGisWidget::self().focusRteByKey(false, keyRte);
    }
}

void CMouseNormal::wheelEvent(QWheelEvent * e)
{
    resetState();
}

void CMouseNormal::keyPressEvent(QKeyEvent * e)
{
    resetState();
}

void CMouseNormal::resetState()
{
    screenUnclutter->clear();
    if(!screenItemOption.isNull())
    {
        screenItemOption->deleteLater();
    }
    stateItemSel = eStateIdle;
}

bool CMouseNormal::setScreenOption(const QPoint& pt, IGisItem * item)
{
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(item);
    if(trk && trk->setMouseFocusByPoint(pt, CGisItemTrk::eFocusMouseClick, "CMouseNormal") == NOPOINTF)
    {
        new CFadingIcon(pt, "://icons/48x48/NoGo.png", canvas);
        return false;
    }

    delete screenItemOption;
    screenItemOption = item->getScreenOptions(pt, this);

    return !screenItemOption.isNull();
}

void CMouseNormal::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
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
        /*
            Collect and draw items close to the last mouse position in the draw method.

            This might be a bit odd but there are two reasons:

            1) Multiple update events are combined by the event loop. Thus multiple mouse move
               events are reduced a single paint event. As getItemsByPos() is quite cycle
               intense this seems like a good idea.

            2) The list of items passed back by getItemsByPos() must not be stored. That is why
               the list has to be generated within the draw handler to access the item's drawHighlight()
               method.

         */
        screenUnclutter->clear();

        QList<IGisItem*> items;
        CGisWidget::self().getItemsByPos(point, items);

        if(items.empty() || items.size() > 8)
        {
            stateItemSel = eStateIdle;
            break;
        }

        for(IGisItem * item : items)
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

        // the screen option might not be destroyed yet, but already hidden
        if(screenItemOption->isVisible())
        {
            screenItemOption->draw(p);
        }
        break;
    }

    default:
        ;
    }
}


void CMouseNormal::slotAddWpt() const
{
    QPointF pt = point;
    gis->convertPx2Rad(pt);
    pt *= RAD_TO_DEG;

    CGisWidget::self().addWptByPos(pt);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotAddTrk() const
{
    QPointF pt = point;
    gis->convertPx2Rad(pt);

    canvas->setMouseEditTrk(pt);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotAddRte() const
{
    QPointF pt = point;
    gis->convertPx2Rad(pt);

    canvas->setMouseEditRte(pt);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotAddArea() const
{
    QPointF pt = point;
    gis->convertPx2Rad(pt);

    canvas->setMouseEditArea(pt);
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotCopyPosition() const
{
    QPointF pt = point;
    gis->convertPx2Rad(pt);

    QString position;
    IUnit::degToStr(pt.x() * RAD_TO_DEG, pt.y() * RAD_TO_DEG, position);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(position);
}

void CMouseNormal::slotCopyPositionGrid() const
{
    QString position;
    QPointF pt = point;
    gis->convertPx2Rad(pt);

    canvas->convertGridPos2Str(pt * RAD_TO_DEG, position, true);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(position);
}

void CMouseNormal::slotSelectArea() const
{
    canvas->setMouseSelect();
}
