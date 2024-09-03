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
#include "units/IUnit.h"

QList<CDemDraw*> CDemDraw::dems;
QStringList CDemDraw::demPaths;
QStringList CDemDraw::supportedFormats = QString("*.vrt|*.wcs").split('|');

CDemDraw::CDemDraw(CCanvas* canvas) : IDrawContext("dem", CCanvas::eRedrawDem, canvas) {
  demList = new CDemList(canvas);
  CMainWindow::self().addDemList(demList, canvas->objectName());
  connect(canvas, &CCanvas::destroyed, demList, &CDemList::deleteLater);
  connect(demList, &CDemList::sigChanged, this, &CDemDraw::emitSigCanvasUpdate);

  buildMapList();

  dems << this;
}

CDemDraw::~CDemDraw() { dems.removeOne(this); }

bool CDemDraw::setProjection(const QString& proj) {
  // --- save the active maps
  QStringList keys;
  saveActiveMapsList(keys);
  // --- now set the new projection
  bool res = IDrawContext::setProjection(proj);
  // --- now build the map list from scratch. This will deactivate -> activate all maps
  //     By that everything is restored with the new projection
  buildMapList();
  restoreActiveMapsList(keys);
  return res;
}

void CDemDraw::setupDemPath() {
  QStringList paths = demPaths;
  CDemPathSetup dlg(paths);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  setupDemPath(paths);
}

void CDemDraw::setupDemPath(const QString& path) {
  QStringList paths(demPaths);
  if (!demPaths.contains(path)) {
    paths << path;
  }
  setupDemPath(paths);
}

void CDemDraw::setupDemPath(const QStringList& paths) {
  demPaths = paths;

  for (CDemDraw* dem : std::as_const(dems)) {
    QStringList keys;
    dem->saveActiveMapsList(keys);
    dem->buildMapList();
    dem->restoreActiveMapsList(keys);
  }
}

void CDemDraw::saveDemPath(QSettings& cfg) { cfg.setValue("demPaths", demPaths); }

void CDemDraw::loadDemPath(QSettings& cfg) { demPaths = cfg.value("demPaths", demPaths).toStringList(); }

void CDemDraw::saveConfig(QSettings& cfg) {
  // store group context for later use
  cfgGroup = cfg.group();
  // -------------------
  QStringList keys;
  cfg.beginGroup("dem");
  saveActiveMapsList(keys, cfg);
  cfg.setValue("active", keys);
  cfg.endGroup();
}

void CDemDraw::loadConfig(QSettings& cfg) {
  // store group context for later use
  cfgGroup = cfg.group();
  // -------------------
  cfg.beginGroup("dem");
  if (cfgGroup.isEmpty()) {
    restoreActiveMapsList(cfg.value("active", "").toStringList(), cfg);
  } else {
    restoreActiveMapsList(cfg.value("active", "").toStringList());
  }

  cfg.endGroup();
}

void CDemDraw::buildMapList() {
  QCryptographicHash md5(QCryptographicHash::Md5);

  QMutexLocker lock(&CDemItem::mutexActiveDems);
  demList->clear();

  for (const QString& path : std::as_const(demPaths)) {
    QDir dir(path);
    // find available maps
    const QStringList& filenames = dir.entryList(supportedFormats, QDir::Files | QDir::Readable, QDir::Name);
    for (const QString& filename : filenames) {
      QFileInfo fi(filename);

      CDemItem* item = new CDemItem(*demList, this);

      item->setText(0, fi.completeBaseName().replace("_", " "));
      item->filename = dir.absoluteFilePath(filename);
      item->updateIcon();

      // calculate MD5 hash from the file's first 1024 bytes
      QFile f(dir.absoluteFilePath(filename));
      f.open(QIODevice::ReadOnly);
      md5.reset();
      md5.addData(f.read(1024));
      item->key = md5.result().toHex();
      f.close();
    }
  }

  demList->sort();
  demList->updateHelpText();
}

void CDemDraw::saveActiveMapsList(QStringList& keys) {
  SETTINGS;
  cfg.beginGroup(cfgGroup);
  cfg.beginGroup("dem");
  saveActiveMapsList(keys, cfg);
  cfg.endGroup();
  cfg.endGroup();
}

void CDemDraw::saveActiveMapsList(QStringList& keys, QSettings& cfg) {
  QMutexLocker lock(&CDemItem::mutexActiveDems);

  for (int i = 0; i < demList->count(); i++) {
    CDemItem* item = demList->item(i);
    if (item && !item->demfile.isNull()) {
      item->saveConfig(cfg);
      keys << item->key;
    }
  }
}

void CDemDraw::loadConfigForDemItem(CDemItem* item) {
  if (cfgGroup.isEmpty()) {
    return;
  }

  SETTINGS;
  cfg.beginGroup(cfgGroup);
  cfg.beginGroup("dem");
  item->loadConfig(cfg);
  cfg.endGroup();
  cfg.endGroup();
}

void CDemDraw::restoreActiveMapsList(const QStringList& keys) {
  QMutexLocker lock(&CDemItem::mutexActiveDems);

  for (const QString& key : keys) {
    for (int i = 0; i < demList->count(); i++) {
      CDemItem* item = demList->item(i);

      if (item && item->key == key) {
        /**
            @Note   the item will load it's configuration upon successful activation
                    by calling loadConfigForDemItem().
         */
        item->activate();
        break;
      }
    }
  }

  demList->updateHelpText();
}

void CDemDraw::restoreActiveMapsList(const QStringList& keys, QSettings& cfg) {
  QMutexLocker lock(&CDemItem::mutexActiveDems);

  for (const QString& key : keys) {
    for (int i = 0; i < demList->count(); i++) {
      CDemItem* item = demList->item(i);

      if (item && item->key == key) {
        if (item->activate()) {
          item->loadConfig(cfg);
        }
        break;
      }
    }
  }

  demList->updateHelpText();
}

qreal CDemDraw::getElevationAt(const QPointF& pos, bool checkScale) {
  qreal ele = NOFLOAT;
  if (CDemItem::mutexActiveDems.tryLock()) {
    if (demList) {
      for (int i = 0; i < demList->count(); i++) {
        CDemItem* item = demList->item(i);

        if (!item || item->demfile.isNull()) {
          // as all active maps have to be at the top of the list
          // it is ok to break as soon as the first map with no
          // active files is hit.
          break;
        }

        ele = item->demfile->getElevationAt(pos, checkScale);
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

        if (!item || item->demfile.isNull()) {
          // as all active maps have to be at the top of the list
          // it is ok to break as soon as the first map with no
          // active files is hit.
          break;
        }

        slope = item->demfile->getSlopeAt(pos, checkScale);
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
  // iterate over all active maps and call the draw method
  CDemItem::mutexActiveDems.lock();
  if (demList) {
    for (int i = 0; i < demList->count(); i++) {
      CDemItem* item = demList->item(i);

      if (!item || item->demfile.isNull()) {
        // as all active maps have to be at the top of the list
        // it is ok to break as soon as the first map with no
        // active files is hit.
        break;
      }

      item->demfile->draw(currentBuffer);
    }
  }
  CDemItem::mutexActiveDems.unlock();
}
