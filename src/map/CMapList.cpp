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

#include "CMainWindow.h"
#include "map/CMapDraw.h"
#include "map/CMapItem.h"
#include "map/CMapList.h"

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

CMapList::CMapList(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(treeWidget,     &CMapTreeWidget::customContextMenuRequested, this, &CMapList::slotContextMenu);
    connect(treeWidget,     &CMapTreeWidget::sigChanged,                 this, &CMapList::sigChanged);
    connect(actionActivate, &QAction::triggered,                         this, &CMapList::slotActivate);
    connect(actionMoveUp,   &QAction::triggered,                         this, &CMapList::slotMoveUp);
    connect(actionMoveDown, &QAction::triggered,                         this, &CMapList::slotMoveDown);
    connect(actionReloadMaps, &QAction::triggered,                       this, &CMapList::slotReloadMaps);
    connect(pushMapHonk,    &QPushButton::clicked,                       this, &CMapList::slotMapHonk);


    menu = new QMenu(this);
    menu->addAction(actionActivate);
    menu->addAction(actionMoveUp);
    menu->addAction(actionMoveDown);
    menu->addSeparator();
    menu->addAction(actionReloadMaps);
    menu->addAction(CMainWindow::self().getMapSetupAction());
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
    bool haveMaps = (treeWidget->topLevelItemCount() > 0);

    pushMapHonk->setVisible(!haveMaps);
    labelHelpFillMapList->setVisible(!haveMaps);

    if(!haveMaps)
    {
        labelIcon->show();
        labelHelpActivateMap->hide();
    }
    else
    {
        CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->topLevelItem(0));

        bool showHelp = !(item && item->isActivated());
        labelIcon->setVisible(showHelp);
        labelHelpActivateMap->setVisible(showHelp);
    }
}

void CMapList::slotActivate()
{
    CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
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

void CMapList::slotMoveUp()
{
    CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
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

void CMapList::slotMoveDown()
{
    CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->currentItem());
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

void CMapList::slotContextMenu(const QPoint& point)
{
    CMapItem * item = dynamic_cast<CMapItem*>(treeWidget->currentItem());

    bool itemIsSelected  = nullptr != item;
    bool itemIsActivated = item ? item->isActivated() : false;

    actionActivate->setEnabled(itemIsSelected);
    actionMoveUp->setEnabled(itemIsSelected);
    actionMoveDown->setEnabled(itemIsSelected);

    actionActivate->setChecked(itemIsActivated);
    actionActivate->setText(itemIsActivated ? tr("Deactivate") : tr("Activate"));

    if(itemIsSelected)
    {
        CMapItem * item1 = dynamic_cast<CMapItem*>(treeWidget->itemBelow(item));
        actionMoveUp->setEnabled(itemIsActivated && (treeWidget->itemAbove(item) != 0));
        actionMoveDown->setEnabled(itemIsActivated && item1 && item1->isActivated());
    }
    QPoint p = treeWidget->mapToGlobal(point);
    menu->exec(p);
}

void saveResource(const QString& name, QDir& dir)
{
    QFile resource1(QString("://map/%1").arg(name));
    resource1.open(QIODevice::ReadOnly);

    QFile file(dir.absoluteFilePath(name));
    file.open(QIODevice::WriteOnly);
    file.write(resource1.readAll());
    file.close();
}

void CMapList::slotMapHonk()
{
    QString path = QFileDialog::getExistingDirectory(CMainWindow::getBestWidgetForParent(), tr("Where do you want to store maps?"), QDir::homePath());
    if(path.isEmpty())
    {
        return;
    }

    QDir dir(path);

    saveResource("WorldSat.wmts", dir);
    saveResource("WorldTopo.wmts", dir);
    saveResource("OpenStreetMap.tms", dir);
    saveResource("OSM_Topo.tms", dir);
    saveResource("OpenCycleMap.tms", dir);

    CMapDraw::setupMapPath(path);

    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->setScales(CCanvas::eScalesSquare);
    }
}

void CMapList::slotReloadMaps()
{
    CMapDraw::setupMapPath(CMapDraw::getMapPaths());
}
