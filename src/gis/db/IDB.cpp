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


quint64 IDB::getLastInsertID(QSqlDatabase& db, const QString& table)
{
    quint64 idChild = 0;
    QSqlQuery query(db);

    if(db.driverName() == "QSQLITE")
    {
        query.prepare("SELECT last_insert_rowid() from " + table);
        QUERY_EXEC(return 0);
        query.next();
        idChild = query.value(0).toULongLong();
    }
    else if(db.driverName() == "QMYSQL")
    {
        query.prepare("SELECT last_insert_id() from " + table);
        QUERY_EXEC(return 0);
        query.next();
        idChild = query.value(0).toULongLong();
    }

    return idChild;
}
