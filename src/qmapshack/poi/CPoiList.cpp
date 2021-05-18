/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CMainWindow.h"
#include "misc.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiItem.h"
#include "poi/CPoiList.h"

CPoiList::CPoiList(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(treeWidget, &CPoiTreeWidget::customContextMenuRequested, this, &CPoiList::slotContextMenu);
    connect(actionActivate, &QAction::triggered, this, &CPoiList::slotActivate);
    connect(actionReloadPoi, &QAction::triggered, this, &CPoiList::slotReloadPoi);
    connect(treeWidget, &CPoiTreeWidget::sigChanged, this, &CPoiList::sigChanged);
    connect(labelHelpFillPoiList, &QLabel::linkActivated, &CMainWindow::self(), static_cast<void (CMainWindow::*)(const QString&)>(&CMainWindow::slotLinkActivated));

    menu = new QMenu(this);
    menu->addAction(actionActivate);
    menu->addSeparator();
    menu->addAction(actionReloadPoi);
    menu->addAction(CMainWindow::self().getPoiSetupAction());
}


void CPoiList::clear()
{
    treeWidget->clear();
}

int CPoiList::count()
{
    return treeWidget->topLevelItemCount();
}

CPoiItem* CPoiList::item(int i)
{
    return dynamic_cast<CPoiItem*>(treeWidget->topLevelItem(i));
}

void CPoiList::updateHelpText()
{
    if(treeWidget->topLevelItemCount() == 0)
    {
        labelIcon->show();
        labelHelpFillPoiList->show();
        labelHelpActivatePoi->hide();
    }
    else
    {
        labelHelpFillPoiList->hide();

        CPoiItem* item = dynamic_cast<CPoiItem*>(treeWidget->topLevelItem(0));
        bool haveActive = item && item->isActivated();
        labelIcon->setVisible(!haveActive);
        labelHelpActivatePoi->setVisible(!haveActive);
    }
}


void CPoiList::sort()
{
    QList<CPoiItem*> items1;
    while(treeWidget->topLevelItemCount())
    {
        CPoiItem* item = dynamic_cast<CPoiItem*>(treeWidget->takeTopLevelItem(0));
        if(item != nullptr)
        {
            items1 << item;
        }
    }

    qSort(items1.begin(), items1.end(), &sortByName<CPoiItem>);

    QList<QTreeWidgetItem*> items2;
    for(CPoiItem* item : qAsConst(items1))
    {
        items2 << item;
    }
    treeWidget->addTopLevelItems(items2);
}

void CPoiList::slotActivate()
{
    CPoiItem* item = dynamic_cast<CPoiItem*>(treeWidget->currentItem());
    if(nullptr == item)
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

void CPoiList::slotReloadPoi()
{
    CPoiDraw::setupPoiPath(CPoiDraw::getPoiPaths());
}

void CPoiList::slotContextMenu(const QPoint& point)
{
    CPoiItem* item = dynamic_cast<CPoiItem*>(treeWidget->currentItem());

    bool itemIsSelected = nullptr != item;
    bool itemIsActivated = item ? item->isActivated() : false;

    actionActivate->setEnabled(itemIsSelected);
    actionActivate->setChecked(itemIsActivated);
    actionActivate->setText(itemIsActivated ? tr("Deactivate") : tr("Activate"));


    QPoint p = treeWidget->mapToGlobal(point);
    menu->exec(p);
}
