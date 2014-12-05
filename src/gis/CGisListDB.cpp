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

#include "gis/CGisListDB.h"
#include "gis/CGisWidget.h"
#include "gis/db/macros.h"
#include "gis/db/CSetupFolder.h"
#include "gis/db/CDBFolderDatabase.h"
#include "gis/db/CDBFolderLostFound.h"
#include "helpers/CSettings.h"
#include "config.h"

#include <QtSql>
#include <QtWidgets>

class CGisListDBEditLock
{
    public:
        CGisListDBEditLock(CGisListDB * widget) : widget(widget){widget->isInternalEdit += 1;}
        ~CGisListDBEditLock(){widget->isInternalEdit -= 1;}
    private:
        CGisListDB * widget;
};


CGisListDB::CGisListDB(QWidget *parent)
    : QTreeWidget(parent)
    , isInternalEdit(0)
{   
    SETTINGS;
    QString path = cfg.value("Database/path", QDir::home().filePath(CONFIGDIR).append("/database.db")).toString();
    db = QSqlDatabase::addDatabase("QSQLITE", "Database");
    db.setDatabaseName(path);
    db.open();
    configDB();


    itemLostFound       = new CDBFolderLostFound(db, this);
    itemDatabase        = new CDBFolderDatabase(db, this);

    menuDatabase        = new QMenu(this);
    actionAddFolder     = menuDatabase->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Folder"), this, SLOT(slotAddFolder()));

    menuProject         = new QMenu(this);
    menuItem            = new QMenu(this);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

    itemDatabase->setExpanded(true);
}

CGisListDB::~CGisListDB()
{
}


void CGisListDB::configDB()
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

void CGisListDB::initDB()
{
    QSqlQuery query(db);

    if(query.exec( "CREATE TABLE versioninfo ( version TEXT )"))
    {
        query.prepare( "INSERT INTO versioninfo (version) VALUES(:version)");
        query.bindValue(":version", DB_VERSION);
        QUERY_EXEC(; );
    }

    if(!query.exec( "CREATE TABLE folders ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "type           INTEGER NOT NULL,"
                    "key            TEXT,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "locked         BOOLEAN DEFAULT FALSE,"
                    "data           BLOB"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }

    if(!query.exec( "CREATE TABLE items ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "type           INTEGER,"
                    "key            TEXT NOT NULL,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "icon           BLOB NOT NULL,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "data           BLOB NOT NULL"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }

    if(!query.exec("INSERT INTO folders (type, name, comment) VALUES (2, 'Database', '')"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }

    if(!query.exec( "CREATE TABLE folder2folder ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "parent         INTEGER NOT NULL,"
                    "child          INTEGER NOT NULL,"
                    "FOREIGN KEY(parent) REFERENCES folders(id),"
                    "FOREIGN KEY(child) REFERENCES folders(id)"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }

    if(!query.exec( "CREATE TABLE folder2item ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "parent         INTEGER NOT NULL,"
                    "child          INTEGER NOT NULL,"
                    "FOREIGN KEY(parent) REFERENCES folders(id),"
                    "FOREIGN KEY(child) REFERENCES items(id)"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
}

void CGisListDB::migrateDB(int version)
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

void CGisListDB::queueDBAction(const action_t& act)
{
    CGisListDBEditLock lock(this);

    switch(act.action)
    {
    case eActW2DInfoProject:
    {
        const action_info_t& info = static_cast<const action_info_t&>(act);
        itemDatabase->update(info);
        break;
    }
    default:;
    }
}

void CGisListDB::slotContextMenu(const QPoint& point)
{
    QPoint p = mapToGlobal(point);
    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if((folder != 0))
    {
        menuDatabase->exec(p);
        return;
    }
}

void CGisListDB::slotAddFolder()
{
    CGisListDBEditLock lock(this);

    IDBFolder * parentFolder = dynamic_cast<IDBFolder*>(currentItem());
    if(parentFolder == 0)
    {
        return;
    }
    quint64 idParent = parentFolder->getId();

    IDBFolder::type_e type = IDBFolder::eTypeProject;
    QString name;
    CSetupFolder dlg(type, name, this);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO folders (name, type) VALUES (:name, :type)");
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    QUERY_EXEC(return);

    query.prepare("SELECT last_insert_rowid() from folders");
    QUERY_EXEC(return);
    query.next();
    quint64 idChild = query.value(0).toULongLong();
    if(idChild == 0)
    {
        qDebug() << "CGisListDB::slotAddFolder(): childId equals 0. bad.";
        return;
    }

    query.prepare("INSERT INTO folder2folder (parent, child) VALUES (:parent, :child)");
    query.bindValue(":parent", idParent);
    query.bindValue(":child", idChild);
    QUERY_EXEC(return);

    IDBFolder::createFolderByType(db, type, idChild, parentFolder);
}

void CGisListDB::slotItemExpanded(QTreeWidgetItem * item)
{   
    CGisListDBEditLock lock(this);

    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(folder == 0)
    {
        return;
    }

    folder->expanding();
}

void CGisListDB::slotItemChanged(QTreeWidgetItem * item, int column)
{
    if(isInternalEdit)
    {
        return;
    }

    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(folder != 0 && (column == IDBFolder::eColumnCheckbox))
    {
        folder->toggle(folder->getId());
        return;
    }
}
