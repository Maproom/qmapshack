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

#ifndef IDBMYSQL_H
#define IDBMYSQL_H

#include "gis/db/IDB.h"

class IDBMysql  : public IDB
{
public:
    IDBMysql();
    virtual ~IDBMysql() = default;

protected:
    bool setupDB(const QString &server, const QString &user, const QString &passwd, const QString &name, const QString &connectionName);
    bool initDB();
    bool migrateDB(int version);

};

#endif //IDBMYSQL_H

