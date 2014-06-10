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
#include "map/CMap.h"

#include <QtWidgets>

CMapList::CMapList(CMap * map, QWidget *parent)
    : QWidget(parent)
    , map(map)
{
    setupUi(this);

    frameButtons->hide();

    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    connect(pushActivate, SIGNAL(clicked()), this, SLOT(slotActivate()));
    connect(pushToTop, SIGNAL(clicked()), this, SLOT(slotToTop()));
}

CMapList::~CMapList()
{

}

void CMapList::updateList()
{
    map->buildMapList();
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

void CMapList::slotSelectionChanged()
{
    CMapItem * item = dynamic_cast<CMapItem*>(listWidget->currentItem());

    if(item)
    {
        bool activated = item->isActivated();
        pushActivate->setChecked(activated);
        pushToTop->setEnabled(activated);
        frameButtons->show();

    }
    else
    {
        frameButtons->hide();
    }

}

void CMapList::slotActivate()
{
    CMapItem * item = dynamic_cast<CMapItem*>(listWidget->currentItem());
    if(item == 0) return;

    bool activated = item->toggleActivate();

    pushActivate->setChecked(activated);
    pushToTop->setEnabled(activated);
    listWidget->setCurrentItem(0);
}

void CMapList::slotToTop()
{
    CMapItem * item = dynamic_cast<CMapItem*>(listWidget->currentItem());
    if(item == 0) return;

    item->moveToTop();
    listWidget->setCurrentItem(0);
}

