/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#include "gis/trk/surface/CSurfaceDbManager.h"

#include <QDir>
#include <QFileInfo>

#include "gis/trk/surface/CSurfaceDb.h"
#include "gis/trk/surface/CSurfaceDbBuilder.h"
#include "helpers/CSettings.h"
#include "setup/IAppSetup.h"

CSurfaceDbManager& CSurfaceDbManager::self() {
  static CSurfaceDbManager* instance = new CSurfaceDbManager();
  return *instance;
}

CSurfaceDbManager::CSurfaceDbManager() {
  SETTINGS;
  path = cfg.value("Surface/path", QString()).toString();
  inactive = cfg.value("Surface/inactive", QStringList()).toStringList();
  if (path.isEmpty()) {
    path = defaultPath();
  }
  rescan();
}

QString CSurfaceDbManager::defaultPath() {
  SETTINGS;
  const QStringList routino = cfg.value("Route/routino/paths", QStringList()).toStringList();
  if (!routino.isEmpty() && !routino.first().isEmpty()) {
    QDir dir(routino.first());
    if (dir.cdUp()) {
      return dir.absoluteFilePath("Surface");
    }
  }
  return IAppSetup::getPlatformInstance()->userDataPath("Surface");
}

void CSurfaceDbManager::save() {
  SETTINGS;
  cfg.setValue("Surface/path", path);
  cfg.setValue("Surface/inactive", inactive);
}

void CSurfaceDbManager::setPath(const QString& newPath) {
  if (newPath == path) {
    return;
  }
  path = newPath;
  save();
  rescan();
}

void CSurfaceDbManager::setActive(const QString& file, bool yes) {
  const QString name = QFileInfo(file).fileName();
  if (yes) {
    inactive.removeAll(name);
  } else if (!inactive.contains(name)) {
    inactive << name;
  }
  save();
  rescan();
}

void CSurfaceDbManager::rescan() {
  QList<db_t> found;
  const QDir dir(path);
  const QStringList files = dir.entryList({"*." + CSurfaceDbBuilder::suffix()}, QDir::Files, QDir::Name);
  for (const QString& name : files) {
    db_t entry;
    entry.file = dir.absoluteFilePath(name);
    entry.active = !inactive.contains(name);

    CSurfaceDb db(entry.file);
    if (db.isValid()) {
      entry.name = db.name();
      entry.source = db.source();
      entry.boundingBox = db.boundingBox();
      entry.osmTimestamp = db.osmTimestamp();
      entry.created = db.created();
      entry.chunks = db.chunkCount();
    } else {
      entry.error = db.errorString();
    }
    found << entry;
  }

  auto key = [](const QList<db_t>& list) {
    QStringList items;
    for (const db_t& db : list) {
      items << db.file + "|" + db.created.toString(Qt::ISODate) + "|" + QString::number(db.active) + "|" + db.error;
    }
    return items;
  };

  const bool changed = key(found) != key(databases);
  databases = found;
  if (changed) {
    generation++;
    emit sigChanged();
  }
}

QStringList CSurfaceDbManager::getActiveFiles() const {
  QStringList files;
  for (const db_t& db : databases) {
    if (db.active && db.error.isEmpty()) {
      files << db.file;
    }
  }
  return files;
}

bool CSurfaceDbManager::covers(const QRectF& area) const {
  for (const db_t& db : databases) {
    if (db.active && db.error.isEmpty() &&
        (db.boundingBox.intersects(area) || db.boundingBox.contains(area.center()))) {
      return true;
    }
  }
  return false;
}
