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

#include "map/CMapList.h"
#include "map/CMapItem.h"

#include <QtWidgets>

void CMapListWidget::dragMoveEvent ( QDragMoveEvent  * event )
{
    CMapItem * item = dynamic_cast<CMapItem*>(itemAt(event->pos()));

    if(item && item->isActivated())
    {
        event->setDropAction(Qt::MoveAction);
        QListWidget::dragMoveEvent(event);
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
    }
}

CMapList::CMapList(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));
    connect(listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(actionActivate, SIGNAL(triggered()), this, SLOT(slotActivate()));

    menu = new QMenu(this);
    menu->addAction(actionActivate);
}

CMapList::~CMapList()
{

}

void CMapList::clear()
{
    listWidget->clear();
}

int CMapList::count()
{
    return listWidget->count();
}

CMapItem * CMapList::item(int i)
{
    return dynamic_cast<CMapItem *>(listWidget->item(i));
}


void CMapList::slotActivate()
{
    CMapItem * item = dynamic_cast<CMapItem*>(listWidget->currentItem());
    if(item == 0) return;

    bool activated = item->toggleActivate();
    if(!activated)
    {
        listWidget->setCurrentItem(0);
    }

    item = dynamic_cast<CMapItem*>(listWidget->item(0));
    if(item && item->isActivated())
    {
        labelHelp->hide();
    }
    else
    {
        labelHelp->show();
    }
}

void CMapList::slotItemSelectionChanged()
{
    CMapItem * item = dynamic_cast<CMapItem*>(listWidget->currentItem());
    if(item && item->isActivated())
    {
        listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    }
    else
    {
        listWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
    }

}

void CMapList::slotContextMenu(const QPoint& point)
{
    CMapItem * item = dynamic_cast<CMapItem*>(listWidget->currentItem());

    if(item == 0)
    {
        return;
    }
    bool activated = item->isActivated();
    actionActivate->setChecked(activated);
    actionActivate->setText(activated ? tr("Deactivate") : tr("Activate"));

    QPoint p = listWidget->mapToGlobal(point);
    menu->exec(p);
}
