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

#include "map/CMapDraw.h"

#include <QtGui>
#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "map/CMapItem.h"
#include "map/CMapList.h"
#include "map/CMapPathSetup.h"
#include "map/IMap.h"
#include "map/cache/CDiskCache.h"
#include "poi/IPoiItem.h"
#include "setup/IAppSetup.h"

QList<CMapDraw*> CMapDraw::maps;
QString CMapDraw::cachePath = "";
QStringList CMapDraw::mapPaths;
QStringList CMapDraw::supportedFormats = QString("*.vrt|*.jnx|*.img|*.rmap|*.wmts|*.tms|*.gemf").split('|');

CMapDraw::CMapDraw(CCanvas* parent) : IDrawContext("map", CCanvas::eRedrawMap, parent) {
  mapList = new CMapList(canvas);
  CMainWindow::self().addMapList(mapList, canvas->objectName());
  connect(canvas, &CCanvas::destroyed, mapList, &CMapList::deleteLater);
  connect(mapList, &CMapList::sigChanged, this, &CMapDraw::emitSigCanvasUpdate);

  buildMapList();

  maps << this;
}

CMapDraw::~CMapDraw() { maps.removeOne(this); }

bool CMapDraw::setProjection(const QString& proj) /* override */
{
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

void CMapDraw::setupMapPath() {
  QStringList paths = mapPaths;
  if (cachePath.isEmpty()) {
    cachePath = IAppSetup::getPlatformInstance()->defaultCachePath();
  }
  CMapPathSetup dlg(paths, cachePath);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  setupMapPath(paths);
}

void CMapDraw::setupMapPath(const QString& path) {
  QStringList paths(mapPaths);
  if (!mapPaths.contains(path)) {
    paths << path;
  }
  setupMapPath(paths);
}

void CMapDraw::setupMapPath(const QStringList& paths) {
  mapPaths = paths;

  for (CMapDraw* map : std::as_const(maps)) {
    QStringList keys;
    map->saveActiveMapsList(keys);
    map->buildMapList();
    map->restoreActiveMapsList(keys);
  }
}

void CMapDraw::saveMapPath(QSettings& cfg) {
  cfg.setValue("mapPath", mapPaths);
  cfg.setValue("cachePath", cachePath);
}

void CMapDraw::loadMapPath(QSettings& cfg) {
  mapPaths = cfg.value("mapPath", mapPaths).toStringList();
  cachePath = cfg.value("cachePath", cachePath).toString();

  if (cachePath.isEmpty()) {
    cachePath = IAppSetup::getPlatformInstance()->defaultCachePath();
  }
}

void CMapDraw::getInfo(const QPoint& px, QString& str) {
  if (isRunning()) {
    return;
  }
  CMapItem::mutexActiveMaps.lock();
  if (mapList) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (!item || item->getMapfile().isNull()) {
        // as all active maps have to be at the top of the list
        // it is ok to break as soon as the first map with no
        // active files is hit.
        break;
      }

      item->getMapfile()->getInfo(px, str);
    }
  }
  CMapItem::mutexActiveMaps.unlock();
}

void CMapDraw::getToolTip(const QPoint& px, QString& str) {
  if (isRunning()) {
    return;
  }
  CMapItem::mutexActiveMaps.lock();
  if (mapList) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (!item || item->getMapfile().isNull()) {
        // as all active maps have to be at the top of the list
        // it is ok to break as soon as the first map with no
        // active files is hit.
        break;
      }

      item->getMapfile()->getToolTip(px, str);
    }
  }
  CMapItem::mutexActiveMaps.unlock();
}

IPoiItem CMapDraw::findPOICloseBy(const QPoint& px) const {
  IPoiItem poi;

  if (isRunning()) {
    return poi;
  }
  CMapItem::mutexActiveMaps.lock();
  if (mapList) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (!item || item->getMapfile().isNull()) {
        // as all active maps have to be at the top of the list
        // it is ok to break as soon as the first map with no
        // active files is hit.
        break;
      }

      item->getMapfile()->findPOICloseBy(px, poi);
      if (poi.pos != NOPOINTF) {
        // stop at the 1st one found
        break;
      }
    }
  }
  CMapItem::mutexActiveMaps.unlock();

  return poi;
}

bool CMapDraw::findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline) {
  if (isRunning()) {
    return false;
  }
  bool res = false;
  CMapItem::mutexActiveMaps.lock();
  if (mapList) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (!item || item->getMapfile().isNull()) {
        // as all active maps have to be at the top of the list
        // it is ok to break as soon as the first map with no
        // active files is hit.
        break;
      }

      res = item->getMapfile()->findPolylineCloseBy(pt1, pt2, threshold, polyline);
      if (res) {
        break;
      }
    }
  }
  CMapItem::mutexActiveMaps.unlock();
  return res;
}

void CMapDraw::saveConfig(QSettings& cfg) /* override */
{
  // store group context for later use
  cfgGroup = cfg.group();
  // -------------------
  QStringList keys;
  cfg.beginGroup("map");
  saveActiveMapsList(keys, cfg);
  cfg.setValue("active", keys);
  cfg.setValue("zoomIndex", zoomIndex);
  cfg.endGroup();
}

void CMapDraw::loadConfig(QSettings& cfg) /* override */
{
  // store group context for later use
  cfgGroup = cfg.group();
  // -------------------
  cfg.beginGroup("map");
  if (cfgGroup.isEmpty()) {
    restoreActiveMapsList(cfg.value("active", "").toStringList(), cfg);
  } else {
    restoreActiveMapsList(cfg.value("active", "").toStringList());
  }
  int idx = cfg.value("zoomIndex", zoomIndex).toInt();
  cfg.endGroup();

  zoom(idx);
}

CMapItem* CMapDraw::createMapItem(const QString& filename, QSet<QString>& maps) {
  CMapItem* item = new CMapItem(*mapList, this);

  QFileInfo fi(filename);
  maps.insert(fi.completeBaseName());

  item->setText(0, fi.completeBaseName().replace("_", " "));
  item->setFilename(filename);
  item->updateIcon();
  return item;
}

void CMapDraw::buildMapList(const QString& filename) {
  QMutexLocker lock(&CMapItem::mutexActiveMaps);
  mapList->clear();

  QSet<QString> maps;
  CMapItem* item = createMapItem(filename, maps);
  item->activate();
}

void CMapDraw::buildMapList() {
  QMutexLocker lock(&CMapItem::mutexActiveMaps);
  mapList->clear();

  QSet<QString> maps;

  for (const QString& path : std::as_const(mapPaths)) {
    QDir dir(path);

    // find available maps
    const QStringList& filenames = dir.entryList(supportedFormats, QDir::Files | QDir::Readable, QDir::Name);
    for (const QString& filename : filenames) {
      createMapItem(dir.absoluteFilePath(filename), maps);
    }
  }

  mapList->sort();

  CDiskCache::cleanupRemovedMaps(maps);

  mapList->updateHelpText();
}

void CMapDraw::saveActiveMapsList(QStringList& keys) {
  SETTINGS;
  cfg.beginGroup(cfgGroup);
  cfg.beginGroup("map");
  saveActiveMapsList(keys, cfg);
  cfg.endGroup();
  cfg.endGroup();
}

void CMapDraw::saveActiveMapsList(QStringList& keys, QSettings& cfg) {
  QMutexLocker lock(&CMapItem::mutexActiveMaps);

  for (int i = 0; i < mapList->count(); i++) {
    CMapItem* item = mapList->item(i);
    if (item && !item->getMapfile().isNull()) {
      item->saveConfig(cfg);
      keys << item->getKey();
    }
  }
}

void CMapDraw::loadConfigForMapItem(CMapItem* item) {
  if (cfgGroup.isEmpty()) {
    return;
  }

  SETTINGS;
  cfg.beginGroup(cfgGroup);
  cfg.beginGroup("map");
  item->loadConfig(cfg);
  cfg.endGroup();
  cfg.endGroup();
}

void CMapDraw::restoreActiveMapsList(const QStringList& keys) {
  QMutexLocker lock(&CMapItem::mutexActiveMaps);

  for (const QString& key : keys) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (item && item->getKey() == key) {
        /**
            @Note   the item will load it's configuration upon successful activation
                    by calling loadConfigForMapItem().
         */
        item->activate();
        break;
      }
    }
  }

  mapList->updateHelpText();
}

void CMapDraw::restoreActiveMapsList(const QStringList& keys, QSettings& cfg) {
  QMutexLocker lock(&CMapItem::mutexActiveMaps);

  for (const QString& key : keys) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (item && item->getKey() == key) {
        if (item->activate()) {
          item->loadConfig(cfg);
        }
        break;
      }
    }
  }

  mapList->updateHelpText();
}

void CMapDraw::reportStatusToCanvas(const QString& key, const QString& msg) { canvas->reportStatus(key, msg); }

void CMapDraw::drawt(IDrawContext::buffer_t& currentBuffer) /* override */
{
  bool seenActiveMap = false;
  // iterate over all active maps and call the draw method
  CMapItem::mutexActiveMaps.lock();
  if (mapList && (mapList->count() != 0)) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (!item || item->getMapfile().isNull()) {
        // as all active maps have to be at the top of the list
        // it is ok to break as soon as the first map with no
        // active files is hit.
        break;
      }

      item->getMapfile()->draw(currentBuffer);
      seenActiveMap = true;
    }
  }
  CMapItem::mutexActiveMaps.unlock();

  if (seenActiveMap != hasActiveMap) {
    hasActiveMap = seenActiveMap;
    emit sigActiveMapsChanged(!hasActiveMap);
  }
}
