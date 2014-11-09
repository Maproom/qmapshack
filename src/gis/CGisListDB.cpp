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
#include "gis/db/macros.h"
#include "gis/db/CSetupFolder.h"
#include "gis/db/CDBFolderDatabase.h"

#include <QtSql>
#include <QtWidgets>



CGisListDB::CGisListDB(QWidget *parent)
    : QTreeWidget(parent)
{
    db = QSqlDatabase::database();

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

    itemLostFound = new QTreeWidgetItem(this, IDBFolder::eTypeLostFound);
    itemLostFound->setIcon(0,QIcon("://icons/32x32/DeleteMultiple.png"));
    itemLostFound->setText(0,tr("Lost & Found"));

    itemDatabase = new CDBFolderDatabase(this);
    itemDatabase->setIcon(0,QIcon("://icons/32x32/Database.png"));
    itemDatabase->setText(0,tr("Database"));

    menuDatabase        = new QMenu(this);
    actionAddFolder     = menuDatabase->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Folder"), this, SLOT(slotAddFolder()));

    menuProject         = new QMenu(this);
    menuItem            = new QMenu(this);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
}

CGisListDB::~CGisListDB()
{
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
                    "type           INTEGER,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "locked       BOOLEAN DEFAULT FALSE"
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
                    "icon           TEXT NOT NULL,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "data           BLOB NOT NULL"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
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

    if(!query.exec("INSERT INTO folders (name, comment) VALUES (database', '')"))
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

void CGisListDB::slotContextMenu(const QPoint& point)
{
    QPoint p = mapToGlobal(point);
    QTreeWidgetItem * item = currentItem();
    if(item == 0 || item == itemDatabase)
    {
        menuDatabase->exec(p);
        return;
    }
}

void CGisListDB::slotAddFolder()
{
    QTreeWidgetItem * item = currentItem();
    if(item == 0)
    {
        return;
    }

    IDBFolder::type_e type = IDBFolder::eTypeProject;
    QString name;

    CSetupFolder dlg(type, name, this);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

}


