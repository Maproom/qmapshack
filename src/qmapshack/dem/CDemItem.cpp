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

#include "dem/CDemItem.h"

#include <QtWidgets>

#include "dem/CDemDraw.h"
#include "dem/CDemVRT.h"
#include "dem/CDemWCS.h"
#include "dem/IDemProp.h"
#include "misc.h"

QRecursiveMutex CDemItem::mutexActiveDems;

CDemItem::CDemItem(CDemDraw* dem) : dem(dem) {
  setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

  itemWidget();
}

CDemItem::~CDemItem() {}

QWidget* CDemItem::itemWidget() {
  if (widget.isNull()) {
    widget = new CMapItemWidget(tr("DEM"));
    QFileInfo fi(filename);
    setName(fi.completeBaseName().replace("_", " "));

    if (QFile::exists(filename)) {
      if (noShadowConfig()) {
        setStatus(CMapItemWidget::eStatus::Unused);
      } else {
        setStatus(demfile.isNull() ? CMapItemWidget::eStatus::Inactive : CMapItemWidget::eStatus::Active);
      }
    } else {
      setStatus(CMapItemWidget::eStatus::Missing);
    }
    widget->setDrawObject(demfile, dem->getScale());

    connect(widget, &CMapItemWidget::sigActivate, this, &CDemItem::slotActivate);
    connect(widget, &CMapItemWidget::destroyed, this, [this] { emit sigUpdateWidget(this); });
    connect(dem, &CDemDraw::sigScaleChanged, widget, &CMapItemWidget::slotScaleChanged);
  }
  return widget;
}

void CDemItem::slotActivate(bool yes) {
  if (yes) {
    activate();
  } else {
    deactivate();
  }

  emit sigChanged();
}

void CDemItem::setName(const QString& name) {
  widget->setName(name);
  setText(0, name);
}

void CDemItem::setStatus(CMapItemWidget::eStatus status) { widget->setStatus(status); }

void CDemItem::setFilename(const QString& name, const QString& fallbackKey) {
  filename = name;

  QFile f(filename);
  if (f.exists() && f.open(QIODevice::ReadOnly)) {
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(f.read(qMin(1024, f.size())));
    key = md5.result().toHex();
    f.close();
  } else {
    key = fallbackKey;
  }
}

void CDemItem::configToShadowConfig(const QSettings& cfg) {
  shadowConfig.clear();
  const QStringList& keys = cfg.childKeys();
  for (const QString& key : keys) {
    shadowConfig[key] = cfg.value(key);
  }
}

void CDemItem::shadowConfigToConfig(QSettings& cfg) const {
  const QStringList& keys = shadowConfig.keys();
  for (const QString& key : keys) {
    cfg.setValue(key, shadowConfig[key]);
  }
}

void CDemItem::saveConfig(QSettings& cfg) const {
  cfg.beginGroup(key);
  if (demfile.isNull()) {
    shadowConfigToConfig(cfg);
  } else {
    demfile->saveConfig(cfg);
    cfg.setValue("isActive", true);
  }
  cfg.setValue("filename", filename);
  cfg.endGroup();
}

void CDemItem::loadConfig(QSettings& cfg, bool triggerActivation) {
  if (!demfile.isNull()) {
    // dem is already active, read the config
    cfg.beginGroup(key);
    configToShadowConfig(cfg);
    demfile->loadConfig(cfg);
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
      // Evil hack: If you activate the DEM directly Qt will crash internally.
      QPointer<CDemItem> self(this);
      QTimer::singleShot(100, this, [self, active]() {
        if (!self.isNull()) self->slotActivate(active);
      });
    }
  }
}

void CDemItem::showChildren(bool yes) {
  if (demfile.isNull()) {
    return;
  }
  if (yes) {
    QTreeWidget* tw = treeWidget();

    QTreeWidgetItem* item = new QTreeWidgetItem(this);
    item->setFlags(Qt::ItemIsEnabled);
    tw->setItemWidget(item, 0, demfile->getSetup());
  } else {
    QList<QTreeWidgetItem*> items = takeChildren();
    qDeleteAll(items);
    delete demfile->getSetup();
  }
}

void CDemItem::updateIcon() {
  if (filename.isEmpty()) {
    return;
  }

  QPixmap img("://icons/32x32/Map.png");
  QFileInfo fi(filename);
  if (fi.suffix().toLower() == "vrt") {
    img = QPixmap("://icons/32x32/MimeDemVRT.png");
  } else if (fi.suffix().toLower() == "wcs") {
    img = QPixmap("://icons/32x32/MimeDemWCS.png");
  }

  setIcon(0, QIcon(img));
}

bool CDemItem::isActivated() {
  QMutexLocker lock(&mutexActiveDems);
  return !demfile.isNull();
}

void CDemItem::deactivate() {
  QMutexLocker lock(&mutexActiveDems);

  if (demfile.isNull()) {
    return;
  }
  // save current configuration into
  // the shadow configuration
  QTemporaryFile file;
  QSettings cfg(file.fileName(), QSettings::IniFormat);
  demfile->saveConfig(cfg);
  configToShadowConfig(cfg);

  // remove demfile setup dialog as child of this item
  showChildren(false);

  // remove demfile object
  delete demfile;

  // maybe used to reflect changes in the icon
  updateIcon();

  setStatus(CMapItemWidget::eStatus::Inactive);
}

bool CDemItem::activate() {
  QMutexLocker lock(&mutexActiveDems);

  delete demfile;

  // load DEM by suffix
  QFileInfo fi(filename);
  if (fi.suffix().toLower() == "vrt") {
    demfile = new CDemVRT(filename, dem);
  } else if (fi.suffix().toLower() == "wcs") {
    demfile = new CDemWCS(filename, dem);
  }

  updateIcon();

  // no demfile loaded? Bad.
  if (demfile.isNull()) {
    setStatus(CMapItemWidget::eStatus::Inactive);
    return false;
  }

  // if DEM is activated successfully add to the list of DEM files
  // else delete all previous loaded DEMs and abort
  if (!demfile->activated()) {
    delete demfile;
    setStatus(CMapItemWidget::eStatus::Inactive);
    return false;
  }

  // setToolTip(0, demfile->getCopyright());

  // setup DEM with settings stored in
  // the shadow config
  QTemporaryFile file;
  QSettings cfg(file.fileName(), QSettings::IniFormat);

  shadowConfigToConfig(cfg);
  demfile->loadConfig(cfg);
  // On first activation the shadow config is empty. (state *new*)
  // Therfore always read back the config and store it as new
  // shadow config
  demfile->saveConfig(cfg);
  configToShadowConfig(cfg);

  // Add the demfile setup dialog as child of this item
  showChildren(true);

  widget->setDrawObject(demfile, dem->getScale());
  setStatus(CMapItemWidget::eStatus::Active);
  return true;
}

void CDemItem::setAccess(const QString& ele) {
  if (!widget.isNull()) {
    widget->setAccess(ele);
  }
}

void CDemItem::setProcessing(bool on) {
  if (!widget.isNull()) {
    widget->setProcessing(on);
  }
}
