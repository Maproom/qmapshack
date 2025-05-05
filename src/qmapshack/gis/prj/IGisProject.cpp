/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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

#include "gis/prj/IGisProject.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "device/CDeviceGarminMtp.h"
#include "device/IDevice.h"
#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/IGisItem.h"
#include "gis/fit/CFitProject.h"
#include "gis/fit2/CFit2Project.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/CDetailsPrj.h"
#include "gis/qlb/CQlbProject.h"
#include "gis/qms/CQmsProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/slf/CSlfProject.h"
#include "gis/suunto/CLogProject.h"
#include "gis/suunto/CSmlProject.h"
#include "gis/tcx/CTcxProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSelectCopyAction.h"
#include "helpers/CSettings.h"
#include "misc.h"

const QString IGisProject::filedialogAllSupported =
    "All Supported (*.gpx *.GPX *.tcx *.TCX *.sml *.log *.qms *.qlb *.slf *.fit)";
const QString IGisProject::filedialogFilterGPX = "GPS Exchange Format (*.gpx *.GPX)";
const QString IGisProject::filedialogFilterTCX = "TCX Garmin Proprietary (*.tcx *.TCX)";
const QString IGisProject::filedialogFilterSML = "Suunto XML format (*.sml)";
const QString IGisProject::filedialogFilterLOG = "Openambit XML format (*.log)";
const QString IGisProject::filedialogFilterQLB = "QLandkarte Binary (*.qlb)";
const QString IGisProject::filedialogFilterQMS = "QMapShack Binary (*.qms)";
const QString IGisProject::filedialogFilterSLF = "Sigma Log Format (*.slf)";
const QString IGisProject::filedialogFilterFIT = "Garmin FIT Format (*.fit)";
const QString IGisProject::filedialogSaveFilters =
    filedialogFilterGPX + ";; " + filedialogFilterQLB + ";; " + filedialogFilterQMS + ";; " + filedialogFilterTCX;
const QString IGisProject::filedialogLoadFilters =
    filedialogAllSupported + ";; " + filedialogFilterGPX + ";; " + filedialogFilterTCX + ";; " + filedialogFilterSML +
    ";; " + filedialogFilterLOG + ";; " + filedialogFilterQLB + ";; " + filedialogFilterQMS + ";; " +
    filedialogFilterSLF + ";; " + filedialogFilterFIT;

QString IGisProject::keyUserFocus;

IGisProject::IGisProject(type_e type, const QString& filename, CGisListWks* parent)
    : QTreeWidgetItem(parent), type(type), filename(filename) {
  memset(cntItemsByType, 0, sizeof(cntItemsByType));
  setCheckState(CGisListWks::eColumnCheckBox, Qt::Checked);

  if (parent) {
    // move project up the list until there a re only projects, no devices
    int newIdx = NOIDX;
    const int myIdx = parent->topLevelItemCount() - 1;
    for (int i = myIdx - 1; i >= 0; i--) {
      IDevice* device = dynamic_cast<IDevice*>(parent->topLevelItem(i));
      if (device != nullptr) {
        newIdx = i;
        continue;
      }
      break;
    }

    if (newIdx != NOIDX) {
      parent->takeTopLevelItem(myIdx);
      parent->insertTopLevelItem(newIdx, this);
    }
  }
}

IGisProject::IGisProject(type_e type, const QString& filename, IDevice* parent)
    : QTreeWidgetItem(parent), type(type), filename(filename) {
  memset(cntItemsByType, 0, sizeof(cntItemsByType));
  setCheckState(CGisListWks::eColumnCheckBox, Qt::Checked);
  nameSuffix = parent->getName();
}

IGisProject::~IGisProject() {
  delete dlgDetails;
  if (key == keyUserFocus) {
    keyUserFocus.clear();
  }
}

IGisProject* IGisProject::create(const QString filename, CGisListWks* parent) {
  IGisProject* item = nullptr;
  QString suffix = QFileInfo(filename).suffix().toLower();
  if (suffix == "gpx") {
    item = new CGpxProject(filename, parent);
  } else if (suffix == "qms") {
    item = new CQmsProject(filename, parent);
  } else if (suffix == "slf") {
    item = new CSlfProject(filename);

    // the CSlfProject does not - as the other C*Project - register itself in the list
    // of currently opened projects. This is done manually here.
    if (parent) {
      parent->addProject(item);
    }
  } else if (suffix == "fit") {
    item = new CFit2Project(filename, parent);
  } else if (suffix == "tcx") {
    item = new CTcxProject(filename, parent);
  } else if (suffix == "sml") {
    item = new CSmlProject(filename, parent);
  } else if (suffix == "log") {
    item = new CLogProject(filename, parent);
  } else if (suffix == "qlb") {
    item = new CQlbProject(filename, parent);
  }

  if (item && !item->isValid()) {
    delete item;
    item = nullptr;
  }

  return item;
}

QString IGisProject::html2Dev(const QString& str) {
  return isOnDevice() == IDevice::eTypeGarmin ? IGisItem::removeHtml(str) : str;
}

bool IGisProject::askBeforClose() {
  int res = QMessageBox::Ok;
  if (isChanged()) {
    {
      CCanvasCursorLock cursorLock(Qt::ArrowCursor, __func__);
      res = QMessageBox::question(CMainWindow::getBestWidgetForParent(), tr("Save project?"),
                                  tr("<h3>%1</h3>The project was changed. Save before closing it?").arg(getName()),
                                  QMessageBox::Save | QMessageBox::No | QMessageBox::Abort, QMessageBox::No);
    }

    if (res == QMessageBox::Save) {
      // some project cannot be saved
      if (canSave()) {
        save();
      } else {
        saveAs();
      }
    }
  }

  return res == QMessageBox::Abort;
}

bool IGisProject::isVisible() const { return checkState(CGisListWks::eColumnCheckBox) == Qt::Checked; }

void IGisProject::genKey() const {
  if (key.isEmpty()) {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_5_2);

    *this >> stream;

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(buffer);
    key = md5.result().toHex();
  }
}

QString IGisProject::getDeviceKey() const {
  IDevice* device = dynamic_cast<IDevice*>(parent());
  if (device) {
    return device->getKey();
  }

  return "";
}

QPixmap IGisProject::getIcon() const { return icon(CGisListWks::eColumnIcon).pixmap(22, 22); }

qint32 IGisProject::isOnDevice() const {
  IDevice* device = dynamic_cast<IDevice*>(parent());
  return device != nullptr ? device->type() : IDevice::eTypeNone;
}

bool IGisProject::isChanged() const { return text(CGisListWks::eColumnDecoration).contains("*"); }

void IGisProject::edit() {
  if (dlgDetails.isNull()) {
    dlgDetails = new CDetailsPrj(*this, 0);
    dlgDetails->setObjectName(getName());
  }

  CMainWindow::self().addWidgetToTab(dlgDetails);
}

void IGisProject::setName(const QString& str) {
  metadata.name = str;
  setText(CGisListWks::eColumnName, getNameEx());
  setChanged();
}

void IGisProject::setKeywords(const QString& str) {
  metadata.keywords = str;
  setChanged();
}

void IGisProject::setDescription(const QString& str) {
  metadata.desc = str;
  setChanged();
}

void IGisProject::setLinks(const QList<IGisItem::link_t>& links) {
  metadata.links = links;
  setChanged();
}

void IGisProject::setSortingRoadbook(sorting_roadbook_e s) {
  changedRoadbookMode = (s != sortingRoadbook);
  sortingRoadbook = s;
  if (changedRoadbookMode) {
    setChanged();
  }
}

void IGisProject::setSortingFolder(sorting_folder_e s) {
  bool changed = (s != sortingFolder);
  sortingFolder = s;
  sortItems();

  if (changed) {
    setChanged();
    if (dlgDetails != nullptr) {
      dlgDetails->updateData();
    }
  }
}

void IGisProject::setChanged() {
  if (autoSave) {
    if (!autoSavePending) {
      autoSavePending = true;
      CGisWorkspace::self().postEventForWks(new CEvtA2WSave(getKey()));
    }
  }

  if (autoSyncToDev) {
    if (!autoSyncToDevPending) {
      autoSyncToDevPending = true;
      CGisWorkspace::self().postEventForWks(new CEvtA2WSync(getKey()));
    }
  }
  updateDecoration(false);
  updateItems();
}

void IGisProject::setAutoSave(bool on) {
  // make sure project is saved one more time to remove autoSave flag in storage
  if (!on && autoSave) {
    CGisWorkspace::self().postEventForWks(new CEvtA2WSave(getKey()));
  }

  autoSave = on;
  setChanged();
}

void IGisProject::setAutoSyncToDevice(bool yes) {
  autoSyncToDev = yes;
  updateDecoration();
}

void IGisProject::switchOnCorrelation() {
  noCorrelation = false;
  hashTrkWpt[0].clear();
  hashTrkWpt[1].clear();
  updateItems();
}

void IGisProject::updateItems() {
  if (noUpdate) {
    return;
  }

  sortItems();
  updateItemCounters();

  if (noCorrelation) {
    return;
  }

  if (!changedRoadbookMode) {
    if ((hashTrkWpt[0] == hashTrkWpt[1]) || (getItemCountByType(IGisItem::eTypeTrk) == 0)) {
      return;
    }
  }
  changedRoadbookMode = false;

  quint32 total = cntTrkPts * cntWpts;
  quint32 current = 0;

  PROGRESS_SETUP(tr("%1: Correlate tracks and waypoints.").arg(getName()), 0, total,
                 CMainWindow::getBestWidgetForParent());

  for (int i = 0; i < childCount(); i++) {
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(child(i));
    if (trk) {
      trk->findWaypointsCloseBy(progress, current);
      if (progress.wasCanceled()) {
        QString msg = tr("<h3>%1</h3>Did that take too long for you? Do you want to skip correlation of tracks and "
                         "waypoints for this project in the future?")
                          .arg(getNameEx());
        int res = QMessageBox::question(&progress, tr("Canceled correlation..."), msg,
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        noCorrelation = res == QMessageBox::Yes;
        break;
      }
    }
  }

  if (dlgDetails != nullptr) {
    dlgDetails->updateData();
  }
}

bool IGisProject::save() {
  if (!canSave()) {
    qWarning() << "This should never be called!";
    return false;
  }

  return saveAs(filename, getFileDialogFilter());
}

bool IGisProject::saveAs(QString fn, QString filter) {
  SETTINGS;

  if (fn.isEmpty()) {
    QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

    // guess the correct extension:
    // by default use the extension provided by the current format,
    // otherwise use gpx
    QString ext = getFileExtension();
    filter = getFileDialogFilter();
    if (ext.isEmpty() || !canSave()) {
      ext = "gpx";
      filter = IGisProject::filedialogFilterGPX;
    }
    path += "/" + getName() + "." + ext;

    fn = QFileDialog::getSaveFileName(CMainWindow::getBestWidgetForParent(), tr("Save \"%1\" to...").arg(getName()),
                                      path, filedialogSaveFilters, &filter);

    if (fn.isEmpty()) {
      return false;
    }
  }

  bool res = false;
  if (filter == getFileDialogFilter()) {
    filename = fn;
    setupName(QFileInfo(fn).completeBaseName());
  }

  if (filter == filedialogFilterGPX) {
    res = CGpxProject::saveAs(fn, *this, false);
  } else if (filter == filedialogFilterQMS) {
    res = CQmsProject::saveAs(fn, *this);
  } else if (filter == filedialogFilterTCX) {
    res = CTcxProject::saveAs(fn, *this);
  } else {
    return false;
  }

  if (res && filter == getFileDialogFilter()) {
    markAsSaved();
  }

  QString path = QFileInfo(fn).absolutePath();
  cfg.setValue("Paths/lastGisPath", path);

  return res;
}

bool IGisProject::saveAsStrictGpx11() {
  SETTINGS;

  QString fn;
  QString path = cfg.value("Paths/lastGisPath", QDir::homePath()).toString();

  // guess the correct extension:
  // by default use the extension provided by the current format,
  // otherwise use gpx
  QString ext = "gpx";
  QString filter = IGisProject::filedialogFilterGPX;
  path += "/" + getName() + "." + ext;

  fn = QFileDialog::getSaveFileName(CMainWindow::getBestWidgetForParent(), tr("Save \"%1\" to...").arg(getName()), path,
                                    "Strict GPX V 1.1 (*.gpx *.GPX)", &filter);

  if (fn.isEmpty()) {
    return false;
  }

  bool res = CGpxProject::saveAs(fn, *this, true);

  path = QFileInfo(fn).absolutePath();
  cfg.setValue("Paths/lastGisPath", path);

  return res;
}

void IGisProject::setupName(const QString& defaultName) {
  if (metadata.name.isEmpty()) {
    metadata.name = defaultName;
  }
  setText(CGisListWks::eColumnName, getName());
}

void IGisProject::markAsSaved() {
  updateDecoration(true);
  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item) {
      continue;
    }
    item->updateDecoration(IGisItem::eMarkNone, IGisItem::eMarkChanged);
  }
}

QString IGisProject::getName() const { return metadata.name; }

QString IGisProject::getNameEx() const {
  if (nameSuffix.isEmpty()) {
    return metadata.name;
  } else {
    return metadata.name + " @ " + nameSuffix;
  }
}

QString IGisProject::getInfo() const {
  QString str = metadata.name.isEmpty() ? text(CGisListWks::eColumnName) : metadata.name;
  str = "<div style='font-weight: bold;'>" + str + "</div>";

  if (metadata.time.isValid()) {
    str += "<br/>\n";
    str += IUnit::datetime2string(metadata.time, IUnit::eTimeFormatLong);
  }

  QString desc = IGisItem::removeHtml(metadata.desc).simplified();
  if (!desc.isEmpty()) {
    str += "<br/>\n";

    if (desc.length() < 100) {
      str += desc;
    } else {
      str += desc.left(97) + "...";
    }
  }

  if (!filename.isEmpty()) {
    str += tr("<br/>\nFilename: %1").arg(filename);
  }

  if (cntItemsByType[IGisItem::eTypeWpt]) {
    str += "<br/>\n" + tr("Waypoints: %1").arg(cntItemsByType[IGisItem::eTypeWpt]);
  }
  if (cntItemsByType[IGisItem::eTypeTrk]) {
    str += "<br/>\n" + tr("Tracks: %1").arg(cntItemsByType[IGisItem::eTypeTrk]);
  }
  if (cntItemsByType[IGisItem::eTypeRte]) {
    str += "<br/>\n" + tr("Routes: %1").arg(cntItemsByType[IGisItem::eTypeRte]);
  }
  if (cntItemsByType[IGisItem::eTypeOvl]) {
    str += "<br/>\n" + tr("Areas: %1").arg(cntItemsByType[IGisItem::eTypeOvl]);
  }

  return str;
}

IGisItem* IGisProject::getItemByKey(const IGisItem::key_t& key) {
  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item) {
      continue;
    }

    if (item->getKey() == key) {
      return item;
    }
  }
  return nullptr;
}

void IGisProject::getItemsByKeys(const QList<IGisItem::key_t>& keys, QList<IGisItem*>& items) {
  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item) {
      continue;
    }

    if (keys.contains(item->getKey())) {
      items << item;
    }
  }
}

void IGisProject::getItemsByPos(const QPointF& pos, QList<IGisItem*>& items) {
  if (!isVisible()) {
    return;
  }

  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item || item->isHidden()) {
      continue;
    }

    if (item->isCloseTo(pos)) {
      items << item;
    }
  }
}

void IGisProject::getItemsByArea(const QRectF& area, IGisItem::selflags_t flags, QList<IGisItem*>& items) {
  if (!isVisible()) {
    return;
  }

  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item || item->isHidden()) {
      continue;
    }

    if (item->isWithin(area, flags)) {
      items << item;
    }
  }
}

void IGisProject::getNogoAreas(QList<IGisItem*>& nogos) const {
  if (!isVisible()) {
    return;
  }

  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (item != nullptr && !item->isHidden() && item->isNogo()) {
      nogos << item;
    }
  }
}

void IGisProject::mouseMove(const QPointF& pos) {
  if (!isVisible()) {
    return;
  }

  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item || item->isHidden()) {
      continue;
    }

    item->mouseMove(pos);
  }
}

bool IGisProject::delItemByKey(const IGisItem::key_t& key, QMessageBox::StandardButtons& last) {
  for (int i = childCount(); i > 0; i--) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i - 1));
    if (nullptr == item) {
      continue;
    }

    if (item->getKey() == key) {
      if (last != QMessageBox::YesToAll) {
        QString msg = tr("Are you sure you want to delete '%1' from project '%2'?")
                          .arg(item->getName(), text(CGisListWks::eColumnName));
        last = QMessageBox::question(CMainWindow::getBestWidgetForParent(), tr("Delete..."), msg,
                                     QMessageBox::YesToAll | QMessageBox::Cancel | QMessageBox::Ok | QMessageBox::No,
                                     QMessageBox::Ok);
        if ((last == QMessageBox::No) || (last == QMessageBox::Cancel)) {
          // as each item in the project has to be unique, we can stop searching.
          return false;
        }
      }
      delete item;

      /*
          Database projects are a bit different. Deleting an item does not really
          mean the project is changed as the item is still stored in the database.
       */
      if (type != eTypeDb) {
        setChanged();
      }

      // as each item in the project has to be unique, we can stop searching.
      return true;
    }
  }
  return false;
}

void IGisProject::editItemByKey(const IGisItem::key_t& key) {
  for (int i = childCount(); i > 0; i--) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i - 1));
    if (nullptr == item) {
      continue;
    }

    if (item->getKey() == key) {
      item->edit();
    }
  }
}

void IGisProject::insertCopyOfItem(IGisItem* item, int off, CSelectCopyAction::result_e& lastResult) {
  bool clone = false;
  IGisItem::key_t key = item->getKey();
  key.project = getKey();
  key.device = getDeviceKey();

  IGisItem* item2 = getItemByKey(key);
  if (item2 != nullptr) {
    CSelectCopyAction::result_e result = lastResult;
    if (lastResult == CSelectCopyAction::eResultNone) {
      CSelectCopyAction dlg(item, item2, CMainWindow::getBestWidgetForParent());
      dlg.exec();
      result = dlg.getResult();
      if (dlg.allOthersToo()) {
        lastResult = result;
      }
    }

    if (result == CSelectCopyAction::eResultSkip) {
      return;
    }
    if (result == CSelectCopyAction::eResultNone) {
      return;
    }
    if (result == CSelectCopyAction::eResultClone) {
      clone = true;
    } else {
      // replace item2 with item
      if (item != item2) {
        delete item2;
      } else {
        // replacing an item with itself does not make sense
        return;
      }
    }
  }

  switch (item->type()) {
    case IGisItem::eTypeTrk: {
      CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(item);
      if (trk != nullptr) {
        CGisItemTrk* newTrk = new CGisItemTrk(*trk, this, off, clone);
        // if the track is on a device, remove hidden trackpoints
        if (isOnDevice()) {
          newTrk->filterDelete();
        }
      }
      break;
    }

    case IGisItem::eTypeWpt: {
      CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(item);
      if (wpt != nullptr) {
        new CGisItemWpt(*wpt, this, off, clone);
      }
      break;
    }

    case IGisItem::eTypeRte: {
      CGisItemRte* rte = dynamic_cast<CGisItemRte*>(item);
      if (rte != nullptr) {
        new CGisItemRte(*rte, this, off, clone);
      }
      break;
    }

    case IGisItem::eTypeOvl: {
      CGisItemOvlArea* area = dynamic_cast<CGisItemOvlArea*>(item);
      if (area != nullptr) {
        new CGisItemOvlArea(*area, this, off, clone);
      }
      break;
    }
  }
}

void IGisProject::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) {
  if (!isVisible()) {
    return;
  }

  for (int i = 0; i < childCount(); i++) {
    if (gis->needsRedraw()) {
      break;
    }

    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item || item->isHidden()) {
      continue;
    }

    item->drawItem(p, viewport, blockedAreas, gis);
  }
}

void IGisProject::drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis) {
  if (!isVisible()) {
    return;
  }

  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item || item->isHidden()) {
      continue;
    }

    item->drawItem(p, viewport, gis);
  }
}

void IGisProject::drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas,
                            const QFontMetricsF& fm, CGisDraw* gis) {
  if (!isVisible()) {
    return;
  }

  for (int i = 0; i < childCount(); i++) {
    if (gis->needsRedraw()) {
      break;
    }

    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item || item->isHidden()) {
      continue;
    }

    item->drawLabel(p, viewport, blockedAreas, fm, gis);
  }
}

void IGisProject::mount() {
  if (!isOnDevice()) {
    return;
  }
  IDevice* device = dynamic_cast<IDevice*>(parent());
  if (device) {
    device->mount();
  }
}

void IGisProject::umount() {
  if (!isOnDevice()) {
    return;
  }
  IDevice* device = dynamic_cast<IDevice*>(parent());
  if (device) {
    device->umount();
  }
}

bool IGisProject::remove() {
#ifdef Q_OS_LINUX
  CDeviceGarminMtp* mtp = dynamic_cast<CDeviceGarminMtp*>(parent());
  if (mtp != nullptr) {
    return mtp->removeFromDevice(filename);
  }
#endif

  CProjectMountLock mountLock(*this);

  /*
     Check if parent is a device and give it a chance to take care of data.

     e.g. Garmin devices remove images attached to the project.
   */
  IDevice* device = dynamic_cast<IDevice*>(parent());
  if (device) {
    device->aboutToRemoveProject(this);
  }

  QFileInfo fi(filename);
  if (fi.isFile()) {
    QFile::remove(filename);
  } else if (fi.isDir()) {
    QDir(filename).removeRecursively();
  }

  return true;
}

void IGisProject::updateItemCounters() {
  // count number of items by type
  memset(cntItemsByType, 0, sizeof(cntItemsByType));
  cntTrkPts = 0;
  cntWpts = 0;
  totalDistance = 0;
  totalAscent = 0;
  totalDescent = 0;
  totalElapsedSeconds = 0;
  totalElapsedSecondsMoving = 0;

  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);
  stream.setByteOrder(QDataStream::LittleEndian);
  stream.setVersion(QDataStream::Qt_5_2);

  for (int i = 0; i < childCount(); i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item) {
      continue;
    }

    cntItemsByType[item->type()]++;

    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(item);
    if (trk) {
      cntTrkPts += trk->getNumberOfVisiblePoints();
      totalDistance += trk->getTotalDistance();
      totalAscent += trk->getTotalAscent();
      totalDescent += trk->getTotalDescent();
      totalElapsedSeconds += trk->getTotalElapsedSeconds();
      totalElapsedSecondsMoving += trk->getTotalElapsedSecondsMoving();
      stream << trk->getHash();
    }

    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(item);
    if (wpt) {
      cntWpts++;
      stream << wpt->getHash();
    }
  }
  QCryptographicHash md5(QCryptographicHash::Md5);
  md5.addData(buffer);

  hashTrkWpt[1] = hashTrkWpt[0];
  hashTrkWpt[0] = md5.result().toHex();
}

void IGisProject::blockUpdateItems(bool yes) {
  noUpdate = yes;
  if (noUpdate == false) {
    updateItems();
  }
}

void IGisProject::updateDecoration() {
  int N = childCount();
  bool saved = true;

  for (int i = 0; i < N; i++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(i));
    if (nullptr == item) {
      continue;
    }
    if (item->isChanged()) {
      saved = false;
      break;
    }
  }
  updateDecoration(saved);
}

void IGisProject::updateDecoration(bool saved) {
  QString str = autoSave ? "A" : saved ? "" : "*";
  if (autoSyncToDev) {
    str += "S";
  }
  setText(CGisListWks::eColumnDecoration, str);
}

void IGisProject::sortItems() {
  QList<IGisItem*> trks;
  QList<IGisItem*> rtes;
  QList<IGisItem*> wpts;
  QList<IGisItem*> ovls;

  QList<QTreeWidgetItem*> items = takeChildren();
  QList<QTreeWidgetItem*> others;  // For example Search
  for (QTreeWidgetItem* item : std::as_const(items)) {
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(item);
    if (trk != nullptr) {
      trks << trk;
      continue;
    }

    CGisItemRte* rte = dynamic_cast<CGisItemRte*>(item);
    if (rte != nullptr) {
      rtes << rte;
      continue;
    }

    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(item);
    if (wpt != nullptr) {
      wpts << wpt;
      continue;
    }

    CGisItemOvlArea* ovl = dynamic_cast<CGisItemOvlArea*>(item);
    if (ovl != nullptr) {
      ovls << ovl;
      continue;
    }

    others << item;
  }

  sortItems(trks);
  sortItems(rtes);
  sortItems(wpts);
  sortItems(ovls);

  items.clear();
  items << others;
  for (IGisItem* item : std::as_const(trks)) {
    items << item;
  }
  for (IGisItem* item : std::as_const(rtes)) {
    items << item;
  }
  for (IGisItem* item : std::as_const(wpts)) {
    items << item;
  }
  for (IGisItem* item : std::as_const(ovls)) {
    items << item;
  }

  addChildren(items);
  if (projectFilter != nullptr) {
    projectFilter->showLineEdit(&projectSearch);
  }
  applyFilters();
}

static bool sortByTime(IGisItem* item1, IGisItem* item2) {
  const QDateTime& t1 = item1->getTimestamp();
  const QDateTime& t2 = item2->getTimestamp();

  // avoid jumping items due to invalid timestamps
  if (!t1.isValid() || !t2.isValid()) {
    return sortByName<IGisItem>(item1, item2);
  }

  return t1 < t2;
}

void IGisProject::sortItems(QList<IGisItem*>& items) const {
  switch (sortingFolder) {
    case IGisProject::eSortFolderName:
      std::sort(items.begin(), items.end(), &sortByName<IGisItem>);
      break;

    case IGisProject::eSortFolderTime:
      std::sort(items.begin(), items.end(), &sortByTime);
      break;

    case IGisProject::eSortFolderRating:
      std::sort(items.begin(), items.end(), [](IGisItem* a, IGisItem* b) { return a->getRating() > b->getRating(); });
      break;
  }
}

void IGisProject::setProjectFilter(const CSearch& search) {
  projectSearch = search;
  applyFilters();
}

void IGisProject::setWorkspaceFilter(const CSearch& search) {
  workspaceSearch = search;
  applyFilters();
}

void IGisProject::applyFilters() {
  const int N = childCount();

  for (int n = 0; n < N; n++) {
    IGisItem* item = dynamic_cast<IGisItem*>(child(n));
    if (item == nullptr) {
      continue;
    }

    bool projectFilterResult = projectSearch.getSearchResult(item);
    bool workspaceFilterResult = workspaceSearch.getSearchResult(item);

    item->setHidden(
        !(projectFilterResult && workspaceFilterResult));  // get search result returns wether the object matches
  }
}

bool IGisProject::findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32& threshold, QPolygonF& polyline) {
  const int N = childCount();
  for (int n = 0; n < N; n++) {
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(child(n));
    if (trk != nullptr) {
      trk->findPolylineCloseBy(pt1, pt2, threshold, polyline);
    }
  }
  return !polyline.isEmpty();
}

void IGisProject::gainUserFocus(bool yes) {
  if (yes) {
    setIcon(CGisListWks::eColumnName, QIcon("://icons/32x32/Focus.png"));
    keyUserFocus = key;
  } else {
    setIcon(CGisListWks::eColumnName, QIcon());
    keyUserFocus.clear();
  }
}

CProjectFilterItem* IGisProject::filterProject(bool filter) {
  if (filter) {
    if (projectFilter == nullptr) {
      projectFilter = new CProjectFilterItem(this);
      insertChild(0, projectFilter);
    }
    // Set expanded anyways to show that search exists
    projectFilter->showLineEdit();
    setExpanded(true);
  } else {
    removeChild(projectFilter);
    delete projectFilter;
    projectFilter = nullptr;
    projectSearch = CSearch("");
  }
  sortItems();
  return projectFilter;
}
