/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/CGisWidget.h"
#include "gis/db/CDBProject.h"
#include "gis/db/CExportDatabaseThread.h"
#include "gis/db/IDBFolder.h"
#include "gis/db/macros.h"
#include "gis/gpx/CGpxProject.h"

#include <QtSql>

CExportDatabaseThread::CExportDatabaseThread(quint64 id, QSqlDatabase &db, QObject *parent)
    : QThread(parent)
    , parentFolderId(id)
    , db(db)
{
}


void CExportDatabaseThread::start(const QString& path, bool saveAsGpx11, bool delOld)
{
    if(isRunning())
    {
        return;
    }

    if(delOld)
    {
        int res = QMessageBox::question(CMainWindow::self().getBestWidgetForParent(), tr("Delete content...")
                                        , tr("Delete all old content in %1").arg(path)
                                        , QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
        {
            QDir dir(path);
            dir.removeRecursively();
        }
    }

    asGpx11 = saveAsGpx11;
    exportPath = path;
    QThread::start();
}

QString CExportDatabaseThread::simplifyString(const QString& str) const
{
    QString s = str;
    return s.replace(QRegExp("[^\\w\\d]"), "_");
}

void CExportDatabaseThread::slotAbort()
{
    QMutexLocker lock(&mutex);
    keepGoing = false;
}

bool CExportDatabaseThread::getKeepGoing() const
{
    QMutexLocker lock(&mutex);
    return keepGoing;
}

void CExportDatabaseThread::run()
{
    {
        QMutexLocker lock(&mutex);
        keepGoing = true;
    }

    try
    {
        QDir dir(exportPath);
        if(!dir.exists())
        {
            emit sigOut(tr("Create %1").arg(dir.absoluteFilePath(exportPath)));
            if(!dir.mkpath(exportPath))
            {
                throw tr("Failed to create %1").arg(dir.absoluteFilePath(exportPath));
            }
        }

        dumpFolder(parentFolderId, "", exportPath);

        emit sigOut(tr("Done!"));
    }
    catch(const QString& msg)
    {
        emit sigErr(msg);
    }
}

void CExportDatabaseThread::dumpFolder(quint64 id, const QString& parentName, const QString& path)
{
    if(!getKeepGoing())
    {
        throw tr("Abort by user!");
    }

    QDir dir(path);

    QSqlQuery query(db);
    query.prepare("SELECT type, name FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(throw tr("Database Error: %1").arg(query.lastError().text()));
    query.next();

    quint32 type = query.value(0).toUInt();
    QString name = query.value(1).toString();

    QString simplifiedName = simplifyString(name);


    if(type < IDBFolder::eTypeProject)
    {
        // if it is a group or database folder create a new subdirectory-

        if(!dir.exists(simplifiedName))
        {
            emit sigOut(tr("Create %1").arg(dir.absoluteFilePath(simplifiedName)));
            if(!dir.mkpath(simplifiedName))
            {
                throw tr("Failed to create %1").arg(path);
            }
        }
        dir.cd(simplifiedName);
    }
    else
    {
        // if it is a project or other folder dump it to a GPX file
        const QString connectionName = db.connectionName();
        CDBProject prj(connectionName, id, 0);

        CEvtD2WShowItems evt(id, connectionName);
        query.prepare("SELECT id, type FROM items WHERE id IN (SELECT child FROM folder2item WHERE parent=:parent)");
        query.bindValue(":parent", id);
        QUERY_EXEC(throw tr("Database Error: %1").arg(query.lastError().text()));
        while(query.next())
        {
            quint64 itemId      = query.value(0).toULongLong();
            quint32 itemType    = query.value(1).toUInt();
            evt.items << evt_item_t(itemId, itemType);
        }
        prj.showItems(&evt);

        QString simplifiedProjName = simplifyString(prj.getName());

        // use simplified project name as filename. If the folder is of type "other" prepend it with the parent folder's name.
        QString filename = dir.absoluteFilePath((!parentName.isEmpty() && (type == IDBFolder::eTypeOther)) ?  parentName + "_" + simplifiedProjName : simplifiedProjName) + ".gpx";
        sigOut(tr("Save project as %1").arg(filename));

        if(!CGpxProject::saveAs(filename,  prj, asGpx11))
        {
            throw tr("Failed!");
        }
    }

    // query all child folders to this folder
    query.prepare("SELECT child FROM folder2folder WHERE parent=:parent");
    query.prepare("SELECT id, name FROM folders WHERE id IN (SELECT child FROM folder2folder WHERE parent=:parent)");
    query.bindValue(":parent", id);
    QUERY_EXEC(throw tr("Database Error: %1").arg(query.lastError().text()));
    while(query.next())
    {
        quint64 childId = query.value(0).toULongLong();
        dumpFolder(childId, simplifiedName, dir.absolutePath());
    }
}
