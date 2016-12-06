/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/db/IDBSqlite.h"
#include "gis/db/macros.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"

#include <QtSql>
#include <QtWidgets>

IDBSqlite::IDBSqlite()
{
}


bool IDBSqlite::setupDB(const QString& filename, const QString& connectionName, QString &error)
{
    // this is important!
    IDB::setup(connectionName);

    if(!QSqlDatabase::contains(connectionName))
    {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(filename);
        if(!db.open())
        {
            qDebug() << "failed to open database" << db.lastError();
        }
    }
    else
    {
        db = QSqlDatabase::database(connectionName);
    }

    QSqlQuery query(db);

    QUERY_RUN("PRAGMA locking_mode=NORMAL",    return false)
    QUERY_RUN("PRAGMA temp_store=MEMORY",      return false)
    QUERY_RUN("PRAGMA default_cache_size=50",  return false)
    QUERY_RUN("PRAGMA page_size=8192",         return false)
    QUERY_RUN("PRAGMA synchronous=off",        return false)

    // When migrating the database these tables are used.
    // Due to caching they can't be dropped right after the
    // migration. That is why we look for them on startup.
    // And delete them as a second chance.
    if(query.exec("select * from tmp_folders"))
    {
        QUERY_RUN("DROP TABLE tmp_folders;", NO_CMD)
    }
    if(query.exec("select * from tmp_items"))
    {
        QUERY_RUN("DROP TABLE tmp_items;", NO_CMD)
    }

    return setupDB(error);
}

bool IDBSqlite::initDB()
{
    QSqlQuery query(db);

    QUERY_RUN("BEGIN TRANSACTION;", return false);
    try
    {
        if(query.exec( "CREATE TABLE versioninfo ( version TEXT, type TEXT )"))
        {
            query.prepare( "INSERT INTO versioninfo (version, type) VALUES(:version, 'QMapShack')");
            query.bindValue(":version", DB_VERSION);
            QUERY_EXEC(throw -1);
        }

        QUERY_RUN("CREATE TABLE folders ("
                  "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "type           INTEGER NOT NULL,"
                  "keyqms         TEXT,"
                  "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                  "name           TEXT NOT NULL,"
                  "comment        TEXT,"
                  "locked         BOOLEAN DEFAULT FALSE,"
                  "data           BLOB,"
                  "sortmode       INTEGER NOT NULL DEFAULT 0"
                  ")", throw -1)

        QUERY_RUN("CREATE TABLE items ("
                  "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "type           INTEGER,"
                  "keyqms         TEXT NOT NULL UNIQUE,"
                  "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                  "icon           BLOB NOT NULL,"
                  "name           TEXT NOT NULL,"
                  "comment        TEXT,"
                  "data           BLOB NOT NULL,"
                  "hash           TEXT NOT NULL,"
                  "last_user      TEXT DEFAULT 'QMapShack',"
                  "last_change    DATETIME DEFAULT CURRENT_TIMESTAMP,"
                  "trash          DATETIME DEFAULT NULL"
                  ")", throw -1)

        QUERY_RUN("CREATE TRIGGER items_update_last_change "
                  "AFTER UPDATE ON items BEGIN "
                  "UPDATE items SET last_change=CURRENT_TIMESTAMP WHERE id=NEW.id; "
                  "END;", throw -1)


        query.prepare("INSERT INTO folders (type, name, comment) VALUES (2, :name, '')");
        query.bindValue(":name", db.connectionName());
        QUERY_EXEC(return false);

        QUERY_RUN("CREATE TABLE folder2folder ("
                  "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "parent         INTEGER NOT NULL,"
                  "child          INTEGER NOT NULL,"
                  "FOREIGN KEY(parent) REFERENCES folders(id),"
                  "FOREIGN KEY(child) REFERENCES folders(id)"
                  ")", throw -1)

        QUERY_RUN("CREATE TABLE folder2item ("
                  "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "parent         INTEGER NOT NULL,"
                  "child          INTEGER NOT NULL,"
                  "FOREIGN KEY(parent) REFERENCES folders(id),"
                  "FOREIGN KEY(child) REFERENCES items(id)"
                  ")", return false)

        QUERY_RUN("CREATE TRIGGER folder2item_insert "
                  "BEFORE INSERT ON folder2item BEGIN "
                  "UPDATE items SET trash=NULL "
                  "WHERE id=NEW.child; "
                  "END;", throw -1);

        QUERY_RUN("CREATE TRIGGER folder2item_delete "
                  "AFTER DELETE ON folder2item BEGIN "
                  "UPDATE items SET trash=CURRENT_TIMESTAMP "
                  "WHERE id=OLD.child AND OLD.child NOT IN(SELECT child FROM folder2item); "
                  "END;", throw -1);

        // create virtual table with search index
        QUERY_RUN("CREATE VIRTUAL TABLE searchindex USING fts4(id, comment)", throw -1);

        QUERY_RUN("CREATE TRIGGER searchindex_update "
                  "AFTER UPDATE ON items BEGIN "
                  "UPDATE searchindex SET comment=NEW.comment "
                  "WHERE id=OLD.id; "
                  "END;", throw -1);

        QUERY_RUN("CREATE TRIGGER searchindex_insert "
                  "AFTER INSERT ON items BEGIN "
                  "INSERT INTO searchindex(id, comment) VALUES(NEW.id, NEW.comment); "
                  "END;", throw -1);

        QUERY_RUN("END TRANSACTION;", throw -1);
    }
    catch(int i)
    {
        if(i == -1)
        {
            QUERY_RUN("ROLLBACK;", return false);
            return false;
        }
    }


    return true;
}

bool IDBSqlite::migrateDB(int version)
{
    QSqlQuery query(db);

    QUERY_RUN("BEGIN TRANSACTION;", return false);
    try
    {
        if(version < 2)
        {
            if(!migrateDB1to2())
            {
                throw -1;
            }
        }

        if(version < 3)
        {
            if(!migrateDB2to3())
            {
                throw -1;
            }
        }

        if(version < 4)
        {
            if(!migrateDB3to4())
            {
                throw -1;
            }
        }

        if(version < 5)
        {
            if(!migrateDB4to5())
            {
                throw -1;
            }
        }

        if(version < 6)
        {
            if(!migrateDB5to6())
            {
                throw -1;
            }
        }

        QUERY_RUN("END TRANSACTION;", throw -1);
    }
    catch(int i)
    {
        if(i == -1)
        {
            QUERY_RUN("ROLLBACK;", return false);
            return false;
        }
    }

    query.prepare( "UPDATE versioninfo set version=:version");
    query.bindValue(":version", DB_VERSION);
    QUERY_EXEC(return false);
    return true;
}

bool IDBSqlite::migrateDB1to2()
{
    QSqlQuery query(db);

    QUERY_RUN("ALTER TABLE folders RENAME TO tmp_folders;", return false)

    QUERY_RUN("CREATE TABLE folders ("
              "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
              "type           INTEGER NOT NULL,"
              "keyqms         TEXT,"
              "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
              "name           TEXT NOT NULL,"
              "comment        TEXT,"
              "locked         BOOLEAN DEFAULT FALSE,"
              "data           BLOB"
              ");", return false);

    QUERY_RUN("INSERT INTO folders(id,type,keyqms,date,name,comment,locked,data) SELECT * FROM tmp_folders;", return false);

    QUERY_RUN("ALTER TABLE items RENAME TO tmp_items;", return false);
    QUERY_RUN("CREATE TABLE items ("
              "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
              "type           INTEGER,"
              "keyqms         TEXT NOT NULL,"
              "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
              "icon           BLOB NOT NULL,"
              "name           TEXT NOT NULL,"
              "comment        TEXT,"
              "data           BLOB NOT NULL"
              ");", return false);
    QUERY_RUN("INSERT INTO items(id,type,keyqms,date,icon,name,comment,data) SELECT * FROM tmp_items;", return false);


    return true;
}

bool IDBSqlite::migrateDB2to3()
{
    QSqlQuery query(db);

    QUERY_RUN("ALTER TABLE items ADD COLUMN hash TEXT NOT NULL DEFAULT '-'",            return false);
    QUERY_RUN("ALTER TABLE items ADD COLUMN last_user TEXT NOT NULL DEFAULT 'QMapShack'", return false);
    QUERY_RUN("ALTER TABLE items ADD COLUMN last_change DATETIME NOT NULL DEFAULT '-'", return false);

    QUERY_RUN("CREATE TRIGGER items_update_last_change "
              "AFTER UPDATE ON items BEGIN "
              "UPDATE items SET last_change=datetime(CURRENT_TIMESTAMP, 'localtime') WHERE id=NEW.id; "
              "END;", return false);

    QUERY_RUN("SELECT Count(*) FROM items", return false);
    query.next();
    quint32 N = query.value(0).toUInt();

    QUERY_RUN("SELECT id, type FROM items WHERE hash='-'", return false);

    PROGRESS_SETUP(tr("Update to database version 3. Migrate all GIS items."), 0, N, CMainWindow::self().getBestWidgetForParent());
    progress.enableCancel(false);
    quint32 cnt = 0;
    while(query.next())
    {
        PROGRESS(cnt++,;
                 );

        quint64 idItem      = query.value(0).toULongLong();
        quint32 typeItem    = query.value(1).toUInt();

        IGisItem *item = IGisItem::newGisItem(typeItem, idItem, db, nullptr);

        if(nullptr == item)
        {
            continue;
        }

        QSqlQuery query2(db);
        query2.prepare("UPDATE items SET hash=:hash, last_user='QMapShack' WHERE id=:id");
        query2.bindValue(":hash", item->getHash());
        query2.bindValue(":id", idItem);
        if(!query2.exec())
        {
            qWarning() << query2.lastQuery();
            qWarning() << query2.lastError();
        }

        delete item;
    }

    return true;
}

bool IDBSqlite::migrateDB3to4()
{
    QSqlQuery query(db);

    QUERY_RUN("ALTER TABLE items ADD COLUMN trash DATETIME DEFAULT NULL", return false);

    QUERY_RUN("CREATE TRIGGER folder2item_insert "
              "BEFORE INSERT ON folder2item BEGIN "
              "UPDATE items SET trash=NULL "
              "WHERE id=NEW.child; "
              "END;", return false);

    QUERY_RUN("CREATE TRIGGER folder2item_delete "
              "AFTER DELETE ON folder2item BEGIN "
              "UPDATE items SET trash=CURRENT_TIMESTAMP "
              "WHERE id=OLD.child AND OLD.child NOT IN(SELECT child FROM folder2item); "
              "END;", return false);

    return true;
}

bool IDBSqlite::migrateDB4to5()
{
    QSqlQuery query(db);

    // create virtual table with search index
    QUERY_RUN("CREATE VIRTUAL TABLE searchindex USING fts4(id, comment)", return false);

    // get number of items in the database
    QUERY_RUN("SELECT Count(*) FROM items", return false);
    query.next();
    quint32 N = query.value(0).toUInt();

    // over all items
    QUERY_RUN("SELECT id, type FROM items", return false);
    PROGRESS_SETUP(tr("Update to database version 5. Migrate all GIS items."), 0, N, CMainWindow::self().getBestWidgetForParent());
    progress.enableCancel(false);
    quint32 cnt = 0;
    while(query.next())
    {
        PROGRESS(cnt++,;
                 );

        quint64 idItem      = query.value(0).toULongLong();
        quint32 typeItem    = query.value(1).toUInt();

        IGisItem *item = IGisItem::newGisItem(typeItem, idItem, db, nullptr);


        if(nullptr == item)
        {
            continue;
        }

        // get full size info text
        QString comment = item->getInfo(true, true);

        // replace comment with full size info text in items table
        QSqlQuery query2(db);
        query2.prepare("UPDATE items SET comment=:comment WHERE id=:id");
        query2.bindValue(":comment", comment);
        query2.bindValue(":id", idItem);
        if(!query2.exec())
        {
            qWarning() << query2.lastQuery();
            qWarning() << query2.lastError();
        }

        // add item id and full size info text to virtual table
        query2.prepare("INSERT INTO searchindex(id, comment) VALUES(:id, :comment)");
        query2.bindValue(":id", idItem);
        query2.bindValue(":comment", comment);
        if(!query2.exec())
        {
            qWarning() << query2.lastQuery();
            qWarning() << query2.lastError();
        }

        delete item;
    }


    QUERY_RUN("CREATE TRIGGER searchindex_update "
              "AFTER UPDATE ON items BEGIN "
              "UPDATE searchindex SET comment=NEW.comment "
              "WHERE id=OLD.id; "
              "END;", return false);
    QUERY_RUN("CREATE TRIGGER searchindex_insert "
              "AFTER INSERT ON items BEGIN "
              "INSERT INTO searchindex(id, comment) VALUES(NEW.id, NEW.comment); "
              "END;", return false);


    return true;
}

bool IDBSqlite::migrateDB5to6()
{
    QSqlQuery query(db);

    QUERY_RUN("ALTER TABLE folders ADD COLUMN sortmode INTEGER NOT NULL DEFAULT 0", return false);

    // get number of items in the database
    QUERY_RUN("SELECT Count(*) FROM items", return false);
    query.next();
    quint32 N = query.value(0).toUInt();

    // over all items
    QUERY_RUN("SELECT id, type FROM items", return false);
    PROGRESS_SETUP(tr("Update to database version 6. Migrate all GIS items."), 0, N, CMainWindow::self().getBestWidgetForParent());
    progress.enableCancel(false);
    quint32 cnt = 0;
    while(query.next())
    {
        PROGRESS(cnt++,;
                 );

        quint64 idItem      = query.value(0).toULongLong();
        quint32 typeItem    = query.value(1).toUInt();

        IGisItem *item = IGisItem::newGisItem(typeItem, idItem, db, nullptr);

        if(nullptr == item)
        {
            continue;
        }

        // get full size info text
        QString comment = item->getInfo(true, true);
        QDateTime date  = item->getTimestamp();

        // replace comment with full size info text in items table
        QSqlQuery query2(db);
        query2.prepare("UPDATE items SET comment=:comment, date=:date WHERE id=:id");
        query2.bindValue(":comment", comment);
        query2.bindValue(":date", date);
        query2.bindValue(":id", idItem);
        if(!query2.exec())
        {
            qWarning() << query2.lastQuery();
            qWarning() << query2.lastError();
        }

        delete item;
    }


    return true;
}

