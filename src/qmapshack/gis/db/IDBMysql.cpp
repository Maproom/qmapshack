/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"


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
        db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

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
                bool ok;
                QString p = QInputDialog::getText(CMainWindow::self().getBestWidgetForParent(), tr("Password..."), tr("Password for database '%1':").arg(name), QLineEdit::Password, "", &ok);
                if (!ok || p.isEmpty())
                {
                    return false;
                }
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
               "data           LONGBLOB,"
               "sortmode       INTEGER NOT NULL DEFAULT 0"
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
               "FULLTEXT INDEX searchindex(comment),"
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

    try
    {
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
    }
    catch(int i)
    {
        if(i == -1)
        {
            return false;
        }
    }

    query.prepare( "UPDATE versioninfo set version=:version");
    query.bindValue(":version", DB_VERSION);
    QUERY_EXEC(return false);
    return true;
}

bool IDBMysql::migrateDB4to5()
{
    QSqlQuery query(db);

    // id and comment to full text search index
    QUERY_RUN("ALTER TABLE items ADD FULLTEXT INDEX searchindex (comment)", return false);


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

        quint64 itemId      = query.value(0).toULongLong();
        quint32 itemType    = query.value(1).toUInt();
        IGisItem *item      = IGisItem::newGisItem(itemType, itemId, db, nullptr);

        if(nullptr == item)
        {
            continue;
        }

        // get full size info text
        QString comment = item->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowFullText);

        // replace comment with full size info text in items table
        QSqlQuery query2(db);
        query2.prepare("UPDATE items SET comment=:comment WHERE id=:id");
        query2.bindValue(":comment", comment);
        query2.bindValue(":id", itemId);
        if(!query2.exec())
        {
            qWarning() << query2.lastQuery();
            qWarning() << query2.lastError();
        }

        delete item;
    }

    return true;
}

bool IDBMysql::migrateDB5to6()
{
    QSqlQuery query(db);

    QUERY_RUN("ALTER TABLE folders ADD COLUMN sortmode INTEGER NOT NULL DEFAULT 0", return false);

    // get number of items in the database
    QUERY_RUN("SELECT Count(*) FROM items", return false);
    query.next();
    quint32 N = query.value(0).toUInt();

    // over all items
    QUERY_RUN("SELECT id, type FROM items", return false);
    PROGRESS_SETUP(("Update to database version 6. Migrate all GIS items."), 0, N, CMainWindow::self().getBestWidgetForParent());
    progress.enableCancel(false);
    quint32 cnt = 0;
    while(query.next())
    {
        PROGRESS(cnt++,;
                 );

        quint64 itemId      = query.value(0).toULongLong();
        quint32 itemType    = query.value(1).toUInt();
        IGisItem *item      = IGisItem::newGisItem(itemType, itemId, db, nullptr);


        if(nullptr == item)
        {
            continue;
        }

        // get full size info text
        QString comment = item->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowFullText);
        QDateTime date  = item->getTimestamp();

        // replace comment with full size info text in items table
        QSqlQuery query2(db);
        query2.prepare("UPDATE items SET comment=:comment, date=:date WHERE id=:id");
        query2.bindValue(":comment", comment);
        query2.bindValue(":date", date);
        query2.bindValue(":id", itemId);
        if(!query2.exec())
        {
            qWarning() << query2.lastQuery();
            qWarning() << query2.lastError();
        }

        delete item;
    }


    return true;
}

