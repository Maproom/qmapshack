/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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
#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
#include "device/CDeviceWatcherLinux.h"
#endif
#ifdef Q_OS_WIN
#include "device/CDeviceWatcherWindows.h"
#endif
#ifdef Q_OS_MAC
#include "device/CDeviceWatcherMac.h"
#endif
#include "device/IDevice.h"
#include "gis/CGisDatabase.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/CSelDevices.h"
#include "gis/IGisItem.h"
#include "gis/db/CDBProject.h"
#include "gis/db/CLostFoundProject.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/CSetupFolder.h"
#include "gis/db/macros.h"
#include "gis/fit/CFitProject.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/qlb/CQlbProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/search/CSearchGoogle.h"
#include "gis/slf/CSlfProject.h"
#include "gis/suunto/CLogProject.h"
#include "gis/suunto/CSmlProject.h"
#include "gis/tcx/CTcxProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSelectProjectDialog.h"
#include "helpers/CSettings.h"
#include "helpers/CWptIconDialog.h"
#include "setup/IAppSetup.h"

#include <QApplication>
#include <QtSql>
#include <QtWidgets>

using std::bind;

#undef  DB_VERSION
#define DB_VERSION 3

class CGisListWksEditLock
{
public:
    CGisListWksEditLock(bool waitCursor, QMutex& mutex) : mutex(mutex), waitCursor(waitCursor)
    {
        if(waitCursor)
        {
            CCanvas::setOverrideCursor(Qt::WaitCursor, "CGisListWksEditLock");
        }
        mutex.lock();
    }
    ~CGisListWksEditLock()
    {
        if(waitCursor)
        {
            CCanvas::restoreOverrideCursor("~CGisListWksEditLock");
        }
        mutex.unlock();
    }
private:
    QMutex& mutex;
    bool waitCursor;
};

CGisListWks::CGisListWks(QWidget *parent)
    : QTreeWidget(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE","Workspace1");
    QString config = QDir(IAppSetup::getPlatformInstance()->userDataPath()).filePath("workspace.db");
    db.setDatabaseName(config);
    db.open();
    configDB();

    menuProjectWks   = new QMenu(this);
    actionEditPrj    = menuProjectWks->addAction(QIcon("://icons/32x32/EditDetails.png"), tr("Edit.."         ), this, SLOT(slotEditPrj()));
    actionCopyPrj    = menuProjectWks->addAction(QIcon("://icons/32x32/Copy.png"       ), tr("Copy to..."     ), this, SLOT(slotCopyProject()));
    actionShowOnMap  = menuProjectWks->addAction(QIcon("://icons/32x32/ShowAll.png"    ), tr("Show on Map"    ), this, SLOT(slotShowOnMap()));
    actionHideFrMap  = menuProjectWks->addAction(QIcon("://icons/32x32/ShowNone.png"   ), tr("Hide from Map"  ), this, SLOT(slotHideFrMap()));

    menuProjectWks->addSeparator();
    actionGroup = new QActionGroup(menuProjectWks);
    actionGroup->setExclusive(true);
    actionSortByTime = addSortAction(menuProjectWks, actionGroup, "://icons/32x32/Time.png", tr("Sort by Time"), IGisProject::eSortFolderTime);
    actionSortByName = addSortAction(menuProjectWks, actionGroup, "://icons/32x32/SortName.png", tr("Sort by Name"), IGisProject::eSortFolderName);

    menuProjectWks->addSeparator();
    actionAutoSave   = menuProjectWks->addAction(QIcon("://icons/32x32/AutoSave.png"), tr("Autom. Save"));
    actionAutoSave->setCheckable(true);

    menuProjectWks->addSeparator();
    actionSave       = menuProjectWks->addAction(QIcon("://icons/32x32/SaveGIS.png"    ), tr("Save"           ), this, SLOT(slotSaveProject()));
    actionSaveAs     = menuProjectWks->addAction(QIcon("://icons/32x32/SaveGISAs.png"  ), tr("Save as..."     ), this, SLOT(slotSaveAsProject()));
    actionSaveAsStrict = menuProjectWks->addAction(QIcon("://icons/32x32/SaveGISAsGpx11.png"), tr("Save as GPX 1.1 w/o ext..."), this, SLOT(slotSaveAsStrictGpx11Project()));

    menuProjectWks->addSeparator();
    actionSyncWksDev = menuProjectWks->addAction(QIcon("://icons/32x32/Device.png"     ), tr("Send to Devices"), this, SLOT(slotSyncWksDev()));
    actionSyncDB     = menuProjectWks->addAction(QIcon("://icons/32x32/DatabaseSync.png"      ), tr("Sync. with Database"), this, SLOT(slotSyncDB()));

    menuProjectWks->addSeparator();
    actionCloseProj = menuProjectWks->addAction(QIcon("://icons/32x32/Close.png"       ), tr("Close"          ), this, SLOT(slotCloseProject()));

    menuProjectDev  = new QMenu(this);
    menuProjectDev->addAction(actionEditPrj);
    menuProjectDev->addAction(actionCopyPrj);
    menuProjectDev->addAction(actionShowOnMap);
    menuProjectDev->addAction(actionHideFrMap);
    menuProjectDev->addSeparator();
    menuProjectDev->addSeparator();
    actionSyncDevWks= menuProjectDev->addAction(QIcon("://icons/32x32/Device.png"),tr("Update Project on Device"), this, SLOT(slotSyncDevWks()));
    menuProjectDev->addSeparator();
    actionDelProj   = menuProjectDev->addAction(QIcon("://icons/32x32/DeleteOne.png"),tr("Delete"),                this, SLOT(slotDeleteProject()));

    menuProjectTrash= new QMenu(this);
    menuProjectTrash->addAction(actionSaveAs);
    menuProjectTrash->addAction(actionSaveAsStrict);
    menuProjectTrash->addAction(actionCloseProj);

    connect(this, &CGisListWks::customContextMenuRequested, this, &CGisListWks::slotContextMenu);
    connect(this, &CGisListWks::itemDoubleClicked,          this, &CGisListWks::slotItemDoubleClicked);
    connect(this, &CGisListWks::itemChanged,                this, &CGisListWks::slotItemChanged);

    menuItemTrk      = new QMenu(this);
    actionEditDetails = menuItemTrk->addAction(QIcon("://icons/32x32/EditDetails.png"),tr("Edit..."), this, SLOT(slotEditItem()));
    actionCopyItem    = menuItemTrk->addAction(QIcon("://icons/32x32/Copy.png"),tr("Copy to..."),     this, SLOT(slotCopyItem()));
    menuItemTrk->addSeparator();
    actionFocusTrk   = menuItemTrk->addAction(QIcon("://icons/32x32/TrkProfile.png"),tr("Track Profile"));
    actionFocusTrk->setCheckable(true);
    actionRangeTrk   = menuItemTrk->addAction(QIcon("://icons/32x32/SelectRange.png"), tr("Select Range"           ), this, SLOT(slotRangeTrk()));
    actionEditTrk    = menuItemTrk->addAction(QIcon("://icons/32x32/LineMove.png"),    tr("Edit Track Points"      ), this, SLOT(slotEditTrk()));
    actionReverseTrk = menuItemTrk->addAction(QIcon("://icons/32x32/Reverse.png"),     tr("Reverse Track"          ), this, SLOT(slotReverseTrk()));
    actionCombineTrk = menuItemTrk->addAction(QIcon("://icons/32x32/Combine.png"),     tr("Combine Tracks"         ), this, SLOT(slotCombineTrk()));
    actionActivityTrk= menuItemTrk->addAction(QIcon("://icons/32x32/Activity.png"), tr("Set Track Activity"), this, SLOT(slotActivityTrk()));
    actionCopyTrkWithWpt = menuItemTrk->addAction(QIcon("://icons/32x32/CopyTrkWithWpt.png"), tr("Copy Track with Waypoints"), this, SLOT(slotCopyTrkWithWpt()));
    actionNogoTrk    = menuItemTrk->addAction(QIcon("://icons/32x32/NoGoTrack.png"),   tr("Toggle Nogo-Line"       ), this, SLOT(slotNogoItem()));
    actionNogoTrk->setCheckable(true);
    menuItemTrk->addSeparator();
    actionDelete    = menuItemTrk->addAction(QIcon("://icons/32x32/DeleteOne.png"),tr("Delete"), this, SLOT(slotDeleteItem()));
    connect(menuItemTrk, &QMenu::triggered, &CGisWorkspace::self(), &CGisWorkspace::slotWksItemSelectionReset);

    menuItemWpt     = new QMenu(this);
    menuItemWpt->addAction(actionEditDetails);
    menuItemWpt->addAction(actionCopyItem);
    menuItemWpt->addSeparator();
    actionBubbleWpt = menuItemWpt->addAction(QIcon("://icons/32x32/Bubble.png"),  tr("Show Bubble"),       this, SLOT(slotBubbleWpt()));
    actionBubbleWpt->setCheckable(true);
    actionMoveWpt   = menuItemWpt->addAction(QIcon("://icons/32x32/WptMove.png"), tr("Move Waypoint"),     this, SLOT(slotMoveWpt()));
    actionProjWpt   = menuItemWpt->addAction(QIcon("://icons/32x32/WptProj.png"), tr("Proj. Waypoint..."), this, SLOT(slotProjWpt()));
    actionEditRadiusWpt = menuItemWpt->addAction(QIcon("://icons/32x32/WptEditProx.png"), tr("Change Radius"), this, SLOT(slotEditRadiusWpt()));
    actionNogoWpt = menuItemWpt->addAction(QIcon("://icons/32x32/WptAvoid.png"),  tr("Toggle Nogo-Area"),  this, SLOT(slotNogoItem()));
    actionNogoWpt->setCheckable(true);
    actionDelRadiusWpt = menuItemWpt->addAction(QIcon("://icons/32x32/WptDelProx.png"), tr("Delete Radius"), this, SLOT(slotDelRadiusWpt()));
    menuItemWpt->addSeparator();
    menuItemWpt->addAction(actionDelete);
    connect(menuItemWpt, &QMenu::triggered, &CGisWorkspace::self(), &CGisWorkspace::slotWksItemSelectionReset);

    menuItemRte     = new QMenu(this);
    menuItemRte->addAction(actionEditDetails);
    menuItemRte->addAction(actionCopyItem);
    menuItemRte->addSeparator();
    actionFocusRte   = menuItemRte->addAction(QIcon("://icons/32x32/RteInstr.png"), tr("Route Instructions"));
    actionFocusRte->setCheckable(true);
    actionCalcRte    = menuItemRte->addAction(QIcon("://icons/32x32/Apply.png"),    tr("Calculate Route"), this, SLOT(slotCalcRte()));
    actionResetRte   = menuItemRte->addAction(QIcon("://icons/32x32/Reset.png"),    tr("Reset Route"),     this, SLOT(slotResetRte()));
    actionEditRte    = menuItemRte->addAction(QIcon("://icons/32x32/LineMove.png"), tr("Edit Route"),      this, SLOT(slotEditRte()));
    actionReverseRte = menuItemRte->addAction(QIcon("://icons/32x32/Reverse.png"),  tr("Reverse Route"),   this, SLOT(slotReverseRte()));
    actionRte2Trk    = menuItemRte->addAction(QIcon("://icons/32x32/Track.png"),    tr("Convert to Track"),this, SLOT(slotRte2Trk()));
    actionNogoRte    = menuItemRte->addAction(QIcon("://icons/32x32/NoGoRoute.png"),tr("Toggle Nogo-Line"),this, SLOT(slotNogoItem()));
    actionNogoRte->setCheckable(true);
    menuItemRte->addSeparator();
    menuItemRte->addAction(actionDelete);
    connect(menuItemRte, &QMenu::triggered, &CGisWorkspace::self(), &CGisWorkspace::slotWksItemSelectionReset);

    menuItemOvl     = new QMenu(this);
    menuItemOvl->addAction(actionEditDetails);
    menuItemOvl->addAction(actionCopyItem);
    menuItemOvl->addSeparator();
    actionEditArea  = menuItemOvl->addAction(QIcon("://icons/32x32/AreaMove.png"),tr("Edit Area Points"), this, SLOT(slotEditArea()));
    actionNogoArea  = menuItemOvl->addAction(QIcon("://icons/32x32/NoGoArea.png"),tr("Toggle Nogo-Area"), this, SLOT(slotNogoItem()));
    actionNogoArea->setCheckable(true);
    menuItemOvl->addSeparator();
    menuItemOvl->addAction(actionDelete);
    connect(menuItemOvl, &QMenu::triggered, &CGisWorkspace::self(), &CGisWorkspace::slotWksItemSelectionReset);

    menuItem        = new QMenu(this);
    menuItem->addAction(actionCopyItem);
    actionRteFromWpt = menuItem->addAction(QIcon("://icons/32x32/Route.png"), tr("Create Route"), this, SLOT(slotRteFromWpt()));
    actionSymWpt    = menuItem->addAction(QIcon("://icons/waypoints/32x32/PinBlue.png"), tr("Change Icon (sel. waypt. only)"), this, SLOT(slotSymWpt()));
    menuItem->addAction(actionCombineTrk);
    menuItem->addAction(actionActivityTrk);
    menuItem->addAction(actionDelete);
    connect(menuItem, &QMenu::triggered, &CGisWorkspace::self(), &CGisWorkspace::slotWksItemSelectionReset);

    connect(actionFocusTrk, &QAction::triggered, this, &CGisListWks::slotFocusTrk);
    connect(actionFocusRte, &QAction::triggered, this, &CGisListWks::slotFocusRte);
    connect(actionAutoSave, &QAction::triggered, this, &CGisListWks::slotAutoSaveProject);
    connect(qApp, &QApplication::aboutToQuit, this, &CGisListWks::slotSaveWorkspace);

    SETTINGS;
    saveOnExit  = cfg.value("Database/saveOnExit", saveOnExit).toBool();
    saveEvery   = cfg.value("Database/saveEvery",  saveEvery).toInt();

    if(saveOnExit && (saveEvery > 0))
    {
        QTimer::singleShot(saveEvery * 60000, this, SLOT(slotSaveWorkspace()));
    }

#ifdef Q_OS_MAC
    deviceWatcher = new CDeviceWatcherMac(this);
    connect(deviceWatcher, &CDeviceWatcherMac::sigChanged, this, &CGisListWks::sigChanged);
#else
    #ifdef Q_OS_WIN
    deviceWatcher = new CDeviceWatcherWindows(this);
    connect(deviceWatcher, &CDeviceWatcherWindows::sigChanged, this, &CGisListWks::sigChanged);
    #else
        #ifdef HAVE_DBUS
    deviceWatcher = new CDeviceWatcherLinux(this);
    connect(deviceWatcher, &CDeviceWatcherLinux::sigChanged, this, &CGisListWks::sigChanged);
        #endif // HAVE_DBUS
    #endif // Q_OS_WIN
#endif // Q_OS_MAC
}

CGisListWks::~CGisListWks()
{
}

void CGisListWks::configDB()
{
    QSqlQuery query(db);

    QUERY_RUN("PRAGMA locking_mode=EXCLUSIVE", return )
    QUERY_RUN("PRAGMA synchronous=OFF",        return )
    QUERY_RUN("PRAGMA temp_store=MEMORY",      return )
    QUERY_RUN("PRAGMA default_cache_size=50",  return )
    QUERY_RUN("PRAGMA page_size=8192",         return )

    // When migrating the database these tables are used.
    // Due to caching they can't be dropped right after the
    // migration. That is why we look for them on startup.
    // And delete them as a second chance.
    if(query.exec("select * from tmp_workspace"))
    {
        QUERY_RUN("DROP TABLE tmp_workspace;", return );
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
        QUERY_EXEC();
    }

    QUERY_RUN("CREATE TABLE workspace ("
              "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
              "type           INTEGER NOT NULL,"
              "name           TEXT NOT NULL,"
              "keyqms         TEXT NOT NULL,"
              "changed        BOOLEAN DEFAULT FALSE,"
              "visible        BOOLEAN DEFAULT TRUE,"
              "data           BLOB NOT NULL"
              ")", NO_CMD)
}

void CGisListWks::migrateDB(int version)
{
    qDebug() << "workspace.db has version " << version << ", migration to version " << DB_VERSION << " required";

    // try to migrate between the database versions step by step (as soon as applicable)
    if(version < 2)
    {
        migrateDB1to2();
    }
    if(version < 3)
    {
        migrateDB2to3();
    }

    // save the new version to the database
    QSqlQuery query(db);
    query.prepare( "UPDATE versioninfo set version=:version");
    query.bindValue(":version", DB_VERSION);
    QUERY_EXEC();
}

void CGisListWks::migrateDB1to2()
{
    qDebug() << "migrating workspace.db from version 1 to version 2";
    // add a new column `visible` to the database
    // the default value is `true`, as - by default in older versions of QMS - all saved projects
    // have been loaded and shown on the map directly after starting
    QSqlQuery query(db);
    QUERY_RUN("ALTER TABLE workspace ADD COLUMN visible BOOLEAN DEFAULT TRUE;", NO_CMD)
}

void CGisListWks::migrateDB2to3()
{
    QSqlQuery query(db);

    QUERY_RUN("BEGIN TRANSACTION;",                             return )
    QUERY_RUN("ALTER TABLE workspace RENAME TO tmp_workspace;", return )
    QUERY_RUN("CREATE TABLE workspace ("
              "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
              "type           INTEGER NOT NULL,"
              "name           TEXT NOT NULL,"
              "keyqms         TEXT NOT NULL,"
              "changed        BOOLEAN DEFAULT FALSE,"
              "visible        BOOLEAN DEFAULT TRUE,"
              "data           BLOB NOT NULL"
              ")", return );
    QUERY_RUN("INSERT INTO workspace(id,type,name,keyqms,changed,visible,data) SELECT * FROM tmp_workspace;", return )
    QUERY_RUN("COMMIT;",                                                                                      return )
    QUERY_RUN("DROP TABLE tmp_workspace;",                                                                    return )
}

void CGisListWks::setExternalMenu(QMenu * project)
{
    menuNone = project;
    connect(CMainWindow::self().findChild<QAction*>("actionAddEmptyProject"),  &QAction::triggered, this, &CGisListWks::slotAddEmptyProject);
    connect(CMainWindow::self().findChild<QAction*>("actionCloseAllProjects"), &QAction::triggered, this, &CGisListWks::slotCloseAllProjects);
    connect(CMainWindow::self().findChild<QAction*>("actionSearchGoogle"),     &QAction::triggered, this, &CGisListWks::slotSearchGoogle);
}

QAction * CGisListWks::addSortAction(QMenu * menu, QActionGroup * actionGroup, const QString& icon, const QString& text, IGisProject::sorting_folder_e mode)
{
    QAction * action = menu->addAction(QIcon(icon), text);
    action->setCheckable(true);

    auto func = std::bind(&CGisListWks::slotSetSortMode, this, mode, std::placeholders::_1);
    connect(action, &QAction::toggled, this, func);

    actionGroup->addAction(action);

    return action;
}

void CGisListWks::dragMoveEvent(QDragMoveEvent  * e )
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    QTreeWidgetItem * item1 = currentItem();
    QTreeWidgetItem * item2 = itemAt(e->pos());

    // changing the item order is only valid for single selected items
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
            e->setDropAction( trk1->parent() == trk2->parent() ? Qt::MoveAction : Qt::CopyAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }

        CGisItemWpt * wpt1 = dynamic_cast<CGisItemWpt*>(item1);
        CGisItemWpt * wpt2 = dynamic_cast<CGisItemWpt*>(item2);

        if(wpt1 && wpt2)
        {
            e->setDropAction( wpt1->parent() == wpt2->parent() ? Qt::MoveAction : Qt::CopyAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }

        CGisItemRte * rte1 = dynamic_cast<CGisItemRte*>(item1);
        CGisItemRte * rte2 = dynamic_cast<CGisItemRte*>(item2);

        if(rte1 && rte2)
        {
            e->setDropAction( rte1->parent() == rte2->parent() ? Qt::MoveAction : Qt::CopyAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }

        CGisItemOvlArea * area1 = dynamic_cast<CGisItemOvlArea*>(item1);
        CGisItemOvlArea * area2 = dynamic_cast<CGisItemOvlArea*>(item2);

        if(area1 && area2)
        {
            e->setDropAction( area1->parent() == area2->parent() ? Qt::MoveAction : Qt::CopyAction);
            QTreeWidget::dragMoveEvent(e);
            return;
        }

        /*
            Never move/copy projects on devices. Data has to be removed or changed
            to store a project and it's items on a device. Moving it back to the
            workspace would conflict with the original project. To much hassle to
            resolve this properly.
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

void CGisListWks::dropEvent( QDropEvent  * e )
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
        project->blockUpdateItems(true);

        int cnt = 1;
        int N   = items.size();
        PROGRESS_SETUP(tr("Drop items..."), 0, N, this);

        for(QTreeWidgetItem * item : items)
        {
            PROGRESS(cnt++, break);

            IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
            if(gisItem)
            {
                project->insertCopyOfItem(gisItem, NOIDX, lastResult);
            }
        }

        project->blockUpdateItems(false);
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

void CGisListWks::addProject(IGisProject *proj)
{
    if(!proj->isValid())
    {
        return;
    }

    addTopLevelItem(proj);

    // move project up the list until there a re only projects, no devices
    int newIdx      = NOIDX;
    const int myIdx = topLevelItemCount() - 1;
    for(int i = myIdx - 1; i >= 0; i--)
    {
        IDevice * device = dynamic_cast<IDevice*>(topLevelItem(i));
        if(nullptr == device)
        {
            break;
        }

        newIdx = i;
    }

    if(newIdx != NOIDX)
    {
        takeTopLevelItem(myIdx);
        insertTopLevelItem(newIdx, proj);
    }
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
        if(item && item->getKey() == key && item != project)
        {
            return true;
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
    return nullptr;
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
    return nullptr;
}

void CGisListWks::slotSaveWorkspace()
{
    CGisListWksEditLock lock(true,IGisItem::mutexItems);

    if(!saveOnExit)
    {
        return;
    }

    QSqlQuery query(db);
    QUERY_RUN("DELETE FROM workspace", return )

    qDebug() << "slotSaveWorkspace()";

    const int total = topLevelItemCount();
    PROGRESS_SETUP(tr("Saving workspace. Please wait."), 0, total, this);

    for(int i = 0; i < total; i++)
    {
        PROGRESS(i, return );

        IGisProject * project = dynamic_cast<IGisProject*>(topLevelItem(i));
        if(nullptr == project)
        {
            continue;
        }

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_2);
        stream.setByteOrder(QDataStream::LittleEndian);

        project->IGisProject::operator>>(stream);

        query.prepare("INSERT INTO workspace (type, keyqms, name, changed, visible, data) VALUES (:type, :keyqms, :name, :changed, :visible, :data)");
        query.bindValue(":type",    project->getType());
        query.bindValue(":keyqms",  project->getKey());
        query.bindValue(":name",    project->getName());
        query.bindValue(":changed", project->isChanged());

        bool visible = (project->checkState(CGisListDB::eColumnCheckbox) == Qt::Checked);
        query.bindValue(":visible", visible);
        query.bindValue(":data",    data);
        QUERY_EXEC(continue);
    }

    if(saveEvery)
    {
        QTimer::singleShot(saveEvery * 60000, this, SLOT(slotSaveWorkspace()));
    }
}

void CGisListWks::slotLoadWorkspace()
{
    CGisListWksEditLock lock(true,IGisItem::mutexItems);

    QSqlQuery query(db);

    QUERY_RUN("SELECT type, keyqms, name, changed, visible, data FROM workspace", return )

    const int total = query.size();
    PROGRESS_SETUP(tr("Loading workspace. Please wait."), 0, total, this);
    quint32 progCnt = 0;

    while(query.next())
    {
        PROGRESS(progCnt++, return );

        int type               = query.value(0).toInt();
        QString name           = query.value(2).toString();
        bool changed           = query.value(3).toBool();
        Qt::CheckState visible = query.value(4).toBool() ? Qt::Checked : Qt::Unchecked;
        QByteArray data        = query.value(5).toByteArray();

        QDataStream stream(&data, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_2);
        stream.setByteOrder(QDataStream::LittleEndian);

        IGisProject *project = nullptr;
        switch(type)
        {
        case IGisProject::eTypeQms:
        {
            project = new CQmsProject(name, this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible); // (1a)
            *project << stream;
            break;
        }

        case IGisProject::eTypeQlb:
        {
            project = new CQlbProject(name, this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible); // (1a)
            *project << stream;
            break;
        }

        case IGisProject::eTypeGpx:
        {
            project = new CGpxProject(name, this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible); // (1b)
            *project << stream;
            break;
        }

        case IGisProject::eTypeDb:
        {
            CDBProject * dbProject;
            project = dbProject = new CDBProject(this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible); // (1c)

            project->IGisProject::operator<<(stream);
            dbProject->restoreDBLink();

            if(!project->isValid())
            {
                delete project;
                project = nullptr;
            }
            else
            {
                dbProject->postStatus(false);
            }
            break;
        }

        case IGisProject::eTypeSlf:
        {
            project = new CSlfProject(name, false);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible); // (1d)
            *project << stream;

            // the CSlfProject does not - as the other C*Project - register itself in the list
            // of currently opened projects. This is done manually here.
            addProject(project);
            break;
        }

        case IGisProject::eTypeFit:
        {
            project = new CFitProject(name, this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible);
            *project << stream;
            break;
        }

        case IGisProject::eTypeTcx:
        {
            project = new CTcxProject(name, this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible);
            *project << stream;
            break;
        }

        case IGisProject::eTypeSml:
        {
            project = new CSmlProject(name, this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible);
            *project << stream;
            break;
        }

        case IGisProject::eTypeLog:
        {
            project = new CSmlProject(name, this);
            project->setCheckState(CGisListDB::eColumnCheckbox, visible);
            *project << stream;
            break;
        }
        }

        if(nullptr != project)
        {
            // Hiding the individual projects from the map (1a, 1b, 1c) could be done here within a single statement,
            // but this results in a visible `the checkbox is being unchecked`, especially in case the project
            // is large and takes some time to load.
            // When done directly after construction there is no `blinking` of the check mark

            project->setToolTip(eColumnName,project->getInfo());
            if(changed)
            {
                project->setChanged();
            }
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

    // check whether all projects are checked or unchecked...
    bool allChecked   = true;
    bool allUnchecked = true;
    bool allCantSave  = true;

    for(QTreeWidgetItem *item : selectedItems())
    {
        IGisProject *project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            // as soon as we find an unchecked element, not all elements are checked (and vice versa)
            if(project->checkState(CGisListDB::eColumnCheckbox) == Qt::Unchecked)
            {
                allChecked = false;
            }
            else
            {
                allUnchecked = false;
            }

            if(project->canSave())
            {
                allCantSave = false;
            }
        }
    }

    // ...and disable entries without any effect
    actionShowOnMap->setEnabled(!allChecked);
    actionHideFrMap->setEnabled(!allUnchecked);
    actionSave->setEnabled(!allCantSave);

    if(selectedItems().count() > 1)
    {
        IGisProject *project = dynamic_cast<IGisProject*>(currentItem());
        if(nullptr != project)
        {
            if(project->isOnDevice())
            {
                menuProjectDev->exec(p);
            }
            else
            {
                actionGroup->setEnabled(false);
                actionSyncWksDev->setEnabled(IDevice::count());
                actionSyncDB->setEnabled(project->getType() == IGisProject::eTypeDb);
                actionAutoSave->setVisible(false);
                menuProjectWks->exec(p);
            }
            return;
        }

        IGisItem *gisItem = dynamic_cast<IGisItem*>(currentItem());
        if(nullptr != gisItem)
        {
            bool hasWpts  = false;
            bool onlyWpts = true;
            bool onlyTrks = true;
            for(QTreeWidgetItem *item : selectedItems())
            {
                if(item->type() != IGisItem::eTypeWpt)
                {
                    onlyWpts = false;
                }
                else
                {
                    hasWpts = true;
                }

                if(item->type() != IGisItem::eTypeTrk)
                {
                    onlyTrks = false;
                }

                if(!onlyTrks && !onlyWpts)
                {
                    break;
                }
            }

            actionRteFromWpt->setEnabled(onlyWpts);
            actionCombineTrk->setEnabled(onlyTrks);
            actionActivityTrk->setEnabled(onlyTrks);
            actionSymWpt->setEnabled(hasWpts);

            menuItem->exec(p);
            return;
        }
        return;
    }

    if(selectedItems().count() == 1)
    {
        IGisProject *project = dynamic_cast<IGisProject*>(currentItem());
        if(nullptr != project)
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
                    actionGroup->setEnabled(true);
                    actionSyncWksDev->setEnabled(IDevice::count());
                    actionSyncDB->setEnabled(project->getType() == IGisProject::eTypeDb);

                    blockSorting = true;
                    switch(project->getSortingFolder())
                    {
                    case IGisProject::eSortFolderName:
                        actionSortByName->setChecked(true);
                        break;

                    case IGisProject::eSortFolderTime:
                        actionSortByTime->setChecked(true);
                        break;
                    }

                    blockSorting = false;

                    actionAutoSave->setVisible(true);
                    actionAutoSave->setEnabled(project->canSave());
                    actionAutoSave->setChecked(project->isAutoSave());

                    menuProjectWks->exec(p);
                }
            }
            return;
        }

        IGisItem *gisItem = dynamic_cast<IGisItem*>(currentItem());
        if(nullptr != gisItem)
        {
            bool isOnDevice = gisItem->isOnDevice();
            IGisProject * project = gisItem->getParentProject();
            bool isProjectVisible = project == nullptr ? false : project->isVisible();

            switch(gisItem->type())
            {
            case IGisItem::eTypeTrk:
            {
                CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(gisItem);
                if(project != nullptr)
                {
                    actionCombineTrk->setEnabled(project->getItemCountByType(IGisItem::eTypeTrk) > 1);
                }
                else
                {
                    actionCombineTrk->setEnabled(false);
                }
                actionRangeTrk->setEnabled(isProjectVisible && !isOnDevice);
                actionReverseTrk->setDisabled(isOnDevice);
                actionEditTrk->setEnabled(isProjectVisible && !isOnDevice);
                actionNogoTrk->setEnabled(isProjectVisible);
                actionNogoTrk->setChecked(gisItem->isNogo());
                actionCopyTrkWithWpt->setEnabled(trk->getNumberOfAttachedWpt() != 0);
                actionFocusTrk->setChecked(gisItem->hasUserFocus());
                actionFocusTrk->setEnabled(isProjectVisible);
                menuItemTrk->exec(p);
                break;
            }

            case IGisItem::eTypeWpt:
            {
                CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(gisItem);
                actionBubbleWpt->setChecked(wpt->hasBubble());
                actionBubbleWpt->setEnabled(isProjectVisible);
                actionEditRadiusWpt->setEnabled(isProjectVisible);
                bool radius = wpt->hasRadius();
                actionDelRadiusWpt->setEnabled(isProjectVisible && radius);
                actionNogoWpt->setEnabled(isProjectVisible && radius);
                actionNogoWpt->setChecked(radius && wpt->isNogo());
                actionMoveWpt->setEnabled(isProjectVisible && !isOnDevice);
                actionProjWpt->setDisabled(isOnDevice);
                menuItemWpt->exec(p);
                break;
            }

            case IGisItem::eTypeRte:
                actionFocusRte->setChecked(gisItem->hasUserFocus());
                actionFocusRte->setEnabled(isProjectVisible);
                actionCalcRte->setEnabled(isProjectVisible);
                actionEditRte->setEnabled(isProjectVisible);
                actionNogoRte->setEnabled(isProjectVisible);
                actionNogoRte->setChecked(gisItem->isNogo());
                actionResetRte->setEnabled(isProjectVisible);
                menuItemRte->exec(p);
                break;

            case IGisItem::eTypeOvl:
                actionEditArea->setEnabled(isProjectVisible && !isOnDevice);
                actionNogoArea->setEnabled(isProjectVisible);
                actionNogoArea->setChecked(gisItem->isNogo());
                menuItemOvl->exec(p);
                break;
            }

            return;
        }
    }
}

void CGisListWks::setVisibilityOnMap(bool visible)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);
    QList<QTreeWidgetItem*> items = selectedItems();
    for(QTreeWidgetItem *item : items)
    {
        IGisProject *project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            project->setCheckState(CGisListDB::eColumnCheckbox, visible ? Qt::Checked : Qt::Unchecked);
        }
    }
    emit sigChanged();
}

void CGisListWks::slotShowOnMap()
{
    setVisibilityOnMap(true);
}

void CGisListWks::slotHideFrMap()
{
    setVisibilityOnMap(false);
}

static void closeProjects(const QList<QTreeWidgetItem*> &items)
{
    for(QTreeWidgetItem * item : items)
    {
        IGisProject *project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            if(project->askBeforClose())
            {
                break;
            }

            if(IGisProject::eTypeGoogle == project->getType())
            {
                CMainWindow::self().findChild<QAction*>("actionSearchGoogle")->setChecked(false);
            }
            delete project;
        }
    }
}

void CGisListWks::slotCloseProject()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    closeProjects(selectedItems());
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
    closeProjects(findItems("*", Qt::MatchWildcard));

    CGisWorkspace::self().slotWksItemSelectionReset();

    emit sigChanged();
}


void CGisListWks::slotDeleteProject()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items = selectedItems();
    for(QTreeWidgetItem * item : items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            CCanvas::setOverrideCursor(Qt::ArrowCursor, "slotDeleteProject");
            int res = QMessageBox::question(CMainWindow::getBestWidgetForParent(), tr("Delete project..."), tr("Do you really want to delete %1?").arg(project->getFilename()), QMessageBox::Ok|QMessageBox::No,QMessageBox::Ok);
            CCanvas::restoreOverrideCursor("slotDeleteProject");
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
    for(QTreeWidgetItem * item : items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            if(project->canSave())
            {
                project->save();
            }
            else
            {
                project->saveAs();
            }
        }
    }
}

void CGisListWks::slotSaveAsProject()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items = selectedItems();
    for(QTreeWidgetItem * item : items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            project->saveAs();
        }
    }
}

void CGisListWks::slotSaveAsStrictGpx11Project()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items = selectedItems();
    for(QTreeWidgetItem * item : items)
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(nullptr != project)
        {
            project->saveAsStrictGpx11();
        }
    }
}

void CGisListWks::slotAutoSaveProject(bool on)
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(project != nullptr)
    {
        project->setAutoSave(on);
    }
}

void CGisListWks::slotEditPrj()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(project != nullptr)
    {
        project->edit();
    }
}

void CGisListWks::slotItemDoubleClicked(QTreeWidgetItem * item, int )
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().slotWksItemSelectionReset();
        IGisProject * project = gisItem->getParentProject();
        if (project != nullptr && project->isVisible())
        {
            CMainWindow::self().zoomCanvasTo(gisItem->getBoundingRect());
            CGisWorkspace::self().focusTrkByKey(true, gisItem->getKey());
        }
    }
}

void CGisListWks::slotItemChanged(QTreeWidgetItem * item, int column)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    if(column == eColumnCheckBox)
    {
        CGisWorkspace::self().slotWksItemSelectionReset();
        emit sigChanged();
    }
}

void CGisListWks::slotEditItem()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    IGisItem * gisItem = dynamic_cast<IGisItem*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().editItemByKey(gisItem->getKey());
    }
}

void CGisListWks::slotDeleteItem()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QList<QTreeWidgetItem*> items       = selectedItems();
    QList<IGisItem::key_t>  keys;
    for(QTreeWidgetItem * item : items)
    {
        IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
        if(gisItem != nullptr)
        {
            keys << gisItem->getKey();
        }
    }

    CGisWorkspace::self().delItemsByKey(keys);
}

void CGisListWks::slotCopyItem()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    /*
     * Item selection is reset when the target project is a new database
     * project. Additionally the list of selected items pointers seems
     * to get invalid, causing a segfault when used.
     *
     * As a fix the keys of the selected items are stored temporarily and
     * later used to retrieve the item on the workspace via CGisWorkspace::getItemByKey()
     * again. This is always safe.
     */
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<IGisItem::key_t>  keys;
    for(QTreeWidgetItem * item : items)
    {
        IGisItem * gisItem = dynamic_cast<IGisItem*>(item);
        if(gisItem != nullptr)
        {
            keys << gisItem->getKey();
        }
    }

    CGisWorkspace::self().copyItemsByKey(keys);
}

void CGisListWks::slotProjWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().projWptByKey(gisItem->getKey());
    }
}

void CGisListWks::slotBubbleWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().toggleWptBubble(gisItem->getKey());
    }
}

void CGisListWks::slotNogoItem()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    IGisItem * gisItem = dynamic_cast<IGisItem*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().toggleNogoItem(gisItem->getKey());
    }
}

void CGisListWks::slotDelRadiusWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().deleteWptRadius(gisItem->getKey());
    }
}

void CGisListWks::slotEditRadiusWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().editWptRadius(gisItem->getKey());
    }
}

void CGisListWks::slotMoveWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemWpt * gisItem = dynamic_cast<CGisItemWpt*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().moveWptByKey(gisItem->getKey());
    }
}

void CGisListWks::slotFocusTrk(bool on)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().focusTrkByKey(on, gisItem->getKey());
    }
}

void CGisListWks::slotEditTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().editTrkByKey(gisItem->getKey());
    }
}

void CGisListWks::slotReverseTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().reverseTrkByKey(gisItem->getKey());
    }
}

void CGisListWks::slotCombineTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QList<IGisItem::key_t>  keys;
    QList<QTreeWidgetItem*> items = selectedItems();
    for(QTreeWidgetItem * item : items)
    {
        CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(item);
        if(gisItem)
        {
            keys << gisItem->getKey();
        }
    }

    if(!keys.isEmpty())
    {
        if(keys.size() == 1)
        {
            CGisWorkspace::self().combineTrkByKey(keys.first());
        }
        else
        {
            CGisWorkspace::self().combineTrkByKey(keys, keys);
        }
    }
}

void CGisListWks::slotActivityTrk()
{
    quint32 flags = CActivityTrk::selectActivity(this);
    if(0xFFFFFFFF != flags)
    {
        CGisListWksEditLock lock(true, IGisItem::mutexItems);
        QList<QTreeWidgetItem*> items = selectedItems();
        for(QTreeWidgetItem * item : items)
        {
            CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(item);
            if(trk)
            {
                trk->setActivity(flags);
            }
        }
    }
}

void CGisListWks::slotRangeTrk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().rangeTrkByKey(gisItem->getKey());
    }
}

void CGisListWks::slotCopyTrkWithWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemTrk * gisItem = dynamic_cast<CGisItemTrk*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().copyTrkWithWptByKey(gisItem->getKey());
    }
}

void CGisListWks::slotFocusRte(bool on)
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    CGisItemRte * gisItem = dynamic_cast<CGisItemRte*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().focusRteByKey(on, gisItem->getKey());
    }
}

void CGisListWks::slotCalcRte()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemRte * gisItem = dynamic_cast<CGisItemRte*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().calcRteByKey(gisItem->getKey());
    }
}

void CGisListWks::slotResetRte()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemRte * gisItem = dynamic_cast<CGisItemRte*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().resetRteByKey(gisItem->getKey());
    }
}


void CGisListWks::slotEditRte()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemRte * gisItem = dynamic_cast<CGisItemRte*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().editRteByKey(gisItem->getKey());
    }
}

void CGisListWks::slotReverseRte()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemRte * gisItem = dynamic_cast<CGisItemRte*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().reverseRteByKey(gisItem->getKey());
    }
}

void CGisListWks::slotRte2Trk()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemRte * gisItem = dynamic_cast<CGisItemRte*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().convertRouteToTrack(gisItem->getKey());
    }
}

void CGisListWks::slotEditArea()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    CGisItemOvlArea * gisItem = dynamic_cast<CGisItemOvlArea*>(currentItem());
    if(gisItem != nullptr)
    {
        CGisWorkspace::self().editAreaByKey(gisItem->getKey());
    }
}

void CGisListWks::slotAddEmptyProject()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QString key, name;
    IGisProject::type_e type;
    CSelectProjectDialog dlg(key, name, type, nullptr);
    if(dlg.exec() == QDialog::Rejected)
    {
        return;
    }
    if(name.isEmpty() && (type != IGisProject::eTypeDb))
    {
        return;
    }

    if(type == IGisProject::eTypeGpx)
    {
        new CGpxProject(name, this);
    }
    else if(type == IGisProject::eTypeQms)
    {
        new CQmsProject(name, this);
    }
    else if(type == IGisProject::eTypeDb)
    {
        quint64 idParent;
        QString db;
        QString host;
        IDBFolder::type_e type;

        CSelectDBFolder dlg1(idParent, db, host, this);
        if(dlg1.exec() == QDialog::Rejected)
        {
            return;
        }

        CSetupFolder dlg2(type, name, false, this);
        if(dlg2.exec() == QDialog::Rejected)
        {
            return;
        }

        CEvtW2DCreate * evt = new CEvtW2DCreate(name, type, idParent, db, host);
        CGisDatabase::self().postEventForDb(evt);
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

    CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CGisListWks::slotSyncWksDev()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    if(IDevice::count() == 0)
    {
        return;
    }

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(nullptr == project)
    {
        return;
    }

    const int N = topLevelItemCount();
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
    else
    {
        for(int n = 0; n < N; n++)
        {
            IDevice *device = dynamic_cast<IDevice*>(topLevelItem(n));
            if(nullptr != device)
            {
                keys << device->getKey();
                break;
            }
        }
    }

    CCanvas *canvas = CMainWindow::self().getVisibleCanvas();
    for(int n = 0; n < N; n++)
    {
        IDevice * device = dynamic_cast<IDevice*>(topLevelItem(n));
        if(nullptr == device || keys.isEmpty() || !keys.contains(device->getKey()))
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
}

void CGisListWks::slotSyncDevWks()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(nullptr == project)
    {
        return;
    }

    IDevice * device = dynamic_cast<IDevice*>(project->parent());
    if(nullptr == device)
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
    if(e->type() > QEvent::User)
    {
        const bool doWaitCursoer = (eEvtA2WCutTrk != event_types_e(e->type()));
        CGisListWksEditLock lock(doWaitCursoer, IGisItem::mutexItems);

        switch(e->type())
        {
        case eEvtD2WReqInfo:
        {
            CEvtD2WReqInfo * evt = (CEvtD2WReqInfo*)e;
            CDBProject * project =  getProjectById(evt->id, evt->db);
            if(nullptr != project)
            {
                project->postStatus(false);
            }
            e->accept();
            emit sigChanged();
            return true;
        }

        case eEvtD2WShowFolder:
        {
            CEvtD2WShowFolder * evt = (CEvtD2WShowFolder*)e;
            CDBProject * project =  getProjectById(evt->id, evt->db);
            if(nullptr == project)
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
            if(project && project->askBeforClose())
            {
                /*
                    Tell the DB view that we aborted to hide the folder by posting it's
                    current status.
                 */
                project->postStatus(false);
                return false;
            }
            delete project;

            e->accept();
            emit sigChanged();
            emit sigItemDeleted();
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
            emit sigItemDeleted();
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

        case eEvtD2WUpdateItems:
        {
            CEvtD2WUpdateItems * evt = (CEvtD2WUpdateItems*)e;
            IGisProject * project = dynamic_cast<IGisProject*>(getProjectById(evt->id, evt->db));
            if(project)
            {
                project->blockUpdateItems(false);
            }
            e->accept();
            emit sigChanged();
            return true;
        }

        case eEvtD2WReload:
        {
            CEvtD2WReload * evt = (CEvtD2WReload*)e;
            QList<CDBProject*> projects;

            const int N = topLevelItemCount();
            for(int i = 0; i < N; i++)
            {
                CDBProject * project = dynamic_cast<CDBProject*>(topLevelItem(i));

                if(project && (project->getDBName() == evt->db))
                {
                    project->update();
                    projects << project;
                }
            }

            for(CDBProject * project : projects)
            {
                project->blockUpdateItems(false);
            }
            e->accept();
            return true;
        }

        case eEvtA2WCutTrk:
        {
            CEvtA2WCutTrk * evt = (CEvtA2WCutTrk*)e;
            CGisWorkspace::self().cutTrkByKey(evt->key);
            e->accept();
            return true;
        }

        case eEvtA2WSave:
        {
            CEvtA2WSave * evt = (CEvtA2WSave*)e;

            IGisProject * project = getProjectByKey(evt->key);
            if(project)
            {
                project->save();
                project->confirmPendingAutoSave();
            }
            e->accept();
            return true;
        }
        }
    }
    return QTreeWidget::event(e);
}



void CGisListWks::slotRteFromWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);

    QList<IGisItem::key_t> keys;
    for(QTreeWidgetItem * item : selectedItems())
    {
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(item);
        if(nullptr != wpt)
        {
            keys << wpt->getKey();
        }
    }

    if(!keys.isEmpty())
    {
        CGisWorkspace::self().makeRteFromWpt(keys);
    }
}

void CGisListWks::slotSyncDB()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    for(QTreeWidgetItem * item : selectedItems())
    {
        CDBProject * project = dynamic_cast<CDBProject*>(item);
        if(project == nullptr)
        {
            continue;
        }

        project->update();
    }
}

void CGisListWks::slotSetSortMode(IGisProject::sorting_folder_e mode, bool checked)
{
    if(!checked || blockSorting)
    {
        return;
    }


    IGisProject * project = dynamic_cast<IGisProject*>(currentItem());
    if(project != nullptr)
    {
        project->setSortingFolder(mode);
    }
}


void CGisListWks::slotCopyProject()
{
    CGisListWksEditLock lock(true, IGisItem::mutexItems);

    QList<IGisItem::key_t>  keys;

    for(QTreeWidgetItem * item : selectedItems())
    {
        IGisProject * project = dynamic_cast<IGisProject*>(item);
        if(project == nullptr)
        {
            continue;
        }

        const int N = project->childCount();
        for(int i = 0; i < N; i++)
        {
            IGisItem * item = dynamic_cast<IGisItem*>(project->child(i));
            if(item != nullptr)
            {
                keys << item->getKey();
            }
        }
    }

    CGisWorkspace::self().copyItemsByKey(keys);
}


void CGisListWks::slotSymWpt()
{
    CGisListWksEditLock lock(false, IGisItem::mutexItems);
    QToolButton tb;
    CWptIconDialog dlg(&tb);
    if(dlg.exec() == QDialog::Rejected)
    {
        return;
    }

    QList<IGisItem::key_t> keys;
    for(QTreeWidgetItem * item : selectedItems())
    {
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(item);
        if(wpt == nullptr)
        {
            continue;
        }

        keys << wpt->getKey();
    }

    CGisWorkspace::self().changeWptSymByKey(keys, tb.objectName());
}
