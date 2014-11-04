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
#include "gis/CGisListWks.h"
#include "gis/CGisWidget.h"
#include "gis/IGisItem.h"
#include "gis/IGisProject.h"
#include "gis/db/macros.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/search/CSearchGoogle.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectProjectDialog.h"

#include <QtSql>
#include <QtWidgets>

CGisListWks::CGisListWks(QWidget *parent)
    : QTreeWidget(parent)
    , menuNone(0)
{
    db = QSqlDatabase::database();

    menuProject     = new QMenu(this);
    actionSaveAs    = menuProject->addAction(QIcon("://icons/32x32/SaveGISAs.png"),tr("Save As..."), this, SLOT(slotSaveAsProject()));
    actionSave      = menuProject->addAction(QIcon("://icons/32x32/SaveGIS.png"),tr("Save"), this, SLOT(slotSaveProject()));
    actionClose     = menuProject->addAction(QIcon("://icons/32x32/Close.png"),tr("Close"), this, SLOT(slotCloseProject()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*,int)));

    menuItem        = new QMenu(this);
    actionEditDetails = menuItem->addAction(QIcon("://icons/32x32/EditDetails.png"),tr("Edit..."), this, SLOT(slotEditItem()));
    actionMoveWpt   = menuItem->addAction(QIcon("://icons/32x32/WptMove.png"),tr("Move Waypoint"), this, SLOT(slotMoveWpt()));
    actionProjWpt   = menuItem->addAction(QIcon("://icons/32x32/WptProj.png"),tr("Proj. Waypoint..."), this, SLOT(slotProjWpt()));
    actionFocusTrk  = menuItem->addAction(QIcon("://icons/32x32/TrkProfile.png"),tr("Track Profile"));
    actionRangeTrk  = menuItem->addAction(QIcon("://icons/32x32/SelectRange.png"),tr("Select Range"), this, SLOT(slotRangeTrk()));
    actionEditTrk   = menuItem->addAction(QIcon("://icons/32x32/LineMove.png"),tr("Edit Track Points"), this, SLOT(slotEditTrk()));
    actionReverseTrk = menuItem->addAction(QIcon("://icons/32x32/Reverse.png"),tr("Reverse Track"), this, SLOT(slotReverseTrk()));
    actionCombineTrk = menuItem->addAction(QIcon("://icons/32x32/Combine.png"),tr("Combine Tracks"), this, SLOT(slotCombineTrk()));
    actionEditArea   = menuItem->addAction(QIcon("://icons/32x32/AreaMove.png"),tr("Edit Area Points"), this, SLOT(slotEditArea()));
    actionFocusTrk->setCheckable(true);
    connect(actionFocusTrk, SIGNAL(triggered(bool)), this, SLOT(slotFocusTrk(bool)));
    actionDelete    = menuItem->addAction(QIcon("://icons/32x32/DeleteOne.png"),tr("Delete"), this, SLOT(slotDeleteItem()));

    connect(qApp, SIGNAL(aboutToQuit ()), this, SLOT(saveWorkspace()));
}

CGisListWks::~CGisListWks()
{
}

void CGisListWks::setExternalMenu(QMenu * project)
{
    menuNone = project;
    connect(CMainWindow::self().findChild<QAction*>("actionAddEmptyProject"), SIGNAL(triggered()), this, SLOT(slotAddEmptyProject()));
    connect(CMainWindow::self().findChild<QAction*>("actionCloseAllProjects"), SIGNAL(triggered(bool)), this, SLOT(slotCloseAllProjects()));
    connect(CMainWindow::self().findChild<QAction*>("actionSearchGoogle"), SIGNAL(triggered(bool)), this, SLOT(slotSearchGoogle(bool)));
}

void CGisListWks::dragMoveEvent (QDragMoveEvent  * e )
{
    /*
        What's happening here?

        1) Cast current item and item under cursor to GIS item type
        2) If type matches for both test for common parent
        2.1) common parent->  move
        2.1) different parent -> copy
        3) go on with dragMoveEvent();

     */

    CGisItemTrk * trk1 = dynamic_cast<CGisItemTrk*>(currentItem());
    CGisItemTrk * trk2 = dynamic_cast<CGisItemTrk*>(itemAt(e->pos()));

    if(trk1 && trk2)
    {
        if(trk1->parent() == trk2->parent())
        {
            e->setDropAction(Qt::MoveAction);
        }
        else
        {
            e->setDropAction(Qt::CopyAction);
        }
        QTreeWidget::dragMoveEvent(e);
        return;
    }

    CGisItemWpt * wpt1 = dynamic_cast<CGisItemWpt*>(currentItem());
    CGisItemWpt * wpt2 = dynamic_cast<CGisItemWpt*>(itemAt(e->pos()));

    if(wpt1 && wpt2)
    {
        if(wpt1->parent() == wpt2->parent())
        {
            e->setDropAction(Qt::MoveAction);
        }
        else
        {
            e->setDropAction(Qt::CopyAction);
        }
        QTreeWidget::dragMoveEvent(e);
        return;
    }

    CGisItemRte * rte1 = dynamic_cast<CGisItemRte*>(currentItem());
    CGisItemRte * rte2 = dynamic_cast<CGisItemRte*>(itemAt(e->pos()));

    if(rte1 && rte2)
    {
        if(rte1->parent() == rte2->parent())
        {
            e->setDropAction(Qt::MoveAction);
        }
        else
        {
            e->setDropAction(Qt::CopyAction);
        }
        QTreeWidget::dragMoveEvent(e);
        return;
    }

    CGisItemOvlArea * area1 = dynamic_cast<CGisItemOvlArea*>(currentItem());
    CGisItemOvlArea * area2 = dynamic_cast<CGisItemOvlArea*>(itemAt(e->pos()));

    if(area1 && area2)
    {
        if(area1->parent() == area2->parent())
        {
            e->setDropAction(Qt::MoveAction);
        }
        else
        {
            e->setDropAction(Qt::CopyAction);
        }
        QTreeWidget::dragMoveEvent(e);
        return;
    }

    IGisProject * proj = dynamic_cast<IGisProject*>(itemAt(e->pos()));
    if(proj && proj != currentItem()->parent())
    {
        e->setDropAction(Qt::CopyAction);
        QTreeWidget::dragMoveEvent(e);
        return;
    }

    e->setDropAction(Qt::IgnoreAction);
    QTreeWidget::dragMoveEvent(e);
}

void CGisListWks::dropEvent ( QDropEvent  * e )
{
    /*
        What's happening here?

        1) Test if item will be inserted above ore below item under cursor.
        2) Cast current item and item under cursor to GIS item type
        3) If type matches for both test for common parent
        3.1) common parent-> go on with default drop event
        3.1) different parent -> create a copy and insert it index
        4) signal change of project

        5) Test if item under cursor is a project
        6) If project and project is not item's project create a copy

     */

    // calc. index offset (below/above item)
    QRect r = visualItemRect(itemAt(e->pos()));
    int y1 = r.top() + r.height()/2;
    int y2 = e->pos().y();
    int off = y2 > y1 ? 1 : 0;

    CGisItemWpt * wpt1 = dynamic_cast<CGisItemWpt*>(currentItem());
    CGisItemWpt * wpt2 = dynamic_cast<CGisItemWpt*>(itemAt(e->pos()));

    if(wpt1 && wpt2)
    {
        if(wpt1->parent() == wpt2->parent())
        {
            QTreeWidget::dropEvent(e);
        }
        else
        {
            IGisProject * project = dynamic_cast<IGisProject*>(wpt2->parent());
            if(project)
            {
                new CGisItemWpt(*wpt1,project, project->indexOfChild(wpt2) + off);
            }
        }
        emit sigChanged();
        return;
    }

    CGisItemTrk * trk1 = dynamic_cast<CGisItemTrk*>(currentItem());
    CGisItemTrk * trk2 = dynamic_cast<CGisItemTrk*>(itemAt(e->pos()));

    if(trk1 && trk2)
    {
        if(trk1->parent() == trk2->parent())
        {
            QTreeWidget::dropEvent(e);
        }
        else
        {
            IGisProject * project = dynamic_cast<IGisProject*>(trk2->parent());
            if(project)
            {
                new CGisItemTrk(*trk1,project, project->indexOfChild(trk2) + off);
            }
        }
        emit sigChanged();
        return;
    }

    CGisItemRte * rte1 = dynamic_cast<CGisItemRte*>(currentItem());
    CGisItemRte * rte2 = dynamic_cast<CGisItemRte*>(itemAt(e->pos()));

    if(rte1 && rte2)
    {
        if(rte1->parent() == rte2->parent())
        {
            QTreeWidget::dropEvent(e);
        }
        else
        {
            IGisProject * project = dynamic_cast<IGisProject*>(rte2->parent());
            if(project)
            {
                new CGisItemRte(*rte1,project, project->indexOfChild(rte2) + off);
            }
        }
        emit sigChanged();
        return;
    }

    CGisItemOvlArea * area1 = dynamic_cast<CGisItemOvlArea*>(currentItem());
    CGisItemOvlArea * area2 = dynamic_cast<CGisItemOvlArea*>(itemAt(e->pos()));

    if(area1 && area2)
    {
        if(area1->parent() == area2->parent())
        {
            QTreeWidget::dropEvent(e);
        }
        else
        {
            IGisProject * project = dynamic_cast<IGisProject*>(area2->parent());
            if(project)
            {
                new CGisItemOvlArea(*area1,project, project->indexOfChild(area2) + off);
            }
        }
        emit sigChanged();
        return;
    }


    IGisProject * project = dynamic_cast<IGisProject*>(itemAt(e->pos()));
    if(project && project != currentItem()->parent())
    {
        if(wpt1 != 0)
        {
            new CGisItemWpt(*wpt1, project, -1);
        }
        else if(trk1 != 0)
        {
            new CGisItemTrk(*trk1, project, -1);
        }
        else if(rte1 != 0)
        {
            new CGisItemRte(*rte1, project, -1);
        }
        else if(area1 != 0)
        {
            new CGisItemOvlArea(*area1, project, -1);
        }
    }
}


bool CGisListWks::hasProject(const QString& key)
{
    QMutexLocker lock(&IGisItem::mutexItems);
    for(int i = 0; i < topLevelItemCount(); i++)
    {
        IGisProject * item = dynamic_cast<IGisProject*>(topLevelItem(i));
        if(item && item->getKey() == key)
        {
            return(true);
        }
    }
    return(false);
}

void CGisListWks::saveWorkspace()
{
    QSqlQuery query(db);
    if(!query.exec("DELETE FROM workspace"))
    {
        QUERY_EXEC(return);
    }

    qDebug() << "saveWorkspace()";

    const int total = topLevelItemCount();
    PROGRESS_SETUP(tr("Saving workspace. Please wait."), total);

    for(int i = 0; i < total; i++)
    {
        PROGRESS(i, return);

        IGisProject * project = dynamic_cast<IGisProject*>(topLevelItem(i));
        if(project == 0)
        {
            continue;
        }

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_2);
        stream.setByteOrder(QDataStream::LittleEndian);

        *project >> stream;

        query.prepare("INSERT INTO workspace (type, key, changed, data) VALUES (:type, :key, :changed, :data)");
        query.bindValue(":changed", project->text(1) == "*");
        query.bindValue(":type", project->getType());
        query.bindValue(":key", project->getKey());
        query.bindValue(":data", data);
        QUERY_EXEC(continue);

    }

}

void CGisListWks::slotContextMenu(const QPoint& point)
{
    if(selectedItems().isEmpty() && menuNone)
    {
        QPoint p = mapToGlobal(point);
        menuNone->exec(p);
        return;
    }

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(project != 0)
    {
        QPoint p = mapToGlobal(point);
        menuProject->exec(p);
        return;
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
            actionEditTrk->setVisible(false);
            actionReverseTrk->setVisible(false);
            actionCombineTrk->setVisible(false);
            actionRangeTrk->setVisible(false);
        }
        else
        {
            actionFocusTrk->setVisible(true);
            actionEditTrk->setVisible(true);
            actionReverseTrk->setVisible(true);
            actionCombineTrk->setVisible(true);
            actionRangeTrk->setVisible(true);
            actionFocusTrk->setChecked(trk->hasUserFocus());
            actionEditTrk->setEnabled(!trk->isReadOnly());
        }

        // try to cast item to track and hide/show actions on result
        CGisItemOvlArea * area = dynamic_cast<CGisItemOvlArea*>(gisItem);
        if(area == 0)
        {
            actionEditArea->setVisible(false);
        }
        else
        {
            actionEditArea->setVisible(true);
        }
        // display menu
        QPoint p = mapToGlobal(point);
        menuItem->exec(p);
        return;
    }
}

void CGisListWks::slotCloseProject()
{
    QMutexLocker lock(&IGisItem::mutexItems);
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
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
        IGisProject * project = dynamic_cast<IGisProject*>(item);
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
        IGisProject * project = dynamic_cast<IGisProject*>(item);
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
        CGisWidget::self().focusTrkByKey(true, gisItem->getKey());
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

void CGisListWks::slotEditTrk()
{
    IGisItem::mutexItems.lock();
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().editTrkByKey(key);
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotReverseTrk()
{
    IGisItem::mutexItems.lock();
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().reverseTrkByKey(key);
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotCombineTrk()
{
    IGisItem::mutexItems.lock();
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().combineTrkByKey(key);
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotRangeTrk()
{
    IGisItem::mutexItems.lock();
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().rangeTrkByKey(key);
    }
    IGisItem::mutexItems.unlock();
}

void CGisListWks::slotEditArea()
{
    IGisItem::mutexItems.lock();
    CGisItemOvlArea * gisItem = dynamic_cast<CGisItemOvlArea*>(currentItem());
    if(gisItem != 0)
    {
        QString key = gisItem->getKey();
        CGisWidget::self().editAreaByKey(key);
    }
    IGisItem::mutexItems.unlock();
}


void CGisListWks::slotAddEmptyProject()
{
    QString key, name;
    CSelectProjectDialog::type_e type;
    CSelectProjectDialog dlg(key, name, type, 0);
    dlg.exec();
    if(name.isEmpty())
    {
        return;
    }

    if(type == CSelectProjectDialog::eTypeGpx)
    {
        new CGpxProject(name, this);
    }
    else if(type == CSelectProjectDialog::eTypeQms)
    {
        new CQmsProject(name, this);
    }
}

void CGisListWks::slotCloseAllProjects()
{
    QMutexLocker lock(&IGisItem::mutexItems);
    QList<QTreeWidgetItem*> items = findItems("*", Qt::MatchWildcard);
    foreach(QTreeWidgetItem * item, items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project != 0)
        {
            delete project;
        }
    }
    emit sigChanged();
}

void CGisListWks::slotSearchGoogle(bool on)
{
    delete searchGoogle;
    if(on)
    {
        searchGoogle = new CSearchGoogle(this);
    }
}

