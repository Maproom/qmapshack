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

#include "gis/db/CExportDatabaseThread.h"
#include "gis/db/macros.h"
#include "gis/db/IDBFolder.h"
#include "gis/db/CDBProject.h"

#include <QtSql>

CExportDatabaseThread::CExportDatabaseThread(quint64 id, QSqlDatabase &db, QObject *parent)
    : QThread(parent)
    , parentFolderId(id)
    , db(db)
{
}


void CExportDatabaseThread::start(const QString& path)
{
    if(isRunning())
    {
        return;
    }
    exportPath = path;
    QThread::start();
}

void CExportDatabaseThread::abort()
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
        dumpFolder(parentFolderId, "", exportPath);
    }
    catch(const QString& msg)
    {
        emit sigErr(msg);
    }

}

bool CExportDatabaseThread::dumpFolder(quint64 id, const QString& parentName, const QString& path)
{
    if(!getKeepGoing())
    {
        throw "";
    }

    QDir dir(path);
    if(!dir.exists())
    {
        emit sigOut(tr("Create %1").arg(path));
        if(!dir.mkpath(path))
        {
            throw tr("Failed to create %1").arg(path);
        }
    }

    QSqlQuery query(db);
    query.prepare("SELECT type, name, data FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(throw tr("Database Error: %1").arg(query.lastError().text()));
    query.next();

    quint32 type    = query.value(0).toUInt();
    QString name    = query.value(1).toString();
    QByteArray data = query.value(2).toByteArray();

    if((type == IDBFolder::eTypeProject) || (type == IDBFolder::eTypeOther))
    {
        CDBProject prj(db.connectionName(), id, 0);

        QString filename = dir.absoluteFilePath((!parentName.isEmpty() && (type == IDBFolder::eTypeOther)) ?  parentName + "_" + prj.getName() : prj.getName()) + ".gpx";
        sigOut(tr("Save project as %1").arg(filename));

        if(!prj.saveAs(filename, IGisProject::filedialogFilterGPX))
        {
            throw tr("Failed!");
        }

    }


    return true;
}
