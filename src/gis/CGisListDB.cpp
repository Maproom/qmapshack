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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "config.h"
#include "gis/CGisListDB.h"
#include "gis/CGisWidget.h"
#include "gis/db/CDBFolderDatabase.h"
#include "gis/db/CDBFolderLostFound.h"
#include "gis/db/CDBItem.h"
#include "gis/db/CSetupDatabase.h"
#include "gis/db/CSetupFolder.h"
#include "gis/db/macros.h"
#include "helpers/CSettings.h"

#include <QtSql>
#include <QtWidgets>

class CGisListDBEditLock
{
public:
    CGisListDBEditLock(bool waitCursor, CGisListDB * widget, const QString& src) : widget(widget), waitCursor(waitCursor), src(src)
    {
        if(waitCursor)
        {
            CCanvas::setOverrideCursor(Qt::WaitCursor, "CGisListDBEditLock: " + src);
        }
        widget->isInternalEdit += 1;
    }
    ~CGisListDBEditLock()
    {
        if(waitCursor)
        {
            CCanvas::restoreOverrideCursor("~CGisListDBEditLock: " + src);
        }
        widget->isInternalEdit -= 1;
    }
private:
    CGisListDB * widget;
    bool waitCursor;
    QString src;
};


CGisListDB::CGisListDB(QWidget *parent)
    : QTreeWidget(parent)
{
    SETTINGS;
    QStringList names = cfg.value("Database/names").toStringList();
    QStringList files = cfg.value("Database/files").toStringList();

    const int N = names.count();
    for(int i = 0; i < N; i++)
    {
        addDatabase(names[i], files[i]);
    }

    menuNone            = new QMenu(this);
    actionAddDatabase   = menuNone->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Database"), this, SLOT(slotAddDatabase()));

    menuFolder          = new QMenu(this);
    actionAddFolder     = menuFolder->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Folder"), this, SLOT(slotAddFolder()));
    actionDelFolder     = menuFolder->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Folder"), this, SLOT(slotDelFolder()));

    menuItem            = new QMenu(this);
    actionDelItem       = menuItem->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Item"), this, SLOT(slotDelItem()));

    menuDatabase        = new QMenu(this);
    menuDatabase->addAction(actionAddFolder);
    actionDelDatabase   = menuDatabase->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Remove Database"), this, SLOT(slotDelDatabase()));

    menuLostFound       = new QMenu(this);
    actionDelLostFound  = menuLostFound->addAction(QIcon("://icons/32x32/Empty.png"), tr("Empty"), this, SLOT(slotDelLostFound()));

    menuLostFoundItem       = new QMenu(this);
    actionDelLostFoundItem  = menuLostFoundItem->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Item"), this, SLOT(slotDelLostFoundItem()));


    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
}

CGisListDB::~CGisListDB()
{
    SETTINGS;
    QStringList names;
    QStringList files;

    const int N = topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        CDBFolderDatabase * database = dynamic_cast<CDBFolderDatabase*>(topLevelItem(n));
        if(database)
        {
            names << database->text(CGisListDB::eColumnName);
            files << database->getFilename();
        }
    }

    cfg.setValue("Database/names", names);
    cfg.setValue("Database/files", files);
}


CDBFolderDatabase * CGisListDB::getDataBase(const QString& name)
{
    CGisListDBEditLock lock(true, this, "getDataBase");
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

bool CGisListDB::hasDatabase(const QString& name)
{
    CGisListDBEditLock lock(true, this, "hasDatabase");
    const int N = topLevelItemCount();
    for(int i = 0; i < N; i++)
    {
        CDBFolderDatabase * folder = dynamic_cast<CDBFolderDatabase*>(topLevelItem(i));
        if(folder && (folder->text(CGisListDB::eColumnName) == name))
        {
            return true;
        }
    }
    return false;
}


bool CGisListDB::event(QEvent * e)
{
    switch(e->type())
    {
    case eEvtW2DAckInfo:
    {
        CGisListDBEditLock lock(true, this, "event");
        CEvtW2DAckInfo * evt        = (CEvtW2DAckInfo*)e;
        CDBFolderDatabase * folder  = getDataBase(evt->db);
        if(folder)
        {
            folder->update(evt);
            if(evt->updateLostFound)
            {
                folder->updateLostFound();
            }
        }
        e->accept();
        return true;
    }

    case eEvtW2DCreate:
    {
        CGisListDBEditLock lock(true, this, "event");
        CEvtW2DCreate * evt         = (CEvtW2DCreate*)e;
        CDBFolderDatabase * db  = getDataBase(evt->db);
        if(db)
        {
            quint64 idChild = 0;
            IDBFolder * folder = db->getFolder(evt->idParent);
            if(folder)
            {
                idChild = folder->addFolder(evt->type, evt->name);
            }
            else
            {
                idChild = IDBFolder::addFolderToDb(evt->type, evt->name, evt->idParent, db->getDb());
            }

            if(idChild)
            {
                evt->idChild = idChild;
                CEvtD2WShowFolder * evt1 = new CEvtD2WShowFolder(idChild, evt->db);
                CGisWidget::self().postEventForWks(evt1);
            }
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

    if(selectedItems().isEmpty())
    {
        menuNone->exec(p);
        return;
    }

    CDBFolderDatabase * database = dynamic_cast<CDBFolderDatabase*>(currentItem());
    if(database)
    {
        menuDatabase->exec(p);
        return;
    }

    CDBFolderLostFound * lostFound = dynamic_cast<CDBFolderLostFound*>(currentItem());
    if(lostFound)
    {
        menuLostFound->exec(p);
        return;
    }

    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if(folder)
    {
        menuFolder->exec(p);
        return;
    }

    CDBItem * item = dynamic_cast<CDBItem*>(currentItem());
    if(item)
    {
        CDBFolderLostFound * lostFound = dynamic_cast<CDBFolderLostFound*>(item->parent());
        if(lostFound)
        {
            menuLostFoundItem->exec(p);
        }
        else
        {
            menuItem->exec(p);
        }

        return;
    }
}

void CGisListDB::slotAddDatabase()
{
    QString name, filename("-");
    CSetupDatabase dlg(name, filename, *this);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    addDatabase(name, filename);
    emit sigChanged();
}

void CGisListDB::addDatabase(const QString& name, const QString& filename)
{
    new CDBFolderDatabase(filename, name, this);
}

void CGisListDB::slotDelDatabase()
{
    CDBFolderDatabase * folder = dynamic_cast<CDBFolderDatabase*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    int res = QMessageBox::question(this, tr("Remove database..."), tr("Do you really want to remove '%1' from the list?").arg(folder->text(CGisListDB::eColumnName)), QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    delete folder;

    emit sigChanged();
}

void CGisListDB::slotAddFolder()
{
    CGisListDBEditLock lock(false, this, "slotAddFolder");

    IDBFolder * folder = dynamic_cast<IDBFolder*>(currentItem());
    if(folder == 0)
    {
        return;
    }

    IDBFolder::type_e type = IDBFolder::eTypeProject;
    QString name;
    CSetupFolder dlg(type, name, true, this);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    folder->addFolder(type, name);
}

void CGisListDB::slotDelFolder()
{
    CGisListDBEditLock lock(false, this, "slotDelFolder");
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

    CDBFolderDatabase * dbfolder = folder->getDBFolder();

    folder->remove();
    delete folder;

    if(dbfolder)
    {
        dbfolder->updateLostFound();
    }
}

void CGisListDB::slotDelLostFound()
{
    CGisListDBEditLock lock(false, this, "slotDelLostFound");
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

    CCanvas::setOverrideCursor(Qt::WaitCursor, "slotDelLostFound");
    folder->clear();
    CCanvas::restoreOverrideCursor("slotDelLostFound");
}

void CGisListDB::slotDelLostFoundItem()
{
    CGisListDBEditLock lock(false, this, "slotDelLostFoundItem");

    int res = QMessageBox::question(this, tr("Remove items..."), tr("Are you sure you want to delete all selected items from Lost&Found? This will remove them permanently."), QMessageBox::Ok|QMessageBox::No);
    if(res != QMessageBox::Ok)
    {
        return;
    }

    CCanvas::setOverrideCursor(Qt::WaitCursor, "slotDelLostFoundItem");
    QSet<CDBFolderLostFound*> folders;
    QList<QTreeWidgetItem*> delItems;
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CDBItem * dbItem            = dynamic_cast<CDBItem*>(item);
        CDBFolderLostFound * folder = dynamic_cast<CDBFolderLostFound*>(dbItem->parent());

        if(folder && dbItem)
        {
            if(folder->delItem(dbItem))
            {
                delItems << dbItem;
                folders << folder;
            }
        }
    }

    qDeleteAll(delItems);
    foreach(CDBFolderLostFound* folder, folders)
    {
        folder->update();
    }
    CCanvas::restoreOverrideCursor("slotDelLostFoundItem");
}


void CGisListDB::slotItemExpanded(QTreeWidgetItem * item)
{
    CGisListDBEditLock lock(true, this, "slotItemExpanded");

    IDBFolder * folder = dynamic_cast<IDBFolder*>(item);
    if(folder == 0)
    {
        return;
    }

    folder->expanding();
}

void CGisListDB::slotDelItem()
{
    CGisListDBEditLock lock(false, this, "slotDelItem");

    int last = QMessageBox::NoButton;

    QList<QTreeWidgetItem*> dbItems;
    QSet<CDBFolderDatabase*> dbFolders;

    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CDBItem * dbItem = dynamic_cast<CDBItem*>(item);
        if(dbItem == 0)
        {
            continue;
        }

        IDBFolder * folder = dynamic_cast<IDBFolder*>(dbItem->parent());
        if(folder == 0)
        {
            continue;
        }

        if(last != QMessageBox::YesToAll)
        {
            QString msg = QObject::tr("Are you sure you want to delete '%1' from folder '%2'?").arg(dbItem->text(CGisListDB::eColumnName)).arg(folder->text(CGisListDB::eColumnName));
            last = QMessageBox::question(CMainWindow::getBestWidgetForParent(), QObject::tr("Delete..."), msg, QMessageBox::YesToAll|QMessageBox::Cancel|QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
        }
        if(last == QMessageBox::No)
        {
            continue;
        }
        if(last == QMessageBox::Cancel)
        {
            return;
        }

        dbItem->remove();
        dbItems << dbItem;
        dbFolders << folder->getDBFolder();
    }

    qDeleteAll(dbItems);
    foreach(CDBFolderDatabase * dbFolder, dbFolders)
    {
        dbFolder->updateLostFound();
    }
}

void CGisListDB::slotItemChanged(QTreeWidgetItem * item, int column)
{
    if(isInternalEdit)
    {
        return;
    }
    CGisListDBEditLock lock(true, this, "slotItemChanged");

    if(column == CGisListDB::eColumnCheckbox)
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

