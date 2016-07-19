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

#ifndef IDBSQLITE_H
#define IDBSQLITE_H

#include "gis/db/IDB.h"

class IDBSqlite : public IDB
{
public:
    IDBSqlite();
    virtual ~IDBSqlite() = default;

protected:
    using IDB::setupDB;
    bool setupDB(const QString &filename, const QString &connectionName, QString &error);
    bool initDB() override;
    bool migrateDB(int version) override;
    bool migrateDB1to2();
    bool migrateDB2to3();
    bool migrateDB3to4();
    bool migrateDB4to5();
    bool migrateDB5to6();
};

#endif //IDBSQLITE_H

