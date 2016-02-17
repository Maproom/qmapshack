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

#include "canvas/CCanvas.h"
#include "gis/db/CDBFolderMysql.h"
#include "gis/db/CDBFolderSqlite.h"
#include "gis/db/CSelectDBFolder.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CSelectDBFolder::CSelectDBFolder(quint64 &id, QString &db, QString &host, QWidget *parent)
    : QDialog(parent)
    , id(id)
    , db(db)
    , host(host)
{
    setupUi(this);

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    SETTINGS;
    cfg.beginGroup("Database");
    QStringList names = cfg.value("names").toStringList();
    cfg.beginGroup("Entries");
    foreach(const QString &name, names)
    {
        cfg.beginGroup(name);
        QString type = cfg.value("type", "SQLite").toString();
        if(type == "SQLite")
        {
            QString filename = cfg.value("filename","").toString();
            new CDBFolderSqlite(filename, name, treeWidget);
        }

        if(type == "MySQL")
        {
            QString server  = cfg.value("server","").toString();
            QString port    = cfg.value("port","").toString();
            QString user    = cfg.value("user","").toString();
            QString passwd  = cfg.value("passwd","").toString();
            bool noPasswd   = cfg.value("noPasswd",false).toBool();
            new CDBFolderMysql(server, port, user, passwd, noPasswd, name, treeWidget);
        }
        cfg.endGroup(); // name
    }
    cfg.endGroup(); // Database

    connect(treeWidget, &QTreeWidget::itemExpanded,         this, &CSelectDBFolder::slotItemExpanded);
    connect(treeWidget, &QTreeWidget::itemSelectionChanged, this, &CSelectDBFolder::slotItemSelectionChanged);

    CCanvas::setOverrideCursor(Qt::ArrowCursor, "CSelectDBFolder");
}

CSelectDBFolder::~CSelectDBFolder()
{
    CCanvas::restoreOverrideCursor("~CSelectDBFolder");
}

void CSelectDBFolder::slotItemExpanded(QTreeWidgetItem * item)
{
    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(nullptr != folder)
    {
        folder->expanding();
    }
}

void CSelectDBFolder::slotItemSelectionChanged()
{
    IDBFolder * folder = dynamic_cast<IDBFolder*>(treeWidget->currentItem());
    if(folder)
    {
        id      = folder->getId();
        db      = folder->getDBName();
        host    = folder->getDBHost();
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else
    {
        id = -1;
        db.clear();
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}
