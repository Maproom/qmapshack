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

#ifndef CDBFOLDERMYSQL_H
#define CDBFOLDERMYSQL_H

#include "gis/db/IDBFolderSql.h"
#include "gis/db/IDBMysql.h"


class CDBFolderMysql : public IDBFolderSql, public IDBMysql
{
public:
    CDBFolderMysql(const QString &server, const QString &user, const QString &passwd, bool noPasswd, const QString &name, QTreeWidget *parent);
    virtual ~CDBFolderMysql() = default;

    const QString& getServer() const {return server; }
    const QString& getUser() const {return user; }
    const QString& getPasswd() const {return passwd; }
    bool hasNoPasswd() const
    {
        return noPasswd;
    }

private:
    const QString server;
    const QString user;
    const QString passwd;
    const bool noPasswd;
};

#endif //CDBFOLDERMYSQL_H

