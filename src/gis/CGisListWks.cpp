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
#include "canvas/CCanvas.h"
#include "config.h"
#ifdef Q_OS_LINUX
#include "device/CDeviceWatcherLinux.h"
#endif
#ifdef Q_OS_WIN
#include "device/CDeviceWatcherWindows.h"
#endif
#include "device/IDevice.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWidget.h"
#include "gis/CSelDevices.h"
#include "gis/IGisItem.h"
#include "gis/db/CDBProject.h"
#include "gis/db/CLostFoundProject.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/CSetupFolder.h"
#include "gis/db/macros.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/search/CSearchGoogle.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSelectProjectDialog.h"
#include "helpers/CSettings.h"

#include <QApplication>
#include <QtSql>
#include <QtWidgets>

#define DB_VERSION 1

class CGisListWksEditLock
{
public:
    CGisListWksEditLock(bool waitCursor, QMutex& mutex) : mutex(mutex), waitCursor(waitCursor)
    {
        if(waitCursor)
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
        }
        mutex.lock();
    }
    ~CGisListWksEditLock()
    {
        if(waitCursor)
        {
            QApplication::restoreOverrideCursor();
        }
        mutex.unlock();
    }
private:
    QMutex& mutex;
    bool waitCursor;
};


CGisListWks::CGisListWks(QWidget *parent)
    : QTreeWidget(parent)
    , menuNone(0)
    , saveOnExit(true)
    , saveEvery(5)
    , deviceWatcher(0)
{
    db = QSqlDatabase::addDatabase("QSQLITE","Workspace1");
    db.setDatabaseName(QDir::home().filePath(CONFIGDIR).append("/workspace.db"));
    db.open();
    configDB();

    menuProjectWks  = new QMenu(this);
    actionEditPrj   = menuProjectWks->addAction(QIcon("://icons/32x32/EditDetails.png"),tr("Edit.."), this, SLOT(slotEditPrj()));
    actionSaveAs    = menuProjectWks->addAction(QIcon("://icons/32x32/SaveGISAs.png"),tr("Save As..."), this, SLOT(slotSaveAsProject()));
    actionSave      = menuProjectWks->addAction(QIcon("://icons/32x32/SaveGIS.png"),tr("Save"), this, SLOT(slotSaveProject()));
    actionSyncWksDev= menuProjectWks->addAction(QIcon("://icons/32x32/Device.png"),tr("Send to Devices"), this, SLOT(slotSyncWksDev()));
    actionCloseProj = menuProjectWks->addAction(QIcon("://icons/32x32/Close.png"),tr("Close"), this, SLOT(slotCloseProject()));

    menuProjectDev  = new QMenu(this);
    menuProjectDev->addAction(actionEditPrj);
    menuProjectDev->addAction(actionSaveAs);
    menuProjectDev->addAction(actionSave);
    actionSyncDevWks= menuProjectDev->addAction(QIcon("://icons/32x32/Device.png"),tr("Update Project on Device"), this, SLOT(slotSyncDevWks()));
    actionDelProj   = menuProjectDev->addAction(QIcon("://icons/32x32/DeleteOne.png"),tr("Delete"), this, SLOT(slotDeleteProject()));

    menuProjectTrash= new QMenu(this);
    menuProjectTrash->addAction(actionSaveAs);
    menuProjectTrash->addAction(actionCloseProj);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

    menuItemTrk     = new QMenu(this);
    actionEditDetails = menuItemTrk->addAction(QIcon("://icons/32x32/EditDetails.png"),tr("Edit..."), this, SLOT(slotEditItem()));
    actionCopyItem  = menuItemTrk->addAction(QIcon("://icons/32x32/Copy.png"),tr("Copy to..."), this, SLOT(slotCopyItem()));
    menuItemTrk->addSeparator();
    actionFocusTrk  = menuItemTrk->addAction(QIcon("://icons/32x32/TrkProfile.png"),tr("Track Profile"));
    actionFocusTrk->setCheckable(true);
    actionRangeTrk  = menuItemTrk->addAction(QIcon("://icons/32x32/SelectRange.png"),tr("Select Range"), this, SLOT(slotRangeTrk()));
    actionEditTrk   = menuItemTrk->addAction(QIcon("://icons/32x32/LineMove.png"),tr("Edit Track Points"), this, SLOT(slotEditTrk()));
    actionReverseTrk = menuItemTrk->addAction(QIcon("://icons/32x32/Reverse.png"),tr("Reverse Track"), this, SLOT(slotReverseTrk()));
    actionCombineTrk = menuItemTrk->addAction(QIcon("://icons/32x32/Combine.png"),tr("Combine Tracks"), this, SLOT(slotCombineTrk()));
    menuItemTrk->addSeparator();
    actionDelete    = menuItemTrk->addAction(QIcon("://icons/32x32/DeleteOne.png"),tr("Delete"), this, SLOT(slotDeleteItem()));

    menuItemWpt     = new QMenu(this);
    menuItemWpt->addAction(actionEditDetails);
    menuItemWpt->addAction(actionCopyItem);
    menuItemWpt->addSeparator();
    actionBubbleWpt = menuItemWpt->addAction(QIcon("://icons/32x32/Bubble.png"),tr("Show Bubble"), this, SLOT(slotBubbleWpt()));
    actionBubbleWpt->setCheckable(true);
    actionMoveWpt   = menuItemWpt->addAction(QIcon("://icons/32x32/WptMove.png"),tr("Move Waypoint"), this, SLOT(slotMoveWpt()));
    actionProjWpt   = menuItemWpt->addAction(QIcon("://icons/32x32/WptProj.png"),tr("Proj. Waypoint..."), this, SLOT(slotProjWpt()));
    menuItemWpt->addSeparator();
    menuItemWpt->addAction(actionDelete);

    menuItemRte     = new QMenu(this);
    menuItemRte->addAction(actionEditDetails);
    menuItemRte->addAction(actionCopyItem);
    menuItemRte->addAction(actionDelete);


    menuItemOvl     = new QMenu(this);
    menuItemOvl->addAction(actionEditDetails);
    menuItemOvl->addAction(actionCopyItem);
    menuItemOvl->addSeparator();
    actionEditArea  = menuItemOvl->addAction(QIcon("://icons/32x32/AreaMove.png"),tr("Edit Area Points"), this, SLOT(slotEditArea()));
    menuItemOvl->addSeparator();
    menuItemOvl->addAction(actionDelete);


    menuItem        = new QMenu(this);
    menuItem->addAction(actionCopyItem);
    menuItem->addAction(actionDelete);

    connect(actionFocusTrk, SIGNAL(triggered(bool)), this, SLOT(slotFocusTrk(bool)));
    connect(qApp, SIGNAL(aboutToQuit ()), this, SLOT(slotSaveWorkspace()));

    SETTINGS;
    saveOnExit  = cfg.value("Database/saveOnExit", saveOnExit).toBool();
    saveEvery   = cfg.value("Database/saveEvery", saveEvery).toInt();

    if(saveOnExit && (saveEvery > 0))
    {
        QTimer::singleShot(saveEvery * 60000, this, SLOT(slotSaveWorkspace()));
    }

#ifdef Q_OS_LINUX
    deviceWatcher = new CDeviceWatcherLinux(this);
    connect(deviceWatcher, SIGNAL(sigChanged()), SIGNAL(sigChanged()));
#endif
#ifdef Q_OS_WIN
    deviceWatcher = new CDeviceWatcherWindows(this);
    connect(deviceWatcher, SIGNAL(sigChanged()), SIGNAL(sigChanged()));
#endif

    QTimer::singleShot(500, this, SLOT(slotLoadWorkspace()));
}

CGisListWks::~CGisListWks()
{
}

void CGisListWks::configDB()
{
    QSqlQuery query(db);
    if(!query.exec("PRAGMA locking_mode=EXCLUSIVE"))
    {
        return;
    }

    if(!query.exec("PRAGMA synchronous=OFF"))
    {
        return;
    }

    if(!query.exec("PRAGMA temp_store=MEMORY"))
    {
        return;
    }

    if(!query.exec("PRAGMA default_cache_size=50"))
    {
        return;
    }

    if(!query.exec("PRAGMA page_size=8192"))
    {
        return;
    }

    if(!query.exec("SELECT version FROM versioninfo"))
    {
        initDB();
    }
    else if(query.next())
    {
        int version = query.value(0).toInt();
        if(version != DB_VERSION)
        {
            migrateDB(version);
        }
    }
    else
    {
        initDB();
    }
}

void CGisListWks::initDB()
{
    QSqlQuery query(db);

    if(query.exec( "CREATE TABLE versioninfo ( version TEXT )"))
    {
        query.prepare( "INSERT INTO versioninfo (version) VALUES(:version)");
        query.bindValue(":version", DB_VERSION);
        QUERY_EXEC(; );
    }

    if(!query.exec( "CREATE TABLE workspace ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "type           INTEGER NOT NULL,"
                    "name           TEXT NOT NULL,"
                    "key            TEXT NOT NULL,"
                    "changed        BOOLEAN DEFAULT FALSE,"
                    "data           BLOB NOT NULL"

                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
}

void CGisListWks::migrateDB(int version)
{
    QSqlQuery query(db);

    for(version++; version <= DB_VERSION; version++)
    {
        switch(version)
        {
        }
    }
    query.prepare( "UPDATE versioninfo set version=:version");
    query.bindValue(":version", version - 1);
    QUERY_EXEC(; );
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
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    QTreeWidgetItem * item1 = currentItem();
    QTreeWidgetItem * item2 = itemAt(e->pos());

    // changeing the item order is only valid for single selected items
    if(selectedItems().count() == 1)
    {
        /*
            What's happening here?

            1) Cast current item and item under cursor to GIS item type
            2) If type matches for both test for common parent
            2.1) common parent->  move
            2.1) different parent -> copy
            3) go on with dragMoveEvent();

         */
        CGisItemTrk * trk1 = dynamic_cast<CGisItemTrk*>(item1);
        CGisItemTrk * trk2 = dynamic_cast<CGisItemTrk*>(item2);

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

        CGisItemWpt * wpt1 = dynamic_cast<CGisItemWpt*>(item1);
        CGisItemWpt * wpt2 = dynamic_cast<CGisItemWpt*>(item2);

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

        CGisItemRte * rte1 = dynamic_cast<CGisItemRte*>(item1);
        CGisItemRte * rte2 = dynamic_cast<CGisItemRte*>(item2);

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

        CGisItemOvlArea * area1 = dynamic_cast<CGisItemOvlArea*>(item1);
        CGisItemOvlArea * area2 = dynamic_cast<CGisItemOvlArea*>(item2);

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

        /*
            Never move/copy projects on devices. Data has to be removed or changed
            to store a project and it's items on a device. Moving it back to the
            workspace would conflict with the original project. To much hassle to
            reslove this properly.
         */
        IGisProject * proj1 = dynamic_cast<IGisProject*>(item1);
        if(proj1 && proj1->isOnDevice())
        {
            e->setDropAction(Qt::IgnoreAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }
    }

    /*
        Test for other project, to change project order. But if other project
        is on a device block the request. A project has to be copied to the
        device via it's device item.
     */
    IGisProject * proj2 = dynamic_cast<IGisProject*>(item2);
    if(proj2)
    {
        IGisProject * proj1 = dynamic_cast<IGisProject*>(item1);
        if(proj1)
        {
            e->setDropAction(proj2->isOnDevice() ? Qt::IgnoreAction : Qt::MoveAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }

        IGisItem * gisItem1 = dynamic_cast<IGisItem*>(item1);
        if(gisItem1)
        {
            e->setDropAction(Qt::CopyAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }
    }

    /*
        Test for device as drop target. A device will copy the project into
        it's own supported format.
     */
    IDevice * device = dynamic_cast<IDevice*>(item2);
    if(device)
    {
        IGisProject * proj1 = dynamic_cast<IGisProject*>(item1);
        if(proj1 && !proj1->isOnDevice())
        {
            e->setDropAction(Qt::CopyAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }
    }

    e->setDropAction(Qt::IgnoreAction);
    QTreeWidget::dragMoveEvent(e);
}

void CGisListWks::dropEvent ( QDropEvent  * e )
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items   = selectedItems();
    if(items.isEmpty())
    {
        return;
    }

    int lastResult = CSelectCopyAction::eResultNone;

    // go on with item insertion
    /*
        What's happening here?

        for single selected items do:
        1) Test if item will be inserted above ore below item under cursor.
        2) Cast current item and item under cursor to GIS item type
        3) If type matches for both test for common parent
        3.1) common parent-> go on with default drop event
        3.1) different parent -> create a copy and insert it index
        4) signal change of project

        for single and multiple selected items, do:
        5) Test if item under cursor is a project
        6) If project and project is not item's project create a copy

     */
    if(items.size() == 1)
    {
        // calc. index offset (below/above item)
        QRect r = visualItemRect(itemAt(e->pos()));
        int y1 = r.top() + r.height()/2;
        int y2 = e->pos().y();
        int off = y2 > y1 ? 1 : 0;

        IGisProject * prj1 = dynamic_cast<IGisProject*>(currentItem());
        IGisProject * prj2 = dynamic_cast<IGisProject*>(itemAt(e->pos()));
        if(prj1 && prj2)
        {
            prj2->setFlags(prj2->flags() & ~Qt::ItemIsDropEnabled);
            QTreeWidget::dropEvent(e);
            prj2->setFlags(prj2->flags() |  Qt::ItemIsDropEnabled);
            emit sigChanged();
            return;
        }

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
                    project->insertCopyOfItem(wpt1, off, lastResult);
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
                    project->insertCopyOfItem(trk1, off, lastResult);
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
                    project->insertCopyOfItem(rte1, off, lastResult);
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
                    project->insertCopyOfItem(area1, off, lastResult);
                }
            }
            emit sigChanged();
            return;
        }
    }

    // check if item at position is a project and insert a copy of all selected items
    IGisProject * project = dynamic_cast<IGisProject*>(itemAt(e->pos()));
    if(project)
    {
        int cnt = 1;
        int N   = items.size();
        QProgressDialog progress("Drop items...", "Abort drop", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);

        foreach(QTreeWidgetItem * item, items)
        {
            progress.setValue((100 * cnt++) / N);
            if (progress.wasCanceled())
            {
                break;
            }

            IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
            if(gisItem)
            {
                project->insertCopyOfItem(gisItem, NOIDX, lastResult);
            }
        }
    }

    IDevice * device = dynamic_cast<IDevice*>(itemAt(e->pos()));
    if(device)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
        if(project)
        {
            CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
            if(canvas)
            {
                canvas->reportStatus("device", tr("<b>Update devices</b><p>Update %1<br/>Please wait...</p>").arg(device->text(CGisListWks::eColumnName)));
                canvas->update();
                qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            }

            int lastResult = CSelectCopyAction::eResultNone;
            device->insertCopyOfProject(project, lastResult);

            if(canvas)
            {
                canvas->reportStatus("device", "");
            }
        }
    }

    emit sigChanged();
}

void CGisListWks::removeDevice(const QString& key)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    for(int i = 0; i < topLevelItemCount(); i++)
    {
        IDevice * device = dynamic_cast<IDevice*>(topLevelItem(i));
        if(device && device->getKey() == key)
        {
            delete device;
            emit sigChanged();
            return;
        }
    }
}

bool CGisListWks::hasProject(IGisProject * project)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    QString key = project->getKey();

    for(int i = 0; i < topLevelItemCount(); i++)
    {
        IGisProject * item = dynamic_cast<IGisProject*>(topLevelItem(i));
        if(item && item->getKey() == key)
        {
            if(item != project)
            {
                return true;
            }
        }
    }
    return false;
}

IGisProject * CGisListWks::getProjectByKey(const QString& key)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    for(int i = 0; i < topLevelItemCount(); i++)
    {
        IGisProject * item = dynamic_cast<IGisProject*>(topLevelItem(i));
        if(item && item->getKey() == key)
        {
            return item;
        }
    }
    return 0;
}

CDBProject * CGisListWks::getProjectById(quint64 id, const QString& db)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    for(int i = 0; i < topLevelItemCount(); i++)
    {
        CDBProject * item = dynamic_cast<CDBProject*>(topLevelItem(i));
        if(item && item->getId() == id && item->getDBName() == db)
        {
            return item;
        }
    }
    return 0;
}

void CGisListWks::slotSaveWorkspace()
{
    if(!saveOnExit)
    {
        return;
    }

    QSqlQuery query(db);
    if(!query.exec("DELETE FROM workspace"))
    {
        QUERY_EXEC(return );
    }

    qDebug() << "slotSaveWorkspace()";

    const int total = topLevelItemCount();
    PROGRESS_SETUP(tr("Saving workspace. Please wait."), total);

    for(int i = 0; i < total; i++)
    {
        PROGRESS(i, return );

        IGisProject * project = dynamic_cast<IGisProject*>(topLevelItem(i));
        if(project == 0)
        {
            continue;
        }

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_2);
        stream.setByteOrder(QDataStream::LittleEndian);

        project->IGisProject::operator>>(stream);

        query.prepare("INSERT INTO workspace (type, key, name, changed, data) VALUES (:type, :key, :name, :changed, :data)");
        query.bindValue(":type", project->getType());
        query.bindValue(":key", project->getKey());
        query.bindValue(":name", project->getName());
        query.bindValue(":changed", project->isChanged());
        query.bindValue(":data", data);
        QUERY_EXEC(continue);
    }

    if(saveEvery)
    {
        QTimer::singleShot(saveEvery * 60000, this, SLOT(slotSaveWorkspace()));
    }
}

void CGisListWks::slotLoadWorkspace()
{
    QSqlQuery query(db);

    query.prepare("SELECT type, key, name, changed, data FROM workspace");
    QUERY_EXEC(return );

    PROGRESS_SETUP(tr("Loading workspace. Please wait."), query.size());
    quint32 progCnt = 0;

    while(query.next())
    {
        PROGRESS(progCnt++, return );

        int type        = query.value(0).toInt();
        QString name    = query.value(2).toString();
        bool changed    = query.value(3).toBool();
        QByteArray data = query.value(4).toByteArray();

        QDataStream stream(&data, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_2);
        stream.setByteOrder(QDataStream::LittleEndian);

        IGisProject * project = 0;
        switch(type)
        {
        case IGisProject::eTypeQms:
        {
            project = new CQmsProject(name, this);
            *project << stream;
            break;
        }

        case IGisProject::eTypeGpx:
        {
            project = new CGpxProject(name, this);
            *project << stream;
            break;
        }

        case IGisProject::eTypeDb:
        {
            CDBProject * dbProject;
            project = dbProject = new CDBProject(this);
            project->IGisProject::operator<<(stream);
            dbProject->restoreDBLink();
            if(!project->isValid())
            {
                delete project;
                project = 0;
            }
            else
            {
                dbProject->postStatus();
            }
            break;
        }
        }

        if(project == 0)
        {
            continue;
        }

        project->setToolTip(eColumnName,project->getInfo());
        if(changed)
        {
            project->setChanged();
        }
    }

    emit sigChanged();
}

void CGisListWks::slotContextMenu(const QPoint& point)
{
    QPoint p = mapToGlobal(point);
    if(selectedItems().isEmpty() && menuNone)
    {
        menuNone->exec(p);
        return;
    }

    if(selectedItems().count() > 1)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
        if(project != 0)
        {
            if(project->isOnDevice())
            {
                menuProjectDev->exec(p);
            }
            else
            {
                actionSyncWksDev->setEnabled(IDevice::count());
                menuProjectWks->exec(p);
            }
            return;
        }

        IGisItem * gisItem = dynamic_cast<IGisItem*>(currentItem());
        if(gisItem != 0)
        {
            menuItem->exec(p);
            return;
        }
        return;
    }

    if(selectedItems().count() == 1)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
        if(project != 0)
        {
            if(project->getType() == IGisProject::eTypeLostFound)
            {
                menuProjectTrash->exec(p);
            }
            else
            {
                if(project->isOnDevice())
                {
                    menuProjectDev->exec(p);
                }
                else
                {
                    actionSyncWksDev->setEnabled(IDevice::count());
                    menuProjectWks->exec(p);
                }
            }
            return;
        }

        IGisItem * gisItem = dynamic_cast<IGisItem*>(currentItem());
        if(gisItem != 0)
        {
            bool isOnDevice = gisItem->isOnDevice();

            switch(gisItem->type())
            {
            case IGisItem::eTypeTrk:
                actionCombineTrk->setDisabled(isOnDevice);
                actionRangeTrk->setDisabled(isOnDevice);
                actionReverseTrk->setDisabled(isOnDevice);
                actionEditTrk->setDisabled(isOnDevice);
                actionFocusTrk->setChecked(gisItem->hasUserFocus());
                menuItemTrk->exec(p);
                break;

            case IGisItem::eTypeWpt:
                actionBubbleWpt->setChecked(dynamic_cast<CGisItemWpt*>(gisItem)->hasBubble());
                actionMoveWpt->setDisabled(isOnDevice);
                actionProjWpt->setDisabled(isOnDevice);
                menuItemWpt->exec(p);
                break;

            case IGisItem::eTypeRte:
                menuItemRte->exec(p);
                break;

            case IGisItem::eTypeOvl:
                actionEditArea->setDisabled(isOnDevice);
                menuItemOvl->exec(p);
                break;
            }
            return;
        }
    }
}

void CGisListWks::slotCloseProject()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project != 0)
        {
            if(project->askBeforClose())
            {
                break;
            }
            delete project;
        }
    }
    emit sigChanged();
}

void CGisListWks::slotCloseAllProjects()
{
    int res = QMessageBox::question(this, tr("Close all projects..."), tr("This will remove all projects from the workspace."), QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);
    if(res != QMessageBox::Ok)
    {
        return;
    }


    CGisListWksEditLock lock(true, IGisItem::mutexItems);
    QList<QTreeWidgetItem*> items = findItems("*", Qt::MatchWildcard);
    foreach(QTreeWidgetItem * item, items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project != 0)
        {
            if(project->askBeforClose())
            {
                break;
            }
            delete project;
        }
    }
    emit sigChanged();
}



void CGisListWks::slotDeleteProject()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project != 0)
        {
            int res = QMessageBox::question(&CMainWindow::self(), QObject::tr("Delete project..."), QObject::tr("Do you really want to delete %1?").arg(project->getFilename()), QMessageBox::Ok|QMessageBox::No,QMessageBox::Ok);
            if(res != QMessageBox::Ok)
            {
                continue;
            }


            if(project->remove())
            {
                delete project;
            }
        }
    }
    emit sigChanged();
}

void CGisListWks::slotSaveProject()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project != 0)
        {
            project->save();
        }
    }
}

void CGisListWks::slotSaveAsProject()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project != 0)
        {
            project->saveAs();
        }
    }
}

void CGisListWks::slotEditPrj()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(project != 0)
    {
        project->edit();
    }
}

void CGisListWks::slotItemDoubleClicked(QTreeWidgetItem * item, int )
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);
    IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
    if(gisItem != 0)
    {
        CMainWindow::self().zoomCanvasTo(gisItem->getBoundingRect());
        CGisWidget::self().focusTrkByKey(true, gisItem->getKey());
    }
}

void CGisListWks::slotItemChanged(QTreeWidgetItem * item, int column)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);
    if(column == CGisListDB::eColumnCheckbox)
    {
        emit sigChanged();
    }
}

void CGisListWks::slotEditItem()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    IGisItem * gisItem = dynamic_cast<IGisItem*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().editItemByKey(gisItem->getKey());
    }
}

void CGisListWks::slotDeleteItem()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items       = selectedItems();
    QMessageBox::StandardButtons last   = QMessageBox::NoButton;

    QSet<CDBProject*> projects;

    foreach(QTreeWidgetItem * item, items)
    {
        IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
        if(gisItem != 0)
        {
            bool yes = false;
            IGisProject * project = dynamic_cast<IGisProject*>(gisItem->parent());
            if(project)
            {
                yes = project->delItemByKey(gisItem->getKey(), last);
            }

            /*
                collect database projects to update their counterpart in
                the database view, after all operations are done.
             */
            if(yes && project->getType() == IGisProject::eTypeDb)
            {
                projects << dynamic_cast<CDBProject*>(project);
            }

            if(last == QMessageBox::Cancel)
            {
                break;
            }
        }
    }

    // make all database projects that are changed to post their new status
    // this will update the database view.
    foreach(CDBProject * project, projects)
    {
        if(project)
        {
            project->postStatus();
        }
    }
}

void CGisListWks::slotCopyItem()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    IGisProject * project = CGisWidget::self().selectProject();
    if(project == 0)
    {
        return;
    }

    int lastResult = CSelectCopyAction::eResultNone;

    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
        if(gisItem == 0)
        {
            continue;
        }

        project->insertCopyOfItem(gisItem, NOIDX, lastResult);
    }
}

void CGisListWks::slotProjWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().projWptByKey(gisItem->getKey());
    }
}

void CGisListWks::slotBubbleWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().toggleWptBubble(gisItem->getKey());
    }
}

void CGisListWks::slotMoveWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().moveWptByKey(gisItem->getKey());
    }
}

void CGisListWks::slotFocusTrk(bool on)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().focusTrkByKey(on, gisItem->getKey());
    }
}

void CGisListWks::slotEditTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().editTrkByKey(gisItem->getKey());
    }
}

void CGisListWks::slotReverseTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().reverseTrkByKey(gisItem->getKey());
    }
}

void CGisListWks::slotCombineTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().combineTrkByKey(gisItem->getKey());
    }
}

void CGisListWks::slotRangeTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().rangeTrkByKey(gisItem->getKey());
    }
}

void CGisListWks::slotEditArea()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    CGisItemOvlArea * gisItem = dynamic_cast<CGisItemOvlArea*>(currentItem());
    if(gisItem != 0)
    {
        CGisWidget::self().editAreaByKey(gisItem->getKey());
    }
}


void CGisListWks::slotAddEmptyProject()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QString key, name;
    CSelectProjectDialog::type_e type;
    CSelectProjectDialog dlg(key, name, type, 0);
    dlg.exec();
    if(name.isEmpty() && (type != CSelectProjectDialog::eTypeDb))
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
    else if(type == CSelectProjectDialog::eTypeDb)
    {
        quint64 idParent;
        QString db;
        IDBFolder::type_e type;

        CSelectDBFolder dlg1(idParent, db, this);
        if(dlg1.exec() == QDialog::Rejected)
        {
            return;
        }

        CSetupFolder dlg2(type, name, false, this);
        if(dlg2.exec() == QDialog::Rejected)
        {
            return;
        }

        CEvtW2DCreate * evt = new CEvtW2DCreate(name, type, idParent, db);
        CGisWidget::self().postEventForDb(evt);
    }
}


void CGisListWks::slotSearchGoogle(bool on)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    delete searchGoogle;
    if(on)
    {
        searchGoogle = new CSearchGoogle(this);
    }
}

void CGisListWks::slotSyncWksDev()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(project == 0)
    {
        return;
    }

    QSet<QString> keys;
    if(IDevice::count() > 1)
    {
        CSelDevices dlg(project, this);
        if(dlg.exec() != QDialog::Accepted)
        {
            return;
        }
        dlg.getSlectedDevices(keys);
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    const int N = topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IDevice * device = dynamic_cast<IDevice*>(topLevelItem(n));
        if(device == 0 || (!keys.isEmpty() && !keys.contains(device->getKey())))
        {
            continue;
        }
        if(canvas)
        {
            canvas->reportStatus("device", tr("<b>Update devices</b><p>Update %1<br/>Please wait...</p>").arg(device->text(CGisListWks::eColumnName)));
            canvas->update();
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        device->updateProject(project);
    }
    if(canvas)
    {
        canvas->reportStatus("device", "");
    }
    emit sigChanged();
    QApplication::restoreOverrideCursor();
}

void CGisListWks::slotSyncDevWks()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(project == 0)
    {
        return;
    }

    IDevice * device = dynamic_cast<IDevice*>(project->parent());
    if(device == 0)
    {
        return;
    }

    QString key = project->getKey();

    project = getProjectByKey(key);
    if(project)
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas)
        {
            canvas->reportStatus("device", tr("<b>Update devices</b><p>Update %1<br/>Please wait...</p>").arg(device->text(CGisListWks::eColumnName)));
            canvas->update();
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        device->updateProject(project);

        if(canvas)
        {
            canvas->reportStatus("device", "");
        }
        emit sigChanged();
    }
}


bool CGisListWks::event(QEvent * e)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    switch(e->type())
    {
    case eEvtD2WReqInfo:
    {
        CEvtD2WReqInfo * evt = (CEvtD2WReqInfo*)e;
        CDBProject * project =  getProjectById(evt->id, evt->db);
        if(project)
        {
            project->postStatus();
        }
        e->accept();
        emit sigChanged();
        return true;
    }

    case eEvtD2WShowFolder:
    {
        CEvtD2WShowFolder * evt = (CEvtD2WShowFolder*)e;
        CDBProject * project =  getProjectById(evt->id, evt->db);
        if(project == 0)
        {
            if(evt->id == 0)
            {
                project = new CLostFoundProject(evt->db, this);
            }
            else
            {
                project = new CDBProject(evt->db, evt->id, this);
            }
            if(!project->isValid())
            {
                delete project;
            }
        }
        e->accept();
        emit sigChanged();
        return true;
    }

    case eEvtD2WHideFolder:
    {
        CEvtD2WHideFolder * evt = (CEvtD2WHideFolder*)e;
        CDBProject * project =  getProjectById(evt->id, evt->db);
        delete project;

        e->accept();
        emit sigChanged();
        return true;
    }

    case eEvtD2WShowItems:
    {
        CEvtD2WShowItems * evt = (CEvtD2WShowItems*)e;
        CDBProject * project =  getProjectById(evt->id, evt->db);
        if(project)
        {
            project->showItems(evt);
        }
        e->accept();
        emit sigChanged();
        return true;
    }

    case eEvtD2WHideItems:
    {
        CEvtD2WHideItems * evt = (CEvtD2WHideItems*)e;
        CDBProject * project =  getProjectById(evt->id, evt->db);
        if(project)
        {
            project->hideItems(evt);
        }
        e->accept();
        emit sigChanged();
        return true;
    }

    case eEvtD2WUpdateLnF:
    {
        CEvtD2WUpdateLnF * evt = (CEvtD2WUpdateLnF*)e;
        CLostFoundProject * project = dynamic_cast<CLostFoundProject*>(getProjectById(evt->id, evt->db));
        if(project)
        {
            project->updateFromDb();
        }
        e->accept();
        emit sigChanged();
        return true;
    }
    }

    return QTreeWidget::event(e);
}

