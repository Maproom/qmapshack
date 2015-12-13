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
#include "gis/db/IDBMysql.h"
#include "gis/db/macros.h"

#include <QtSql>
#include <QtWidgets>


IDBMysql::IDBMysql()
{
}


bool IDBMysql::setupDB(const QString& server,const QString& user, const QString& passwd, const QString& name, const QString& connectionName)
{
    // this is important!
    IDB::setup(connectionName);

    if(!QSqlDatabase::contains(connectionName))
    {
        db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setDatabaseName(name);
        db.setHostName(server);
        db.setUserName(user);

        if(passwd.isEmpty())
        {
            QString p = QInputDialog::getText(CMainWindow::self().getBestWidgetForParent(), QObject::tr("Password..."), QObject::tr("Password for database '%1':").arg(name), QLineEdit::Password, "");
            db.setPassword(p);
        }
        else
        {
            db.setPassword(passwd);
        }



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

bool IDBMysql::initDB()
{
    QSqlQuery query(db);

    if(query.exec( "CREATE TABLE versioninfo ( version TEXT, type TEXT )"))
    {
        query.prepare( "INSERT INTO versioninfo (version, type) VALUES(:version, 'QMapShack')");
        query.bindValue(":version", DB_VERSION);
        QUERY_EXEC(return false);
    }

    if(!query.exec( "CREATE TABLE folders ("
                    "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
                    "type           INTEGER NOT NULL,"
                    "keyqms         TEXT,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "locked         BOOLEAN DEFAULT FALSE,"
                    "data           LONGBLOB"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }

    if(!query.exec( "CREATE TABLE items ("
                    "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
                    "type           INTEGER,"
                    "keyqms         TEXT NOT NULL,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "icon           BLOB NOT NULL,"
                    "name           TEXT NOT NULL,"
                    "comment        TEXT,"
                    "data           LONGBLOB NOT NULL,"
                    "hash           TEXT NOT NULL,"
                    "last_user      TEXT DEFAULT NULL,"
                    "last_change    DATETIME DEFAULT NOW() ON UPDATE NOW()"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }

    if(!query.exec("CREATE TRIGGER items_insert_last_user BEFORE INSERT ON items FOR EACH ROW SET NEW.last_user = USER();"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }

    if(!query.exec("CREATE TRIGGER items_update_last_user BEFORE UPDATE ON items FOR EACH ROW SET NEW.last_user = USER();"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }

    query.prepare("INSERT INTO folders (type, name, comment) VALUES (2, :name, '')");
    query.bindValue(":name", db.connectionName());
    QUERY_EXEC(return false);

    if(!query.exec( "CREATE TABLE folder2folder ("
                    "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
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
                    "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
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

bool IDBMysql::migrateDB(int version)
{
    QSqlQuery query(db);

    return false;

    query.prepare( "UPDATE versioninfo set version=:version");
    query.bindValue(":version", DB_VERSION);
    QUERY_EXEC(return false);
    return true;
}
