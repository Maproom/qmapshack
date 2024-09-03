/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include <QtNetwork>
#include <QtSql>
#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "config.h"
#include "gis/CGisWorkspace.h"
#include "gis/db/CDBFolderLostFound.h"
#include "gis/db/CDBFolderMysql.h"
#include "gis/db/CDBFolderSqlite.h"
#include "gis/db/CDBItem.h"
#include "gis/db/CDBProject.h"
#include "gis/db/CSearchDatabase.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/CSetupDatabase.h"
#include "gis/db/CSetupFolder.h"
#include "helpers/CSettings.h"

class CGisListDBEditLock {
 public:
  CGisListDBEditLock(bool waitCursor, CGisListDB* widget, const QString& src)
      : widget(widget), waitCursor(waitCursor), src(src) {
    if (waitCursor) {
      CCanvas::setOverrideCursor(Qt::WaitCursor, "CGisListDBEditLock: " + src);
    }
    widget->isInternalEdit++;
  }
  ~CGisListDBEditLock() {
    if (waitCursor) {
      CCanvas::restoreOverrideCursor("~CGisListDBEditLock: " + src);
    }
    widget->isInternalEdit--;
  }

 private:
  CGisListDB* widget;
  bool waitCursor;
  QString src;
};

CGisListDB::CGisListDB(QWidget* parent) : QTreeWidget(parent), socket(nullptr) {
  setProperty("showItems", true);
  setProperty("showCheckBoxes", true);
  setProperty("showLostFound", true);

  SETTINGS;
  cfg.beginGroup("Database");
  const QString& path = cfg.value("lastDatabasePath", QDir::homePath()).toString();
  const QStringList& names = cfg.value("names").toStringList();
  cfg.beginGroup("Entries");
  for (const QString& name : names) {
    cfg.beginGroup(name);
    QString type = cfg.value("type", "SQLite").toString();
    if (type == "SQLite") {
      QString filename = cfg.value("filename", "").toString();
      if (filename.isEmpty()) {
        QMessageBox::information(this, name,
                                 tr("Due to changes in the database system QMapShack forgot about the filename of your "
                                    "database '%1'. You have to select it again in the next step.")
                                     .arg(name),
                                 QMessageBox::Ok);
        filename = QFileDialog::getOpenFileName(this, tr("Select database file."), path, "QMapShack Database (*.db)");
        if (filename.isEmpty()) {
          cfg.endGroup();  // name;
          continue;
        }
      }
      new CDBFolderSqlite(filename, name, this);
    }
    if (type == "MySQL") {
      QString server = cfg.value("server", "").toString();
      QString port = cfg.value("port", "").toString();
      QString user = cfg.value("user", "").toString();
      QString passwd = cfg.value("passwd", "").toString();
      bool noPasswd = cfg.value("noPasswd", false).toBool();

      if (server.isEmpty() || user.isEmpty()) {
        cfg.endGroup();  // name
        continue;
      }
      new CDBFolderMysql(server, port, user, passwd, noPasswd, name, this);
    }
    cfg.endGroup();  // name
  }
  cfg.endGroup();  // Entries
  cfg.endGroup();  // Database

  menuNone = new QMenu(this);
  actionAddDatabase =
      menuNone->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Database"), this, &CGisListDB::slotAddDatabase);

  menuFolder = new QMenu(this);
  actionAddFolder =
      menuFolder->addAction(QIcon("://icons/32x32/Add.png"), tr("Add Folder"), this, &CGisListDB::slotAddFolder);
  actionRenameFolder =
      menuFolder->addAction(QIcon("://icons/32x32/A.png"), tr("Rename Folder"), this, &CGisListDB::slotRenameFolder);
  actionCopyFolder =
      menuFolder->addAction(QIcon("://icons/32x32/Copy.png"), tr("Copy Folder"), this, &CGisListDB::slotCopyFolder);
  actionMoveFolder =
      menuFolder->addAction(QIcon("://icons/32x32/Move.png"), tr("Move Folder"), this, &CGisListDB::slotMoveFolder);
  actionDelFolder = menuFolder->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Folder"), this,
                                          &CGisListDB::slotDelFolder);
  menuFolder->addSeparator();
  actionImport = menuFolder->addAction(QIcon("://icons/32x32/LoadGIS.png"), tr("Import from Files..."), this,
                                       &CGisListDB::slotImport);
  actionExportToGpx = menuFolder->addAction(QIcon("://icons/32x32/SaveGIS.png"), tr("Export to GPX..."), this,
                                            &CGisListDB::slotExportToGpx);

  menuItem = new QMenu(this);
  actionDelItem =
      menuItem->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Item"), this, &CGisListDB::slotDelItem);

  menuDatabase = new QMenu(this);
  menuDatabase->addAction(actionAddFolder);
  actionSearch = menuDatabase->addAction(QIcon("://icons/32x32/Zoom.png"), tr("Search Database"), this,
                                         &CGisListDB::slotSearchDatabase);
  actionUpdate = menuDatabase->addAction(QIcon("://icons/32x32/DatabaseSync.png"), tr("Sync. with Database"), this,
                                         &CGisListDB::slotUpdateDatabase);
  actionDelDatabase = menuDatabase->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Remove Database"), this,
                                              &CGisListDB::slotDelDatabase);
  menuDatabase->addSeparator();
  menuDatabase->addAction(actionImport);
  menuDatabase->addAction(actionExportToGpx);

  menuLostFound = new QMenu(this);
  actionDelLostFound =
      menuLostFound->addAction(QIcon("://icons/32x32/Empty.png"), tr("Empty"), this, &CGisListDB::slotDelLostFound);

  menuLostFoundItem = new QMenu(this);
  actionDelLostFoundItem = menuLostFoundItem->addAction(QIcon("://icons/32x32/DeleteOne.png"), tr("Delete Item"), this,
                                                        &CGisListDB::slotDelLostFoundItem);

  connect(this, &CGisListDB::customContextMenuRequested, this, &CGisListDB::slotContextMenu);
  connect(this, &CGisListDB::itemExpanded, this, &CGisListDB::slotItemExpanded);
  connect(this, &CGisListDB::itemChanged, this, &CGisListDB::slotItemChanged);

  bool enabled = cfg.value("Database/listenUpdate", false).toBool();
  if (enabled) {
    quint16 port = cfg.value("Database/port", UDP_PORT).toUInt();

    socket = new QUdpSocket(this);
    if (!socket->bind(QHostAddress::Any, port, QUdpSocket::ShareAddress)) {
      qDebug() << socket->errorString();
    }
    connect(socket, &QUdpSocket::readyRead, this, &CGisListDB::slotReadyRead);
  }
}

CGisListDB::~CGisListDB() { saveDatabaseConfiguration(); }

void CGisListDB::addGlobalActions(const QList<QAction*> actions) {
  menuNone->addSeparator();
  menuNone->addActions(actions);
  menuFolder->addSeparator();
  menuFolder->addActions(actions);
  menuDatabase->addSeparator();
  menuDatabase->addActions(actions);
  menuItem->addSeparator();
  menuItem->addActions(actions);
  menuItem->addSeparator();
  menuItem->addActions(actions);
  menuItem->addSeparator();
  menuItem->addActions(actions);
}

void CGisListDB::saveDatabaseConfiguration() {
  QStringList names;

  SETTINGS;
  cfg.beginGroup("Database");

  cfg.beginGroup("Entries");
  cfg.remove("");

  const int N = topLevelItemCount();
  for (int n = 0; n < N; n++) {
    CDBFolderSqlite* sqlite = dynamic_cast<CDBFolderSqlite*>(topLevelItem(n));
    if (sqlite) {
      QString name = sqlite->text(CGisListDB::eColumnName);
      names << name;

      cfg.beginGroup(name);
      cfg.setValue("type", "SQLite");
      cfg.setValue("filename", sqlite->getFilename());
      cfg.endGroup();  // name
    }
    CDBFolderMysql* mysql = dynamic_cast<CDBFolderMysql*>(topLevelItem(n));
    if (mysql) {
      QString name = mysql->text(CGisListDB::eColumnName);
      names << name;

      cfg.beginGroup(name);
      cfg.setValue("type", "MySQL");
      cfg.setValue("server", mysql->getServer());
      cfg.setValue("port", mysql->getPort());
      cfg.setValue("user", mysql->getUser());
      cfg.setValue("passwd", mysql->getPasswd());
      cfg.setValue("noPasswd", mysql->hasNoPasswd());
      cfg.endGroup();  // name
    }
  }

  cfg.endGroup();  // Entries
  cfg.setValue("names", names);
  cfg.endGroup();  // Database
}

IDBFolderSql* CGisListDB::getDataBase(const QString& name, const QString& host) {
  CGisListDBEditLock lock(true, this, "getDataBase");
  const int N = topLevelItemCount();
  for (int n = 0; n < N; n++) {
    IDBFolderSql* database = dynamic_cast<IDBFolderSql*>(topLevelItem(n));
    if (database && (database->getDBName() == name)) {
      if (!host.isEmpty()) {
        if (database->getDBHost() != host) {
          continue;
        }
      }

      return database;
    }
  }
  return nullptr;
}

bool CGisListDB::hasDatabase(const QString& name) {
  CGisListDBEditLock lock(true, this, "hasDatabase");
  const int N = topLevelItemCount();
  for (int i = 0; i < N; i++) {
    IDBFolderSql* folder = dynamic_cast<IDBFolderSql*>(topLevelItem(i));
    if (folder && (folder->text(CGisListDB::eColumnName) == name)) {
      return true;
    }
  }
  return false;
}

bool CGisListDB::event(QEvent* e) {
  if (!dlgSearch.isNull()) {
    dlgSearch->event(e);
  }

  switch (e->type()) {
    case eEvtW2DAckInfo: {
      CGisListDBEditLock lock(true, this, "event");
      CEvtW2DAckInfo* evt = (CEvtW2DAckInfo*)e;
      IDBFolderSql* folder = getDataBase(evt->db, evt->host);
      if (folder) {
        folder->update(evt);
        if (evt->updateLostFound) {
          folder->updateLostFound();
          folder->announceChange();
        }
      }
      e->accept();
      return true;
    }

    case eEvtW2DCreate: {
      CGisListDBEditLock lock(true, this, "event");
      CEvtW2DCreate* evt = (CEvtW2DCreate*)e;
      IDBFolderSql* db = getDataBase(evt->db, evt->host);
      if (db) {
        quint64 idChild = 0;
        IDBFolder* folder = db->getFolder(evt->idParent);
        if (folder) {
          idChild = folder->addFolder(evt->type, evt->name);
        } else {
          idChild = IDBFolder::addFolderToDb(evt->type, evt->name, evt->idParent, db->getDb());
        }

        if (idChild) {
          if (folder) {
            folder->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
          }

          evt->idChild = idChild;
          CEvtD2WShowFolder* evt1 = new CEvtD2WShowFolder(idChild, evt->db);
          CGisWorkspace::self().postEventForWks(evt1);
        }

        db->announceChange();
      }
      e->accept();
      return true;
    }
  }

  return QTreeWidget::event(e);
}

void CGisListDB::slotContextMenu(const QPoint& point) {
  QPoint p = mapToGlobal(point);

  if (selectedItems().isEmpty()) {
    menuNone->exec(p);
    return;
  }

  bool isSingleSelection = selectedItems().count() == 1;

  actionUpdate->setEnabled(true);
  actionAddFolder->setEnabled(isSingleSelection);
  actionExportToGpx->setEnabled(isSingleSelection);
  actionImport->setEnabled(isSingleSelection);

  IDBFolderSql* database = dynamic_cast<IDBFolderSql*>(currentItem());
  if (database) {
    bool enabled = database->getDb().isOpen();
    actionUpdate->setEnabled(enabled);
    actionAddFolder->setEnabled(enabled);
    actionSearch->setEnabled(enabled);

    menuDatabase->exec(p);

    return;
  }

  CDBFolderLostFound* lostFound = dynamic_cast<CDBFolderLostFound*>(currentItem());
  if (lostFound) {
    menuLostFound->exec(p);
    return;
  }

  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (folder) {
    bool isGroupFolder = folder->type() == IDBFolder::eTypeGroup;
    actionRenameFolder->setVisible(isGroupFolder);
    menuFolder->exec(p);
    return;
  }

  CDBItem* item = dynamic_cast<CDBItem*>(currentItem());
  if (item) {
    CDBFolderLostFound* lostFound = dynamic_cast<CDBFolderLostFound*>(item->parent());
    if (lostFound) {
      menuLostFoundItem->exec(p);
    } else {
      menuItem->exec(p);
    }

    return;
  }
}

void CGisListDB::slotAddDatabase() {
  CSetupDatabase dlg(*this);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  QString name = dlg.getName();

  IDBFolder* folder = nullptr;
  bool isUsable = true;

  if (dlg.isSqlite()) {
    QString filename = dlg.getFilename();
    CDBFolderSqlite* sfolder = new CDBFolderSqlite(filename, name, this);
    sfolder->setToolTip(eColumnName, sfolder->getDBInfo());
    isUsable = sfolder->isUsable();
    folder = sfolder;
  } else if (dlg.isMysql()) {
    QString server = dlg.getServer();
    QString port = dlg.getPort();
    QString user = dlg.getUser();
    QString passwd = dlg.getPasswd();
    bool noPasswd = dlg.noPasswd();

    CDBFolderMysql* mfolder = new CDBFolderMysql(server, port, user, passwd, noPasswd, name, this);
    mfolder->setToolTip(eColumnName, mfolder->getDBInfo());
    isUsable = mfolder->isUsable();
    folder = mfolder;
  } else {
    return;
  }

  if (isUsable) {
    folder->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  }

  emit sigChanged();

  saveDatabaseConfiguration();
}

void CGisListDB::slotDelDatabase() {
  IDBFolderSql* folder = dynamic_cast<IDBFolderSql*>(currentItem());
  if (nullptr == folder) {
    return;
  }

  int res = QMessageBox::question(
      this, tr("Remove database..."),
      tr("Do you really want to remove '%1' from the list?").arg(folder->text(CGisListDB::eColumnName)),
      QMessageBox::Ok | QMessageBox::Abort, QMessageBox::Ok);
  if (res != QMessageBox::Ok) {
    return;
  }

  delete folder;

  emit sigChanged();

  saveDatabaseConfiguration();
}

void CGisListDB::slotAddFolder() {
  CGisListDBEditLock lock(false, this, "slotAddFolder");

  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (nullptr == folder) {
    return;
  }

  IDBFolder::type_e type = IDBFolder::eTypeProject;
  QString name;
  CSetupFolder dlg(type, name, true, this);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  if (folder->addFolder(type, name) == 0) {
    return;
  }

  if (!folder->isExpanded()) {
    folder->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    folder->setExpanded(true);
  }

  IDBFolderSql* dbfolder = folder->getDBFolder();
  if (dbfolder) {
    dbfolder->announceChange();
  }
}

void CGisListDB::slotDelFolder() {
  CGisListDBEditLock lock(false, this, "slotDelFolder");
  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (folder == nullptr) {
    return;
  }

  IDBFolderSql* dbfolder = folder->getDBFolder();
  if (dbfolder == nullptr) {
    return;
  }

  int res = QMessageBox::question(
      this, tr("Delete database folder..."),
      tr("Are you sure you want to delete selected folders and all subfolders from the database?"),
      QMessageBox::Ok | QMessageBox::No);
  if (res != QMessageBox::Ok) {
    return;
  }

  QList<QTreeWidgetItem*> itemsToDelete;
  const QList<QTreeWidgetItem*>& items = selectedItems();
  for (QTreeWidgetItem* item : items) {
    // only pick the project/other folders to copy
    folder = dynamic_cast<IDBFolder*>(item);
    if ((folder == nullptr) || (folder->type() < IDBFolder::eTypeGroup)) {
      continue;
    }

    folder->remove();
    // Because some items can be parent of other selected items
    // it's a bad idea to delete them asap. Better collect them first.
    itemsToDelete << folder;
  }

  // iterate over all items to be deleted.
  for (QTreeWidgetItem* item : std::as_const(itemsToDelete)) {
    // Test if the item's parent is also in the list.
    // If it is skip it because it will be deleted together with its parent.
    if (itemsToDelete.contains(item->parent())) {
      continue;
    }

    delete item;
  }

  dbfolder->updateLostFound();
  dbfolder->announceChange();
}

void CGisListDB::slotCopyFolder() {
  CGisListDBEditLock lock(false, this, "slotCopyFolder");

  // no way to continue if the current item is not a folder (we need the database it is attached to)
  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (folder == nullptr) {
    return;
  }

  // get the database the folder is attached to
  IDBFolderSql* dbfolder = folder->getDBFolder();
  if (dbfolder == nullptr) {
    return;
  }

  // next we need to get the target folder
  // NOTE: By pre-setting db and host, we limit the selection to the current database
  QList<quint64> ids;
  QString db = folder->getDBName();
  QString host = folder->getDBHost();

  CSelectDBFolder dlg(ids, db, host, this);
  if ((dlg.exec() == QDialog::Rejected) || ids.isEmpty()) {
    return;
  }

  quint64 idTarget = ids[0];

  // --- at this point we should have all data to perform the copy without interruption ---

  // now iterate over all selected items
  const QList<QTreeWidgetItem*>& items = selectedItems();
  for (QTreeWidgetItem* item : items) {
    // only pick the project/other folders to copy
    folder = dynamic_cast<IDBFolder*>(item);
    if ((folder == nullptr) || (folder->type() < IDBFolder::eTypeGroup)) {
      continue;
    }

    IDBFolder* parent = dynamic_cast<IDBFolder*>(folder->parent());
    if ((parent == nullptr) || (parent->getId() == idTarget) || (folder->getId() == idTarget)) {
      // skip operation if the current parent is the same as the traget parent
      continue;
    }

    dbfolder->copyFolder(folder->getId(), idTarget);
  }

  // tell the parent folder to show all changes
  IDBFolder* target = dbfolder->getFolder(idTarget);
  if (target != nullptr) {
    target->update();
  }

  // tell other clients to show changes
  dbfolder->announceChange();
}

void CGisListDB::slotMoveFolder() {
  CGisListDBEditLock lock(false, this, "slotMoveFolder");

  // no way to continue if the current item is not a folder (we need the database it is attached to)
  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (folder == nullptr) {
    return;
  }

  // get the database the folder is attached to
  IDBFolderSql* dbfolder = folder->getDBFolder();
  if (dbfolder == nullptr) {
    return;
  }

  // next we need to get the target folder
  // NOTE: By pre-setting db and host, we limit the selection to the current database
  QList<quint64> ids;
  QString db = folder->getDBName();
  QString host = folder->getDBHost();

  CSelectDBFolder dlg(ids, db, host, this);
  if ((dlg.exec() == QDialog::Rejected) || ids.isEmpty()) {
    return;
  }

  quint64 idTarget = ids[0];

  // --- at this point we should have all data to perform the copy without interruption ---

  // now iterate over all selected items
  QList<IDBFolder*> foldersToDelete;
  const QList<QTreeWidgetItem*>& items = selectedItems();
  for (QTreeWidgetItem* item : items) {
    // only pick the project/other folders to copy
    folder = dynamic_cast<IDBFolder*>(item);
    if ((folder == nullptr) || (folder->type() < IDBFolder::eTypeGroup)) {
      continue;
    }

    IDBFolder* parent = dynamic_cast<IDBFolder*>(folder->parent());
    if ((parent == nullptr) || (parent->getId() == idTarget)) {
      // skip operation if the current parent is the same as the target parent
      continue;
    }

    // if(target->isSiblingFrom(folder))
    if (ids.contains(folder->getId())) {
      QMessageBox::warning(this, tr("Bad operation...."),
                           tr("The target folder is a subfolder of the one to move. This will not work."),
                           QMessageBox::Abort);
      continue;
    }

    // copy to new location
    dbfolder->copyFolder(folder->getId(), idTarget);
    // Because some items can be parent of other selected items
    // it's a bad idea to delete them asap. Better collect them first.
    foldersToDelete << folder;
  }

  // iterate over all items to be deleted.
  for (IDBFolder* folder : std::as_const(foldersToDelete)) {
    // Test if the item's parent is also in the list.
    // If it is skip it because it will be deleted together with it's parent.
    if (foldersToDelete.contains(dynamic_cast<IDBFolder*>(folder->parent()))) {
      continue;
    }

    folder->remove();
    delete folder;
  }

  // tell the parent folder to show all changes
  IDBFolder* target = dbfolder->getFolder(idTarget);
  if (target != nullptr) {
    target->update();
  }
  // tell other clients to show changes
  dbfolder->announceChange();
}

void CGisListDB::slotRenameFolder() {
  CGisListDBEditLock lock(false, this, "slotRenameFolder");

  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (folder == nullptr) {
    return;
  }

  // get the database the folder is attached to
  IDBFolderSql* dbfolder = folder->getDBFolder();
  if (dbfolder == nullptr) {
    return;
  }

  const QList<QTreeWidgetItem*>& items = selectedItems();
  for (QTreeWidgetItem* item : items) {
    folder = dynamic_cast<IDBFolder*>(item);
    if ((folder == nullptr) || (folder->type() != IDBFolder::eTypeGroup)) {
      continue;
    }

    QString name1 = folder->getName();
    QString name2 = QInputDialog::getText(this, tr("Folder name..."), tr("Rename folder:"), QLineEdit::Normal, name1);

    if (!name2.isEmpty() && (name1 != name2)) {
      folder->setName(name2);
    }
  }

  // tell other clients to show changes
  dbfolder->announceChange();
}

void CGisListDB::slotDelLostFound() {
  CGisListDBEditLock lock(false, this, "slotDelLostFound");
  CDBFolderLostFound* folder = dynamic_cast<CDBFolderLostFound*>(currentItem());
  if (folder == nullptr) {
    return;
  }

  int res = QMessageBox::question(
      this, tr("Remove items..."),
      tr("Are you sure you want to delete all items from Lost&Found? This will remove them permanently."),
      QMessageBox::Ok | QMessageBox::No);
  if (res != QMessageBox::Ok) {
    return;
  }

  CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
  folder->clear();

  IDBFolderSql* dbfolder = folder->getDBFolder();
  if (dbfolder) {
    dbfolder->announceChange();
  }
}

void CGisListDB::slotDelLostFoundItem() {
  CGisListDBEditLock lock(false, this, "slotDelLostFoundItem");

  int res = QMessageBox::question(
      this, tr("Remove items..."),
      tr("Are you sure you want to delete all selected items from Lost&Found? This will remove them permanently."),
      QMessageBox::Ok | QMessageBox::No);
  if (res != QMessageBox::Ok) {
    return;
  }

  CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
  QSet<CDBFolderLostFound*> folders;
  QList<QTreeWidgetItem*> delItems;
  const QList<QTreeWidgetItem*>& items = selectedItems();
  for (QTreeWidgetItem* item : items) {
    CDBItem* dbItem = dynamic_cast<CDBItem*>(item);
    if (dbItem == nullptr) {
      continue;
    }

    CDBFolderLostFound* folder = dynamic_cast<CDBFolderLostFound*>(dbItem->parent());
    if (folder == nullptr) {
      continue;
    }

    if (folder->delItem(dbItem)) {
      delItems << dbItem;
      folders << folder;
    }
  }

  qDeleteAll(delItems);
  for (CDBFolderLostFound* folder : std::as_const(folders)) {
    folder->update();

    IDBFolderSql* dbfolder = folder->getDBFolder();
    if (dbfolder) {
      dbfolder->announceChange();
    }
  }
}

void CGisListDB::slotItemExpanded(QTreeWidgetItem* item) {
  CGisListDBEditLock lock(true, this, "slotItemExpanded");

  IDBFolder* folder = dynamic_cast<IDBFolder*>(item);
  if (folder == nullptr) {
    return;
  }

  folder->expanding();
}

void CGisListDB::slotDelItem() {
  CGisListDBEditLock lock(false, this, "slotDelItem");

  int last = QMessageBox::NoButton;

  QSet<IDBFolder*> folders;
  QList<QTreeWidgetItem*> dbItems;
  QSet<IDBFolderSql*> dbFolders;

  const QList<QTreeWidgetItem*>& items = selectedItems();
  for (QTreeWidgetItem* item : items) {
    CDBItem* dbItem = dynamic_cast<CDBItem*>(item);
    if (dbItem == nullptr) {
      continue;
    }

    IDBFolder* folder = dynamic_cast<IDBFolder*>(dbItem->parent());
    if (folder == nullptr) {
      continue;
    }

    if (last != QMessageBox::YesToAll) {
      QString msg = tr("Are you sure you want to delete '%1' from folder '%2'?")
                        .arg(dbItem->text(CGisListDB::eColumnName), folder->text(CGisListDB::eColumnName));
      last = QMessageBox::question(CMainWindow::getBestWidgetForParent(), tr("Delete..."), msg,
                                   QMessageBox::YesToAll | QMessageBox::Cancel | QMessageBox::Ok | QMessageBox::No,
                                   QMessageBox::Ok);
    }
    if (last == QMessageBox::No) {
      continue;
    }
    if (last == QMessageBox::Cancel) {
      return;
    }

    dbItem->remove();

    folders << folder;
    dbItems << dbItem;
    dbFolders << folder->getDBFolder();
  }

  qDeleteAll(dbItems);
  for (IDBFolderSql* dbFolder : std::as_const(dbFolders)) {
    dbFolder->updateLostFound();
    dbFolder->announceChange();
  }

  // tell all folders to update their statistics and waypoint/track correlations
  for (IDBFolder* folder : std::as_const(folders)) {
    folder->updateItemsOnWks();
  }
}

void CGisListDB::slotItemChanged(QTreeWidgetItem* item, int column) {
  if (isInternalEdit) {
    return;
  }
  CGisListDBEditLock lock(true, this, "slotItemChanged");

  if (column == CGisListDB::eColumnCheckbox) {
    IDBFolder* folder = dynamic_cast<IDBFolder*>(item);
    if (folder != nullptr) {
      folder->toggle();

      // tell folder to update its statistics and waypoint/track correlations
      folder->updateItemsOnWks();
      return;
    }

    CDBItem* dbItem = dynamic_cast<CDBItem*>(item);
    if (dbItem != nullptr) {
      dbItem->toggle();

      // tell folder to update its statistics and waypoint/track correlations
      folder = dynamic_cast<IDBFolder*>(dbItem->parent());
      if (folder) {
        folder->updateItemsOnWks();
      }
      return;
    }
  }
}

void CGisListDB::slotUpdateDatabase() {
  CGisListDBEditLock lock(true, this, "slotUpdateDatabase");

  const QList<QTreeWidgetItem*>& items = selectedItems();
  for (QTreeWidgetItem* item : items) {
    IDBFolder* folder = dynamic_cast<IDBFolder*>(item);
    if (folder == nullptr) {
      continue;
    }

    if (folder->type() == IDBFolder::eTypeDatabase) {
      folder->update();

      CEvtD2WReload* evt = new CEvtD2WReload(folder->getDBName());
      CGisWorkspace::self().postEventForWks(evt);
    }
  }
}

void CGisListDB::slotSearchDatabase() {
  CGisListDBEditLock lock(false, this, "slotSearchDatabase");

  IDBFolder* db = dynamic_cast<IDBFolder*>(currentItem());
  if (db == nullptr) {
    return;
  }

  isInternalEdit--;
  dlgSearch = new CSearchDatabase(*db, this);
  connect(dlgSearch.data(), &CSearchDatabase::sigItemChanged, this, &CGisListDB::slotItemChanged);
  dlgSearch->exec();
  delete dlgSearch;
  isInternalEdit++;
}

void CGisListDB::slotReadyRead() {
  CGisListDBEditLock lock(true, this, "slotReadyRead");

  while (socket->hasPendingDatagrams()) {
    QByteArray datagram;
    datagram.resize(socket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    QDataStream stream(&datagram, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_5_2);

    quint32 msgId;
    quint32 tan;
    qint32 id;
    QString driver;
    QString dbName;
    QString dbHost;

    stream >> msgId >> tan >> id >> driver >> dbName >> dbHost;

    if ((lastTan == tan) || (msgId != 0)) {
      continue;
    }

    lastTan = tan;

    // check for our own message
    if (id == CMainWindow::self().id) {
      continue;
    }

    qDebug() << "Receive database update from:" << sender << senderPort;
    qDebug() << "with"
             << "tan:" << lastTan << "app ID:" << id << "driver:" << driver << "DB name:" << dbName
             << "DB host:" << dbHost;

    IDBFolderSql* folder = getDataBase(dbName, dbHost);
    if (folder) {
      folder->update();

      CEvtD2WReload* evt = new CEvtD2WReload(folder->getDBName());
      CGisWorkspace::self().postEventForWks(evt);
    }
  }
}

void CGisListDB::slotExportToGpx() {
  CGisListDBEditLock lock(false, this, "slotExportToGpx");

  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (folder == nullptr) {
    return;
  }

  folder->exportToGpx();
}

void CGisListDB::slotImport() {
  CGisListDBEditLock lock(false, this, "slotImport");

  IDBFolder* folder = dynamic_cast<IDBFolder*>(currentItem());
  if (folder == nullptr) {
    return;
  }

  SETTINGS;
  QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();
  QString filter = cfg.value("Paths/lastGisFilter", IGisProject::filedialogAllSupported).toString();

  const QStringList& filenames =
      QFileDialog::getOpenFileNames(this, tr("Import GIS Data..."), path, IGisProject::filedialogLoadFilters, &filter);

  if (filenames.isEmpty()) {
    return;
  }

  for (const QString& filename : filenames) {
    CDBProject* prjDb = new CDBProject(filename, folder, nullptr);
    if (prjDb->isValid()) {
      prjDb->save();
    }

    delete prjDb;
  }

  IDBFolderSql* dbfolder = folder->getDBFolder();
  if (dbfolder) {
    dbfolder->update();
    dbfolder->announceChange();
  }

  path = QFileInfo(filenames.first()).absolutePath();
  cfg.setValue("Paths/lastGisPath", path);
  cfg.setValue("Paths/lastGisFilter", filter);
}
