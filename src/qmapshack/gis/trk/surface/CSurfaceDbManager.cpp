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

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QThread>

#include "canvas/CCanvas.h"
#include "gis/trk/surface/CSurfaceDb.h"
#include "gis/trk/surface/CSurfaceDbBuilder.h"
#include "helpers/CSettings.h"
#include "misc.h"
#include "setup/IAppSetup.h"

CSurfaceDbManager::CSurfaceDbManager() {
  SETTINGS;
  if (cfg.contains("Surface/paths")) {
    paths = cfg.value("Surface/paths").toStringList();
  } else {
    // saved at once: changing the Routino paths later must not move the databases' folder
    paths = {defaultPath()};
    cfg.setValue("Surface/paths", paths);
  }
  inactive = cfg.value("Surface/inactive", QStringList()).toStringList();

  // each job opens its own database connections, a few in parallel are plenty
  pool.setMaxThreadCount(2);
  connect(qApp, &QCoreApplication::aboutToQuit, this, &CSurfaceDbManager::slotAboutToQuit);

  timerCanvasUpdate.setSingleShot(true);
  timerCanvasUpdate.setInterval(200);
  connect(&timerCanvasUpdate, &QTimer::timeout, this, [] { CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis); });

  rescan();
}

CSurfaceDbManager& CSurfaceDbManager::self() {
  Q_ASSERT(QThread::isMainThread());
  static CSurfaceDbManager* instance = new CSurfaceDbManager();
  return *instance;
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

QString CSurfaceDbManager::keyOf(const QString& file) {
  const QFileInfo fi(file);
  const QString canonical = fi.canonicalFilePath();
  return canonical.isEmpty() ? fi.absoluteFilePath() : canonical;
}

void CSurfaceDbManager::save() {
  SETTINGS;
  cfg.setValue("Surface/paths", paths);
  cfg.setValue("Surface/inactive", inactive);
}

void CSurfaceDbManager::setPaths(const QStringList& newPaths) {
  if (newPaths == paths) {
    return;
  }
  paths = newPaths;
  save();
  rescan();
}

void CSurfaceDbManager::setActive(const QString& file, bool yes) {
  const QString key = keyOf(file);
  if (yes) {
    inactive.removeAll(key);
  } else if (!inactive.contains(key)) {
    inactive << key;
  }
  save();
  rescan();
}

void CSurfaceDbManager::rescan() {
  QList<db_t> found;
  QStringList scanned;
  for (const QString& path : std::as_const(paths)) {
    // the same directory listed twice (e.g. through a link) must not double its databases
    if (containsPath(scanned, path)) {
      continue;
    }
    scanned << path;

    const QDir dir(path);
    const QStringList files = dir.entryList({"*." + CSurfaceDbBuilder::suffix()}, QDir::Files, QDir::Name);
    for (const QString& name : files) {
      db_t entry;
      entry.file = dir.absoluteFilePath(name);
      entry.active = !inactive.contains(keyOf(entry.file));

      // the tags are read by the jobs that need them
      CSurfaceDb db(entry.file, CSurfaceDb::eModeMeta);
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
  // the cells of a file switched off or deleted are of no use any longer
  CSurfaceDb::releaseShared(getActiveFiles());
  if (changed) {
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

CSurfaceDbManager::coverage_t CSurfaceDbManager::getCoverage(const QRectF& area) const {
  coverage_t coverage;
  for (const db_t& db : databases) {
    if (!db.error.isEmpty() || !CSurfaceDb::overlaps(db.boundingBox, area)) {
      continue;
    }
    if (db.active) {
      coverage.files << db.file;
      coverage.key = qHashMulti(coverage.key, db.file, db.created.toMSecsSinceEpoch());
    } else {
      coverage.switchedOff = true;
    }
  }
  coverage.key = qHashMulti(coverage.key, coverage.switchedOff);
  return coverage;
}

void CSurfaceDbManager::triggerCanvasUpdate() {
  // not restarted, so a steady stream of finished analyses still redraws now and then
  if (!timerCanvasUpdate.isActive()) {
    timerCanvasUpdate.start();
  }
}

void CSurfaceDbManager::slotAboutToQuit() {
  // the instance is never deleted, so this is the last chance to join the threads
  emit sigQuit();
  pool.clear();
  pool.waitForDone();
}
