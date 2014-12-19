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

#include "gis/CGisListDB.h"
#include "gis/CGisWidget.h"
#include "gis/db/macros.h"
#include "gis/db/CSetupFolder.h"
#include "gis/db/CDBFolderDatabase.h"
#include "gis/db/CDBFolderLostFound.h"
#include "gis/db/CDBItem.h"
#include "helpers/CSettings.h"
#include "config.h"

#include <QtSql>
#include <QtWidgets>

class CGisListDBEditLock
{
    public:
        CGisListDBEditLock(bool waitCursor, CGisListDB * widget) : widget(widget), waitCursor(waitCursor)
        {
            if(waitCursor)
            {
                QApplication::setOverrideCursor(Qt::WaitCursor);
            }
            widget->isInternalEdit += 1;
        }
        ~CGisListDBEditLock()
        {
            if(waitCursor)
            {
                QApplication::restoreOverrideCursor();
            }
            widget->isInternalEdit -= 1;
        }
    private:
        CGisListDB * widget;
        bool waitCursor;
};


CGisListDB::CGisListDB(QWidget *parent)
    : QTreeWidget(parent)
    , isInternalEdit(0)
{   

    SETTINGS;
    QString path = cfg.value("Database/path", QDir::home().filePath(CONFIGDIR).append("/database.db")).toString();
    setupDB(path, "Database");
    folderLostFound     = new CDBFolderLostFound(db, this);
    folderDatabase      = new CDBFolderDatabase(db, this);

    menuFolder          = new QMenu(this);
    actionAddFolder     = menuFolder->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Folder"), this, SLOT(slotAddFolder()));
    actionDelFolder     = menuFolder->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Folder"), this, SLOT(slotDelFolder()));

    menuDatabase        = new QMenu(this);
    menuDatabase->addAction(actionAddFolder);

    menuLostFound       = new QMenu(this);
    actionDelLostFound  = menuLostFound->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete"), this, SLOT(slotDelLostFound()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

    folderDatabase->setExpanded(true);
    folderLostFound->update();
}

CGisListDB::~CGisListDB()
{
}


CDBFolderDatabase * CGisListDB::getDataBase(const QString& name)
{
    const int N = topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        CDBFolderDatabase * database = dynamic_cast<CDBFolderDatabase*>(topLevelItem(n));
        if(database && (database->getDBName() == name))
        {
            return database;
        }
    }
    return 0;
}

bool CGisListDB::event(QEvent * e)
{
    CGisListDBEditLock lock(true, this);

    switch(e->type())
    {
    case eEvtW2DAckInfo:
    {
        CEvtW2DAckInfo * evt = (CEvtW2DAckInfo*)e;
        IDBFolder * folder = getDataBase(evt->db);
        if(folder)
        {
            folder->update(evt);

        }
        e->accept();
        return true;
    }
    }

    return QTreeWidget::event(e);
}

void CGisListDB::slotContextMenu(const QPoint& point)
{
    QPoint p = mapToGlobal(point);
    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if((folder == folderDatabase))
    {
        menuDatabase->exec(p);
    }
    else if((folder == folderLostFound))
    {
        menuLostFound->exec(p);
    }
    else if(folder != 0)
    {
        menuFolder->exec(p);
    }
}

void CGisListDB::slotAddFolder()
{
    CGisListDBEditLock lock(false, this);

    IDBFolder * parentFolder = dynamic_cast<IDBFolder*>(currentItem());
    if(parentFolder == 0)
    {
        return;
    }
    quint64 idParent = parentFolder->getId();

    IDBFolder::type_e type = IDBFolder::eTypeProject;
    QString name;
    CSetupFolder dlg(type, name, this);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO folders (name, type) VALUES (:name, :type)");
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    QUERY_EXEC(return);

    query.prepare("SELECT last_insert_rowid() from folders");
    QUERY_EXEC(return);
    query.next();
    quint64 idChild = query.value(0).toULongLong();
    if(idChild == 0)
    {
        qDebug() << "CGisListDB::slotAddFolder(): childId equals 0. bad.";
        return;
    }

    query.prepare("INSERT INTO folder2folder (parent, child) VALUES (:parent, :child)");
    query.bindValue(":parent", idParent);
    query.bindValue(":child", idChild);
    QUERY_EXEC(return);

    IDBFolder::createFolderByType(db, type, idChild, parentFolder);
}

void CGisListDB::slotDelFolder()
{
    CGisListDBEditLock lock(false, this);
    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Delete database folder..."), tr("Are you sure you want to delete \"%1\" from the database?").arg(folder->text(1)), QMessageBox::Ok|QMessageBox::No);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    folder->remove();
    delete folder;

    folderLostFound->update();
}

void CGisListDB::slotDelLostFound()
{
    CGisListDBEditLock lock(false, this);
    CDBFolderLostFound * folder = dynamic_cast<CDBFolderLostFound*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Remove items..."), tr("Are you sure you want to delete all items from Lost&Found? This will remove them permanently."), QMessageBox::Ok|QMessageBox::No);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    folder->clear();
}

void CGisListDB::slotItemExpanded(QTreeWidgetItem * item)
{   
    CGisListDBEditLock lock(true, this);

    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(folder == 0)
    {
        return;
    }

    folder->expanding();
}

void CGisListDB::slotItemChanged(QTreeWidgetItem * item, int column)
{
    if(isInternalEdit)
    {
        return;
    }
    CGisListDBEditLock lock(true, this);

    if(column == IDBFolder::eColumnCheckbox)
    {
        IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
        if(folder != 0)
        {
            folder->toggle();
            return;
        }

        CDBItem * dbItem = dynamic_cast<CDBItem*>(item);
        if(dbItem != 0)
        {
            dbItem->toggle();
            return;
        }
    }
}
