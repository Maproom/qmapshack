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

#include "gis/CGisListDB.h"
#include "gis/db/CDBFolderMysql.h"
#include "gis/db/macros.h"

#include <QtSql>

CDBFolderMysql::CDBFolderMysql(const QString& server, const QString& port, const QString& user, const QString& passwd, bool noPasswd, const QString& name, QTreeWidget* parent)
    : IDBFolderSql(IDB::db, parent)
    , server(server)
    , port(port)
    , user(user)
    , passwd(passwd)
    , noPasswd(noPasswd)
    , name(name)
{
    setText(CGisListDB::eColumnName, name);
    QTimer::singleShot(200, this, &CDBFolderMysql::slotDelayedSetup);
}

void CDBFolderMysql::slotDelayedSetup()
{
    if(setupDB(server, port, user, passwd, noPasswd, name, name))
    {
        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/MySQL.png"));

        setupFromDB();

        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    else
    {
        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/MySQLNoConn.png"));
    }

    setToolTip(CGisListDB::eColumnName, getDBInfo());
}

QString CDBFolderMysql::getDBInfo() const
{
    QString str = "<div style='font-weight: bold;'>" + IDB::db.connectionName() + "</div><br />";
    str += tr("MySQL Database") + "<br />";
    if(!port.isEmpty())
    {
        str += tr("Server: ") + QString("<i>%1:%2</i>").arg(server, port);
    }
    else
    {
        str += tr("Server: ") + QString("<i>%1</i>").arg(server);
    }

    if(noPasswd)
    {
        str += tr(" (No PW)");
    }

    if(!isUsable())
    {
        const QString& dbError = IDB::db.lastError().databaseText();
        const QString& drError = IDB::db.lastError().driverText();

        str += "<br />" + tr("Error: ") + QString("<span style=\"color:#f00; font-weight:bold;\">%1</span>").arg(dbError.isEmpty() ? drError : dbError);
    }

    return str;
}

bool CDBFolderMysql::search(const QString& str, QSqlQuery& query)
{
    query.prepare("SELECT id FROM items WHERE MATCH(comment) AGAINST (:str IN BOOLEAN MODE)");
    query.bindValue(":str", str);
    QUERY_EXEC(return false);

    return true;
}

void CDBFolderMysql::copyFolder(quint64 child, quint64 parent) //override;
{
    QSqlQuery query(IDB::db);
    query.prepare("INSERT INTO folder2folder (parent, child) SELECT :parent, :child FROM DUAL WHERE NOT EXISTS (SELECT id FROM folder2folder WHERE parent=:parent AND child=:child)");
    query.bindValue(":parent", parent);
    query.bindValue(":child", child);
    QUERY_EXEC(return );
}
