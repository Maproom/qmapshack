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

#include "dem/CDemDraw.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "dem/CDemItem.h"
#include "dem/CDemList.h"
#include "dem/CDemPathSetup.h"
#include "dem/IDem.h"
#include "gis/IGisLine.h"
#include "helpers/CSettings.h"
#include "misc.h"
#include "units/IUnit.h"

QList<CDemDraw*> CDemDraw::dems;
QStringList CDemDraw::demPaths;
QStringList CDemDraw::supportedFormats = QString("*.vrt|*.wcs").split('|');

CDemDraw::CDemDraw(CCanvas* canvas) : IDrawContext("dem", CCanvas::eRedrawDem, canvas) {
  demList = new CDemList(canvas);
  CMainWindow::self().addDemList(demList, canvas->objectName());
  connect(canvas, &CCanvas::destroyed, demList, &CDemList::deleteLater);
  connect(demList, &CDemList::sigChanged, this, &CDemDraw::slotChanged);
  dems << this;

  timerDelayedSave = new QTimer(this);
  timerDelayedSave->setInterval(1000);
  timerDelayedSave->setSingleShot(true);
  timerDelayedSave->callOnTimeout([this]() { saveDemList(); });
}

CDemDraw::~CDemDraw() { dems.removeOne(this); }

void CDemDraw::slotChanged() {
  emitSigCanvasUpdate();
  demList->updateHelpText();
  timerDelayedSave->start();
}

bool CDemDraw::setProjection(const QString& proj) {
  // --- set the new projection
  bool res = IDrawContext::setProjection(proj);
  // --- now build the dem list from scratch. This will deactivate -> activate all dems
  //     By that everything is restored with the new projection
  if (demList->count()) {
    loadDemList();
  }
  return res;
}

void CDemDraw::setupDemPath() {
  qDebug() << "CDemDraw::setupDemPath()";
  QStringList paths = demPaths;
  CDemPathSetup dlg(paths);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  setupDemPath(paths);
}

void CDemDraw::setupDemPath(const QString& path) {
  qDebug() << "CDemDraw::setupDemPath(path)";
  QStringList paths(demPaths);
  if (!demPaths.contains(path)) {
    paths << path;
  }
  setupDemPath(paths);
}

void CDemDraw::setupDemPath(const QStringList& paths) {
  qDebug() << "CDemDraw::setupDemPath(paths)";
  demPaths = paths;

  for (CDemDraw* dem : std::as_const(dems)) {
    dem->saveDemList();
    dem->loadDemList();
  }
}

void CDemDraw::saveDemPath(QSettings& cfg) {
  qDebug() << "CDemDraw::saveDemPath(cfg)";
  cfg.setValue("demPaths", demPaths);
}

void CDemDraw::loadDemPath(QSettings& cfg) {
  qDebug() << "CDemDraw::loadDemPath(cfg)";
  demPaths = cfg.value("demPaths", demPaths).toStringList();
}

void CDemDraw::saveConfig(QSettings& cfg) {
  qDebug() << "CDemDraw::saveConfig(cfg)";
  // -------------------
  saveDemList(cfg);
  cfg.beginGroup("dem2");
  cfg.setValue("zoomIndex", zoomIndex);
  cfg.endGroup();
}

void CDemDraw::loadConfig(QSettings& cfg) /* override */
{
  qDebug() << "CDemDraw::loadConfig(cfg)";
  // -------------------
  loadDemList(cfg);
  cfg.beginGroup("dem2");
  int idx = cfg.value("zoomIndex", zoomIndex).toInt();
  cfg.endGroup();

  zoom(idx);
}

CDemItem* CDemDraw::createDemItem(const QString& filename, const QString& fallbackKey) {
  CDemItem* item = new CDemItem(this);

  QFileInfo fi(filename);
  item->setName(fi.completeBaseName().replace("_", " "));
  item->setFilename(filename, fallbackKey);
  item->updateIcon();
  return item;
}

void CDemDraw::loadDemList() {
  qDebug() << "CDemDraw::loadDemList()";
  SETTINGS;
  cfg.beginGroup("Canvas");
  cfg.beginGroup("Views");
  cfg.beginGroup(canvas->objectName());
  loadDemList(cfg);
  cfg.endGroup();
  cfg.endGroup();
  cfg.endGroup();
}

void CDemDraw::loadDemList(QSettings& cfg) {
  qDebug() << "CDemDraw::loadDemList(cfg)";
  QMutexLocker lock(&CDemItem::mutexActiveDems);
  demList->clear();

  // create a temp list of all dems in the paths
  QMap<QString, CDemItem*> demsFound;
  for (const QString& path : std::as_const(demPaths)) {
    QDir dir(path);

    // find available dems
    const QStringList& filenames = dir.entryList(supportedFormats, QDir::Files | QDir::Readable, QDir::Name);
    for (const QString& filename : filenames) {
      CDemItem* item = createDemItem(dir.absoluteFilePath(filename), "");
      demsFound[item->getKey()] = item;
    }
  }

  cfg.beginGroup("dem2");

  // create a list of dems in the same order as the stored keys
  const QStringList& keysKnownDems = cfg.value("keysKnownDems", {}).toStringList();
  for (const QString& key : keysKnownDems) {
    CDemItem* dem = demsFound.take(key);

    // We have a key but no dem found. Either the dem is missing or
    // the dem has a new hash due to changes -> create a new dem item to investigate
    if (dem == nullptr) {
      cfg.beginGroup(key);
      const QString& filename = cfg.value("filename", "").toString();
      dem = createDemItem(filename, key);
      cfg.endGroup();
    }

    // If the dem content changed the derived key will not match the one
    // we expect. Delete this dem item, as it will have a reincarnation
    // as new dem.
    // If the dem path is not part of the dem paths list also delete
    // the file as it is no longer part of the used dems.
    QFileInfo fi(dem->getFilename());
    if (key != dem->getKey() || !demPaths.contains(fi.absolutePath())) {
      delete dem;
      continue;
    }

    // finally load the config and add the item to the list
    dem->loadConfig(cfg, true);

    demList->addDem(dem);
  }

  cfg.endGroup();  // dem2

  // let's see what's left over in the found dems list. These are all new
  // dem items.
  QList<CDemItem*> demsUnknown = demsFound.values();
  std::sort(demsUnknown.begin(), demsUnknown.end(), &sortByName<CDemItem>);
  for (CDemItem* dem : demsUnknown) {
    dem->setStatus(CMapItemWidget::eStatus::Unused);
    demList->addDem(dem);
  }

  // ------------- stat --------------
  // Apply configuration data from an older version.
  if (cfg.childGroups().contains("dem")) {
    cfg.beginGroup("dem");
    QStringList active = cfg.value("active", {}).toStringList();
    std::reverse(active.begin(), active.end());
    for (const QString& key : active) {
      const int N = demList->count();
      for (int n = 0; n < N; n++) {
        CDemItem* item = demList->item(n);
        if (item && item->getKey() == key) {
          item->loadConfig(cfg, false);
          demList->moveDemToTop(item);
          item->activate();
          break;
        }
      }
    }
    cfg.endGroup();

  }  // ------------- stop --------------

  demList->updateHelpText();
}

void CDemDraw::saveDemList() {
  qDebug() << "CDemDraw::saveDemList()";
  SETTINGS;
  cfg.beginGroup("Canvas");
  cfg.beginGroup("Views");
  cfg.beginGroup(canvas->objectName());
  saveDemList(cfg);
  cfg.endGroup();
  cfg.endGroup();
  cfg.endGroup();
}

void CDemDraw::saveDemList(QSettings& cfg) {
  qDebug() << "CDemDraw::saveDemList(cfg)";
  QMutexLocker lock(&CDemItem::mutexActiveDems);

  cfg.beginGroup("dem2");

  QStringList keys;
  for (int i = 0; i < demList->count(); i++) {
    CDemItem* item = demList->item(i);
    item->saveConfig(cfg);
    keys << item->getKey();
  }
  cfg.setValue("keysKnownDems", keys);
  cfg.endGroup();  // dem2
}

qreal CDemDraw::getElevationAt(const QPointF& pos, bool checkScale) {
  qreal ele = NOFLOAT;
  if (CDemItem::mutexActiveDems.tryLock()) {
    if (demList) {
      for (int i = 0; i < demList->count(); i++) {
        CDemItem* item = demList->item(i);

        if (!item || !item->isActivated()) {
          continue;
        }

        ele = item->getDemFile()->getElevationAt(pos, checkScale);
        if (ele != NOFLOAT) {
          break;
        }
      }
    }
    CDemItem::mutexActiveDems.unlock();
  }
  return ele;
}

qreal CDemDraw::getSlopeAt(const QPointF& pos, bool checkScale) {
  qreal slope = NOFLOAT;
  if (CDemItem::mutexActiveDems.tryLock()) {
    if (demList) {
      for (int i = 0; i < demList->count(); i++) {
        CDemItem* item = demList->item(i);

        if (!item || !item->isActivated()) {
          continue;
        }

        slope = item->getDemFile()->getSlopeAt(pos, checkScale);
        if (slope != NOFLOAT) {
          break;
        }
      }
    }
    CDemItem::mutexActiveDems.unlock();
  }
  return slope;
}

void CDemDraw::getElevationAt(const QPolygonF& pos, QPolygonF& ele) {
  for (int i = 0; i < pos.size(); i++) {
    qreal tmp = getElevationAt(pos[i]);
    ele[i].ry() = (tmp == NOFLOAT) ? NOFLOAT : tmp;
  }
}

void CDemDraw::getSlopeAt(const QPolygonF& pos, QPolygonF& slope) {
  for (int i = 0; i < pos.size(); i++) {
    slope[i].ry() = getSlopeAt(pos[i]);
  }
}

void CDemDraw::getElevationAt(SGisLine& line) { line.updateElevation(this); }

void CDemDraw::drawt(buffer_t& currentBuffer) {
  // iterate over all active dems and call the draw method
  CDemItem::mutexActiveDems.lock();
  if (demList) {
    for (int i = 0; i < demList->count(); i++) {
      CDemItem* item = demList->item(i);

      if (!item || !item->isActivated()) {
        continue;
      }

      item->getDemFile()->draw(currentBuffer);
    }
  }
  CDemItem::mutexActiveDems.unlock();
}
