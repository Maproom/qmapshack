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

#include "gis/db/macros.h"
#include "qlgt/CQlgtDb.h"
#include "qlgt/CQlgtDiary.h"
#include "qlgt/CQlgtFolder.h"

#include <QtSql>

CQlgtFolder::CQlgtFolder(quint64 id, QSqlDatabase &db)
    : type(0)
    , locked(false)
    , diary(nullptr)
    , id(id)
{
    QSqlQuery query(db);

    query.prepare("SELECT type, name, comment, locked FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );

    if(query.next())
    {
        type = query.value(0).toInt();
        name = query.value(1).toString();
        comment = query.value(2).toString();
        locked = query.value(3).toBool();
    }

    query.prepare("SELECT id, data FROM diarys WHERE parent=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );

    if(query.next())
    {
        quint64 idDiary = query.value(0).toULongLong();
        QByteArray data = query.value(1).toByteArray();
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_5);

        diary = new CQlgtDiary(idDiary, this);
        stream >> *diary;
    }

    query.prepare("SELECT child FROM folder2item WHERE parent=:folder");
    query.bindValue(":folder", id);
    QUERY_EXEC(return );
    while(query.next())
    {
        items << query.value(0).toULongLong();
    }
}

CQlgtFolder::~CQlgtFolder()
{
}

