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

void CMapTreeWidget::dragEnterEvent(QDragEnterEvent * e)
{
    collapseAll();
    QTreeWidget::dragEnterEvent(e);
}

void CMapTreeWidget::dragMoveEvent(QDragMoveEvent  * e)
{
    CMapItem * item = dynamic_cast<CMapItem*>(itemAt(e->pos()));

    if(item && item->isActivated())
    {
        e->setDropAction(Qt::MoveAction);
        QTreeWidget::dragMoveEvent(e);
    }
    else
    {
        e->setDropAction(Qt::IgnoreAction);
    }
}

void CMapTreeWidget::dropEvent (QDropEvent  * e)
{

    CMapItem * item = dynamic_cast<CMapItem*>(currentItem());
    if(item)
    {
        item->showChildren(false);
    }

    QTreeWidget::dropEvent(e);

    if(item)
    {
        item->showChildren(true);
    }


    emit sigChanged();
}

CMapList::CMapList(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));
    connect(treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(actionActivate, SIGNAL(triggered()), this, SLOT(slotActivate()));

    connect(treeWidget, SIGNAL(sigChanged()), SIGNAL(sigChanged()));

    menu = new QMenu(this);
    menu->addAction(actionActivate);
}

CMapList::~CMapList()
{

}

void CMapList::clear()
{
    treeWidget->clear();
}

int CMapList::count()
{
    return treeWidget->topLevelItemCount();
}

CMapItem * CMapList::item(int i)
{
    return dynamic_cast<CMapItem *>(treeWidget->topLevelItem(i));
}

void CMapList::updateHelpText()
{
    if(treeWidget->topLevelItemCount() == 0)
    {
        labelIcon->show();
        labelHelpFillMapList->show();
        labelHelpActivateMap->hide();
    }
    else
    {
        labelHelpFillMapList->hide();

        CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->topLevelItem(0));
        if(item && item->isActivated())
        {
            labelIcon->hide();
            labelHelpActivateMap->hide();
        }
        else
        {
            labelIcon->show();
            labelHelpActivateMap->show();
        }
    }
}

void CMapList::slotActivate()
{
    CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
    if(item == 0) return;

    bool activated = item->toggleActivate();
    if(!activated)
    {
        treeWidget->setCurrentItem(0);
    }

    updateHelpText();
}

void CMapList::slotItemSelectionChanged()
{
    CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
    if(item && item->isActivated())
    {
        treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    }
    else
    {
        treeWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
    }

}

void CMapList::slotContextMenu(const QPoint& point)
{
    CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->currentItem());

    if(item == 0)
    {
        return;
    }
    bool activated = item->isActivated();
    actionActivate->setChecked(activated);
    actionActivate->setText(activated ? tr("Deactivate") : tr("Activate"));

    QPoint p = treeWidget->mapToGlobal(point);
    menu->exec(p);
}
