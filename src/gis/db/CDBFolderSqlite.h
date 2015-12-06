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

#ifndef CDBFOLDERSQLITE_H
#define CDBFOLDERSQLITE_H

#include "gis/db/IDBFolder.h"
#include <gis/db/IDBSqlite.h>

class CDBFolderLostFound;

class CDBFolderSqlite : public IDBFolder, private IDBSqlite
{
public:
    CDBFolderSqlite(const QString &filename, const QString &name, QTreeWidget *parent);
    virtual ~CDBFolderSqlite();

    void expanding();
    void updateLostFound();
    const QString& getFilename()
    {
        return filename;
    }

    QSqlDatabase& getDb()
    {
        return IDB::db;
    }

private:
    QString filename;
    CDBFolderLostFound * folderLostFound = 0;
};
#endif //CDBFOLDERSQLITE_H

