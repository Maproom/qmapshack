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

bool IDBMysql::setupDB(const QString& server, const QString& port, const QString& user, const QString& passwd, bool noPasswd, const QString& name, const QString& connectionName)
{
    // this is important!
    IDB::setup(connectionName);

    if(!QSqlDatabase::contains(connectionName))
    {
        db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setDatabaseName(name);
        db.setHostName(server);

        if(!port.isEmpty())
        {
            quint16 port16 = port.toUInt();
            db.setPort(port16);
        }

        db.setUserName(user);

        if(!noPasswd)
        {
            if(passwd.isEmpty())
            {
                QString p = QInputDialog::getText(CMainWindow::self().getBestWidgetForParent(), tr("Password..."), tr("Password for database '%1':").arg(name), QLineEdit::Password, "");
                db.setPassword(p);
            }
            else
            {
                db.setPassword(passwd);
            }
        }

        qDebug() << "open MySQL database" << name << "@" << server << ":" << port << "as user" << user;

        if(!db.open())
        {
            qDebug() << "failed to open database" << db.lastError();
            return false;
        }
    }
    else
    {
        db = QSqlDatabase::database(connectionName);
    }

    QString error;
    return setupDB(error);
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

    QUERY_RUN( "CREATE TABLE folders ("
               "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
               "type           INTEGER NOT NULL,"
               "keyqms         TEXT,"
               "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
               "name           TEXT NOT NULL,"
               "comment        TEXT,"
               "locked         BOOLEAN DEFAULT FALSE,"
               "data           LONGBLOB"
               ")", return false);

    QUERY_RUN( "CREATE TABLE items ("
               "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
               "type           INTEGER,"
               "keyqms         VARCHAR(64) NOT NULL,"
               "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
               "icon           BLOB NOT NULL,"
               "name           TEXT NOT NULL,"
               "comment        TEXT,"
               "data           LONGBLOB NOT NULL,"
               "hash           TEXT NOT NULL,"
               "last_user      TEXT DEFAULT NULL,"
               "last_change    DATETIME DEFAULT NOW() ON UPDATE NOW(),"
               "trash          DATETIME DEFAULT NULL,"
               "UNIQUE KEY (keyqms)"
               ")", return false);

    QUERY_RUN("CREATE TRIGGER items_insert_last_user "
              "BEFORE INSERT ON items "
              "FOR EACH ROW SET NEW.last_user = USER();"
              , return false);

    QUERY_RUN("CREATE TRIGGER items_update_last_user "
              "BEFORE UPDATE ON items "
              "FOR EACH ROW SET NEW.last_user = USER();"
              , return false);

    query.prepare("INSERT INTO folders (type, name, comment) VALUES (2, :name, '')");
    query.bindValue(":name", db.connectionName());
    QUERY_EXEC(return false);

    QUERY_RUN( "CREATE TABLE folder2folder ("
               "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
               "parent         INTEGER NOT NULL,"
               "child          INTEGER NOT NULL,"
               "FOREIGN KEY(parent) REFERENCES folders(id),"
               "FOREIGN KEY(child) REFERENCES folders(id)"
               ")", return false);

    QUERY_RUN( "CREATE TABLE folder2item ("
               "id             INTEGER PRIMARY KEY AUTO_INCREMENT,"
               "parent         INTEGER NOT NULL,"
               "child          INTEGER NOT NULL,"
               "FOREIGN KEY(parent) REFERENCES folders(id),"
               "FOREIGN KEY(child) REFERENCES items(id)"
               ")", return false);

    QUERY_RUN("CREATE TRIGGER folder2item_insert "
              "BEFORE INSERT ON folder2item "
              "FOR EACH ROW UPDATE items SET trash=NULL "
              "WHERE id=NEW.child;"
              , return false);

    QUERY_RUN("CREATE TRIGGER folder2item_delete "
              "AFTER DELETE ON folder2item "
              "FOR EACH ROW UPDATE items SET trash=CURRENT_TIMESTAMP "
              "WHERE id=OLD.child AND OLD.child NOT IN(SELECT child FROM folder2item);"
              , return false);

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

