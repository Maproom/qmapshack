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

#include "qlgt/CQmsDb.h"

#include <QtSql>
#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/db/CDBProject.h"
#include "gis/db/IDBFolder.h"
#include "gis/db/macros.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "qlgt/CQlgtDb.h"
#include "qlgt/CQlgtFolder.h"
#include "qlgt/CQlgtRoute.h"
#include "qlgt/CQlgtTrack.h"
#include "qlgt/CQlgtWpt.h"
#include "qlgt/IQlgtOverlay.h"
#include "tool/CImportDatabase.h"

CQmsDb::CQmsDb(const QString& filename, CImportDatabase* parent)
    : QObject(parent),
      valid(false),
      gui(parent)

{
  if (QFile::exists(filename)) {
    int res = QMessageBox::question(CMainWindow::getBestWidgetForParent(), tr("Existing file..."),
                                    tr("Remove existing %1?").arg(filename), QMessageBox::Ok | QMessageBox::Abort,
                                    QMessageBox::Ok);
    if (res != QMessageBox::Ok) {
      return;
    }
    gui->stdErr(tr("Remove existing file %1").arg(filename));
    QFile::remove(filename);
  }
  QString error;
  valid = setupDB(filename, "qlgt2qms", error);
  if (!valid) {
    return;
  }

  mapFolderTypes[CQlgtDb::eFolder1] = IDBFolder::eTypeGroup;
  mapFolderTypes[CQlgtDb::eFolder2] = IDBFolder::eTypeProject;
  mapFolderTypes[CQlgtDb::eFolderN] = IDBFolder::eTypeOther;

  mapFolderIDs[1] = 1;
  mapFolderIDs[0] = 1;
}

CQmsDb::~CQmsDb() { db.close(); }

void CQmsDb::addFolder2FolderRelation(quint64 parent, quint64 child) {
  QSqlQuery query(db);
  query.prepare("INSERT INTO folder2folder (parent, child) VALUES (:parent, :child)");
  query.bindValue(":parent", mapFolderIDs[parent]);
  query.bindValue(":child", mapFolderIDs[child]);
  QUERY_EXEC(return );
}

void CQmsDb::addFolder2ItemRelation(quint64 parent, quint64 child) {
  QSqlQuery query(db);
  query.prepare("INSERT INTO folder2item (parent, child) VALUES (:parent, :child)");
  query.bindValue(":parent", mapFolderIDs[parent]);
  query.bindValue(":child", mapItemIDs[child]);
  QUERY_EXEC(return );
}

void CQmsDb::addFolder(CQlgtFolder& folder) {
  if (folder.id < 2) {
    return;
  }

  QSqlQuery query(db);

  // folders without child items
  if (folder.items.isEmpty()) {
    query.prepare("INSERT INTO folders (type, name, locked) VALUES (:type, :name, :locked)");
    query.bindValue(":type", mapFolderTypes[folder.type]);
    query.bindValue(":name", folder.name);
    query.bindValue(":locked", folder.locked);
    QUERY_EXEC(return );

    query.prepare("SELECT last_insert_rowid() from folders");
    QUERY_EXEC(return );
    query.next();
    quint64 id = query.value(0).toULongLong();
    if (id == 0) {
      qDebug() << "CGisListDB::slotAddFolder(): childId equals 0. bad.";
      return;
    }
    mapFolderIDs[folder.id] = id;
    return;
  }

  /*
      Folders with child items will be loaded as complete CDBProject first, to
      generate key and info text properly
   */
  CDBProject project(folder);
  for (quint64 id : std::as_const(folder.items)) {
    quint64 idChild = mapItemIDs[id];
    query.prepare("SELECT type FROM items WHERE id=:id");
    query.bindValue(":id", idChild);
    QUERY_EXEC(continue);
    if (query.next()) {
      int type = query.value(0).toInt();
      switch (type) {
        case IGisItem::eTypeWpt:
          new CGisItemWpt(idChild, db, &project);
          break;

        case IGisItem::eTypeTrk:
          new CGisItemTrk(idChild, db, &project);
          break;

        case IGisItem::eTypeRte:
          new CGisItemRte(idChild, db, &project);
          break;

        case IGisItem::eTypeOvl:
          new CGisItemOvlArea(idChild, db, &project);
          break;

        default:;
      }
    } else {
      gui->stdErr(tr("%1: drop item with QLGT DB ID %2").arg(folder.name).arg(id));
    }
  }

  // serialize metadata of project
  QByteArray data;
  QDataStream in(&data, QIODevice::WriteOnly);
  in.setByteOrder(QDataStream::LittleEndian);
  in.setVersion(QDataStream::Qt_5_2);
  project >> in;

  query.prepare(
      "INSERT INTO folders (type, keyqms, name, comment, locked, data) VALUES (:type, :keyqms, :name, :comment, "
      ":locked, :data)");
  query.bindValue(":type", mapFolderTypes[folder.type]);
  query.bindValue(":keyqms", project.getKey());
  query.bindValue(":name", project.getName());
  query.bindValue(":comment", project.getInfo());
  query.bindValue(":locked", folder.locked);
  query.bindValue(":data", data);
  QUERY_EXEC(return );

  query.prepare("SELECT last_insert_rowid() from folders");
  QUERY_EXEC(return );
  query.next();
  quint64 id = query.value(0).toULongLong();
  if (id == 0) {
    qDebug() << "CGisListDB::slotAddFolder(): childId equals 0. bad.";
    return;
  }
  mapFolderIDs[folder.id] = id;
}

void CQmsDb::addWpt(CQlgtWpt& wpt1) {
  CGisItemWpt wpt(wpt1);
  quint64 id = store(wpt);
  if (id != 0) {
    mapItemIDs[wpt1.id] = id;
  }
}

void CQmsDb::addTrk(CQlgtTrack& trk1) {
  CGisItemTrk trk(trk1);
  quint64 id = store(trk);
  if (id != 0) {
    mapItemIDs[trk1.id] = id;
  }
}

void CQmsDb::addTrk(IQlgtOverlay& trk1) {
  CGisItemTrk trk(trk1);
  quint64 id = store(trk);
  if (id != 0) {
    mapItemIDs[trk1.id] = id;
  }
}

void CQmsDb::addRte(CQlgtRoute& rte1) {
  CGisItemRte rte(rte1);
  quint64 id = store(rte);
  if (id != 0) {
    mapItemIDs[rte1.id] = id;
  }
}

void CQmsDb::addArea(IQlgtOverlay& ovl1) {
  CGisItemOvlArea ovl(ovl1);
  quint64 id = store(ovl);
  if (id != 0) {
    mapItemIDs[ovl1.id] = id;
  }
}

quint64 CQmsDb::store(IGisItem& item) {
  // serialize complete history of item
  QByteArray data;
  QDataStream in(&data, QIODevice::WriteOnly);
  in.setByteOrder(QDataStream::LittleEndian);
  in.setVersion(QDataStream::Qt_5_2);
  in << item.getHistory();

  QBuffer buffer;
  buffer.open(QIODevice::ReadWrite);
  QPixmap pixmap = item.getDisplayIcon();
  pixmap.save(&buffer, "PNG");
  buffer.seek(0);

  QSqlQuery query(db);
  // item is unknown to database -> create item in database
  query.prepare(
      "INSERT INTO items (type, keyqms, icon, name, date, comment, data, hash) VALUES (:type, :keyqms, :icon, :name, "
      ":date, :comment, :data, :hash)");
  query.bindValue(":type", item.type());
  query.bindValue(":keyqms", item.getKey().item);
  query.bindValue(":icon", buffer.data());
  query.bindValue(":name", item.getName());
  query.bindValue(":date", item.getTimestamp());
  query.bindValue(":comment", item.getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowFullText));
  query.bindValue(":data", data);
  query.bindValue(":hash", item.getHash());
  QUERY_EXEC(return 0);

  query.prepare("SELECT last_insert_rowid() from items");
  QUERY_EXEC(return 0);
  query.next();
  quint64 idItem = query.value(0).toULongLong();

  return idItem;
}
