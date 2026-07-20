/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CWksItemDelegate.h"

#include <QMouseEvent>
#include <QToolTip>

#include "gis/CGisListWks.h"
#include "gis/IWksItem.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/search/CGeoSearch.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "helpers/CRowBuilder.h"
#include "helpers/CSettings.h"
#include "misc.h"

constexpr int kFontSizeDiffProject = 2;
constexpr int kFontSizeDiffItem = 3;
constexpr int kFontSizeInvalid = -1;  // statusSize* sentinel: hide the status line entirely
constexpr int kProgressBarHeight = 5;
constexpr int kProgressBarBottomInset = 1;

CWksItemDelegate::CWksItemDelegate(CGisListWks* parent) : QStyledItemDelegate(parent), treeWidget(parent) {
  SETTINGS;
  cfg.beginGroup("Workspace");
  cfg.beginGroup("Decorations");
  itemStatusControl.prj.flags = cfg.value("prj", itemStatusControl.prj.flags).toUInt();
  itemStatusControl.trk.flags = cfg.value("trk", itemStatusControl.trk.flags).toUInt();
  itemStatusControl.wpt.flags = cfg.value("wpt", itemStatusControl.wpt.flags).toUInt();
  itemStatusControl.rte.flags = cfg.value("rte", itemStatusControl.rte.flags).toUInt();
  itemStatusControl.area.flags = cfg.value("area", itemStatusControl.area.flags).toUInt();
  itemStatusControl.statusSizePrj = cfg.value("statusSizePrj", kFontSizeDiffProject).toInt();
  itemStatusControl.statusSizeItem = cfg.value("statusSizeItem", kFontSizeDiffItem).toInt();
  cfg.endGroup();  // Decorations
  cfg.endGroup();  // Workspace
}

void CWksItemDelegate::setStatusItemsControl(const item_status_ctrl_t& settings) {
  itemStatusControl = settings;

  SETTINGS;
  cfg.beginGroup("Workspace");
  cfg.beginGroup("Decorations");
  cfg.setValue("prj", itemStatusControl.prj.flags);
  cfg.setValue("trk", itemStatusControl.trk.flags);
  cfg.setValue("wpt", itemStatusControl.wpt.flags);
  cfg.setValue("rte", itemStatusControl.rte.flags);
  cfg.setValue("area", itemStatusControl.area.flags);
  cfg.setValue("statusSizePrj", itemStatusControl.statusSizePrj);
  cfg.setValue("statusSizeItem", itemStatusControl.statusSizeItem);
  cfg.endGroup();  // Decorations
  cfg.endGroup();  // Workspace

  emit treeWidget->model()->layoutChanged();
}

IWksItem* CWksItemDelegate::indexToItem(const QModelIndex& index) const {
  return dynamic_cast<IWksItem*>(treeWidget->itemFromIndex(index));
}

QSize CWksItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  const IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return QSize(opt.rect.width(), 22);
  }

  const QFontMetrics fmName(opt.font);

  QFont fontStatusPrj = opt.font;
  fontStatusPrj.setPointSize(opt.font.pointSize() - itemStatusControl.statusSizePrj);
  const QFontMetrics fmStatusPrj(fontStatusPrj);

  QFont fontStatusItem = opt.font;
  fontStatusItem.setPointSize(opt.font.pointSize() - itemStatusControl.statusSizeItem);
  const QFontMetrics fmStatusItem(fontStatusItem);

  QFont fontStatusOther = opt.font;
  fontStatusOther.setPointSize(opt.font.pointSize() - kFontSizeDiffProject);
  const QFontMetrics fmStatusOther(fontStatusOther);

  switch (item->getBaseType()) {
    case IWksItem::eBaseType::Project: {
      const int statusH = (itemStatusControl.statusSizePrj != kFontSizeInvalid) ? fmStatusPrj.height() : 0;
      return QSize(opt.rect.width(), std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height(), statusH)));
    }

    case IWksItem::eBaseType::GeoSearch:
    case IWksItem::eBaseType::Device:
      return QSize(opt.rect.width(),
                   std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height(), fmStatusOther.height())));

    case IWksItem::eBaseType::GeoSearchError:
      return QSize(opt.rect.width(), std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height())));

    case IWksItem::eBaseType::Item: {
      const int statusH = (itemStatusControl.statusSizeItem != kFontSizeInvalid) ? fmStatusItem.height() : 0;
      return QSize(opt.rect.width(), std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height(), statusH)));
    }

    default:;
      return QSize(opt.rect.width(), 22);
  }
}

CWksItemDelegate::ProjectLayout CWksItemDelegate::getRectanglesProject(const QStyleOptionViewItem& opt,
                                                                       IWksItem& item) const {
  ProjectLayout layout;
  layout.fontName = opt.font;
  const QFontMetrics fmName(layout.fontName);

  layout.fontStatus = opt.font;
  layout.fontStatus.setPointSize(layout.fontStatus.pointSize() - itemStatusControl.statusSizePrj);
  const QFontMetrics fmStatus(layout.fontStatus);

  const bool isOnDevice = item.isOnDevice() != IWksItem::eTypeNone;

  CRowBuilder row(opt.rect, kCellPad, kInnerGap);
  layout.rectIcon = row.takeLeft(row.height());
  row.markStatusColumn();
  layout.rectVisible = row.takeButton(fmName.height());

  // All optional button rects stay default-constructed (invalid) unless set below.
  // Buttons with an invalid rect are simply skipped by the paint and hit-test code.
  if (!isOnDevice && item.type() != IWksItem::eTypeLostFound) {
    if (item.holdUiFocus(opt)) {
      layout.rectActiveProject = row.takeButton(fmName.height());
      layout.rectSave = row.takeButton(fmName.height());
      if (treeWidget->hasDeviceSupport()) {
        layout.rectAutoSyncDev = row.takeButton(fmName.height());
      }
    } else {
      if (item.hasUserFocus()) {
        layout.rectActiveProject = row.takeButton(fmName.height());
      }
      if (item.isChanged() && !item.isAutoSave()) {
        layout.rectSave = row.takeButton(fmName.height());
      }
    }
  } else if (isOnDevice) {
    if (item.holdUiFocus(opt)) {
      layout.rectSave = row.takeButton(fmName.height());
    }
  }

  layout.rectName = row.nameSlice(fmName.height());

  if (itemStatusControl.statusSizePrj != kFontSizeInvalid) {
    layout.rectStatus = row.fullStatusSlice(fmStatus.height());
  }

  layout.rectProgress = row.fullStatusSlice(kProgressBarHeight).adjusted(kInnerGap, 0, -kInnerGap, 0);

  return layout;
}

CWksItemDelegate::ItemLayout CWksItemDelegate::getRectanglesItem(const QStyleOptionViewItem& opt,
                                                                 const IWksItem& item) const {
  ItemLayout layout;
  layout.fontName = opt.font;
  const QFontMetrics fmName(layout.fontName);

  layout.fontStatus = opt.font;
  layout.fontStatus.setPointSize(layout.fontStatus.pointSize() - itemStatusControl.statusSizeItem);
  const QFontMetrics fmStatus(layout.fontStatus);

  CRowBuilder row(opt.rect, kCellPad, kInnerGap);
  layout.rectIcon = row.takeLeft(row.height());
  row.markStatusColumn();

  const CGeoSearch* search = dynamic_cast<const CGeoSearch*>(item.parent());
  const bool isOnGeoSearch = search != nullptr;
  if (item.isChanged() && !item.isOnDevice() && !isOnGeoSearch) {
    layout.rectChanged = row.takeButton(fmName.height());
  }

  layout.rectName = row.nameSlice(fmName.height());

  if (itemStatusControl.statusSizeItem != kFontSizeInvalid) {
    layout.rectStatus = row.fullStatusSlice(fmStatus.height());
  }

  return layout;
}

CWksItemDelegate::DeviceLayout CWksItemDelegate::getRectanglesDevice(const QStyleOptionViewItem& opt,
                                                                     const IWksItem& item) const {
  DeviceLayout layout;
  layout.fontName = opt.font;
  const QFontMetrics fmName(layout.fontName);

  layout.fontStatus = opt.font;
  layout.fontStatus.setPointSize(layout.fontStatus.pointSize() - kFontSizeDiffProject);
  const QFontMetrics fmStatus(layout.fontStatus);

  CRowBuilder row(opt.rect, kCellPad, kInnerGap);
  layout.rectIcon = row.takeLeft(row.height());
  row.markStatusColumn();
  layout.rectVisible = row.takeButton(fmName.height());
  layout.rectName = row.nameSlice(fmName.height());
  layout.rectStatus = row.fullStatusSlice(fmStatus.height());
  layout.rectProgress = row.fullStatusSlice(kProgressBarHeight);

  return layout;
}

CWksItemDelegate::GeoSearchLayout CWksItemDelegate::getRectanglesGeoSearch(const QStyleOptionViewItem& opt) const {
  GeoSearchLayout layout;
  layout.fontSearch = opt.font;
  const QFontMetrics fmSearch(layout.fontSearch);

  layout.fontStatus = opt.font;
  layout.fontStatus.setPointSize(layout.fontStatus.pointSize() - kFontSizeDiffProject);
  const QFontMetrics fmStatus(layout.fontStatus);

  CRowBuilder row(opt.rect, kCellPad, kInnerGap);
  layout.rectIcon = row.takeLeft(row.height());
  layout.rectSetup = row.takeLeftButton(fmSearch.height());
  layout.rectVisible = row.takeButton(fmSearch.height());
  layout.rectWptIcon = row.takeButton(fmSearch.height());
  layout.rectLineEdit = row.nameSlice(CRowBuilder::buttonSize(fmSearch.height()));
  layout.rectStatus = row.statusSlice(fmStatus.height());

  return layout;
}

CWksItemDelegate::GeoSearchErrorLayout CWksItemDelegate::getRectanglesGeoSearchError(
    const QStyleOptionViewItem& opt) const {
  GeoSearchErrorLayout layout;
  layout.font = opt.font;

  const QRect& r = opt.rect.adjusted(kCellPad, kCellPad, -kCellPad, -kCellPad);
  layout.rectIcon = QRect(r.left(), r.top(), r.height(), r.height());
  layout.rectName = r.adjusted(layout.rectIcon.width() + kMargin, 0, 0, 0);

  return layout;
}

void CWksItemDelegate::drawProgressBar(QPainter* p, const QRect& rect, qreal progress) {
  quint32 width = qRound(rect.width() * progress / 100.0);
  const QLine line(rect.left(), rect.bottom() - kProgressBarBottomInset, rect.left() + width,
                   rect.bottom() - kProgressBarBottomInset);
  p->setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap));
  p->drawLine(line);
  p->setPen(QPen(Qt::darkGreen, 3, Qt::SolidLine, Qt::RoundCap));
  p->drawLine(line);
}

QString CWksItemDelegate::distanceAscentDescentStatus(qreal distance, qreal ascent, qreal descent,
                                                      bool elevationInvalid, bool showDistance, bool showAscent,
                                                      bool showDescent) {
  QString status;

  // U+21A6 ↦ distance   U+2197 ↗ ascent   U+2198 ↘ descent
  if (distance != NOFLOAT && !qFuzzyIsNull(distance) && showDistance) {
    QString unit, val;
    IUnit::self().meter2distance(distance, val, unit);
    status += QString("%1%2%3 ").arg(QChar(0x21A6)).arg(val, unit);
  }

  if (!elevationInvalid) {
    if (ascent != NOFLOAT && !qFuzzyIsNull(ascent) && showAscent) {
      QString unit, val;
      IUnit::self().meter2elevation(ascent, val, unit);
      status += QString("%1%2%3 ").arg(QChar(0x2197)).arg(val, unit);
    }

    if (descent != NOFLOAT && !qFuzzyIsNull(descent) && showDescent) {
      QString unit, val;
      IUnit::self().meter2elevation(descent, val, unit);
      status += QString("%1%2%3 ").arg(QChar(0x2198)).arg(val, unit);
    }
  }

  return status;
}

void CWksItemDelegate::drawRatingStars(qreal rating, QPainter* p, QIcon::Mode iconMode, QRect& rectStatus) const {
  if (const qint32 N = qRound(rating); N > 0) {
    QRect rectStar(rectStatus.left() + kMargin, rectStatus.top() + kMargin, rectStatus.height() - 2 * kMargin,
                   rectStatus.height() - 2 * kMargin);
    // Local, not static: a static would pin the colour scheme live at first paint.
    const QIcon star("://icons/RatingStar.svgt");
    for (int i = 0; i < N; i++) {
      star.paint(p, rectStar, Qt::AlignCenter, iconMode);
      rectStar.translate(kMargin + rectStar.width(), 0);
    }
    // Advance rectStatus past the drawn stars so subsequent text starts to the right.
    rectStatus.setLeft(rectStar.left() + kMargin);
  }
}

void CWksItemDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return QStyledItemDelegate::paint(p, opt, index);
  }

  p->save();
  USE_ANTI_ALIASING(*p, true);
  // Standard background & selection
  QStyledItemDelegate::paint(p, opt, index);

  switch (item->getBaseType()) {
    case IWksItem::eBaseType::Project:
      paintProject(p, opt, index, *item);
      break;

    case IWksItem::eBaseType::Item:
      paintItem(p, opt, index, *item);
      break;

    case IWksItem::eBaseType::Device:
      paintDevice(p, opt, index, *item);
      break;

    case IWksItem::eBaseType::GeoSearch:
      paintGeoSearch(p, opt, index, *item);
      break;

    case IWksItem::eBaseType::GeoSearchError:
      paintGeoSearchError(p, opt, index, *item);
      break;

    default:;
      p->setPen(Qt::black);
      p->setBrush(Qt::NoBrush);
      p->drawRect(opt.rect);
      p->drawText(opt.rect, item->getName());
  }

  p->restore();
}

void CWksItemDelegate::paintProject(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                    IWksItem& item) const {
  IGisProject* project = dynamic_cast<IGisProject*>(&item);
  if (project == nullptr) {
    return;
  }

  auto layout = getRectanglesProject(opt, *project);

  const bool isOnDevice = item.isOnDevice() != IWksItem::eTypeNone;
  const bool isVisible = item.isVisible();

  // derive strings colors
  const QColor colorName = CDraw::itemNameColor(opt, isVisible);

  // draw icon
  item.getIcon().paint(p, layout.rectIcon, Qt::AlignCenter, item.isVisible() ? QIcon::Normal : QIcon::Disabled);

  // draw tool button to toggle visibility
  CDraw::drawToolButton(p, opt, layout.rectVisible,
                        isVisible ? QIcon(":/icons/ShowAll.svgt") : QIcon(":/icons/ShowNone.svgt"), true, isVisible);

  const float opacityOfFocusBasedItems = item.getOpacityOfFocusBasedItems();

  if (layout.rectSave.isValid()) {
    if (item.isOnDevice() == IWksItem::eTypeNone) {
      // draw save/ auto save button
      if (item.isChanged() && !item.isAutoSave()) {
        // show save button
        CDraw::drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/Save.svgt"), true, false);
      } else {
        p->setOpacity(opacityOfFocusBasedItems);
        if (item.isAutoSave()) {
          // show auto save button pressed, to disable autosave
          CDraw::drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/AutoSaveA.svgt"), true, true);
        } else if (item.canSave()) {
          // show auto save button only if project can be saved
          CDraw::drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/AutoSaveNoA.svgt"), true, false);
        }
        p->setOpacity(1.0);
      }
    } else {
      p->setOpacity(opacityOfFocusBasedItems);
      CDraw::drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/Copy.svgt"), true, false);
      p->setOpacity(1.0);
    }
  }

  if (layout.rectAutoSyncDev.isValid()) {
    p->setOpacity(opacityOfFocusBasedItems);
    // auto sync. w. dev.
    if (item.isAutoSyncToDev()) {
      CDraw::drawToolButton(p, opt, layout.rectAutoSyncDev, QIcon(":/icons/DeviceSync.svgt"), true, true);
    } else {
      CDraw::drawToolButton(p, opt, layout.rectAutoSyncDev, QIcon(":/icons/DeviceNoSync.svgt"), true, false);
    }
    p->setOpacity(1.0);
  }

  if (layout.rectActiveProject.isValid()) {
    if (item.holdUiFocus(opt)) {
      p->setOpacity(opacityOfFocusBasedItems);
      CDraw::drawToolButton(p, opt, layout.rectActiveProject,
                            item.hasUserFocus() ? QIcon(":/icons/Focus.svgt") : QIcon(":/icons/UnFocus.svgt"), true,
                            true);
      p->setOpacity(1.0);
    } else {
      QIcon(":/icons/Focus.svgt")
          .paint(p, layout.rectActiveProject.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin),
                 Qt::AlignCenter);
    }
  }

  // draw name
  layout.fontName.setBold(item.hasUserFocus());
  p->setPen(colorName);
  p->setFont(layout.fontName);
  p->drawText(layout.rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop,
              isOnDevice ? project->getName() : project->getNameEx());

  // -- start ------------ status line ---------------------------------------
  auto [hasProgress, progress] = item.getProgress();
  if (hasProgress) {
    drawProgressBar(p, layout.rectProgress, progress);
  } else if (layout.rectStatus.isValid()) {
    QString status;
    const QString& keywords = project->getKeywords();
    if (!keywords.isEmpty() && itemStatusControl.prj.keywords) {
      status += keywords + " ";
    }

    status +=
        distanceAscentDescentStatus(project->getTotalDistance(), project->getTotalAscent(), project->getTotalDescent(),
                                    project->isTrkElevationInvalid(), itemStatusControl.prj.distance,
                                    itemStatusControl.prj.ascent, itemStatusControl.prj.descent);

    if (itemStatusControl.prj.gisStats) {
      const qint32 cntTrk = project->getItemCountByType(IGisItem::eTypeTrk);
      if (cntTrk != 0) {
        status += QString("T: %1 ").arg(cntTrk);
      }
      const qint32 cntWpt = project->getItemCountByType(IGisItem::eTypeWpt);
      if (cntWpt != 0) {
        status += QString("W: %1 ").arg(cntWpt);
      }
      const qint32 cntRte = project->getItemCountByType(IGisItem::eTypeRte);
      if (cntRte != 0) {
        status += QString("R: %1 ").arg(cntRte);
      }
      const qint32 cntArea = project->getItemCountByType(IGisItem::eTypeOvl);
      if (cntArea != 0) {
        status += QString("A: %1 ").arg(cntArea);
      }
    }
    p->setPen(colorName);
    p->setFont(layout.fontStatus);
    p->drawText(layout.rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
  }
  // -- stop ------------ status line ---------------------------------------
}

void CWksItemDelegate::paintDevice(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                   const IWksItem& item) const {
  auto layout = getRectanglesDevice(opt, item);

  const bool isVisible = item.isVisible();

  // derive strings colors
  const QColor colorName = CDraw::itemNameColor(opt, isVisible);

  // draw name
  layout.fontName.setBold(item.hasUserFocus());
  p->setPen(colorName);
  p->setFont(layout.fontName);
  p->drawText(layout.rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item.getName());

  // draw icon
  // MTP devices supply a raster icon read off the device; QIcon will not upscale one, so stretch it to fill.
  const QIcon::Mode iconMode = isVisible ? QIcon::Normal : QIcon::Disabled;
  const QIcon& deviceIcon = item.getIcon();
  if (deviceIcon.actualSize(layout.rectIcon.size()) == layout.rectIcon.size()) {
    deviceIcon.paint(p, layout.rectIcon, Qt::AlignCenter, iconMode);
  } else {
    const QPixmap icon = deviceIcon.pixmap(deviceIcon.actualSize(QSize(1024, 1024)))
                             .scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, iconMode);
  }

  // draw tool button to activate
  CDraw::drawToolButton(p, opt, layout.rectVisible,
                        isVisible ? QIcon(":/icons/ShowAll.svgt") : QIcon(":/icons/ShowNone.svgt"), true, isVisible);

  // draw progress bar
  auto [hasProgress, progress] = item.getProgress();
  if (hasProgress) {
    drawProgressBar(p, layout.rectProgress, progress);
  } else {
    // draw status
    p->setPen(colorName);
    p->setFont(layout.fontStatus);
    p->drawText(layout.rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop,
                item.getInfo(IWksItem::eFeatureShowName));
  }
}

void CWksItemDelegate::paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                 const IWksItem& item) const {
  auto layout = getRectanglesItem(opt, item);

  const bool isVisible = item.isVisible();
  const QIcon::Mode iconMode = isVisible ? QIcon::Normal : QIcon::Disabled;

  // derive strings colors
  const QColor colorName = CDraw::itemNameColor(opt, isVisible);

  // draw name
  layout.fontName.setBold(item.hasUserFocus());
  p->setPen(colorName);
  p->setFont(layout.fontName);
  p->drawText(layout.rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item.getName());

  // -- start ------------ status line ---------------------------------------
  if (layout.rectStatus.isValid()) {
    QString status;
    const CGisItemTrk* trk = dynamic_cast<const CGisItemTrk*>(&item);
    if (trk != nullptr && itemStatusControl.trk.flags != 0) {
      if (itemStatusControl.trk.rating) {
        drawRatingStars(item.getRating(), p, iconMode, layout.rectStatus);
      }

      if (itemStatusControl.trk.tags) {
        status = item.getTags().values().join(", ");
        if (!status.isEmpty()) {
          status += " ";
        }
      }

      status += distanceAscentDescentStatus(trk->getTotalDistance(), trk->getTotalAscent(), trk->getTotalDescent(),
                                            trk->isTrkElevationInvalid(), itemStatusControl.trk.distance,
                                            itemStatusControl.trk.ascent, itemStatusControl.trk.descent);
    }

    const CGisItemWpt* wpt = dynamic_cast<const CGisItemWpt*>(&item);
    if (wpt != nullptr) {
      if (wpt->isGeocache()) {
        status = wpt->getGeoCache().name;
      } else if (itemStatusControl.wpt.flags != 0) {
        if (itemStatusControl.wpt.rating) {
          drawRatingStars(item.getRating(), p, iconMode, layout.rectStatus);
        }

        if (itemStatusControl.wpt.tags) {
          status = item.getTags().values().join(", ");
          if (!status.isEmpty()) {
            status += " ";
          }
        }

        qreal ele = wpt->getElevation();
        if (ele != NOFLOAT && itemStatusControl.wpt.elevation) {
          QString unit, val;
          IUnit::self().meter2elevation(ele, val, unit);
          status += QString("%1%2 ").arg(val, unit);
        }
      }
    }

    const CGisItemRte* rte = dynamic_cast<const CGisItemRte*>(&item);
    if (rte != nullptr && itemStatusControl.rte.flags != 0) {
      if (itemStatusControl.rte.rating) {
        drawRatingStars(item.getRating(), p, iconMode, layout.rectStatus);
      }

      if (itemStatusControl.rte.tags) {
        status = item.getTags().values().join(", ");
        if (!status.isEmpty()) {
          status += " ";
        }
      }
    }

    const CGisItemOvlArea* area = dynamic_cast<const CGisItemOvlArea*>(&item);
    if (area != nullptr && itemStatusControl.area.flags != 0) {
      if (itemStatusControl.area.rating) {
        drawRatingStars(item.getRating(), p, iconMode, layout.rectStatus);
      }

      if (itemStatusControl.area.tags) {
        status = item.getTags().values().join(", ");
        if (!status.isEmpty()) {
          status += " ";
        }
      }
    }

    p->setFont(layout.fontStatus);
    p->drawText(layout.rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
  }
  // -- stop ------------ status line ---------------------------------------

  // draw icon
  // GIS item icons stay raster (Task 2): pull the pixmap from the QIcon and stretch it, as before.
  const QIcon& itemIcon = item.getIcon();
  const QPixmap icon = itemIcon.pixmap(itemIcon.actualSize(QSize(1024, 1024)))
                           .scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, iconMode);

  // draw save/changed icon
  if (layout.rectChanged.isValid()) {
    QIcon(":/icons/Save.svgt")
        .paint(p, layout.rectChanged.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin), Qt::AlignCenter,
               iconMode);
  }
}

void CWksItemDelegate::paintGeoSearch(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                      const IWksItem& item) const {
  const CGeoSearch* search = dynamic_cast<const CGeoSearch*>(&item);
  if (search == nullptr) {
    return;
  }
  const auto& layout = getRectanglesGeoSearch(opt);
  const bool isVisible = item.isVisible();

  // The icon is either SVG (plain service) or a composed raster ("accumulative results"), so ask
  // whether it can fill the cell rather than assume -- same test as paintDevice.
  const QIcon::Mode searchMode = isVisible ? QIcon::Normal : QIcon::Disabled;
  const QIcon& searchIcon = item.getIcon();
  if (searchIcon.actualSize(layout.rectIcon.size()) == layout.rectIcon.size()) {
    searchIcon.paint(p, layout.rectIcon, Qt::AlignCenter, searchMode);
  } else {
    const QPixmap icon = searchIcon.pixmap(searchIcon.actualSize(QSize(1024, 1024)))
                             .scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, searchMode);
  }
  QIcon(":/icons/Apply.svgt").paint(p, layout.rectSetup, Qt::AlignCenter, QIcon::Normal);
  QIcon(search->getWptIcon()).paint(p, layout.rectWptIcon, Qt::AlignCenter, QIcon::Normal);

  // draw tool button to activate
  CDraw::drawToolButton(p, opt, layout.rectVisible,
                        isVisible ? QIcon(":/icons/ShowAll.svgt") : QIcon(":/icons/ShowNone.svgt"), true, isVisible);

  const QString& address = search->getLastAddress();
  const QColor& colorSearch = opt.palette.color(
      isVisible && search->isInputEnabled() && !address.isEmpty() ? QPalette::Active : QPalette::Disabled,
      QPalette::WindowText);

  p->setPen(colorSearch);
  p->setFont(layout.fontSearch);
  p->drawText(layout.rectLineEdit.adjusted(kMargin, -1, 1, 0), Qt::AlignTop | Qt::AlignLeft,
              address.isEmpty() ? tr("Enter address...") : address);

  const QColor& colorStatus =
      opt.palette.color(isVisible ? QPalette::Active : QPalette::Disabled, QPalette::WindowText);

  p->setPen(colorStatus);
  p->setFont(layout.fontStatus);
  p->drawText(layout.rectStatus.adjusted(kMargin, -1, 0, 1), Qt::AlignTop | Qt::AlignLeft, search->getServiceName());
}

void CWksItemDelegate::paintGeoSearchError(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                           const IWksItem& item) const {
  const auto& layout = getRectanglesGeoSearchError(opt);
  const bool isVisible = item.isVisible();

  item.getIcon().paint(p, layout.rectIcon, Qt::AlignCenter, isVisible ? QIcon::Normal : QIcon::Disabled);
  const QColor& color = opt.palette.color(isVisible ? QPalette::Active : QPalette::Disabled, QPalette::WindowText);

  p->setPen(color);
  p->setFont(layout.font);
  p->drawText(layout.rectName.adjusted(kMargin, -1, 0, 1), Qt::AlignTop | Qt::AlignLeft, item.getName());
}

bool CWksItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                                   const QModelIndex& index) {
  IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return QStyledItemDelegate::editorEvent(event, model, opt, index);
  }

  if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);

    switch (item->getBaseType()) {
      case IWksItem::eBaseType::Project:
        return mousePressProject(me, opt, index, *item);
      case IWksItem::eBaseType::Device:
        return mousePressDevice(me, opt, index, *item);
      case IWksItem::eBaseType::GeoSearch:
        return mousePressGeoSearch(me, opt, index, *item);
      default:;
    }
  }
  return QStyledItemDelegate::editorEvent(event, model, opt, index);
}

bool CWksItemDelegate::mousePressProject(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                         IWksItem& item) {
  const auto& layout = getRectanglesProject(opt, item);
  if (layout.rectVisible.contains(me->pos())) {
    item.setVisibility(!item.isVisible());
    emit sigUpdateCanvas();
    return true;
  } else if (layout.rectSave.contains(me->pos())) {
    if (item.isOnDevice() == IWksItem::eTypeNone) {
      if (item.isAutoSave()) {
        item.setAutoSave(false);
      } else {
        if (item.isChanged()) {
          IGisProject* project = dynamic_cast<IGisProject*>(&item);
          if (project == nullptr) {
            return false;
          }
          if (project->canSave()) {
            project->save();
          } else {
            project->saveAs();
          }
        } else {
          item.setAutoSave(true);
        }
      }
    } else {
      treeWidget->slotCopyProject();
    }
    return true;
  } else if (layout.rectAutoSyncDev.contains(me->pos())) {
    item.setAutoSyncToDev(!item.isAutoSyncToDev());
    return true;

  } else if (layout.rectActiveProject.contains(me->pos())) {
    IGisProject* project = dynamic_cast<IGisProject*>(&item);
    if (project == nullptr) {
      return false;
    }
    if ((opt.state & QStyle::State_HasFocus) != 0) {
      treeWidget->setUserFocus(project->getKey(), !project->hasUserFocus());
    }
    return true;
  }

  return false;
}

bool CWksItemDelegate::mousePressDevice(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                        IWksItem& item) {
  const auto& layout = getRectanglesDevice(opt, item);

  if (layout.rectVisible.contains(me->pos())) {
    item.setVisibility(!item.isVisible());
    emit sigUpdateCanvas();
    return true;
  }
  return false;
}

bool CWksItemDelegate::mousePressGeoSearch(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                           IWksItem& item) {
  CGeoSearch* search = dynamic_cast<CGeoSearch*>(&item);
  if (search == nullptr) {
    return false;
  }

  const auto& layout = getRectanglesGeoSearch(opt);

  if (layout.rectVisible.contains(me->pos())) {
    item.setVisibility(!item.isVisible());
    emit sigUpdateCanvas();
    return true;
  }
  if (layout.rectSetup.contains(me->pos())) {
    search->selectService(layout.rectSetup);
    return true;
  }
  if (layout.rectWptIcon.contains(me->pos())) {
    search->changeSymbol();
    return true;
  }

  // A click on the line edit is not consumed here, so the view's default
  // editorEvent handling can start editing the geo search name. Clicks
  // anywhere else on the row are consumed to suppress that default behavior.
  if (layout.rectLineEdit.contains(me->pos())) {
    return false;
  }

  return true;
}

bool CWksItemDelegate::helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                                 const QModelIndex& index) {
  if (!event || !view) {
    return QStyledItemDelegate::helpEvent(event, view, opt, index);
  }
  IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return QStyledItemDelegate::helpEvent(event, view, opt, index);
  }

  const QPoint& pos = event->pos();
  const QPoint& posGlobal = event->globalPos();
  switch (item->getBaseType()) {
    case IWksItem::eBaseType::Project:
      return helpEventProject(pos, posGlobal, view, opt, *item);
    case IWksItem::eBaseType::Item:
      return helpEventItem(pos, posGlobal, view, opt, *item);
    case IWksItem::eBaseType::GeoSearch:
      return helpEventGeoSearch(pos, posGlobal, view, opt, *item);
    case IWksItem::eBaseType::GeoSearchError:
      return helpEventGeoSearchError(pos, posGlobal, view, opt, *item);
    default:;
  }

  return QStyledItemDelegate::helpEvent(event, view, opt, index);
}

bool CWksItemDelegate::helpEventProject(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                        const QStyleOptionViewItem& opt, IWksItem& item) {
  const auto& layout = getRectanglesProject(opt, item);
  if (layout.rectVisible.contains(pos)) {
    if (item.isVisible()) {
      QToolTip::showText(posGlobal, toRichText(tr("Hide project on map.")), view, {}, 3000);
    } else {
      QToolTip::showText(posGlobal, toRichText(tr("Show project on map.")), view, {}, 3000);
    }
    return true;
  } else if (layout.rectSave.contains(pos)) {
    if (item.isOnDevice() == IWksItem::eTypeNone) {
      if (item.isChanged() && !item.isAutoSave()) {
        QToolTip::showText(posGlobal, toRichText(tr("Save project.")), view, {}, 3000);
      } else {
        if (item.isAutoSave()) {
          QToolTip::showText(posGlobal, toRichText(tr("Disable auto save.")), view, {}, 3000);
        } else if (item.canSave()) {
          QToolTip::showText(posGlobal, toRichText(tr("Enable auto save.")), view, {}, 3000);
        }
      }
    } else {
      QToolTip::showText(posGlobal, toRichText(tr("Copy content of project into a project in the workspace.")), view,
                         {}, 3000);
    }
    return true;
  } else if (layout.rectAutoSyncDev.contains(pos)) {
    if (item.isAutoSyncToDev()) {
      QToolTip::showText(posGlobal, toRichText(tr("Disable automatic synchonization with GPS device.")), view, {},
                         3000);
    } else {
      QToolTip::showText(posGlobal, toRichText(tr("Enable automatic synchonization with GPS device.")), view, {}, 3000);
    }
    return true;
  } else if (layout.rectActiveProject.contains(pos)) {
    if (item.hasUserFocus()) {
      QToolTip::showText(
          posGlobal,
          toRichText(tr("This is the active project. All new items will be attached to this project automatically.")),
          view, {}, 5000);
    } else {
      QToolTip::showText(
          posGlobal,
          toRichText(
              tr("Make this project the active one. All new items will be attached to this project automatically.")),
          view, {}, 5000);
    }
    return true;
  } else if (layout.rectName.contains(pos)) {
    QToolTip::showText(posGlobal, item.getInfo(IWksItem::eFeatureShowName), view);
    return true;
  } else if (layout.rectStatus.contains(pos)) {
    if (itemStatusControl.prj.flags == 0) {
      QToolTip::showText(
          posGlobal,
          toRichText(
              tr("This is the status line. You can select additional information to be displayed in the workspace "
                 "setup. See menu->Workspace->Setup Workspace")),
          view, {}, 5000);
    } else {
      QToolTip::showText(posGlobal, item.getInfo(IWksItem::eFeatureShowName), view);
    }
    return true;
  }
  return false;
}

bool CWksItemDelegate::helpEventItem(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                     const QStyleOptionViewItem& opt, const IWksItem& item) {
  const auto& layout = getRectanglesItem(opt, item);

  if (layout.rectName.contains(pos)) {
    QToolTip::showText(posGlobal, item.getInfo(IWksItem::eFeatureShowName), view);
    return true;
  } else if (layout.rectStatus.contains(pos)) {
    if (itemStatusControl.trk.flags == 0 && itemStatusControl.wpt.flags == 0 && itemStatusControl.rte.flags == 0 &&
        itemStatusControl.area.flags == 0) {
      QToolTip::showText(
          posGlobal,
          toRichText(
              tr("This is the status line. You can select additional information to be displayed in the workspace "
                 "setup. See menu->Workspace->Setup Workspace")),
          view, {}, 5000);
    } else {
      QToolTip::showText(posGlobal, item.getInfo(IWksItem::eFeatureShowName), view);
    }
    return true;
  } else if (layout.rectChanged.contains(pos)) {
    if (item.isChanged()) {
      QToolTip::showText(posGlobal, toRichText(tr("Item is changed and needs to be saved.")), view, {}, 3000);
      return true;
    }
  }
  return false;
}

bool CWksItemDelegate::helpEventGeoSearch(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                          const QStyleOptionViewItem& opt, const IWksItem& item) {
  const auto& layout = getRectanglesGeoSearch(opt);
  if (layout.rectSetup.contains(pos)) {
    QToolTip::showText(posGlobal, toRichText(tr("Setup Search")), view, {}, 3000);
    return true;
  }

  return false;
}

bool CWksItemDelegate::helpEventGeoSearchError(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                               const QStyleOptionViewItem& opt, const IWksItem& item) {
  QToolTip::showText(posGlobal, "<div>" + item.getInfo(IWksItem::eFeatureShowName) + "</div>", view);
  return true;
}

QWidget* CWksItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& opt,
                                        const QModelIndex& index) const {
  CGeoSearch* search = dynamic_cast<CGeoSearch*>(indexToItem(index));
  if (search == nullptr || !search->isInputEnabled()) {
    return nullptr;
  }

  QLineEdit* editor = new QLineEdit(search->getLastAddress(), parent);
  editor->setObjectName("GeoSearch");
  editor->setPlaceholderText(tr("Enter address..."));

  return editor;
}

void CWksItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& opt,
                                            const QModelIndex& index) const {
  if (editor->objectName() == "GeoSearch") {
    const auto& layout = getRectanglesGeoSearch(opt);
    editor->setGeometry(layout.rectLineEdit.adjusted(kMargin, -2, 0, 2));
  } else {
    editor->setGeometry(opt.rect);
  }
}

void CWksItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
  QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
  if (lineEdit == nullptr) {
    return;
  }

  CGeoSearch* search = dynamic_cast<CGeoSearch*>(indexToItem(index));
  if (search == nullptr) {
    return;
  }

  search->startSearch(lineEdit->text());
}

void CWksItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
  QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
  if (lineEdit == nullptr) {
    return;
  }

  CGeoSearch* search = dynamic_cast<CGeoSearch*>(indexToItem(index));
  if (search == nullptr) {
    return;
  }

  lineEdit->setText(search->getLastAddress());
}
