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
#include "helpers/CSettings.h"
#include "map/CMapItem.h"
#include "map/CMapList.h"
#include "map/CMapPathSetup.h"
#include "map/IMap.h"
#include "map/cache/CDiskCache.h"
#include "misc.h"
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
  connect(mapList, &CMapList::sigChanged, this, &CMapDraw::slotChanged);
  maps << this;

  timerDelayedSave = new QTimer(this);
  timerDelayedSave->setInterval(1000);
  timerDelayedSave->setSingleShot(true);
  timerDelayedSave->callOnTimeout([this]() { saveMapList(); });
}

CMapDraw::~CMapDraw() { maps.removeOne(this); }

void CMapDraw::slotChanged() {
  emitSigCanvasUpdate();
  mapList->updateHelpText();
  timerDelayedSave->start();
}

bool CMapDraw::setProjection(const QString& proj) /* override */
{
  // --- set the new projection
  bool res = IDrawContext::setProjection(proj);
  // --- now build the map list from scratch. This will deactivate -> activate all maps
  //     By that everything is restored with the new projection
  if (mapList->count()) {
    loadMapList();
  }
  return res;
}

void CMapDraw::setupMapPath() {
  qDebug() << "CMapDraw::setupMapPath()";
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
  qDebug() << "CMapDraw::setupMapPath(path)";
  QStringList paths(mapPaths);
  if (!mapPaths.contains(path)) {
    paths << path;
  }
  setupMapPath(paths);
}

void CMapDraw::setupMapPath(const QStringList& paths) {
  qDebug() << "CMapDraw::setupMapPath(paths)";
  mapPaths = paths;

  for (CMapDraw* map : std::as_const(maps)) {
    map->saveMapList();
    map->loadMapList();
  }
}

void CMapDraw::saveMapPath(QSettings& cfg) {
  qDebug() << "CMapDraw::saveMapPath(cfg)";
  cfg.setValue("mapPath", mapPaths);
  cfg.setValue("cachePath", cachePath);
}

void CMapDraw::loadMapPath(QSettings& cfg) {
  qDebug() << "CMapDraw::loadMapPath(cfg)";
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

      if (!item || !item->isActivated()) {
        continue;
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

      if (!item || !item->isActivated()) {
        continue;
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

      if (!item || !item->isActivated()) {
        continue;
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

      if (!item || !item->isActivated()) {
        continue;
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

void CMapDraw::buildMapList(const QString& filename) {
  qDebug() << "CMapDraw::buildMapList(filename)";
  QMutexLocker lock(&CMapItem::mutexActiveMaps);
  mapList->clear();

  CMapItem* item = createMapItem(filename, "");
  mapList->addMap(item);
  item->activate();
  canvas->zoom(30);
}

void CMapDraw::saveConfig(QSettings& cfg) /* override */
{
  qDebug() << "CMapDraw::saveConfig(cfg)";
  // -------------------
  saveMapList(cfg);
  cfg.beginGroup("map2");
  cfg.setValue("zoomIndex", zoomIndex);
  cfg.endGroup();
}

void CMapDraw::loadConfig(QSettings& cfg) /* override */
{
  qDebug() << "CMapDraw::loadConfig(cfg)";
  // -------------------
  loadMapList(cfg);
  cfg.beginGroup("map2");
  int idx = cfg.value("zoomIndex", zoomIndex).toInt();
  cfg.endGroup();

  zoom(idx);
}

CMapItem* CMapDraw::createMapItem(const QString& filename, const QString& fallbackKey) {
  CMapItem* item = new CMapItem(this);

  QFileInfo fi(filename);
  item->setName(fi.completeBaseName().replace("_", " "));
  item->setFilename(filename, fallbackKey);
  item->updateIcon();
  return item;
}

void CMapDraw::loadMapList() {
  qDebug() << "CMapDraw::loadMapList()";
  SETTINGS;
  cfg.beginGroup("Canvas");
  cfg.beginGroup("Views");
  cfg.beginGroup(canvas->objectName());
  loadMapList(cfg);
  cfg.endGroup();
  cfg.endGroup();
  cfg.endGroup();
}

void CMapDraw::loadMapList(QSettings& cfg) {
  qDebug() << "CMapDraw::loadMapList(cfg)";
  QMutexLocker lock(&CMapItem::mutexActiveMaps);
  mapList->clear();

  // create a temp list of all maps in the paths
  QMap<QString, CMapItem*> mapsFound;
  for (const QString& path : std::as_const(mapPaths)) {
    QDir dir(path);

    // find available maps
    const QStringList& filenames = dir.entryList(supportedFormats, QDir::Files | QDir::Readable, QDir::Name);
    for (const QString& filename : filenames) {
      CMapItem* item = createMapItem(dir.absoluteFilePath(filename), "");
      mapsFound[item->getKey()] = item;
    }
  }

  cfg.beginGroup("map2");

  // collect map file names used to clean up the map cache later
  QSet<QString> maps;
  // create a list of maps in the same order as the stored keys
  const QStringList& keysKnownMaps = cfg.value("keysKnownMaps", {}).toStringList();
  for (const QString& key : keysKnownMaps) {
    CMapItem* map = mapsFound.take(key);

    // We have a key but no map found. Either the map is missing or
    // the map has a new hash due to changes -> create a new map item to investigate
    if (map == nullptr) {
      cfg.beginGroup(key);
      const QString& filename = cfg.value("filename", "").toString();
      map = createMapItem(filename, key);
      cfg.endGroup();
    }

    // If the map content changed the derived key will not match the one
    // we expect. Delete this map item, as it will have a reincarnation
    // as new map.
    // If the map path is not part of the map paths list also delete
    // the file as it is no longer part of the used maps.
    QFileInfo fi(map->getFilename());
    if (key != map->getKey() || !mapPaths.contains(fi.absolutePath())) {
      delete map;
      continue;
    }

    // finally load the config and add the item to the list
    map->loadConfig(cfg, true);

    maps << fi.completeBaseName();
    mapList->addMap(map);
  }

  cfg.endGroup();  // map2

  // let's see what's left over in the found maps list. These are all new
  // map items.
  QList<CMapItem*> mapsUnknown = mapsFound.values();
  std::sort(mapsUnknown.begin(), mapsUnknown.end(), &sortByName<CMapItem>);
  for (CMapItem* map : mapsUnknown) {
    map->setStatus(CMapItemWidget::eStatus::Unused);

    QFileInfo fi(map->getFilename());
    maps << fi.completeBaseName();
    mapList->addMap(map);
  }

  // ------------- stat --------------
  // Apply configuration data from an older version.
  if (cfg.childGroups().contains("map")) {
    cfg.beginGroup("map");
    QStringList active = cfg.value("active", {}).toStringList();
    std::reverse(active.begin(), active.end());
    for (const QString& key : active) {
      const int N = mapList->count();
      for (int n = 0; n < N; n++) {
        CMapItem* item = mapList->item(n);
        if (item && item->getKey() == key) {
          item->loadConfig(cfg, false);
          mapList->moveMapToTop(item);
          item->activate();
          break;
        }
      }
    }
    cfg.endGroup();

  }  // ------------- stop --------------

  CDiskCache::cleanupRemovedMaps(maps);
  mapList->updateHelpText();
}

void CMapDraw::saveMapList() {
  qDebug() << "CMapDraw::saveMapList()";
  SETTINGS;
  cfg.beginGroup("Canvas");
  cfg.beginGroup("Views");
  cfg.beginGroup(canvas->objectName());
  saveMapList(cfg);
  cfg.endGroup();
  cfg.endGroup();
  cfg.endGroup();
}

void CMapDraw::saveMapList(QSettings& cfg) {
  qDebug() << "CMapDraw::saveMapList(cfg)";
  QMutexLocker lock(&CMapItem::mutexActiveMaps);

  cfg.beginGroup("map2");

  QStringList keys;
  for (int i = 0; i < mapList->count(); i++) {
    CMapItem* item = mapList->item(i);
    item->saveConfig(cfg);
    keys << item->getKey();
  }
  cfg.setValue("keysKnownMaps", keys);
  cfg.endGroup();  // map2
}

void CMapDraw::reportStatusToCanvas(const QString& key, const QString& msg) {
  if (canvas) {
    canvas->reportStatus(key, msg);
  }
}

void CMapDraw::drawt(IDrawContext::buffer_t& currentBuffer) /* override */
{
  bool seenActiveMap = false;
  // iterate over all active maps and call the draw method
  CMapItem::mutexActiveMaps.lock();
  if (mapList && (mapList->count() != 0)) {
    for (int i = 0; i < mapList->count(); i++) {
      CMapItem* item = mapList->item(i);

      if (!item || !item->isActivated()) {
        continue;
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
