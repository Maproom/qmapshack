/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Softwareation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "qlgt/CImportDatabase.h"
#include "qlgt/CQlb.h"
#include "qlgt/CQlgtDb.h"
#include "qlgt/CQlgtFolder.h"
#include "qlgt/CQlgtRoute.h"
#include "qlgt/CQlgtTrack.h"
#include "qlgt/CQlgtWpt.h"
#include "qlgt/CQmsDb.h"
#include "qlgt/IQlgtOverlay.h"

#include "gis/WptIcons.h"
#include "gis/db/macros.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

#include "CMainWindow.h"

#include <QtSql>
#include <QtWidgets>

#define DB_QLGT_VERSION 9

CQlgtDb::CQlgtDb(const QString &filename, CImportDatabase *parent)
    : gui(parent)
    , nItems(0)
    , nFolders(0)
    , nWpt(0)
    , nTrk(0)
    , nRte(0)
    , nOvl(0)
    , nDiary(0)
{
    db = QSqlDatabase::addDatabase("QSQLITE","qlandkarte");
    db.setDatabaseName(filename);
    db.open();

    QFileInfo fi(filename);
    path = fi.absoluteDir();
    name = fi.fileName();

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

    if(!query.exec("PRAGMA default_cache_size=1000"))
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
        if(version != DB_QLGT_VERSION)
        {
            migrateDB(version);
        }
    }
    else
    {
        initDB();
    }

    printStatistic();
}

CQlgtDb::~CQlgtDb()
{
}

void CQlgtDb::initDB()
{
    qDebug() << "void CGeoDB::initDB()";
    QSqlQuery query(db);

    if(query.exec( "CREATE TABLE versioninfo ( version TEXT )"))
    {
        query.prepare( "INSERT INTO versioninfo (version) VALUES(:version)");
        query.bindValue(":version", DB_QLGT_VERSION);
        QUERY_EXEC(; );
    }

    if(!query.exec( "CREATE TABLE folders ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "type           INTEGER,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "icon           TEXT NOT NULL,"
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
                    "changed        BOOLEAN DEFAULT FALSE,"
                    "data           BLOB NOT NULL,"
                    "key            TEXT NOT NULL"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }

    if(!query.exec("INSERT INTO folders (icon, name, comment) VALUES ('', 'database', '')"))
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

    if(!query.exec( "CREATE TABLE diarys ("
                    "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "parent         INTEGER NOT NULL,"
                    "key            TEXT NOT NULL,"
                    "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "data           BLOB NOT NULL,"
                    "FOREIGN KEY(parent) REFERENCES folders(id)"
                    ")"))
    {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
}


void CQlgtDb::migrateDB(int version)
{
    qDebug() << "void CGeoDB::migrateDB(int version)" << version;
    QSqlQuery query(db);

    for(version++; version <= DB_QLGT_VERSION; version++)
    {
        switch(version)
        {
        case 1:
            break;

        case 2:
        {
            if(!query.exec( "CREATE TABLE workspace ("
                            "id             INTEGER PRIMARY KEY NOT NULL,"
                            "changed        BOOLEAN DEFAULT FALSE,"
                            "data           BLOB NOT NULL"
                            ")"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            break;
        }

        case 3:
        {
            if(!query.exec("ALTER TABLE workspace ADD COLUMN key TEXT"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }
            if(!query.exec("ALTER TABLE workspace ADD COLUMN type INTEGER"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }
            break;
        }

        case 4:
        {
            if(!query.exec("ALTER TABLE folders ADD COLUMN type INTEGER"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            query.prepare("UPDATE folders SET type=:type WHERE icon=:icon");
            query.bindValue("type", eFolder1);
            query.bindValue("icon", ":/icons/iconFolderBlue16x16.png");
            if(!query.exec())
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            query.prepare("UPDATE folders SET type=:type WHERE icon=:icon");
            query.bindValue(":type", eFolder2);
            query.bindValue(":icon", ":/icons/iconFolderGreen16x16.png");
            if(!query.exec())
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            break;
        }

        case 5:
        {
            if (QFile::exists(path.absoluteFilePath("qlgt_save_v4.db")))
            {
                QFile::remove(path.absoluteFilePath("qlgt_save_v4.db"));
            }
            QFile f(path.absoluteFilePath(name));
            f.copy(path.absoluteFilePath("qlgt_save_v4.db"));

            QSqlQuery query2(db);

            if(!query.exec("SELECT id, type, icon FROM items"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            quint32 progCnt = 0;
            PROGRESS_SETUP(tr("Migrating database from version 4 to 5."), query.size(), &CMainWindow::self());

            while(query.next())
            {
                QPixmap pixmap;

                if(query.value(1).toInt() == eWpt || query.value(1).toInt() == eRte)
                {
                    QPointF focus;
                    pixmap = getWptIconByName(query.value(2).toString(), focus);
                }
                else if(query.value(1).toInt() == eTrk)
                {
                    pixmap = QPixmap(16,16);
                    pixmap.fill(query.value(2).toString());
                }
                else
                {
                    pixmap = QPixmap(query.value(2).toString());
                }

                QByteArray bytes;
                QBuffer buffer(&bytes);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer, "XPM");

                query2.prepare("UPDATE items SET icon=:icon WHERE id=:id");
                query2.bindValue(":id", query.value(0).toULongLong());
                query2.bindValue(":icon", bytes);
                if(!query2.exec())
                {
                    qDebug() << query.lastQuery();
                    qDebug() << query.lastError();
                    return;
                }

                PROGRESS(progCnt++, continue);
            }
            break;
        }

        case 6:
        {
            QSqlQuery query2(db);

            if(!query.exec("SELECT id, data, type FROM items"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            quint32 progCnt = 0;
            PROGRESS_SETUP(tr("Migrating database from version 5 to 6."), query.size(), &CMainWindow::self());

            while(query.next())
            {
                QByteArray array = query.value(1).toByteArray();
                QBuffer buffer(&array);
                CQlb qlb(this);
                qlb.load(&buffer);

                switch(query.value(2).toInt())
                {
                case eWpt:
                    array = qlb.waypoints();
                    break;

                case eTrk:
                    array = qlb.tracks();
                    break;

                case eRte:
                    array = qlb.routes();
                    break;

                case eOvl:
                    array = qlb.overlays();
                    break;
                }

                query2.prepare("UPDATE items SET data=:data WHERE id=:id");
                query2.bindValue(":data", array);
                query2.bindValue(":id", query.value(0));

                if(!query2.exec())
                {
                    qDebug() << query.lastQuery();
                    qDebug() << query.lastError();
                    return;
                }

                PROGRESS(progCnt++, continue);
            }

            break;
        }

        case 7:
        {
            QSqlQuery query2(db);

            if(!query.exec("SELECT id, data, type FROM items"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            quint32 progCnt = 0;
            PROGRESS_SETUP(tr("Migrating database from version 6 to 7."), query.size(), &CMainWindow::self());

            while(query.next())
            {
                QString comment;
                QByteArray array = query.value(1).toByteArray();
                QDataStream stream(&array, QIODevice::ReadOnly);
                stream.setVersion(QDataStream::Qt_4_5);

                quint64 id = query.value(0).toULongLong();

                switch(query.value(2).toInt())
                {
                case eWpt:
                {
                    CQlgtWpt wpt(id, 0);
                    stream >> wpt;
                    comment = wpt.getInfo();
                }
                break;

                case eTrk:
                {
                    CQlgtTrack trk(id, 0);
                    stream >> trk;
                    comment = trk.getInfo();
                }
                break;

                case eRte:
                {
                    CQlgtRoute rte(id, 0);
                    stream >> rte;
                    comment = rte.getInfo();
                }
                break;

                case eOvl:
                {
                    //                            IOverlay ovl(0);
                    //                            stream >> ovl;
                    //                            comment = ovl.getInfo();
                    continue;
                }
                break;
                }

                query2.prepare("UPDATE items SET comment=:comment WHERE id=:id");
                query2.bindValue(":comment", comment);
                query2.bindValue(":id", query.value(0));

                if(!query2.exec())
                {
                    qDebug() << query.lastQuery();
                    qDebug() << query.lastError();
                    return;
                }

                PROGRESS(progCnt++, continue);
            }

            break;
        }

        case 8:
        {
            PROGRESS_SETUP(tr("Migrating database from version 7 to 8."), 1, &CMainWindow::self());

            if(!query.exec( "CREATE TABLE diarys ("
                            "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "parent         INTEGER NOT NULL,"
                            "key            TEXT NOT NULL,"
                            "date           DATETIME DEFAULT CURRENT_TIMESTAMP,"
                            "data           BLOB NOT NULL,"
                            "FOREIGN KEY(parent) REFERENCES folders(id)"
                            ")"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            PROGRESS(1, return );
            break;
        }

        case 9:
        {
            if (QFile::exists(path.absoluteFilePath(name)))
            {
                QFile::remove(path.absoluteFilePath("qlgt_save_v8.db"));
            }
            QFile f(path.absoluteFilePath(name));
            f.copy(path.absoluteFilePath("qlgt_save_v4.db"));

            PROGRESS_SETUP(tr("Migrating database from version 8 to 9."), 1, &CMainWindow::self());

            if(!query.exec("ALTER TABLE folders ADD COLUMN locked BOOLEAN DEFAULT FALSE"))
            {
                qDebug() << query.lastQuery();
                qDebug() << query.lastError();
                return;
            }

            PROGRESS(1, return );
            break;
        }
        }
    }
    query.prepare( "UPDATE versioninfo set version=:version");
    query.bindValue(":version", version - 1);
    QUERY_EXEC(; );
}

void CQlgtDb::printStatistic()
{
    QSqlQuery query(db);

    gui->stdOut(tr("Open database: %1").arg(db.databaseName()));

    nItems = 0;

    query.prepare("SELECT COUNT() FROM folders");
    QUERY_EXEC(; );
    if(query.next())
    {
        nFolders = query.value(0).toInt();
        gui->stdOut(tr("Folders:          %1").arg(nFolders));
    }

    query.prepare("SELECT COUNT() FROM items WHERE type=:type");
    query.bindValue(":type", eTrk);
    QUERY_EXEC(; );
    if(query.next())
    {
        nItems += query.value(0).toInt();
        gui->stdOut(tr("Tracks:           %1").arg(query.value(0).toInt()));
    }
    query.prepare("SELECT COUNT() FROM items WHERE type=:type");
    query.bindValue(":type", eRte);
    QUERY_EXEC(; );
    if(query.next())
    {
        nItems += query.value(0).toInt();
        gui->stdErr(tr("Routes:           %1 (Only the basic route will be copied)").arg(query.value(0).toInt()));
    }
    query.prepare("SELECT COUNT() FROM items WHERE type=:type");
    query.bindValue(":type", eWpt);
    QUERY_EXEC(; );
    if(query.next())
    {
        nItems += query.value(0).toInt();
        gui->stdOut(tr("Waypoints:        %1").arg(query.value(0).toInt()));
    }
    query.prepare("SELECT COUNT() FROM items WHERE type=:type");
    query.bindValue(":type", eOvl);
    QUERY_EXEC(; );
    if(query.next())
    {
        nItems += query.value(0).toInt();
        gui->stdErr(tr("Overlays:         %1 (areas will be converted as areas, distance lines will be converted to tracks, all other overlay items will be lost)").arg(query.value(0).toInt()));
    }
    query.prepare("SELECT COUNT() FROM diarys");
    query.bindValue(":type", eDry);
    QUERY_EXEC(; );
    if(query.next())
    {
        gui->stdOut(tr("Diaries:          %1").arg(query.value(0).toInt()));
    }
    query.prepare("SELECT COUNT() FROM items WHERE type=:type");
    query.bindValue(":type", eMap);
    QUERY_EXEC(; );
    if(query.next())
    {
        gui->stdErr(tr("Map selections:   %1 (can't be converted to QMapShack)").arg(query.value(0).toInt()));
    }
}

void CQlgtDb::start(const QString& filename)
{
    gui->stdOut(tr("------ Start to convert database to %1------").arg(filename));
    dbQms = new CQmsDb(filename, gui);
    if(!dbQms->isValid())
    {
        gui->stdErr(tr("Failed to create target database."));
        gui->stdOut(tr("------ Abort ------"));
        return;
    }


    xferItems();
    xferFolders();

    QSqlQuery query(db);
    query.prepare("Select parent, child FROM folder2folder");
    QUERY_EXEC(return; );
    while(query.next())
    {
        quint64 idParent    = query.value(0).toULongLong();
        quint64 idChild     = query.value(1).toULongLong();
        dbQms->addFolder2FolderRelation(idParent, idChild);
    }

    query.prepare("Select parent, child FROM folder2item");
    QUERY_EXEC(return; );
    while(query.next())
    {
        quint64 idParent    = query.value(0).toULongLong();
        quint64 idChild     = query.value(1).toULongLong();
        dbQms->addFolder2ItemRelation(idParent, idChild);
    }

    delete dbQms;
    gui->stdOut(tr("------ Done ------"));
}

void CQlgtDb::xferFolders()
{
    nDiary = 0;

    quint32 cnt = 1;
    QProgressDialog progress(tr("Restore folders..."),tr("Abort"), 0, 100, gui);
    progress.setWindowModality(Qt::WindowModal);

    QSqlQuery query(db);
    query.prepare("SELECT id FROM folders");
    QUERY_EXEC(return; );
    while(query.next())
    {
        progress.setValue(cnt++ *100 / nFolders);
        if (progress.wasCanceled())
        {
            break;
        }

        quint64 idFolder = query.value(0).toULongLong();

        CQlgtFolder folder1(idFolder, db);
        if(folder1.diary)
        {
            nDiary++;
        }

        dbQms->addFolder(folder1);

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    progress.setValue(100);
    gui->stdOut(tr("Imported %1 folders and %2 diaries").arg(nFolders).arg(nDiary));
}

void CQlgtDb::xferItems()
{
    quint32 cnt = 1;
    QProgressDialog progress(tr("Copy items..."),tr("Abort"), 0, 100, gui);
    progress.setWindowModality(Qt::WindowModal);

    nWpt = 0;
    nTrk = 0;
    nRte = 0;
    nOvl = 0;


    QSqlQuery query(db);
    query.prepare("SELECT id FROM items");
    QUERY_EXEC(return; );
    while(query.next())
    {
        progress.setValue(cnt++ *100 / nItems);
        if (progress.wasCanceled())
        {
            break;
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        xferItem(query.value(0).toULongLong());
    }
    progress.setValue(100);

    gui->stdOut(tr("Imported %1 tracks, %2 waypoints, %3 routes, %4 areas").arg(nTrk).arg(nWpt).arg(nRte).arg(nOvl));
    gui->stdOut(tr("Import folders..."));

    query.prepare("SELECT id FROM folders");
    QUERY_EXEC(return; );
}

void CQlgtDb::xferItem(quint64 id)
{
    QSqlQuery query(db);
    query.prepare("SELECT type, data FROM items WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return; );

    if(query.next())
    {
        QByteArray data = query.value(1).toByteArray();
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_5);


        switch(query.value(0).toInt())
        {
        case eWpt:
        {
            CQlgtWpt wpt1(id, 0);
            stream >> wpt1;
            dbQms->addWpt(wpt1);
            nWpt++;
            break;
        }

        case eTrk:
        {
            CQlgtTrack trk1(id, 0);
            stream >> trk1;
            dbQms->addTrk(trk1);
            nTrk++;
            break;
        }

        case eRte:
        {
            CQlgtRoute rte1(id, 0);
            stream >> rte1;
            dbQms->addRte(rte1);
            nRte++;
            break;
        }

        case eOvl:
        {
            IQlgtOverlay ovl1(id, 0);
            stream >> ovl1;
            if(ovl1.type == "Area")
            {
                dbQms->addArea(ovl1);
                nOvl++;
            }
            else if(ovl1.type == "Distance")
            {
                dbQms->addTrk(ovl1);
                nTrk++;
            }
            else
            {
                gui->stdErr(tr("Overlay of type '%1' cant be converted").arg(ovl1.type));
                nOvl++;
                break;
            }


            break;
        }
        }
    }
}
