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

#include "map/CMapItem.h"

#include <QtGui>
#include <QtWidgets>

#include "map/CMapDraw.h"
#include "map/CMapGEMF.h"
#include "map/CMapIMG.h"
#include "map/CMapJNX.h"
#include "map/CMapMAP.h"
#include "map/CMapRMAP.h"
#include "map/CMapTMS.h"
#include "map/CMapVRT.h"
#include "map/CMapWMTS.h"
#include "map/IMapProp.h"

QRecursiveMutex CMapItem::mutexActiveMaps;

CMapItem::CMapItem(CMapDraw* map) : map(map) {
  setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

  itemWidget();
}

CMapItem::~CMapItem() {}

QWidget* CMapItem::itemWidget() {
  if (widget.isNull()) {
    widget = new CMapItemWidget(tr("map"));
    QFileInfo fi(filename);
    setName(fi.completeBaseName().replace("_", " "));

    if (QFile::exists(filename)) {
      if (noShadowConfig()) {
        setStatus(CMapItemWidget::eStatus::Unused);
      } else {
        setStatus(mapfile.isNull() ? CMapItemWidget::eStatus::Inactive : CMapItemWidget::eStatus::Active);
      }
    } else {
      setStatus(CMapItemWidget::eStatus::Missing);
    }

    connect(widget, &CMapItemWidget::sigActivate, this, &CMapItem::slotActivate);
    connect(widget, &CMapItemWidget::destroyed, this, [this] { emit sigUpdateWidget(this); });
    connect(map, &CMapDraw::sigScaleChanged, widget, &CMapItemWidget::slotScaleChanged);
  }
  return widget;
}

void CMapItem::slotActivate(bool yes) {
  if (yes) {
    activate();
  } else {
    deactivate();
  }

  emit sigChanged();
}

void CMapItem::setName(const QString& name) { widget->setName(name); }

void CMapItem::setStatus(CMapItemWidget::eStatus status) { widget->setStatus(status); }

void CMapItem::setFilename(const QString& name, const QString& fallbackKey) {
  filename = name;

  QFile f(filename);
  if (f.exists() && f.open(QIODevice::ReadOnly)) {
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(f.read(qMin(0x1000LL, f.size())));
    key = md5.result().toHex();
    f.close();
  } else {
    key = fallbackKey;
  }
}

void CMapItem::configToShadowConfig(const QSettings& cfg) {
  shadowConfig.clear();
  const QStringList& keys = cfg.childKeys();
  for (const QString& key : keys) {
    shadowConfig[key] = cfg.value(key);
  }
}

void CMapItem::shadowConfigToConfig(QSettings& cfg) const {
  const QStringList& keys = shadowConfig.keys();
  for (const QString& key : keys) {
    cfg.setValue(key, shadowConfig[key]);
  }
}

void CMapItem::saveConfig(QSettings& cfg) const {
  cfg.beginGroup(key);
  if (mapfile.isNull()) {
    shadowConfigToConfig(cfg);
  } else {
    mapfile->saveConfig(cfg);
    cfg.setValue("isActive", true);
  }
  cfg.setValue("filename", filename);
  cfg.endGroup();  // key
}

void CMapItem::loadConfig(QSettings& cfg, bool triggerActivation) {
  if (!mapfile.isNull()) {
    // map is already active, read the config
    cfg.beginGroup(key);
    configToShadowConfig(cfg);
    mapfile->loadConfig(cfg);
    cfg.endGroup();
    return;
  }

  // let's see what we can do...
  cfg.beginGroup(key);
  bool active = cfg.value("isActive", false).toBool();
  configToShadowConfig(cfg);
  cfg.endGroup();

  if (!QFile::exists(filename)) {
    setStatus(CMapItemWidget::eStatus::Missing);
    return;
  } else if (noShadowConfig()) {
    setStatus(CMapItemWidget::eStatus::Unused);
  } else {
    setStatus(CMapItemWidget::eStatus::Inactive);
    if (triggerActivation) {
      // Evil hack: If you activate the map directly Qt will crash internally.
      QPointer<CMapItem> self(this);
      QTimer::singleShot(100, this, [self, active]() {
        if (!self.isNull()) self->slotActivate(active);
      });
    }
  }
}

void CMapItem::showChildren(bool yes) {
  if (mapfile.isNull()) {
    return;
  }
  if (yes) {
    QTreeWidget* tw = treeWidget();

    QTreeWidgetItem* item = new QTreeWidgetItem(this);
    item->setFlags(Qt::ItemIsEnabled);
    tw->setItemWidget(item, 0, mapfile->getSetup());
  } else {
    QList<QTreeWidgetItem*> items = takeChildren();
    qDeleteAll(items);
    delete mapfile->getSetup();
  }
}

void CMapItem::updateIcon() {
  if (filename.isEmpty()) {
    return;
  }

  static QHash<QString, QString> icons{
      {"rmap", "://icons/32x32/MimeRMAP.png"}, {"jnx", "://icons/32x32/MimeJNX.png"},
      {"vrt", "://icons/32x32/MimeVRT.png"},   {"img", "://icons/32x32/MimeIMG.png"},
      {"map", "://icons/32x32/MimeMAP.png"},   {"wmts", "://icons/32x32/MimeWMTS.png"},
      {"tms", "://icons/32x32/MimeTMS.png"},   {"gemf", "://icons/32x32/MimeGEMF.png"}};

  const QString& suffix = QFileInfo(filename).suffix().toLower();
  QPixmap img(icons.contains(suffix) ? icons[suffix] : "://icons/32x32/Map.png");

  setIcon(/* col */ 0, QIcon(img));
}

bool CMapItem::isActivated() const {
  QMutexLocker lock(&mutexActiveMaps);
  return !mapfile.isNull();
}

void CMapItem::deactivate() {
  QMutexLocker lock(&mutexActiveMaps);

  if (mapfile.isNull()) {
    return;
  }
  // save current configuration into
  // the shadow configuration
  QTemporaryFile file;
  QSettings cfg(file.fileName(), QSettings::IniFormat);
  mapfile->saveConfig(cfg);
  configToShadowConfig(cfg);

  // remove mapfile setup dialog as child of this item
  showChildren(false);

  // remove mapfile object
  delete mapfile;

  // maybe used to reflect changes in the icon
  updateIcon();

  map->reportStatusToCanvas(text(0), "");

  setStatus(CMapItemWidget::eStatus::Inactive);
}

bool CMapItem::activate() {
  QMutexLocker lock(&mutexActiveMaps);

  delete mapfile;

  // load map by suffix
  QFileInfo fi(filename);
  if (fi.suffix().toLower() == "rmap") {
    mapfile = new CMapRMAP(filename, map);
  } else if (fi.suffix().toLower() == "jnx") {
    mapfile = new CMapJNX(filename, map);
  } else if (fi.suffix().toLower() == "img") {
    mapfile = new CMapIMG(filename, map);
  } else if (fi.suffix().toLower() == "vrt") {
    mapfile = new CMapVRT(filename, map);
  } else if (fi.suffix().toLower() == "map") {
    mapfile = new CMapMAP(filename, map);
  } else if (fi.suffix().toLower() == "wmts") {
    mapfile = new CMapWMTS(filename, map);
  } else if (fi.suffix().toLower() == "tms") {
    mapfile = new CMapTMS(filename, map);
  } else if (fi.suffix().toLower() == "gemf") {
    mapfile = new CMapGEMF(filename, map);
  }

  updateIcon();

  // no mapfiles loaded? Bad.
  if (mapfile.isNull()) {
    setStatus(CMapItemWidget::eStatus::Inactive);
    return false;
  }

  // if map is activated successfully add to the list of map files
  // else delete all previous loaded maps and abort
  if (!mapfile->activated()) {
    delete mapfile;
    setStatus(CMapItemWidget::eStatus::Inactive);
    return false;
  }

  setToolTip(0, mapfile->getCopyright());

  // setup map with settings stored in
  // the shadow config
  QTemporaryFile file;
  QSettings cfg(file.fileName(), QSettings::IniFormat);

  shadowConfigToConfig(cfg);
  mapfile->loadConfig(cfg);
  // On first activation the shadow config is empty. (state *new*)
  // Therfore always read back the config and store it as new
  // shadow config
  mapfile->saveConfig(cfg);
  configToShadowConfig(cfg);

  // Add the mapfile setup dialog as child of this item
  showChildren(true);

  widget->setDrawObject(mapfile, map->getScale());
  setStatus(CMapItemWidget::eStatus::Active);
  return true;
}
