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


bool IDBSqlite::setupDB(const QString& filename, const QString& connectionName)
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

    query.prepare("PRAGMA locking_mode=EXCLUSIVE");
    QUERY_EXEC(return false);
    query.prepare("PRAGMA temp_store=MEMORY");
    QUERY_EXEC(return false);
    query.prepare("PRAGMA default_cache_size=50");
    QUERY_EXEC(return false);
    query.prepare("PRAGMA page_size=8192");
    QUERY_EXEC(return false);
    query.prepare("PRAGMA synchronous=off");
    QUERY_EXEC(return false);

    // When migrating the database these tables are used.
    // Due to caching they can't be dropped right after the
    // migration. That is why we look for them on startup.
    // And delete them as a second chance.
    if(query.exec("select * from tmp_folders"))
    {
        query.prepare("DROP TABLE tmp_folders;");
        QUERY_EXEC();
    }
    if(query.exec("select * from tmp_items"))
    {
        query.prepare("DROP TABLE tmp_items;");
        QUERY_EXEC();
    }


    if(!query.exec("SELECT version FROM versioninfo"))
    {
        return initDB();
    }
    else if(query.next())
    {
        int version = query.value(0).toInt();
        if(version != DB_VERSION)
        {
            return migrateDB(version);
        }
    }
    else
    {
        return initDB();
    }

    query.prepare( "UPDATE folders SET name=:name WHERE id=1");
    query.bindValue(":name", connectionName);
    QUERY_EXEC()

    return true;
}

bool IDBSqlite::initDB()
{
    QSqlQuery query(db);

    if(query.exec( "CREATE TABLE versioninfo ( version TEXT, type TEXT )"))
    {
        query.prepare( "INSERT INTO versioninfo (version, type) VALUES(:version, 'QMapShack')");
        query.bindValue(":version", DB_VERSION);
        QUERY_EXEC(return false);
    }

    if(!query.exec( "CREATE TABLE folders ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "type           INTEGER NOT NULL,"
                    "keyqms         TEXT,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "locked         BOOLEAN DEFAULT FALSE,"
                    "data           BLOB"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }

    if(!query.exec( "CREATE TABLE items ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "type           INTEGER,"
                    "keyqms         TEXT NOT NULL,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "icon           BLOB NOT NULL,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "data           BLOB NOT NULL,"
                    "hash           TEXT NOT NULL,"
                    "last_user      TEXT DEFAULT 'n/a',"
                    "last_change    DATETIME DEFAULT CURRENT_TIMESTAMP"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }

    if(!query.exec("CREATE TRIGGER items_update_last_change "
                   "AFTER UPDATE ON items BEGIN "
                   "UPDATE items SET last_change=CURRENT_TIMESTAMP WHERE id=NEW.id; "
                   "END;"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }


    query.prepare("INSERT INTO folders (type, name, comment) VALUES (2, :name, '')");
    query.bindValue(":name", db.connectionName());
    QUERY_EXEC(return false);

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
        return false;
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
        return false;
    }
    return true;
}

bool IDBSqlite::migrateDB(int version)
{
    QString msg = QObject::tr("The internal database format has changed. QMapShack will migrate your database, now. "
                              "After the migration the database won't be usable with older versions of QMapShack. "
                              "It is recommended to backup the database first.");
    int res = QMessageBox::warning(CMainWindow::self().getBestWidgetForParent(),
                                   QObject::tr("Migrate database..."),
                                   msg,
                                   QMessageBox::Ok|QMessageBox::Abort);
    if(res != QMessageBox::Ok)
    {
        exit(0);
    }

    QSqlQuery query(db);

    if(version < 2)
    {
        if(!migrateDB1to2())
        {
            return false;
        }
    }

    if(version < 3)
    {
        if(!migrateDB2to3())
        {
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

    query.prepare("BEGIN TRANSACTION;");
    QUERY_EXEC(return false);
    query.prepare("ALTER TABLE folders RENAME TO tmp_folders;");
    QUERY_EXEC(return false);
    query.prepare("CREATE TABLE folders ("
                  "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "type           INTEGER NOT NULL,"
                  "keyqms         TEXT,"
                  "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                  "name           TEXT NOT NULL,"
                  "comment        TEXT,"
                  "locked         BOOLEAN DEFAULT FALSE,"
                  "data           BLOB"
                  ");");
    QUERY_EXEC(return false);
    query.prepare("INSERT INTO folders(id,type,keyqms,date,name,comment,locked,data) SELECT * FROM tmp_folders;");
    QUERY_EXEC(return false);
    query.prepare("COMMIT;");
    QUERY_EXEC(return false);

    query.prepare("BEGIN TRANSACTION;");
    QUERY_EXEC(return false);
    query.prepare("ALTER TABLE items RENAME TO tmp_items;");
    QUERY_EXEC(return false);
    query.prepare("CREATE TABLE items ("
                  "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "type           INTEGER,"
                  "keyqms         TEXT NOT NULL,"
                  "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                  "icon           BLOB NOT NULL,"
                  "name           TEXT NOT NULL,"
                  "comment        TEXT,"
                  "data           BLOB NOT NULL"
                  ");");
    QUERY_EXEC(return false);
    query.prepare("INSERT INTO items(id,type,keyqms,date,icon,name,comment,data) SELECT * FROM tmp_items;");
    QUERY_EXEC(return false);
    query.prepare("COMMIT;");
    QUERY_EXEC(return false);


    return true;
}

bool IDBSqlite::migrateDB2to3()
{
    QSqlQuery query(db);

    query.prepare("ALTER TABLE items ADD COLUMN hash TEXT NOT NULL DEFAULT '-'");
    QUERY_EXEC(return false);

    query.prepare("ALTER TABLE items ADD COLUMN last_user TEXT NOT NULL DEFAULT 'n/a'");
    QUERY_EXEC(return false);

    query.prepare("ALTER TABLE items ADD COLUMN last_change DATETIME NOT NULL DEFAULT '-'");
    QUERY_EXEC(return false);

    query.prepare("CREATE TRIGGER items_update_last_change "
                  "AFTER UPDATE ON items BEGIN "
                  "UPDATE items SET last_change=datetime(CURRENT_TIMESTAMP, 'localtime') WHERE id=NEW.id; "
                  "END;");
    QUERY_EXEC(return false);

    query.prepare("SELECT Count(*) FROM items");
    QUERY_EXEC(return false);
    query.next();
    quint32 N = query.value(0).toUInt();

    query.prepare("SELECT id, type FROM items WHERE hash='-'");
    QUERY_EXEC(return false);

    PROGRESS_SETUP("Migrate all GIS items.", 0, N, CMainWindow::self().getBestWidgetForParent());
    progress.enableCancel(false);
    quint32 cnt = 0;
    while(query.next())
    {
        PROGRESS(cnt++,;
                 );

        quint64 idItem      = query.value(0).toULongLong();
        quint32 typeItem    = query.value(1).toUInt();

        IGisItem * item = 0;

        // load item from database for a compare
        switch(typeItem)
        {
        case IGisItem::eTypeWpt:
            item = new CGisItemWpt(idItem, db, 0);
            break;

        case IGisItem::eTypeTrk:
            item = new CGisItemTrk(idItem, db, 0);
            break;

        case IGisItem::eTypeRte:
            item = new CGisItemRte(idItem, db, 0);
            break;

        case IGisItem::eTypeOvl:
            item = new CGisItemOvlArea(idItem, db, 0);
            break;

        default:
            ;
        }

        if(item == 0)
        {
            continue;
        }

        QSqlQuery query2(db);
        query2.prepare("UPDATE items SET hash=:hash, last_user='QMapShack' WHERE id=:id");
        query2.bindValue(":hash", item->getHash());
        query2.bindValue(":id", idItem);
        if(!query2.exec())
        {
            qDebug() << query2.lastQuery();
            qDebug() << query2.lastError();
        }

        delete item;
    }

    return true;
}
