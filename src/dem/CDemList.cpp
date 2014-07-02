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

#include "dem/CDemList.h"
#include "dem/CDemItem.h"

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

    emit sigChanged();;
}


CDemList::CDemList(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(actionActivate, SIGNAL(triggered()), this, SLOT(slotActivate()));

    connect(treeWidget, SIGNAL(sigChanged()), SIGNAL(sigChanged()));

    menu = new QMenu(this);
    menu->addAction(actionActivate);
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
    if(item == 0) return;

    bool activated = item->toggleActivate();
    if(!activated)
    {
        treeWidget->setCurrentItem(0);
    }

    updateHelpText();
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

    QPoint p = treeWidget->mapToGlobal(point);
    menu->exec(p);
}

