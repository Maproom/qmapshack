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

#include "dem/CDemItem.h"
#include "dem/CDemList.h"
#include "dem/CDemDraw.h"
#include "units/IUnit.h"

#include <QtWidgets>

void CDemTreeWidget::dragMoveEvent ( QDragMoveEvent  * event )
{
    CDemItem * item = dynamic_cast<CDemItem*>(itemAt(event->pos()));

    if(item && item->isActivated())
    {
        event->setDropAction(Qt::MoveAction);
        QTreeWidget::dragMoveEvent(event);
    }
    else
    {
        event->setDropAction(Qt::IgnoreAction);
    }
}

void CDemTreeWidget::dropEvent ( QDropEvent  * event )
{
    CDemItem * item = dynamic_cast<CDemItem*>(currentItem());
    if(item)
    {
        item->showChildren(false);
    }

    QTreeWidget::dropEvent(event);

    if(item)
    {
        item->showChildren(true);
    }

    emit sigChanged();
}


CDemList::CDemList(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(treeWidget,     &CDemTreeWidget::customContextMenuRequested, this, &CDemList::slotContextMenu);
    connect(actionMoveUp,   &QAction::triggered,                         this, &CDemList::slotMoveUp);
    connect(actionMoveDown, &QAction::triggered,                         this, &CDemList::slotMoveDown);
    connect(actionActivate, &QAction::triggered,                         this, &CDemList::slotActivate);
    connect(actionReloadDem, &QAction::triggered,                        this, &CDemList::slotReloadDem);

    connect(treeWidget,     &CDemTreeWidget::sigChanged,                 this, &CDemList::sigChanged);

    menu = new QMenu(this);
    menu->addAction(actionActivate);
    menu->addAction(actionMoveUp);
    menu->addAction(actionMoveDown);
    menu->addSeparator();
    menu->addAction(actionReloadDem);
}

CDemList::~CDemList()
{
}

void CDemList::clear()
{
    treeWidget->clear();
}

int CDemList::count()
{
    return treeWidget->topLevelItemCount();
}

CDemItem * CDemList::item(int i)
{
    return dynamic_cast<CDemItem *>(treeWidget->topLevelItem(i));
}

void CDemList::updateHelpText()
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

        CDemItem * item = dynamic_cast<CDemItem*>(treeWidget->topLevelItem(0));
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

void CDemList::slotActivate()
{
    CDemItem * item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
    if(item == 0)
    {
        return;
    }

    bool activated = item->toggleActivate();
    if(!activated)
    {
        treeWidget->setCurrentItem(0);
    }

    updateHelpText();
}

void CDemList::slotMoveUp()
{
    CDemItem * item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
    if(item == nullptr)
    {
        return;
    }

    int index = treeWidget->indexOfTopLevelItem(item);
    if(index == NOIDX)
    {
        return;
    }

    item->showChildren(false);
    treeWidget->takeTopLevelItem(index);
    treeWidget->insertTopLevelItem(index-1, item);
    item->showChildren(true);
    treeWidget->setCurrentItem(0);
    emit treeWidget->sigChanged();
}

void CDemList::slotMoveDown()
{
    CDemItem * item = dynamic_cast<CDemItem*>(treeWidget->currentItem());
    if(item == nullptr)
    {
        return;
    }

    int index = treeWidget->indexOfTopLevelItem(item);
    if(index == NOIDX)
    {
        return;
    }

    item->showChildren(false);
    treeWidget->takeTopLevelItem(index);
    treeWidget->insertTopLevelItem(index+1, item);
    item->showChildren(true);
    treeWidget->setCurrentItem(0);
    emit treeWidget->sigChanged();
}


void CDemList::slotContextMenu(const QPoint& point)
{
    CDemItem * item = dynamic_cast<CDemItem*>(treeWidget->currentItem());

    if(item == 0)
    {
        return;
    }
    bool activated = item->isActivated();
    actionActivate->setChecked(activated);
    actionActivate->setText(activated ? tr("Deactivate") : tr("Activate"));

    CDemItem * item1 = dynamic_cast<CDemItem*>(treeWidget->itemBelow(item));
    actionMoveUp->setEnabled(activated && (treeWidget->itemAbove(item) != 0));
    actionMoveDown->setEnabled(activated && item1 && item1->isActivated());

    QPoint p = treeWidget->mapToGlobal(point);
    menu->exec(p);
}

void CDemList::slotReloadDem()
{
    CDemDraw::setupDemPath(CDemDraw::getDemPaths());
}
