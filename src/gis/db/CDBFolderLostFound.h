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

#ifndef CDBFOLDERLOSTFOUND_H
#define CDBFOLDERLOSTFOUND_H

#include "gis/db/IDBFolder.h"

class CDBFolderLostFound : public IDBFolder
{
public:
    CDBFolderLostFound(QSqlDatabase &db, QTreeWidgetItem *parent);
    virtual ~CDBFolderLostFound();

    void update(CEvtW2DAckInfo * info) override;
    void update() override;
    void expanding() override
    {
    }
    void clear();
    bool delItem(CDBItem * item);


protected:
    virtual void setupFromDB() override;
};

#endif //CDBFOLDERLOSTFOUND_H

