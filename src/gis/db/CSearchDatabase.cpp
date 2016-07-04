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

#include "gis/CGisListDB.h"
#include "gis/db/CSearchDatabase.h"
#include "gis/db/IDBFolder.h"

CSearchDatabase::CSearchDatabase(IDBFolder &db, CGisListDB *parent)
    : QDialog(parent)
    , db(db)
{
    setupUi(this);

    connect(pushSearch, &QPushButton::clicked, this, &CSearchDatabase::slotSearch);
    connect(pushClose, &QPushButton::clicked, this, &CSearchDatabase::accept);
}


void CSearchDatabase::slotSearch()
{
    treeResult->clear();
    db.search(lineQuery->text(), treeResult);
}
