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

#include "CMainWindow.h"
#include "gis/db/IDB.h"
#include "gis/db/macros.h"

#include <QtSql>
#include <QtWidgets>

QMap<QString,int> IDB::references;

IDB::IDB()
{
}

IDB::~IDB()
{
    references[db.connectionName()]--;
    if(references[db.connectionName()] == 0 && db.isOpen())
    {
        qDebug() << "close database" << db.connectionName();
        db.close();
    }
}

void IDB::setup(const QString &connectionName)
{
    references[connectionName]++;
}

bool IDB::setupDB()
{
    QSqlQuery query(db);

    if(!query.exec("SELECT version FROM versioninfo"))
    {
        return initDB();
    }
    else if(query.next())
    {
        int version = query.value(0).toInt();
        if(version < DB_VERSION)
        {
            QString msg = tr("The internal database format of '%1'' has changed. QMapShack will migrate your database, now. "
                                      "After the migration the database won't be usable with older versions of QMapShack. "
                                      "It is recommended to backup the database first.").arg(db.connectionName());
            int res = QMessageBox::warning(CMainWindow::self().getBestWidgetForParent(),
                                           tr("Migrate database..."),
                                           msg,
                                           QMessageBox::Ok|QMessageBox::Abort);
            if(res != QMessageBox::Ok)
            {
                exit(0);
            }

            if(!migrateDB(version))
            {
                QString msg = tr("Failed to migrate '%1'.").arg(db.connectionName());
                QMessageBox::critical(CMainWindow::self().getBestWidgetForParent(),
                                      tr("Error..."),
                                      msg,
                                      QMessageBox::Abort);

                return false;
            }
        }
        else if(version > DB_VERSION)
        {
            QString msg = tr("The database version of '%1'' is more advanced as the one understood by your "
                                      "QMapShack installation. This won't work.").arg(db.connectionName());
            QMessageBox::critical(CMainWindow::self().getBestWidgetForParent(),
                                  tr("Wrong database version..."),
                                  msg,
                                  QMessageBox::Abort);
            return false;
        }
    }
    else
    {
        if(!initDB())
        {
            QString msg = tr("Failed to initialize '%1'.").arg(db.connectionName());
            QMessageBox::critical(CMainWindow::self().getBestWidgetForParent(),
                                  tr("Error..."),
                                  msg,
                                  QMessageBox::Abort);

            return false;
        }
    }

    query.prepare( "UPDATE folders SET name=:name WHERE id=1");
    query.bindValue(":name", db.connectionName());
    QUERY_EXEC(return false);

    return true;
}

quint64 IDB::getLastInsertID(QSqlDatabase& db, const QString& table)
{
    quint64 idChild = 0;
    QSqlQuery query(db);

    if(db.driverName() == "QSQLITE")
    {
        QUERY_RUN("SELECT last_insert_rowid() from " + table, return 0)
        query.next();
        idChild = query.value(0).toULongLong();
    }
    else if(db.driverName() == "QMYSQL")
    {
        QUERY_RUN("SELECT last_insert_id() from " + table, return 0)
        query.next();
        idChild = query.value(0).toULongLong();
    }

    return idChild;
}
