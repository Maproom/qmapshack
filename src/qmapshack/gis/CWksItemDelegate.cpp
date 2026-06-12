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
#include "helpers/CSettings.h"
#include "misc.h"

constexpr int kMargin = 1;
constexpr int kFontSizeDiffProject = 2;
constexpr int kFontSizeDiffItem = 3;
constexpr int kFontSizeInvalid = -1;
constexpr int kProgressBarHeight = 5;
constexpr int kProgressBarHeightHalf = 1;

namespace {
/// Cursor laying out a row of right-aligned, equally sized tool buttons. Each call to
/// next() returns the rect for the next button and moves left by one button width + margin.
struct button_row_cursor_t {
  int left;
  int top;
  int size;

  QRect next() {
    const QRect rect(left, top, size, size);
    left -= size + kMargin;
    return rect;
  }
};
}  // namespace

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
  IWksItem* item = dynamic_cast<IWksItem*>(treeWidget->itemFromIndex(index));
  return item;
}

QSize CWksItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  const IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return QSize(opt.rect.width(), 22);
  }

  QFont fontName = opt.font;
  fontName.setBold(true);
  QFontMetrics fmName(fontName);

  QFont fontStatusPrj = opt.font;
  fontStatusPrj.setPointSize(fontName.pointSize() - itemStatusControl.statusSizePrj);
  QFontMetrics fmStatusPrj(fontStatusPrj);

  QFont fontStatusItem = opt.font;
  fontStatusItem.setPointSize(fontName.pointSize() - itemStatusControl.statusSizeItem);
  QFontMetrics fmStatusItem(fontStatusItem);

  QFont fontStatusOther = opt.font;
  fontStatusOther.setPointSize(fontName.pointSize() - kFontSizeDiffProject);
  QFontMetrics fmStatusOther(fontStatusOther);

  switch (item->getBaseType()) {
    case IWksItem::eBaseType::Project:
      if (itemStatusControl.statusSizePrj != kFontSizeInvalid) {
        return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height() + fmStatusPrj.height()));
      } else {
        return QSize(opt.rect.width(), std::max(22, 5 * kMargin + fmName.height()));
      }

    case IWksItem::eBaseType::GeoSearch:
    case IWksItem::eBaseType::Device:
      return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height() + fmStatusOther.height()));

    case IWksItem::eBaseType::GeoSearchError:
      return QSize(opt.rect.width(), std::max(22, 5 * kMargin + fmName.height()));

    case IWksItem::eBaseType::Item:
      if (itemStatusControl.statusSizeItem != kFontSizeInvalid) {
        return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height() + fmStatusItem.height()));
      } else {
        return QSize(opt.rect.width(), std::max(22, 5 * kMargin + fmName.height()));
      }

    default:;
      return QSize(opt.rect.width(), 22);
  }
}

CWksItemDelegate::ProjectLayout CWksItemDelegate::getRectanglesProject(const QStyleOptionViewItem& opt,
                                                                       IWksItem& item) const {
  const QFont fontName = opt.font;
  const QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - itemStatusControl.statusSizePrj);
  const QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin);
  const int buttonTop = r.top();
  const int buttonWidth = fmName.height();
  const int buttonHeight = buttonWidth;
  const bool isOnDevice = item.isOnDevice() != IWksItem::eTypeNone;

  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());

  // Set rectName initially to span the complete width. The right edge
  // will be adjusted in the and by the last button rectangle.
  QRect rectName(rectIcon.right() + 2 * kMargin, r.top(), r.width() - rectIcon.width(), fmName.height());
  QRect rectVisible(r.right() - buttonWidth, buttonTop, buttonWidth, buttonHeight);

  // Tool buttons are added left of the rectVisible, growing further left
  // with each button.
  button_row_cursor_t buttons{rectVisible.left() - buttonWidth - kMargin, buttonTop, buttonWidth};

  // All tool button rectangles are initially invalid. If a tool button is need
  // the rectangle is set to a valid rectangle at the correct position.
  // As a consequence the code using the rectangles simply tests if they
  // are valid. Buttons with non valid rectangles are skipped.
  QRect rectActiveProject;
  QRect rectSave;
  QRect rectAutoSyncDev;

  if (isOnDevice == false && item.type() != IWksItem::eTypeLostFound) {
    if (item.holdUiFocus(opt)) {
      rectActiveProject = buttons.next();
      rectSave = buttons.next();

      if (treeWidget->hasDeviceSupport()) {
        rectAutoSyncDev = buttons.next();
      }
    } else {
      if (item.hasUserFocus()) {
        rectActiveProject = buttons.next();
      }
      if (item.isChanged() && !item.isAutoSave()) {
        rectSave = buttons.next();
      }
    }
  } else if (isOnDevice == true) {
    if (item.holdUiFocus(opt)) {
      rectSave = buttons.next();
    }
  }
  // As rectName should span up to the right of the last button left has
  // to be corrected by a button width.
  rectName.setRight(buttons.left + buttonWidth - 2 * kMargin);

  QRect rectStatus;
  if (itemStatusControl.statusSizePrj != kFontSizeInvalid) {
    rectStatus.setRect(rectIcon.right() + 2 * kMargin, r.bottom() - fmStatus.height(),
                       r.width() - rectIcon.width() - 2 * kMargin, fmStatus.height());
  }

  const QRect rectProgress(rectIcon.right() + 4 * kMargin, r.bottom() - kProgressBarHeight,
                           r.width() - rectIcon.width() - 8 * kMargin, kProgressBarHeight);

  return {fontName,     fontStatus,  rectIcon, rectName,          rectStatus,
          rectProgress, rectVisible, rectSave, rectActiveProject, rectAutoSyncDev};
}

CWksItemDelegate::ItemLayout CWksItemDelegate::getRectanglesItem(const QStyleOptionViewItem& opt,
                                                                 const IWksItem& item) const {
  QFont fontName = opt.font;
  QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - itemStatusControl.statusSizeItem);
  QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin);
  const int buttonTop = r.top();
  const int buttonWidth = fmName.height();

  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());

  // Set rectName initially to span the complete width. The right edge
  // will be adjusted in the and by the last button rectangle.
  QRect rectName(rectIcon.right() + 2 * kMargin, r.top(), r.width() - rectIcon.width(), fmName.height());

  // Tool buttons are added left of the right edge, growing further left
  // with each button.
  button_row_cursor_t buttons{r.right() - buttonWidth - kMargin, buttonTop, buttonWidth};

  // All tool button rectangles are initially invalid. If a tool button is need
  // the rectangle is set to a valid rectangle at the correct position.
  // As a consequence the code using the rectangles simply tests if they
  // are valid. Buttons with non valid rectangles are skipped.
  QRect rectChanged;

  const CGeoSearch* search = dynamic_cast<const CGeoSearch*>(item.parent());
  const bool isOnGeoSearch = search != nullptr;
  if (item.isChanged() && !item.isOnDevice() && !isOnGeoSearch) {
    rectChanged = buttons.next();
  }

  // As rectName should span up to the right of the last button left has
  // to be corrected by a button width.
  rectName.setRight(buttons.left + buttonWidth - 2 * kMargin);

  QRect rectStatus;
  if (itemStatusControl.statusSizeItem != kFontSizeInvalid) {
    rectStatus = QRect(rectIcon.right() + 2 * kMargin, r.bottom() - fmStatus.height(),
                       r.width() - rectIcon.width() - 2 * kMargin, fmStatus.height());
  }

  return {fontName, fontStatus, rectIcon, rectName, rectStatus, rectChanged};
}

CWksItemDelegate::DeviceLayout CWksItemDelegate::getRectanglesDevice(const QStyleOptionViewItem& opt,
                                                                     const IWksItem& item) const {
  QFont fontName = opt.font;
  QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffProject);
  QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin);
  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());
  const QRect rectVisible(r.right() - fmName.height(), r.top(), fmName.height(), fmName.height());
  const QRect rectName(rectIcon.right() + 2 * kMargin, r.top(),
                       r.width() - rectIcon.width() - rectVisible.width() - 2 * kMargin, fmName.height());
  const QRect rectStatus(rectIcon.right() + 2 * kMargin, r.bottom() - fmStatus.height(),
                         r.width() - rectIcon.width() - 2 * kMargin, fmStatus.height());
  const QRect rectProgress(rectIcon.right() + 4 * kMargin, r.bottom() - kProgressBarHeight,
                           r.width() - rectIcon.width() - 8 * kMargin, kProgressBarHeight);

  return {fontName, fontStatus, rectIcon, rectName, rectStatus, rectProgress, rectVisible};
}

CWksItemDelegate::GeoSearchLayout CWksItemDelegate::getRectanglesGeoSearch(const QStyleOptionViewItem& opt) const {
  const QFont fontSearch = opt.font;
  const QFontMetrics fmSearch(fontSearch);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffProject);
  const QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin);
  const quint32 height = r.height() / 2;

  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());
  const QRect rectSetup(rectIcon.right() + kMargin, r.top(), height, height);
  const QRect rectVisible(r.right() - fmSearch.height(), r.top(), height, height);
  const QRect rectWptIcon(rectVisible.left() - height - kMargin, r.top(), height, height);
  const QRect rectLineEdit(
      rectSetup.right() + kMargin, r.top(),
      r.width() - rectSetup.width() - rectIcon.width() - rectWptIcon.width() - rectVisible.width() - 4 * kMargin,
      height + 4 * kMargin);
  const QRect rectStatus(
      rectSetup.right() + kMargin, r.bottom() - fmStatus.height(),
      r.width() - rectSetup.width() - rectIcon.width() - rectWptIcon.width() - rectVisible.width() - 4 * kMargin,
      fmStatus.height());

  return {fontSearch, fontStatus, rectIcon, rectSetup, rectLineEdit, rectStatus, rectWptIcon, rectVisible};
}

CWksItemDelegate::GeoSearchErrorLayout CWksItemDelegate::getRectanglesGeoSearchError(
    const QStyleOptionViewItem& opt) const {
  const QFont font = opt.font;

  const QRect& r = opt.rect.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin);
  // clang-format off
  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());
  const QRect& rectName = r.adjusted(rectIcon.width() + kMargin,0,0,0);
  // clang-format on

  return {font, rectIcon, rectName};
}
void CWksItemDelegate::drawToolButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect,
                                      const QIcon& icon, bool enabled, bool pressed) {
  QStyleOptionToolButton btnOpt;
  btnOpt.initFrom(opt.widget);
  btnOpt.rect = rect;
  btnOpt.icon = icon;
  btnOpt.iconSize = rect.adjusted(2 * kMargin, 2 * kMargin, -2 * kMargin, -2 * kMargin).size();
  btnOpt.toolButtonStyle = Qt::ToolButtonIconOnly;
  btnOpt.subControls = QStyle::SC_ToolButton;
  btnOpt.activeSubControls = QStyle::SC_ToolButton;
  btnOpt.state =
      (enabled ? QStyle::State_Enabled : QStyle::State_None) | (pressed ? QStyle::State_Sunken : QStyle::State_Raised);
  opt.widget->style()->drawComplexControl(QStyle::CC_ToolButton, &btnOpt, p, opt.widget);
}

void CWksItemDelegate::drawProgressBar(QPainter* p, const QRect& rect, qreal progress) {
  quint32 width = qRound(rect.width() * progress / 100.0);
  const QLine line(rect.left(), rect.bottom() - kProgressBarHeightHalf, rect.left() + width,
                   rect.bottom() - kProgressBarHeightHalf);
  p->setPen(QPen(Qt::white, 5, Qt::SolidLine, Qt::RoundCap));
  p->drawLine(line);
  p->setPen(QPen(Qt::darkGreen, 3, Qt::SolidLine, Qt::RoundCap));
  p->drawLine(line);
}

QString CWksItemDelegate::distanceAscentDescentStatus(qreal distance, qreal ascent, qreal descent,
                                                      bool elevationInvalid, bool showDistance, bool showAscent,
                                                      bool showDescent) {
  QString status;

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
  const qint32 N = qRound(rating);
  if (rating != 0) {
    QRect rectStar(rectStatus.left() + kMargin, rectStatus.top() + kMargin, rectStatus.height() - 2 * kMargin,
                   rectStatus.height() - 2 * kMargin);
    for (int i = 0; i < N; i++) {
      QIcon("://icons/cache/32x32/star.png").paint(p, rectStar, Qt::AlignCenter, iconMode);
      rectStar.translate(kMargin + rectStar.width(), 0);
    }
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
  const QPixmap& icon = item.getIcon().scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, item.isVisible() ? QIcon::Normal : QIcon::Disabled);

  // draw tool button to toggle visibility
  drawToolButton(p, opt, layout.rectVisible,
                 isVisible ? QIcon(":/icons/32x32/ShowAll.png") : QIcon(":/icons/32x32/ShowNone.png"), true, isVisible);

  const float opacityOfFocusBasedItems = item.getOpacityOfFocusBasedItems();

  if (layout.rectSave.isValid()) {
    if (item.isOnDevice() == false) {
      // draw save/ auto save button
      if (item.isChanged() && !item.isAutoSave()) {
        // show save button
        drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/32x32/Save.png"), true, false);
      } else {
        p->setOpacity(opacityOfFocusBasedItems);
        if (item.isAutoSave()) {
          // show auto save button pressed, to disable autosave
          drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/32x32/AutoSaveA.png"), true, true);
        } else if (item.canSave()) {
          // show auto save button only if project can be saved
          drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/32x32/AutoSaveNoA.png"), true, false);
        }
        p->setOpacity(1.0);
      }
    } else {
      p->setOpacity(opacityOfFocusBasedItems);
      drawToolButton(p, opt, layout.rectSave, QIcon(":/icons/32x32/Copy.png"), true, false);
      p->setOpacity(1.0);
    }
  }

  if (layout.rectAutoSyncDev.isValid()) {
    p->setOpacity(opacityOfFocusBasedItems);
    // auto sync. w. dev.
    if (item.isAutoSyncToDev()) {
      drawToolButton(p, opt, layout.rectAutoSyncDev, QIcon(":/icons/32x32/DeviceSync.png"), true, true);
    } else {
      drawToolButton(p, opt, layout.rectAutoSyncDev, QIcon(":/icons/32x32/DeviceNoSync.png"), true, false);
    }
    p->setOpacity(1.0);
  }

  if (layout.rectActiveProject.isValid()) {
    if (item.holdUiFocus(opt)) {
      p->setOpacity(opacityOfFocusBasedItems);
      drawToolButton(p, opt, layout.rectActiveProject,
                     item.hasUserFocus() ? QIcon(":/icons/32x32/Focus.png") : QIcon(":/icons/32x32/UnFocus.png"), true,
                     true);
      p->setOpacity(1.0);
    } else {
      QIcon(":/icons/32x32/Focus.png")
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
      const IGisProject* const prj = dynamic_cast<IGisProject*>(&item);
      const qint32 cntTrk = prj->getItemCountByType(IGisItem::eTypeTrk);
      if (cntTrk != 0) {
        status += QString("T: %1 ").arg(cntTrk);
      }
      const qint32 cntWpt = prj->getItemCountByType(IGisItem::eTypeWpt);
      if (cntWpt != 0) {
        status += QString("W: %1 ").arg(cntWpt);
      }
      const qint32 cntRte = prj->getItemCountByType(IGisItem::eTypeRte);
      if (cntRte != 0) {
        status += QString("R: %1 ").arg(cntRte);
      }
      const qint32 cntArea = prj->getItemCountByType(IGisItem::eTypeOvl);
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
  const QPixmap& icon = item.getIcon().scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, isVisible ? QIcon::Normal : QIcon::Disabled);

  // draw tool button to activate
  drawToolButton(p, opt, layout.rectVisible,
                 isVisible ? QIcon(":/icons/32x32/ShowAll.png") : QIcon(":/icons/32x32/ShowNone.png"), true, isVisible);

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
        if (ele != NOFLOAT && ele != NOFLOAT && itemStatusControl.wpt.elevation) {
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
  const QPixmap& icon = item.getIcon().scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, iconMode);

  // draw save/changed icon
  if (layout.rectChanged.isValid()) {
    QIcon(":/icons/32x32/Save.png")
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

  const QPixmap& icon = item.getIcon().scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, isVisible ? QIcon::Normal : QIcon::Disabled);
  QIcon(":/icons/32x32/Apply.png").paint(p, layout.rectSetup, Qt::AlignCenter, QIcon::Normal);
  QIcon(search->getWptIcon()).paint(p, layout.rectWptIcon, Qt::AlignCenter, QIcon::Normal);

  // draw tool button to activate
  drawToolButton(p, opt, layout.rectVisible,
                 isVisible ? QIcon(":/icons/32x32/ShowAll.png") : QIcon(":/icons/32x32/ShowNone.png"), true, isVisible);

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

  const QPixmap& icon = item.getIcon().scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter, isVisible ? QIcon::Normal : QIcon::Disabled);
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
    if (item.isOnDevice() == false) {
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
    if (item.isOnDevice() == false) {
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
