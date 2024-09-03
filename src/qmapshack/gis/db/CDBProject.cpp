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

#include "gis/db/CDBProject.h"

#include <QtSql>
#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisDatabase.h"
#include "gis/CGisWorkspace.h"
#include "gis/db/CResolveDatabaseConflict.h"
#include "gis/db/CSelectSaveAction.h"
#include "gis/db/IDB.h"
#include "gis/db/macros.h"
#include "gis/prj/CDetailsPrj.h"
#include "helpers/CProgressDialog.h"
CDBProject::CDBProject(CGisListWks* parent) : IGisProject(eTypeDb, "", parent), id(0) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/DBProject.png"));
}

CDBProject::CDBProject(const QString& dbName, quint64 id, CGisListWks* parent)
    : IGisProject(eTypeDb, dbName, parent), id(id) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/DBProject.png"));
  db = QSqlDatabase::database(dbName);

  QSqlQuery query(db);
  query.prepare("SELECT date, name, data FROM folders WHERE id=:id");
  query.bindValue(":id", id);
  QUERY_EXEC(return );
  query.next();

  QString date = query.value(0).toString();
  QString name = query.value(1).toString();
  QByteArray data = query.value(2).toByteArray();

  if (data.isEmpty()) {
    // Make sure the key is reset
    key.clear();
    metadata.name = name;

    // The time format can differ by database type
    if (date.contains('T')) {
      metadata.time = QDateTime::fromString(date, "yyyy-MM-ddThh:mm:ss");
    } else {
      metadata.time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
    }

    // Still no valid date? Bad as we need it to produce an unique key.
    if (!metadata.time.isValid()) {
      metadata.time = QDateTime::currentDateTimeUtc();
    }

    query.prepare("UPDATE folders SET keyqms=:keyqms WHERE id=:id");
    query.bindValue(":keyqms", getKey());
    query.bindValue(":id", id);
    QUERY_EXEC(return );
  } else {
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);
    *this << in;
    filename = dbName;
  }

  CDBProject::setupName(name);
  setToolTip(CGisListWks::eColumnName, getInfo());
  updateItems();

  valid = true;
}

CDBProject::CDBProject(const QString& filename, IDBFolder* parentFolder, CGisListWks* parent)
    : IGisProject(eTypeDb, parentFolder->getDBName(), parent), db(parentFolder->getDb()) {
  IGisProject* prjIn = IGisProject::create(filename, nullptr);
  if (prjIn == nullptr) {
    QMessageBox::information(
        CMainWindow::self().getBestWidgetForParent(), tr("Failed to load..."),
        tr("Can't load file \"%1\" . It will be skipped.").arg(QFileInfo(filename).completeBaseName()),
        QMessageBox::Ok);
    return;
  }

  // test if the project has been imported already
  QSqlQuery query(db);
  query.prepare("SELECT id FROM folders WHERE keyqms=:keyqms");
  query.bindValue(":keyqms", prjIn->getKey());
  QUERY_EXEC(return );
  if (query.next()) {
    QMessageBox::information(
        CMainWindow::self().getBestWidgetForParent(), tr("Project already in database..."),
        tr("The project \"%1\" has already been imported into the database. It will be skipped.").arg(prjIn->getName()),
        QMessageBox::Ok);
    return;
  }

  // create a new folder in the database
  id = parentFolder->addFolder(IDBFolder::eTypeProject, prjIn->getName());
  query.prepare("UPDATE folders SET keyqms=:keyqms WHERE id=:id");
  query.bindValue(":keyqms", prjIn->getKey());
  query.bindValue(":id", id);
  QUERY_EXEC(return );

  // copy data
  key = prjIn->getKey();
  metadata = prjIn->getMetadata();

  QList<QTreeWidgetItem*> items = prjIn->takeChildren();
  addChildren(items);

  // set change indication else the item will not be saved
  for (QTreeWidgetItem* item : std::as_const(items)) {
    IGisItem* gisItem = dynamic_cast<IGisItem*>(item);
    if (gisItem) {
      gisItem->updateDecoration(IGisItem::eMarkChanged, IGisItem::eMarkNone);
    }
  }

  valid = true;
}

CDBProject::~CDBProject() {
  CEvtW2DAckInfo* evt = new CEvtW2DAckInfo(Qt::Unchecked, getId(), getDBName(), getDBHost());
  CGisDatabase::self().postEventForDb(evt);

  /*
      The project is not connected to the workspace tree widget. This is because it's a
      temporary project to manipulate the database outside the workspace. In this case
      we have to tell the workspace tree to update the databse tree with it's status.

      Thus the first event will remove all check marks. And this event will post a status
      to set the check marks according to what ever is loaded in the workspace.
   */
  if (treeWidget() == nullptr) {
    CEvtD2WReqInfo* evt = new CEvtD2WReqInfo(getId(), getDBName());
    CGisWorkspace::self().postEventForWks(evt);
  }
}

void CDBProject::restoreDBLink() {
  db = QSqlDatabase::database(filename);

  QSqlQuery query(db);
  query.prepare("SELECT id FROM folders WHERE keyqms=:keyqms");
  query.bindValue(":keyqms", getKey());
  QUERY_EXEC(return );
  if (query.next()) {
    id = query.value(0).toULongLong();
    setupName("----");
    valid = true;
  }
}

void CDBProject::setupName(const QString& defaultName) {
  IGisProject::setupName(defaultName);

  // look for a parent folder's name to be used as suffix
  QSqlQuery query(db);
  query.prepare(
      "SELECT t1.name FROM folders AS t1 WHERE id=(SELECT parent FROM folder2folder WHERE child=:id) AND "
      "(t1.type=:type1 OR t1.type=:type2)");
  query.bindValue(":id", id);
  query.bindValue(":type1", IDBFolder::eTypeGroup);
  query.bindValue(":type2", IDBFolder::eTypeProject);
  QUERY_EXEC();
  if (query.next()) {
    nameSuffix = query.value(0).toString();
  }
  setText(CGisListWks::eColumnName, getNameEx());
}

void CDBProject::postStatus(bool updateLostFound) {
  // collect the keys of all child items and post them to the database view
  CEvtW2DAckInfo* info = new CEvtW2DAckInfo(getId(), getDBName(), getDBHost());

  bool changedItems = false;
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(n));
    if (item) {
      info->keysChildren << item->getKey().item;
      changedItems |= item->isChanged();
    }
  }

  // check if all items are loaded
  if (type != eTypeLostFound) {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM folder2item WHERE parent=:parent");
    query.bindValue(":parent", getId());
    QUERY_EXEC();
    query.next();

    const int nChildrenAttached = query.value(0).toInt();

    if ((nChildrenAttached != 0) && (nChildrenAttached != info->keysChildren.count())) {
      checkState = Qt::PartiallyChecked;
    } else {
      checkState = Qt::Checked;
    }
  } else {
    checkState = Qt::Checked;
  }
  info->checkState = checkState;
  info->updateLostFound = updateLostFound;

  // update item counters and track/waypoint correlation
  // updateItems(); <--- don't! this is causing a crash
  if (!changedItems) {
    setText(CGisListWks::eColumnDecoration, autoSave ? "A" : "");
  }

  CGisDatabase::self().postEventForDb(info);
}

CDBProject::action_e CDBProject::checkForAction2(IGisItem* item, quint64& itemId, QString& hashItem,
                                                 action_e& action2ForAll, QSqlQuery& query) {
  action_e action = eActionNone;

  query.prepare("SELECT hash, last_user, last_change FROM items WHERE id=:id");
  query.bindValue(":id", itemId);
  QUERY_EXEC(throw eReasonQueryFail);

  if (query.next()) {
    QString hash = query.value(0).toString();
    QString user = query.value(1).toString();
    QString date = query.value(2).toString();

    if (hash == hashItem) {
      // there seems to be no difference
      return action;
    }

    hashItem = hash;

    QString msg = tr("The item %1 has been changed by %2 (%3). \n\n"
                     "To solve this conflict you can create and save a clone, force your version or drop "
                     "your version and take the one from the database")
                      .arg(item->getNameEx(), user, date);

    CResolveDatabaseConflict dialog(msg, item, action2ForAll, CMainWindow::self().getBestWidgetForParent());
    action = dialog.getAction();
  } else {
    // item has been removed. By throwing eReasonConflict
    // the save procedure is restarted for the item and
    // the item should be inserted into the database.
    throw eReasonConflict;
  }

  return action;
}

void CDBProject::updateItem(IGisItem*& item, quint64 idItem, action_e& action2ForAll, QSqlQuery& query) {
  // serialize complete history of item
  QByteArray data;
  QDataStream in(&data, QIODevice::WriteOnly);
  in.setByteOrder(QDataStream::LittleEndian);
  in.setVersion(QDataStream::Qt_5_2);
  in << item->getHistory();

  // prepare icon to be saved
  QBuffer buffer;
  buffer.open(QIODevice::ReadWrite);
  QPixmap pixmap = item->getDisplayIcon();
  pixmap.save(&buffer, "PNG");
  buffer.seek(0);

  QString hashInDb = item->getLastDatabaseHash();

  query.prepare(
      "UPDATE items SET type=:type, keyqms=:keyqms, icon=:icon, name=:name, date=:date, comment=:comment, data=:data, "
      "hash=:hash WHERE id=:id AND hash=:oldhash");
  query.bindValue(":type", item->type());
  query.bindValue(":keyqms", item->getKey().item);
  query.bindValue(":icon", buffer.data());
  query.bindValue(":name", item->getName());
  query.bindValue(":date", item->getTimestamp());
  query.bindValue(":comment", item->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowFullText));
  query.bindValue(":data", data);
  query.bindValue(":hash", item->getHash());
  query.bindValue(":id", idItem);
  query.bindValue(":oldhash", hashInDb);
  QUERY_EXEC(throw eReasonQueryFail);

  if (query.numRowsAffected()) {
    // the update has been successful.
    // set current hash as database hash.
    item->setLastDatabaseHash(idItem, db);
  } else {
    // there are two reasons why an update does not affect a row
    // 1) the hash is different because another user changed the item
    // 2) the id was not found because another user removed the item
    // 3) the items was completely identical, therefore no row was affected.
    int action = checkForAction2(item, idItem, hashInDb, action2ForAll, query);

    switch (action) {
      case eActionClone: {
        IGisItem* item2 = item->createClone();
        quint64 idItem = insertItem(item2, query);

        delete item;
        item = item2;

        query.prepare("INSERT INTO folder2item (parent, child) VALUES (:parent, :child)");
        query.bindValue(":parent", id);
        query.bindValue(":child", idItem);
        QUERY_EXEC(throw eReasonQueryFail);
        break;
      }

      case eActionUpdate: {
        // hashInDb has been updated by checkForAction2() by the one stored in the database
        // therefore the update should succeed now.
        query.prepare(
            "UPDATE items SET type=:type, keyqms=:keyqms, icon=:icon, name=:name, date=:date, comment=:comment, "
            "data=:data, hash=:hash WHERE id=:id AND hash=:oldhash");
        query.bindValue(":type", item->type());
        query.bindValue(":keyqms", item->getKey().item);
        query.bindValue(":icon", buffer.data());
        query.bindValue(":name", item->getName());
        query.bindValue(":date", item->getTimestamp());
        query.bindValue(":comment", item->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowFullText));
        query.bindValue(":data", data);
        query.bindValue(":hash", item->getHash());
        query.bindValue(":id", idItem);
        query.bindValue(":oldhash", hashInDb);
        QUERY_EXEC(throw eReasonQueryFail);

        if (query.numRowsAffected()) {
          item->setLastDatabaseHash(idItem, db);
        } else {
          // in the case someone updated the item between calling
          // checkForAction2() and this update our update fails.
          // In this case we throw eReasonConflict to restart the
          // save procedure for this item.
          throw eReasonConflict;
        }
        break;
      }

      case eActionReload:
        item->updateFromDB(idItem, db);
        break;
    }
  }
}

quint64 CDBProject::insertItem(IGisItem* item, QSqlQuery& query) {
  quint64 idItem = 0;

  // serialize complete history of item
  QByteArray data;
  QDataStream in(&data, QIODevice::WriteOnly);
  in.setByteOrder(QDataStream::LittleEndian);
  in.setVersion(QDataStream::Qt_5_2);
  in << item->getHistory();

  // prepare icon to be saved
  QBuffer buffer;
  buffer.open(QIODevice::ReadWrite);
  QPixmap pixmap = item->getDisplayIcon();
  pixmap.save(&buffer, "PNG");
  buffer.seek(0);

  query.prepare(
      "INSERT INTO items (type, keyqms, icon, name, date, comment, data, hash) VALUES (:type, :keyqms, :icon, :name, "
      ":date, :comment, :data, :hash)");
  query.bindValue(":type", item->type());
  query.bindValue(":keyqms", item->getKey().item);
  query.bindValue(":icon", buffer.data());
  query.bindValue(":name", item->getName());
  query.bindValue(":date", item->getTimestamp());
  query.bindValue(":comment", item->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowFullText));
  query.bindValue(":data", data);
  query.bindValue(":hash", item->getHash());
  QUERY_EXEC(throw eReasonQueryFail);

  if (query.numRowsAffected()) {
    idItem = IDB::getLastInsertID(db, "items");
    if (idItem == 0) {
      qDebug() << "childId equals 0. bad.";
      throw eReasonUnexpected;
    }
    item->setLastDatabaseHash(idItem, db);
  } else {
    throw eReasonConflict;
  }

  return idItem;
}

CDBProject::action_e CDBProject::checkForAction1(IGisItem* item, quint64& itemId,
                                                 CSelectSaveAction::result_e& action1ForAll, QSqlQuery& query) {
  int action = eActionNone;

  // test if item exists in database
  quint32 itemType = 0;
  query.prepare("SELECT id, type FROM items WHERE keyqms=:keyqms");
  query.bindValue(":keyqms", item->getKey().item);
  QUERY_EXEC(throw eReasonQueryFail);

  if (query.next()) {
    itemId = query.value(0).toULongLong();
    itemType = query.value(1).toUInt();

    // check if relation already exists.
    query.prepare("SELECT id FROM folder2item WHERE parent=:parent AND child=:child");
    query.bindValue(":parent", id);
    query.bindValue(":child", itemId);
    QUERY_EXEC(throw eReasonQueryFail);

    if (!query.next()) {
      // item is already in database but folder relation does not exit
      CSelectSaveAction::result_e result = action1ForAll;

      if (action1ForAll == CSelectSaveAction::eResultNone) {
        // Build the dialog to ask for user action
        IGisItem* item1 = IGisItem::newGisItem(itemType, itemId, db, nullptr);

        if (nullptr == item1) {
          qDebug() << "no item to compare!?.";
          throw eReasonUnexpected;
        }

        CSelectSaveAction dlg(item, item1, CMainWindow::self().getBestWidgetForParent());
        dlg.exec();

        result = dlg.getResult();
        if (dlg.allOthersToo()) {
          action1ForAll = result;
        }
      }

      if (result == CSelectSaveAction::eResultNone) {
        // no decision by user, cancel operation.
        // this is different to a skip as a skip will
        // just skip saving the data, but the item to folder
        // link will be still processed.
        return eActionNone;
      }

      // the item is in the database and has no relation to the folder -> update only if the user confirms.
      action = eActionLink;

      switch (result) {
        case CSelectSaveAction::eResultSave:
          action |= eActionUpdate;
          break;

        case CSelectSaveAction::eResultSkip:
          action |= eActionReload;
          break;

        case CSelectSaveAction::eResultClone:
          action |= eActionClone;
          break;
      }
    } else {
      // the item is in the database and has a relation to the folder -> simply update item
      action = eActionUpdate;
    }
  } else {
    action = eActionInsert | eActionLink;
  }

  return (action_e)action;
}

bool CDBProject::save() { return save(CSelectSaveAction::eResultNone, eActionNone); }

bool CDBProject::save(CSelectSaveAction::result_e action1ForAll, action_e action2ForAll) {
  QSqlQuery query(db);
  bool stop = false;
  bool success = true;

  // check if project is still part of the database
  query.prepare("SELECT keyqms FROM folders WHERE id=:id");
  query.bindValue(":id", id);
  QUERY_EXEC(return false);
  if (!query.next()) {
    QMessageBox::critical(CMainWindow::self().getBestWidgetForParent(), tr("Missing folder..."),
                          tr("Failed to save project. The folder has been deleted in the database."),
                          QMessageBox::Abort);
    return false;
  }

  int N = childCount();
  PROGRESS_SETUP(tr("Save ..."), 0, N, CMainWindow::getBestWidgetForParent());

  for (int i = 0; (i < N) && !stop; i++) {
    try {
      PROGRESS(i, throw eReasonCancel);

      IGisItem* item = dynamic_cast<IGisItem*>(child(i));
      if (nullptr == item) {
        continue;
      }

      // skip unchanged items
      if (!item->isChanged()) {
        continue;
      }

      quint64 idItem = 0;

      int action = checkForAction1(item, idItem, action1ForAll, query);

      if (action & eActionInsert) {
        idItem = insertItem(item, query);
      }

      if (action & eActionUpdate) {
        updateItem(item, idItem, action2ForAll, query);
      }

      if (action & eActionReload) {
        item->updateFromDB(idItem, db);
      }

      if (action & eActionClone) {
        IGisItem* item2 = item->createClone();
        idItem = insertItem(item2, query);

        delete item;
        item = item2;
      }

      if ((action & eActionLink) && (idItem != 0)) {
        query.prepare("INSERT INTO folder2item (parent, child) VALUES (:parent, :child)");
        query.bindValue(":parent", id);
        query.bindValue(":child", idItem);
        QUERY_EXEC(throw eReasonQueryFail);
      }
      item->updateDecoration(IGisItem::eMarkNone,
                             IGisItem::eMarkChanged | IGisItem::eMarkNotPart | IGisItem::eMarkNotInDB);
    } catch (reasons_e reason) {
      CProgressDialog::setAllVisible(false);
      switch (reason) {
        case eReasonQueryFail:
          QMessageBox::critical(&progress, tr("Error"),
                                tr("There was an unexpected database error:\n\n%1").arg(query.lastError().text()),
                                QMessageBox::Abort);

        case eReasonCancel:
        case eReasonUnexpected:
          stop = true;
          success = false;
          break;

        case eReasonConflict:
          i--;
          break;
      }

      CProgressDialog::setAllVisible(true);
    }
  }

  // serialize metadata of project
  QByteArray data;
  QDataStream in(&data, QIODevice::WriteOnly);
  in.setByteOrder(QDataStream::LittleEndian);
  in.setVersion(QDataStream::Qt_5_2);
  *this >> in;

  // update folder entry in database
  query.prepare("UPDATE folders SET name=:name, comment=:comment, data=:data, sortmode=:sortmode WHERE id=:id");
  query.bindValue(":name", getName());
  query.bindValue(":comment", getInfo());
  query.bindValue(":data", data);
  query.bindValue(":sortmode", getSortingFolder());
  query.bindValue(":id", getId());
  QUERY_EXEC(return false);

  postStatus(true);
  // update change flag
  updateDecoration();
  return success;
}

void CDBProject::showItems(CEvtD2WShowItems* evt, action_e action2ForAll) {
  bool restoreDlgDetails = false;
  if (evt->addItemsExclusively) {
    restoreDlgDetails = !dlgDetails.isNull();
    delete dlgDetails;

    qDeleteAll(takeChildren());
  }

  for (const evt_item_t& item : std::as_const(evt->items)) {
    IGisItem* gisItem = IGisItem::newGisItem(item.type, item.id, db, this);

    /* [Issue #72] Database/Workspace inconsistency in QMS 1.4.0

       When an item with no key is loaded it is "healed". The healing
       will mark it as changed. To avoid this save all items that are
       marked as changed right after loading from the database.

     */
    if (gisItem && gisItem->isChanged()) {
      bool success = true;
      try {
        QSqlQuery query(db);
        updateItem(gisItem, item.id, action2ForAll, query);
      } catch (int) {
        success = false;
      }

      if (success) {
        gisItem->updateDecoration(IGisItem::eMarkNone, IGisItem::eMarkChanged);
      }
    }
  }

  sortItems();
  postStatus(false);
  setToolTip(CGisListWks::eColumnName, getInfo());

  if (restoreDlgDetails) {
    edit();
  }
}

void CDBProject::hideItems(CEvtD2WHideItems* evt) {
  IGisItem::key_t key;
  key.project = getKey();

  QMessageBox::StandardButtons last = QMessageBox::YesToAll;

  for (const QString& k : std::as_const(evt->keys)) {
    key.item = k;
    delItemByKey(key, last);
  }

  postStatus(false);
  setToolTip(CGisListWks::eColumnName, getInfo());
}

void CDBProject::update() {
  Qt::CheckState state = checkState;

  if (isChanged()) {
    QString msg =
        tr("The project '%1' is about to update itself from the database. However there are changes not saved.")
            .arg(getName());
    int res = QMessageBox::question(CMainWindow::self().getBestWidgetForParent(), tr("Save changes?"), msg,
                                    QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Abort, QMessageBox::Save);

    if (res == QMessageBox::Abort) {
      return;
    }
    if (res == QMessageBox::Save) {
      if (!save()) {
        return;
      }
    }
  }

  // read project properties
  QSqlQuery query(db);
  query.prepare("SELECT date, name, data FROM folders WHERE id=:id");
  query.bindValue(":id", getId());
  QUERY_EXEC(return );
  query.next();

  QString name = query.value(1).toString();
  QByteArray data = query.value(2).toByteArray();

  if (!data.isEmpty()) {
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_2);
    *this << in;
    filename = getDBName();
  }

  setupName(name);
  setToolTip(CGisListWks::eColumnName, getInfo());

  /*
      The further proceeding depends on the check state of the project. If the project
      is partially loaded we simply update all items. If it is completely loaded we
      reload it.
   */

  if (state == Qt::Checked) {
    // get keys of all children attached to the project in the database
    query.prepare("SELECT id, type FROM items WHERE id IN (SELECT child FROM folder2item WHERE parent=:parent)");
    query.bindValue(":parent", getId());
    QUERY_EXEC(return );

    CEvtD2WShowItems* evt = new CEvtD2WShowItems(getId(), getDBName());
    evt->addItemsExclusively = true;

    while (query.next()) {
      evt->items << evt_item_t(query.value(0).toULongLong(), query.value(1).toUInt());
    }

    CGisWorkspace::self().postEventForWks(evt);
  } else {
    // Iterate over all children and update
    const int N = childCount();
    for (int i = 0; i < N; i++) {
      IGisItem* item = dynamic_cast<IGisItem*>(child(i));
      if (item == nullptr) {
        continue;
      }

      const IGisItem::key_t& key = item->getKey();
      // update item from database
      query.prepare("SELECT id FROM items WHERE keyqms=:keyqms");
      query.bindValue(":keyqms", key.item);
      QUERY_EXEC(return );

      if (query.next()) {
        // item is in the database
        quint64 idItem = query.value(0).toULongLong();

        QSqlQuery query2(db);
        query2.prepare("SELECT id FROM folder2item WHERE parent=:parent AND child=:child");
        query2.bindValue(":parent", getId());
        query2.bindValue(":child", idItem);
        query2.exec();

        if (query2.next()) {
          // item is connected to this project
          item->updateFromDB(idItem, db);
          item->updateDecoration(IGisItem::eMarkNone, IGisItem::eMarkChanged);
        } else {
          // item is not connected to this project
          item->updateFromDB(idItem, db);
          item->updateDecoration(IGisItem::eMarkNotPart | IGisItem::eMarkChanged, IGisItem::eMarkNone);
        }
      } else {
        // item is not in the database at all.
        item->updateDecoration(IGisItem::eMarkNotInDB | IGisItem::eMarkChanged, IGisItem::eMarkNone);
      }
    }

    postStatus(false);
  }

  updateDecoration();
}
