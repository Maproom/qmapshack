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

#include "gis/db/CDBProject.h"
#include "gis/CGisWidget.h"
#include "gis/db/macros.h"

#include <QtSql>

CDBProject::CDBProject(const QString& dbName, quint64 id, CGisListWks *parent)
    : IGisProject(eTypeDb, "", parent)
    , id(id)    
{
    setIcon(0,QIcon("://icons/32x32/DBProject.png"));
    db = QSqlDatabase::database(dbName);

    QSqlQuery query(db);
    query.prepare("SELECT date, name, data FROM folders WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return);
    query.next();

    QString date    = query.value(0).toString();
    QString name    = query.value(1).toString();
    QByteArray data = query.value(2).toByteArray();

    if(data.isEmpty())
    {
        metadata.name = name;
        metadata.time = QDateTime::fromString(date,"yyyy-MM-dd hh:mm:ss");
    }
    else
    {
        QDataStream in(&data, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_5_2);
        *this << in;
    }

    setText(0, name);
    setToolTip(0, getInfo());

    valid = true;
}

CDBProject::~CDBProject()
{
    CGisWidget::self().queueActionForDb(action_t(eActW2DCloseProject, "", id, 0));
}

void CDBProject::save()
{

}

void CDBProject::saveAs()
{

}

