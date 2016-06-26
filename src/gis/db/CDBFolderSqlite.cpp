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

#include "gis/CGisListDB.h"
#include "gis/db/CDBFolderSqlite.h"

#include <QtCore>

CDBFolderSqlite::CDBFolderSqlite(const QString& filename, const QString& name, QTreeWidget *parent)
    : IDBFolderSql(IDB::db, parent)
    , filename(filename)
{
    setToolTip(CGisListDB::eColumnName, tr("All your data grouped by folders."));

    setText(CGisListDB::eColumnName, name);

    if(setupDB(filename, name, error)) {
        setupFromDB();

        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/SQLite.png"));
        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    else
    {
        IDB::db.close();

        setIcon(CGisListDB::eColumnCheckbox, QIcon("://icons/32x32/SQLiteNoConn.png"));
        setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    }
}

QString CDBFolderSqlite::getDBInfo() const
{
    QString str = "<div style='font-weight: bold;'>" + IDB::db.connectionName() + "</div><br />";
    str += tr("SQLite Database") + "<br />";

    QString path = IDB::db.databaseName();
    #ifndef Q_OS_WIN
    if(path.startsWith(QDir::homePath()))
    {
        path = "~" + path.remove(0, QDir::homePath().length());
    }
    #endif

    str += tr("File: ") + QString("<i>%1</i>").arg(path);

    if(!isUsable())
    {
        str += "<br />" + tr("Error: ") + QString("<span style=\"color:#f00; font-weight:bold;\">%1</span>").arg(error);
    }

    return str;
}
