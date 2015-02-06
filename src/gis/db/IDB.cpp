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

#include "gis/db/IDB.h"
#include "gis/db/macros.h"

#include <QtSql>

IDB::IDB()
{
}

IDB::~IDB()
{
    if(db.isOpen())
    {
        db.close();
    }
}

bool IDB::setupDB(const QString& filename, const QString& connectionName)
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(filename);   
    if(!db.open())
    {
        qDebug() << "failed to open database" << db.lastError();
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
    query.prepare("PRAGMA synchronous=OFF");
    QUERY_EXEC(return false);

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
    QUERY_EXEC(; )

    return true;
}

bool IDB::initDB()
{
    QSqlQuery query(db);

    if(query.exec( "CREATE TABLE versioninfo ( version TEXT, type TEXT )"))
    {
        query.prepare( "INSERT INTO versioninfo (version, type) VALUES(:version, 'QMapShack')");
        query.bindValue(":version", DB_VERSION);
        QUERY_EXEC(return false;  );
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
        return false;
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
        return false;
    }

    query.prepare("INSERT INTO folders (type, name, comment) VALUES (2, :name, '')");
    query.bindValue(":name", db.connectionName());
    QUERY_EXEC(return false;  );

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

bool IDB::migrateDB(int version)
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
    QUERY_EXEC(return false;  );
    return true;
}

