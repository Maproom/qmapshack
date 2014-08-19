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

#include "gis/CGisListWks.h"
#include "gis/CGisProject.h"
#include "gis/IGisItem.h"
#include "gis/CGisWidget.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CGisItemTrk.h"
#include "CMainWindow.h"

#include <QtWidgets>

CGisListWks::CGisListWks(QWidget *parent)
    : QTreeWidget(parent)
{

    menuProject     = new QMenu(this);
    actionSaveAs    = menuProject->addAction(QIcon("://icons/32x32/SaveGIS.png"),tr("Save As..."), this, SLOT(slotSaveAsProject()));
    actionSave      = menuProject->addAction(QIcon("://icons/32x32/SaveGIS.png"),tr("Save"), this, SLOT(slotSaveProject()));    
    actionClose     = menuProject->addAction(QIcon("://icons/32x32/Close.png"),tr("Close"), this, SLOT(slotCloseProject()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*,int)));

    menuItem        = new QMenu(this);
    actionEditDetails = menuItem->addAction(QIcon("://icons/32x32/EditDetails.png"),tr("Edit..."), this, SLOT(slotEditItem()));
    actionMoveWpt   = menuItem->addAction(QIcon("://icons/32x32/WptMove.png"),tr("Move Waypoint"), this, SLOT(slotMoveWpt()));
    actionProjWpt   = menuItem->addAction(QIcon("://icons/32x32/WptProj.png"),tr("Proj. Waypoint..."), this, SLOT(slotProjWpt()));
    actionFocusTrk  = menuItem->addAction(QIcon("://icons/32x32/TrkProfile.png"),tr("Track Profile"));
    actionFocusTrk->setCheckable(true);
    connect(actionFocusTrk, SIGNAL(triggered(bool)), this, SLOT(slotFocusTrk(bool)));
    actionDelete    = menuItem->addAction(QIcon("://icons/32x32/DeleteOne.png"),tr("Delete"), this, SLOT(slotDeleteItem()));

}

CGisListWks::~CGisListWks()
{

}

bool CGisListWks::hasProject(const QString& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < topLevelItemCount(); i++)
    {
        CGisProject * item = dynamic_cast<CGisProject*>(topLevelItem(i));
        if(item && item->getKey() == key)
        {
            return true;
        }
    }
    return false;
}

void CGisListWks::slotContextMenu(const QPoint& point)
{
    CGisProject * project = dynamic_cast<CGisProject*>(currentItem());
    if(project != 0)
    {
        QPoint p = mapToGlobal(point);
        menuProject->exec(p);
    }

    IGisItem * gisItem = dynamic_cast<IGisItem*>(currentItem());
    if(gisItem != 0)
    {
        // try to cast item to waypoint and hide/show actions on result
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(gisItem);
        if(wpt == 0)
        {
            actionProjWpt->setVisible(false);
            actionMoveWpt->setVisible(false);
        }
        else
        {
            actionProjWpt->setVisible(true);
            actionMoveWpt->setVisible(true);
            actionMoveWpt->setEnabled(!wpt->isReadOnly());
            actionProjWpt->setEnabled(!wpt->isGeocache());
        }

        // try to cast item to track and hide/show actions on result
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(gisItem);
        if(trk == 0)
        {
            actionFocusTrk->setVisible(false);
        }
        else
        {
            actionFocusTrk->setVisible(true);
            actionFocusTrk->setChecked(trk->hasUserFocus());
        }
        // display menu
        QPoint p = mapToGlobal(point);
        menuItem->exec(p);
    }

}

void CGisListWks::slotCloseProject()
{
    QMutexLocker lock(&IGisItem::mutexItems);
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(item);
        if(project != 0)
        {
            delete project;
        }
    }
    emit sigChanged();
}

void CGisListWks::slotSaveProject()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    IGisItem::mutexItems.lock();

    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(item);
        if(project != 0)
        {
            project->save();
        }
    }
    IGisItem::mutexItems.unlock();
    QApplication::restoreOverrideCursor();
}

void CGisListWks::slotSaveAsProject()
{
    IGisItem::mutexItems.lock();

    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(item);
        if(project != 0)
        {
            project->saveAs();
        }
    }
    IGisItem::mutexItems.unlock();

}

void CGisListWks::slotItemDoubleClicked(QTreeWidgetItem * item, int )
{
    IGisItem::mutexItems.lock();
    IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
    if(gisItem != 0)
    {
        CMainWindow::self().zoomCanvasTo(gisItem->getBoundingRect());
    }    
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotEditItem()
{
    IGisItem::mutexItems.lock();
    IGisItem * gisItem = dynamic_cast<IGisItem*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().editItemByKey(key);
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotDeleteItem()
{
    IGisItem::mutexItems.lock();

    foreach(QTreeWidgetItem * item, selectedItems())
    {
        IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
        if(gisItem != 0)
        {
            QString key = gisItem->getKey();
            CGisWidget::self().delItemByKey(key);
        }
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotProjWpt()
{
    IGisItem::mutexItems.lock();
    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().projWptByKey(key);
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotMoveWpt()
{
    IGisItem::mutexItems.lock();
    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().moveWptByKey(key);
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotFocusTrk(bool on)
{
    IGisItem::mutexItems.lock();
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().focusTrkByKey(on, key);
    }
    IGisItem::mutexItems.unlock();
}
