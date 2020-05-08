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

#ifndef IDBFOLDERSQL_H
#define IDBFOLDERSQL_H

#include "gis/db/IDBFolder.h"

class CDBFolderLostFound;
class QUdpSocket;

#define UDP_PORT 34123

class IDBFolderSql : public IDBFolder, public QObject
{
public:
    IDBFolderSql(QSqlDatabase& db, QTreeWidget * parent);
    virtual ~IDBFolderSql() = default;

    void expanding() override;
    void updateLostFound();
    void update(CEvtW2DAckInfo * info) override
    {
        IDBFolder::update(info);
    }
    bool update() override;

    void announceChange() const;

    virtual void copyFolder(quint64 child, quint64 parent) = 0;

protected:
    CDBFolderLostFound * folderLostFound = nullptr;

    QUdpSocket * socket;
};

#endif //IDBFOLDERSQL_H

