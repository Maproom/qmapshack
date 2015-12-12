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
#include "gis/CGisListDB.h"
#include "gis/CGisWidget.h"
#include "gis/db/CDBFolderLostFound.h"
#include "gis/db/CDBFolderMysql.h"
#include "gis/db/CDBFolderSqlite.h"
#include "gis/db/CDBItem.h"
#include "gis/db/CSetupDatabase.h"
#include "gis/db/CSetupFolder.h"
#include "gis/db/macros.h"
#include "helpers/CSettings.h"

#include <QtSql>
#include <QtWidgets>

class CGisListDBEditLock
{
public:
    CGisListDBEditLock(bool waitCursor, CGisListDB * widget, const QString& src) : widget(widget), waitCursor(waitCursor), src(src)
    {
        if(waitCursor)
        {
            CCanvas::setOverrideCursor(Qt::WaitCursor, "CGisListDBEditLock: " + src);
        }
        widget->isInternalEdit += 1;
    }
    ~CGisListDBEditLock()
    {
        if(waitCursor)
        {
            CCanvas::restoreOverrideCursor("~CGisListDBEditLock: " + src);
        }
        widget->isInternalEdit -= 1;
    }
private:
    CGisListDB * widget;
    bool waitCursor;
    QString src;
};


CGisListDB::CGisListDB(QWidget *parent)
    : QTreeWidget(parent)
{
    SETTINGS;
    cfg.beginGroup("Database");
    QString path = cfg.value("lastDatabasePath", QDir::homePath()).toString();
    QStringList names = cfg.value("names").toStringList();
    cfg.beginGroup("Entries");
    foreach(const QString& name, names)
    {
        cfg.beginGroup(name);
        QString type = cfg.value("type", "SQLite").toString();
        if(type == "SQLite")
        {
            QString filename = cfg.value("filename","").toString();
            if(filename.isEmpty())
            {
                QMessageBox::information(this, name, tr("Due to changes in the database system QMapShack forgot about the filename of your database '%1'. You have to select it again in the next step.").arg(name), QMessageBox::Ok);
                filename = QFileDialog::getOpenFileName(this, tr("Select database file."), path, "QMapShack Database (*.db)");
                if(filename.isEmpty())
                {
                    cfg.endGroup(); // name;
                    continue;
                }
            }
            new CDBFolderSqlite(filename, name, this);
        }
        if(type == "MySQL")
        {
            QString server  = cfg.value("server","").toString();
            QString user    = cfg.value("user","").toString();
            QString passwd  = cfg.value("passwd","").toString();

            if(server.isEmpty() || user.isEmpty())
            {
                cfg.endGroup(); // name
                continue;
            }
            new CDBFolderMysql(server, user, passwd, name, this);
        }
        cfg.endGroup(); // name
    }
    cfg.endGroup(); // Entries
    cfg.endGroup(); // Database


    menuNone            = new QMenu(this);
    actionAddDatabase   = menuNone->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Database"), this, SLOT(slotAddDatabase()));

    menuFolder          = new QMenu(this);
    actionAddFolder     = menuFolder->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Folder"), this, SLOT(slotAddFolder()));
    actionDelFolder     = menuFolder->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Folder"), this, SLOT(slotDelFolder()));

    menuItem            = new QMenu(this);
    actionDelItem       = menuItem->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Item"), this, SLOT(slotDelItem()));

    menuDatabase        = new QMenu(this);
    menuDatabase->addAction(actionAddFolder);
    actionDelDatabase   = menuDatabase->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Remove Database"), this, SLOT(slotDelDatabase()));

    menuLostFound       = new QMenu(this);
    actionDelLostFound  = menuLostFound->addAction(QIcon("://icons/32x32/Empty.png"), tr("Empty"), this, SLOT(slotDelLostFound()));

    menuLostFoundItem       = new QMenu(this);
    actionDelLostFoundItem  = menuLostFoundItem->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Item"), this, SLOT(slotDelLostFoundItem()));


    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
}

CGisListDB::~CGisListDB()
{
    QStringList names;

    SETTINGS;
    cfg.beginGroup("Database");

    cfg.beginGroup("Entries");
    cfg.remove("");

    const int N = topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        CDBFolderSqlite * sqlite = dynamic_cast<CDBFolderSqlite*>(topLevelItem(n));
        if(sqlite)
        {
            QString name = sqlite->text(CGisListDB::eColumnName);
            names << name;

            cfg.beginGroup(name);
            cfg.setValue("type", "SQLite");
            cfg.setValue("filename", sqlite->getFilename());
            cfg.endGroup(); // name
        }
        CDBFolderMysql * mysql = dynamic_cast<CDBFolderMysql*>(topLevelItem(n));
        if(mysql)
        {
            QString name = mysql->text(CGisListDB::eColumnName);
            names << name;

            cfg.beginGroup(name);
            cfg.setValue("type", "MySQL");
            cfg.setValue("server", mysql->getServer());
            cfg.setValue("user", mysql->getUser());
            cfg.setValue("passwd", mysql->getPasswd());
            cfg.endGroup(); // name
        }
    }

    cfg.endGroup(); // Entries
    cfg.setValue("names", names);
    cfg.endGroup(); // Database
}


IDBFolderSql * CGisListDB::getDataBase(const QString& name)
{
    CGisListDBEditLock lock(true, this, "getDataBase");
    const int N = topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IDBFolderSql * database = dynamic_cast<IDBFolderSql*>(topLevelItem(n));
        if(database && (database->getDBName() == name))
        {
            return database;
        }
    }
    return 0;
}

bool CGisListDB::hasDatabase(const QString& name)
{
    CGisListDBEditLock lock(true, this, "hasDatabase");
    const int N = topLevelItemCount();
    for(int i = 0; i < N; i++)
    {
        IDBFolderSql * folder = dynamic_cast<IDBFolderSql*>(topLevelItem(i));
        if(folder && (folder->text(CGisListDB::eColumnName) == name))
        {
            return true;
        }
    }
    return false;
}


bool CGisListDB::event(QEvent * e)
{
    switch(e->type())
    {
    case eEvtW2DAckInfo:
    {
        CGisListDBEditLock lock(true, this, "event");
        CEvtW2DAckInfo * evt    = (CEvtW2DAckInfo*)e;
        IDBFolderSql * folder   = getDataBase(evt->db);
        if(folder)
        {
            folder->update(evt);
            if(evt->updateLostFound)
            {
                folder->updateLostFound();
            }
        }
        e->accept();
        return true;
    }

    case eEvtW2DCreate:
    {
        CGisListDBEditLock lock(true, this, "event");
        CEvtW2DCreate * evt = (CEvtW2DCreate*)e;
        IDBFolderSql * db   = getDataBase(evt->db);
        if(db)
        {
            quint64 idChild = 0;
            IDBFolder * folder = db->getFolder(evt->idParent);
            if(folder)
            {
                idChild = folder->addFolder(evt->type, evt->name);
            }
            else
            {
                idChild = IDBFolder::addFolderToDb(evt->type, evt->name, evt->idParent, db->getDb());
            }

            if(idChild)
            {
                evt->idChild = idChild;
                CEvtD2WShowFolder * evt1 = new CEvtD2WShowFolder(idChild, evt->db);
                CGisWidget::self().postEventForWks(evt1);
            }
        }
        e->accept();
        return true;
    }
    }

    return QTreeWidget::event(e);
}

void CGisListDB::slotContextMenu(const QPoint& point)
{
    QPoint p = mapToGlobal(point);

    if(selectedItems().isEmpty())
    {
        menuNone->exec(p);
        return;
    }

    IDBFolderSql * database = dynamic_cast<IDBFolderSql*>(currentItem());
    if(database)
    {
        menuDatabase->exec(p);
        return;
    }

    CDBFolderLostFound * lostFound = dynamic_cast<CDBFolderLostFound*>(currentItem());
    if(lostFound)
    {
        menuLostFound->exec(p);
        return;
    }

    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if(folder)
    {
        menuFolder->exec(p);
        return;
    }

    CDBItem * item = dynamic_cast<CDBItem*>(currentItem());
    if(item)
    {
        CDBFolderLostFound * lostFound = dynamic_cast<CDBFolderLostFound*>(item->parent());
        if(lostFound)
        {
            menuLostFoundItem->exec(p);
        }
        else
        {
            menuItem->exec(p);
        }

        return;
    }
}

void CGisListDB::slotAddDatabase()
{
    CSetupDatabase dlg(*this);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    QString name = dlg.getName();

    if(dlg.isSqlite())
    {
        QString filename = dlg.getFilename();
        new CDBFolderSqlite(filename, name, this);
    }
    else if(dlg.isMysql())
    {
        QString server  = dlg.getServer();
        QString user    = dlg.getUser();
        QString passwd  = dlg.getPasswd();
        new CDBFolderMysql(server, user, passwd, name, this);
    }
    else
    {
        return;
    }

    emit sigChanged();
}

void CGisListDB::slotDelDatabase()
{
    IDBFolderSql * folder = dynamic_cast<IDBFolderSql*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Remove database..."), tr("Do you really want to remove '%1' from the list?").arg(folder->text(CGisListDB::eColumnName)), QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    delete folder;

    emit sigChanged();
}

void CGisListDB::slotAddFolder()
{
    CGisListDBEditLock lock(false, this, "slotAddFolder");

    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    IDBFolder::type_e type = IDBFolder::eTypeProject;
    QString name;
    CSetupFolder dlg(type, name, true, this);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    folder->addFolder(type, name);
}

void CGisListDB::slotDelFolder()
{
    CGisListDBEditLock lock(false, this, "slotDelFolder");
    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Delete database folder..."), tr("Are you sure you want to delete \"%1\" from the database?").arg(folder->text(1)), QMessageBox::Ok|QMessageBox::No);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    IDBFolderSql * dbfolder = folder->getDBFolder();

    folder->remove();
    delete folder;

    if(dbfolder)
    {
        dbfolder->updateLostFound();
    }
}

void CGisListDB::slotDelLostFound()
{
    CGisListDBEditLock lock(false, this, "slotDelLostFound");
    CDBFolderLostFound * folder = dynamic_cast<CDBFolderLostFound*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Remove items..."), tr("Are you sure you want to delete all items from Lost&Found? This will remove them permanently."), QMessageBox::Ok|QMessageBox::No);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    CCanvas::setOverrideCursor(Qt::WaitCursor, "slotDelLostFound");
    folder->clear();
    CCanvas::restoreOverrideCursor("slotDelLostFound");
}

void CGisListDB::slotDelLostFoundItem()
{
    CGisListDBEditLock lock(false, this, "slotDelLostFoundItem");

    int res = QMessageBox::question(this, tr("Remove items..."), tr("Are you sure you want to delete all selected items from Lost&Found? This will remove them permanently."), QMessageBox::Ok|QMessageBox::No);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    CCanvas::setOverrideCursor(Qt::WaitCursor, "slotDelLostFoundItem");
    QSet<CDBFolderLostFound*> folders;
    QList<QTreeWidgetItem*> delItems;
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CDBItem * dbItem            = dynamic_cast<CDBItem*>(item);
        if(dbItem == 0)
        {
            continue;
        }

        CDBFolderLostFound * folder = dynamic_cast<CDBFolderLostFound*>(dbItem->parent());
        if(folder == 0)
        {
            continue;
        }

        if(folder->delItem(dbItem))
        {
            delItems << dbItem;
            folders << folder;
        }
    }

    qDeleteAll(delItems);
    foreach(CDBFolderLostFound* folder, folders)
    {
        folder->update();
    }
    CCanvas::restoreOverrideCursor("slotDelLostFoundItem");
}


void CGisListDB::slotItemExpanded(QTreeWidgetItem * item)
{
    CGisListDBEditLock lock(true, this, "slotItemExpanded");

    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(folder == 0)
    {
        return;
    }

    folder->expanding();
}

void CGisListDB::slotDelItem()
{
    CGisListDBEditLock lock(false, this, "slotDelItem");

    int last = QMessageBox::NoButton;

    QSet<IDBFolder*>        folders;
    QList<QTreeWidgetItem*> dbItems;
    QSet<IDBFolderSql*>     dbFolders;


    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CDBItem * dbItem = dynamic_cast<CDBItem*>(item);
        if(dbItem == 0)
        {
            continue;
        }

        IDBFolder * folder = dynamic_cast<IDBFolder*>(dbItem->parent());
        if(folder == 0)
        {
            continue;
        }

        if(last != QMessageBox::YesToAll)
        {
            QString msg = QObject::tr("Are you sure you want to delete '%1' from folder '%2'?").arg(dbItem->text(CGisListDB::eColumnName)).arg(folder->text(CGisListDB::eColumnName));
            last = QMessageBox::question(CMainWindow::getBestWidgetForParent(), QObject::tr("Delete..."), msg, QMessageBox::YesToAll|QMessageBox::Cancel|QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
        }
        if(last == QMessageBox::No)
        {
            continue;
        }
        if(last == QMessageBox::Cancel)
        {
            return;
        }

        dbItem->remove();

        folders << folder;
        dbItems << dbItem;
        dbFolders << folder->getDBFolder();
    }

    qDeleteAll(dbItems);
    foreach(IDBFolderSql * dbFolder, dbFolders)
    {
        dbFolder->updateLostFound();
    }

    // tell all folders to update their statistics and waypoint/track correlations
    foreach(IDBFolder * folder, folders)
    {
        folder->updateItemsOnWks();
    }
}

void CGisListDB::slotItemChanged(QTreeWidgetItem * item, int column)
{
    if(isInternalEdit)
    {
        return;
    }
    CGisListDBEditLock lock(true, this, "slotItemChanged");

    if(column == CGisListDB::eColumnCheckbox)
    {
        IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
        if(folder != 0)
        {
            folder->toggle();

            // tell folder to update its statistics and waypoint/track correlations
            folder->updateItemsOnWks();
            return;
        }

        CDBItem * dbItem = dynamic_cast<CDBItem*>(item);
        if(dbItem != 0)
        {
            dbItem->toggle();

            // tell folder to update its statistics and waypoint/track correlations
            folder = dynamic_cast<IDBFolder*>(dbItem->parent());
            if(folder)
            {
                folder->updateItemsOnWks();
            }
            return;
        }
    }
}

