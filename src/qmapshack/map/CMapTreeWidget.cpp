/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#include "map/CMapTreeWidget.h"
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

void CMapTreeWidget::dropEvent(QDropEvent  * e)
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

void CMapTreeWidget::resizeEvent(QResizeEvent * e)
{
    QTreeWidget::resizeEvent(e);

    const int N = topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        CMapItem * item = dynamic_cast<CMapItem*>(topLevelItem(n));
        if(item != nullptr)
        {
            item->resize();
        }
    }
}


