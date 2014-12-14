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
#include "qlgt/CQmsDb.h"
#include "qlgt/CImportDatabase.h"
#include "qlgt/CQlgtWpt.h"
#include "gis/wpt/CGisItemWpt.h"

#include <QtSql>

CQmsDb::CQmsDb(const QString &filename, CImportDatabase *parent)
    : QObject(parent)
    , gui(parent)
{
    if(QFile::exists(filename))
    {
        gui->stdErr(tr("Remove existing file %1").arg(filename));
        QFile::remove(filename);
    }
    setupDB(filename, "qlgt2qms");

}

CQmsDb::~CQmsDb()
{

}

void CQmsDb::addWpt(CQlgtWpt& wpt1)
{
    CGisItemWpt wpt(wpt1);
    quint64 id = store(wpt);
    if(id != 0)
    {
        mapItemIDs[wpt1.id] = id;
    }
}

quint64 CQmsDb::store(IGisItem& item)
{
    // serialize complete history of item
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);
    in << item.getHistory();

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QPixmap pixmap = item.getIcon();
    pixmap.save(&buffer, "PNG");
    buffer.seek(0);

    QSqlQuery query(db);
    // item is unknown to database -> create item in database
    query.prepare("INSERT INTO items (type, key, icon, name, comment, data) VALUES (:type, :key, :icon, :name, :comment, :data)");
    query.bindValue(":type",    item.type());
    query.bindValue(":key",     item.getKey().item);
    query.bindValue(":icon",    buffer.data());
    query.bindValue(":name",    item.getName());
    query.bindValue(":comment", item.getInfo());
    query.bindValue(":data", data);
    QUERY_EXEC(return 0);

    query.prepare("SELECT last_insert_rowid() from items");
    QUERY_EXEC(return 0);
    query.next();
    quint64 idItem = query.value(0).toULongLong();

    return idItem;

}
