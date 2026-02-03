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
#include "gis/prj/IGisProject.h"
#include "gis/search/CGeoSearch.h"
#include "helpers/CDraw.h"

constexpr int kMargin = 2;
constexpr int kFontSizeDiffProject = 2;
constexpr int kFontSizeDiffItem = 3;

CWksItemDelegate::CWksItemDelegate(CGisListWks* parent) : QStyledItemDelegate(parent), treeWidget(parent) {}

IWksItem* CWksItemDelegate::indexToItem(const QModelIndex& index) const {
  IWksItem* item = dynamic_cast<IWksItem*>(treeWidget->itemFromIndex(index));
  return item;
}

std::tuple<QFont, QFont, QRect, QRect, QRect, QRect, QRect, QRect, QRect> CWksItemDelegate::getRectanglesProject(
    const QStyleOptionViewItem& opt) {
  QFont fontName = opt.font;
  QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffProject);
  QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  const QRect& rectIcon = r.adjusted(-kMargin, -kMargin, -(r.width() - r.height()), kMargin);

  const int top = r.top();
  const int buttonHeight = fmName.height();  // std::max(16, fmName.height());

  // clang-format off
  const QRect& rectAutoSyncDev = QRect(
    r.right() - 3 * kMargin - 4 * buttonHeight,
    top,
    buttonHeight,
    buttonHeight);

  const QRect& rectActiveProject = QRect(
    r.right() - 2 * kMargin - 3 * buttonHeight,
    top,
    buttonHeight,
    buttonHeight);

  const QRect& rectSave = QRect(
    r.right() - 1 * kMargin - 2 * buttonHeight,
    top,
    buttonHeight,
    buttonHeight);

  const QRect& rectVisible = QRect(
    r.right() - 1 * buttonHeight,
    top,
    buttonHeight,
    buttonHeight);

  const QRect& rectName =QRect(
    rectIcon.right() + kMargin,
    r.top(),
    r.width() - rectIcon.width() - 2 * kMargin,
    fmName.height());

  const QRect& rectStatus = QRect(
    rectIcon.right() + kMargin,
    r.bottom() - fmStatus.height(),
    r.width() - rectIcon.width() - 2 * kMargin,
    fmStatus.height());

  // clang-format on

  return {fontName,    fontStatus, rectIcon,          rectName,       rectStatus,
          rectVisible, rectSave,   rectActiveProject, rectAutoSyncDev};
}

std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> CWksItemDelegate::getRectanglesItem(
    const QStyleOptionViewItem& opt) {
  QFont fontName = opt.font;
  QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffItem);
  QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  const QRect& rectIcon = r.adjusted(-kMargin, -kMargin, -(r.width() - r.height()), kMargin);

  // clang-format off
  const QRect& rectChanged = QRect(
    r.right() - fmName.height(),
    r.top(),
    fmName.height(),
    fmName.height());

  const QRect& rectName = QRect(
    rectIcon.right() + kMargin,
    r.top(),
    rectChanged.left() - rectIcon.right() - 2*kMargin,
    fmName.height());

  const QRect& rectStatus = QRect(
    rectIcon.right() + kMargin,
    r.bottom() - fmStatus.height(),
    r.right() - rectIcon.right() - 2*kMargin,
    fmStatus.height());
  // clang-format on

  return {fontName, fontStatus, rectIcon, rectName, rectStatus, rectChanged};
}

std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> CWksItemDelegate::getRectanglesDevice(
    const QStyleOptionViewItem& opt) {
  QFont fontName = opt.font;
  QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffProject);
  QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  const QRect& rectIcon = r.adjusted(-kMargin, -kMargin, -(r.width() - r.height()), kMargin);

  // clang-format off
  const QRect& rectVisible = QRect(
    r.right() - 1 * (kMargin + fmStatus.height()),
    r.top() + fmName.height() + kMargin,
    fmStatus.height(),
    fmStatus.height());

  const QRect& rectName = QRect(
    rectIcon.right() + kMargin,
    r.top(),
    r.width() - rectIcon.width() - 2 * kMargin,
    fmName.height());

  const QRect& rectStatus = QRect(
    rectIcon.right() + kMargin,
    r.bottom() - fmStatus.height(),
    r.width() - rectIcon.width() - rectVisible.width() - 2 * kMargin,
    fmStatus.height());
  // clang-format on

  return {fontName, fontStatus, rectIcon, rectName, rectStatus, rectVisible};
}

std::tuple<QFont, QRect, QRect, QRect, QRect, QRect> CWksItemDelegate::getRectanglesGeoSearch(
    const QStyleOptionViewItem& opt) {
  const QFont font = opt.font;
  const QFontMetrics fm(font);

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  // clang-format off
  const QRect rectIcon = QRect(r.left(), r.top(), r.height(), r.height());
  const QRect rectSetup = QRect(
    rectIcon.right() + kMargin,
    r.top(),
    r.height(),
    r.height());
  const QRect rectVisible = QRect(
    r.right() - fm.height(),
    r.top(),
    fm.height(),
    fm.height());
  const QRect rectWptIcon = QRect(
    rectVisible.left() - r.height() - kMargin,
    r.top(),
    r.height(),
    r.height());
  
  const QRect rectLineEdit = QRect(
    rectSetup.right() + kMargin,
    r.top(),
    r.width() - rectSetup.width() - rectIcon.width() - rectWptIcon.width() - rectVisible.width() - 4 * kMargin,
    r.height());
  // clang-format on

  return {font, rectIcon, rectSetup, rectLineEdit, rectWptIcon, rectVisible};
}

std::tuple<QFont, QRect, QRect> CWksItemDelegate::getRectanglesGeoSearchError(const QStyleOptionViewItem& opt) {
  const QFont font = opt.font;

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  // clang-format off
  const QRect rectIcon = QRect(r.left(), r.top(), r.height(), r.height());
  const QRect rectName = r.adjusted(rectIcon.width() + kMargin,0,0,0);
  // clang-format on

  return {font, rectIcon, rectName};
}
void CWksItemDelegate::drawToolButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect,
                                      const QIcon& icon, bool enabled, bool pressed) {
  QStyleOptionToolButton btnOpt;
  btnOpt.initFrom(opt.widget);
  btnOpt.rect = rect;
  btnOpt.icon = icon;
  btnOpt.iconSize = rect.adjusted(kMargin, kMargin, -kMargin, -kMargin).size();
  btnOpt.toolButtonStyle = Qt::ToolButtonIconOnly;
  btnOpt.subControls = QStyle::SC_ToolButton;
  btnOpt.activeSubControls = QStyle::SC_ToolButton;
  btnOpt.state =
      (enabled ? QStyle::State_Enabled : QStyle::State_None) | (pressed ? QStyle::State_Sunken : QStyle::State_Raised);
  opt.widget->style()->drawComplexControl(QStyle::CC_ToolButton, &btnOpt, p, opt.widget);
}

QSize CWksItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  const IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return QSize();
  }

  QFont font1 = opt.font;
  font1.setBold(true);
  QFontMetrics fm1(font1);

  QFont font2 = opt.font;
  font2.setPointSize(font2.pointSize() - kFontSizeDiffProject);
  QFontMetrics fm2(font2);

  QFont font3 = opt.font;
  font3.setPointSize(font3.pointSize() - kFontSizeDiffItem);
  QFontMetrics fm3(font3);

  // qDebug() << fm1.height() << fm2.height() << (3 * kMargin + fm1.height() + fm2.height());
  // qDebug() << fm1.height() << fm3.height() << (3 * kMargin + fm1.height() + fm3.height());

  switch (item->getBaseType()) {
    case IWksItem::eBaseType::Project:
      return QSize(opt.rect.width(), std::max(32, 3 * kMargin + fm1.height() + fm2.height()));
    case IWksItem::eBaseType::Device:
      return QSize(opt.rect.width(), std::max(32, 3 * kMargin + fm1.height() + fm2.height()));
    case IWksItem::eBaseType::Item:
      return QSize(opt.rect.width(), std::max(22, 3 * kMargin + fm1.height() + fm3.height()));
    case IWksItem::eBaseType::GeoSearch:
      return QSize(opt.rect.width(), std::max(22, 2 * kMargin + fm1.height()));
  }
  return QStyledItemDelegate::sizeHint(opt, index);
}

void CWksItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
  QStyledItemDelegate::initStyleOption(option, index);
}

void CWksItemDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return;
  }

  p->save();
  USE_ANTI_ALIASING(*p, true);
  // Standard background & selection
  QStyledItemDelegate::paint(p, opt, index);

  switch (item->getBaseType()) {
    case IWksItem::eBaseType::Project:
      paintProject(p, opt, index, item);
      break;

    case IWksItem::eBaseType::Item:
      paintItem(p, opt, index, item);
      break;

    case IWksItem::eBaseType::Device:
      paintDevice(p, opt, index, item);
      break;

    case IWksItem::eBaseType::GeoSearch:
      paintGeoSearch(p, opt, index, item);
      break;

    case IWksItem::eBaseType::GeoSearchError:
      paintGeoSearchError(p, opt, index, item);
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
                                    const IWksItem* item) const {
  const IGisProject* project = dynamic_cast<const IGisProject*>(item);
  if (project == nullptr) {
    return;
  }

  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectVisible, rectSave, rectActiveProject,
        rectAutoSyncDev] = getRectanglesProject(opt);

  const bool isOnDevice = item->isOnDevice() != IWksItem::eTypeNone;
  const bool isVisible = item->isVisible();
  const QColor& colorName =
      opt.palette.color(isVisible ? QPalette::Active : QPalette::Disabled,
                        opt.state & QStyle::State_Selected ? QPalette::BrightText : QPalette::WindowText);

  // draw icon
  QIcon(item->getIcon()).paint(p, rectIcon, Qt::AlignCenter, item->isVisible() ? QIcon::Normal : QIcon::Disabled);

  // draw tool button to toggle visibility
  drawToolButton(p, opt, rectVisible,
                 isVisible ? QIcon(":/icons/32x32/ShowAll.png") : QIcon(":/icons/32x32/ShowNone.png"), true, isVisible);
  rectName.setRight(rectVisible.left() - kMargin);

  if (isOnDevice == false) {
    // draw save/ auto save button
    if (item->isChanged() && !item->isAutoSave()) {
      // show save button
      drawToolButton(p, opt, rectSave, QIcon(":/icons/32x32/Save.png"), true, false);
    } else {
      if (item->isAutoSave()) {
        // show auto save button pressed, to disable autosave
        drawToolButton(p, opt, rectSave, QIcon(":/icons/32x32/AutoSaveA.png"), true, true);
      } else if (item->canSave()) {
        // show auto save button only if project can be saved
        drawToolButton(p, opt, rectSave, QIcon(":/icons/32x32/AutoSaveNoA.png"), true, false);
      }
    }

    // active project
    if (item->hasUserFocus()) {
      drawToolButton(p, opt, rectActiveProject, QIcon(":/icons/32x32/Focus.png"), true, true);
    } else {
      drawToolButton(p, opt, rectActiveProject, QIcon(":/icons/32x32/Focus.png"), false, false);
    }
    rectName.setRight(rectActiveProject.left() - kMargin);

    // auto sync. w. dev.
    if (treeWidget->hasDeviceSupport()) {
      if (item->isAutoSyncToDev()) {
        drawToolButton(p, opt, rectAutoSyncDev, QIcon(":/icons/32x32/DeviceSync.png"), true, true);
      } else {
        drawToolButton(p, opt, rectAutoSyncDev, QIcon(":/icons/32x32/DeviceNoSync.png"), true, false);
      }
      rectName.setRight(rectAutoSyncDev.left() - kMargin);
    }
  }

  // draw name
  fontName.setBold(item->hasUserFocus());
  p->setPen(colorName);
  p->setFont(fontName);
  p->drawText(rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop,
              isOnDevice ? project->getName() : project->getNameEx());

  QString status;
  const QString& keywords = project->getKeywords();
  if (!keywords.isEmpty()) {
    status += keywords + " ";
  }
  // int cnt = project->getItemCountByType(IGisItem::eTypeTrk);
  // if (cnt) {
  //   status += tr("T: %1 ").arg(cnt);
  // }
  // cnt = project->getItemCountByType(IGisItem::eTypeWpt);
  // if (cnt) {
  //   status += tr("W: %1 ").arg(cnt);
  // }
  // cnt = project->getItemCountByType(IGisItem::eTypeRte);
  // if (cnt) {
  //   status += tr("R: %1 ").arg(cnt);
  // }
  // cnt = project->getItemCountByType(IGisItem::eTypeOvl);
  // if (cnt) {
  //   status += tr("A: %1 ").arg(cnt);
  // }
  p->setPen(colorName);
  p->setFont(fontStatus);
  p->drawText(rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
}

void CWksItemDelegate::paintDevice(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                   const IWksItem* item) const {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectVisible] = getRectanglesDevice(opt);

  const bool isVisible = true;
  const QColor& colorName =
      opt.palette.color(isVisible ? QPalette::Active : QPalette::Disabled,
                        opt.state & QStyle::State_Selected ? QPalette::BrightText : QPalette::WindowText);

  // draw name
  fontName.setBold(item->hasUserFocus());
  p->setPen(colorName);
  p->setFont(fontName);
  p->drawText(rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item->getName());

  // draw status
  p->setPen(colorName);
  p->setFont(fontStatus);
  p->drawText(rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item->getToolTipName());

  // draw icon
  QIcon(item->getIcon()).paint(p, rectIcon, Qt::AlignCenter, isVisible ? QIcon::Normal : QIcon::Disabled);

  // draw tool button to activate
  drawToolButton(p, opt, rectVisible,
                 item->isVisible() ? QIcon(":/icons/32x32/ShowAll.png") : QIcon(":/icons/32x32/ShowNone.png"), true,
                 item->isVisible());
}

void CWksItemDelegate::paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                 const IWksItem* item) const {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectChanged] = getRectanglesItem(opt);

  const bool isVisible = item->isVisible();
  const QIcon::Mode iconMode = isVisible ? QIcon::Normal : QIcon::Disabled;
  const QColor& colorName =
      opt.palette.color(isVisible ? QPalette::Active : QPalette::Disabled,
                        opt.state & QStyle::State_Selected ? QPalette::BrightText : QPalette::WindowText);

  // draw name
  fontName.setBold(item->hasUserFocus());
  p->setPen(colorName);
  p->setFont(fontName);
  p->drawText(rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignVCenter, item->getName());

  // draw rating
  qint32 rating = qRound(item->getRating());
  if (rating != 0) {
    QRect rectStar(rectStatus.left(), rectStatus.top(), rectStatus.height(), rectStatus.height());
    for (int i = 0; i < qRound(item->getRating()); i++) {
      QIcon("://icons/cache/32x32/star.png").paint(p, rectStar, Qt::AlignCenter, iconMode);
      rectStar.translate(kMargin + rectStar.width(), 0);
    }
    rectStatus.setLeft(rectStar.left() + kMargin);
  }

  // draw tags
  const QSet<QString>& tags = item->getTags();
  if (!tags.isEmpty()) {
    // draw tags
    p->setFont(fontStatus);
    p->drawText(rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignVCenter, tags.values().join(", "));
  }

  // draw icon
  QIcon(item->getIcon()).paint(p, rectIcon, Qt::AlignCenter, iconMode);

  // draw save/changed icon
  const CGeoSearch* search = dynamic_cast<const CGeoSearch*>(item->parent());
  const bool isOnGeoSearch = search != nullptr;
  if (item->isChanged() && !item->isOnDevice() && !isOnGeoSearch) {
    QIcon(":/icons/32x32/Save.png")
        .paint(p, rectChanged.adjusted(kMargin, kMargin, -kMargin, -kMargin), Qt::AlignCenter, iconMode);
  }
}

void CWksItemDelegate::paintGeoSearch(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                      const IWksItem* item) const {
  const CGeoSearch* search = dynamic_cast<const CGeoSearch*>(item);
  if (search == nullptr) {
    return;
  }
  auto [font, rectIcon, rectSetup, rectLineEdit, rectWptIcon, rectVisible] = getRectanglesGeoSearch(opt);
  const bool isVisible = item->isVisible();

  QIcon(item->getIcon()).paint(p, rectIcon, Qt::AlignCenter, isVisible ? QIcon::Normal : QIcon::Disabled);
  QIcon(":/icons/32x32/Apply.png").paint(p, rectSetup, Qt::AlignCenter, QIcon::Normal);
  QIcon(search->getWptIcon()).paint(p, rectWptIcon, Qt::AlignCenter, QIcon::Normal);

  // draw tool button to activate
  drawToolButton(p, opt, rectVisible,
                 isVisible ? QIcon(":/icons/32x32/ShowAll.png") : QIcon(":/icons/32x32/ShowNone.png"), true, isVisible);

  const QString& address = search->getLastAddress();
  const QColor& color = opt.palette.color(
      isVisible && search->isInputEnabled() && !address.isEmpty() ? QPalette::Active : QPalette::Disabled,
      QPalette::WindowText);

  p->setPen(color);
  p->setFont(font);
  p->drawText(rectLineEdit.adjusted(kMargin, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft,
              address.isEmpty() ? tr("Enter address...") : address);
}

void CWksItemDelegate::paintGeoSearchError(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                           const IWksItem* item) const {
  auto [font, rectIcon, rectName] = getRectanglesGeoSearchError(opt);
  const bool isVisible = item->isVisible();

  QIcon(item->getIcon()).paint(p, rectIcon, Qt::AlignCenter, isVisible ? QIcon::Normal : QIcon::Disabled);
  const QColor& color = opt.palette.color(isVisible ? QPalette::Active : QPalette::Disabled, QPalette::WindowText);

  p->setPen(color);
  p->setFont(font);
  p->drawText(rectName.adjusted(kMargin, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, item->getName());
}

bool CWksItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                                   const QModelIndex& index) {
  IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return false;
  }

  if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);

    switch (item->getBaseType()) {
      case IWksItem::eBaseType::Project:
        return mousePressProject(me, opt, index, item);
      case IWksItem::eBaseType::Device:
        return mousePressDevice(me, opt, index, item);
      case IWksItem::eBaseType::GeoSearch:
        return mousePressGeoSearch(me, opt, index, item);
      default:;
    }
  }
  return QStyledItemDelegate::editorEvent(event, model, opt, index);
}

bool CWksItemDelegate::mousePressProject(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                         IWksItem* item) {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectVisible, rectSave, rectActiveProject,
        rectAutoSyncDev] = getRectanglesProject(opt);
  if (rectVisible.contains(me->pos())) {
    item->setVisibility(!item->isVisible());
    emit sigUpdateCanvas();
    return true;
  } else if (rectSave.contains(me->pos())) {
    if (item->isAutoSave()) {
      item->setAutoSave(false);
    } else {
      if (item->isChanged()) {
        IGisProject* project = dynamic_cast<IGisProject*>(item);
        if (project == nullptr) {
          return false;
        }
        if (project->canSave()) {
          project->save();
        } else {
          project->saveAs();
        }
      } else {
        item->setAutoSave(true);
      }
    }
    return true;
  } else if (rectAutoSyncDev.contains(me->pos())) {
    item->setAutoSyncToDev(!item->isAutoSyncToDev());
    return true;

  } else if (rectActiveProject.contains(me->pos())) {
    IGisProject* project = dynamic_cast<IGisProject*>(item);
    if (project == nullptr) {
      return false;
    }
    treeWidget->setUserFocus(project->getKey(), !project->hasUserFocus());
    return true;
  }

  return false;
}

bool CWksItemDelegate::mousePressDevice(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                        IWksItem* item) {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectVisible] = getRectanglesDevice(opt);

  if (rectVisible.contains(me->pos())) {
    item->setVisibility(!item->isVisible());
    emit sigUpdateCanvas();
    return true;
  }
  return false;
}

bool CWksItemDelegate::mousePressGeoSearch(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                           IWksItem* item) {
  CGeoSearch* search = dynamic_cast<CGeoSearch*>(item);
  if (search == nullptr) {
    return false;
  }

  auto [QFont, rectIcon, rectSetup, rectLineEdit, rectWptIcon, rectVisible] = getRectanglesGeoSearch(opt);
  if (rectVisible.contains(me->pos())) {
    item->setVisibility(!item->isVisible());
    emit sigUpdateCanvas();
    return true;
  }
  if (rectSetup.contains(me->pos())) {
    search->selectService(rectSetup);
    return true;
  }
  if (rectWptIcon.contains(me->pos())) {
    search->changeSymbol();
    return true;
  }

  return false;
}

bool CWksItemDelegate::helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                                 const QModelIndex& index) {
  if (!event || !view) {
    return false;
  }
  IWksItem* item = indexToItem(index);
  if (item == nullptr) {
    return false;
  }

  const QPoint& pos = event->pos();
  const QPoint& posGlobal = event->globalPos();
  switch (item->getBaseType()) {
    case IWksItem::eBaseType::Project:
      return helpEventProject(pos, posGlobal, view, opt, item);
    case IWksItem::eBaseType::Item:
      return helpEventItem(pos, posGlobal, view, opt, item);
    case IWksItem::eBaseType::GeoSearch:
      return helpEventGeoSearch(pos, posGlobal, view, opt, item);
    case IWksItem::eBaseType::GeoSearchError:
      return helpEventGeoSearchError(pos, posGlobal, view, opt, item);
    default:;
  }

  return false;
}

bool CWksItemDelegate::helpEventProject(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                        const QStyleOptionViewItem& opt, const IWksItem* item) {
  bool ret = false;
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectVisible, rectSave, rectActiveProject,
        rectAutoSyncDev] = getRectanglesProject(opt);
  if (rectName.contains(pos) || rectStatus.contains(pos)) {
    QToolTip::showText(posGlobal, item->getToolTipName(), view);
    ret = true;
  } else if (rectVisible.contains(pos)) {
    if (item->isVisible()) {
      QToolTip::showText(posGlobal, trRichText("Hide project on map."), view, {}, 3000);
    } else {
      QToolTip::showText(posGlobal, trRichText("Show project on map."), view, {}, 3000);
    }
    ret = true;
  } else if (rectSave.contains(pos)) {
    if (item->isChanged() && !item->isAutoSave()) {
      QToolTip::showText(posGlobal, trRichText("Save project."), view, {}, 3000);
    } else {
      if (item->isAutoSave()) {
        QToolTip::showText(posGlobal, trRichText("Disable auto save."), view, {}, 3000);
      } else if (item->canSave()) {
        QToolTip::showText(posGlobal, trRichText("Enable auto save."), view, {}, 3000);
      }
    }
    ret = true;
  } else if (rectAutoSyncDev.contains(pos)) {
    if (item->isAutoSyncToDev()) {
      QToolTip::showText(posGlobal, trRichText("Disable automatic synchonization with device."), view, {}, 3000);
    } else {
      QToolTip::showText(posGlobal, trRichText("Enable automatic synchonization with device."), view, {}, 3000);
    }
    ret = true;
  } else if (rectActiveProject.contains(pos)) {
    if (item->hasUserFocus()) {
      QToolTip::showText(
          posGlobal,
          trRichText("This is the active project. All new items will be attached to this project automatically."), view,
          {}, 5000);
    } else {
      QToolTip::showText(
          posGlobal,
          trRichText("Make this project the active one. All new items will be attached to this project automatically."),
          view, {}, 5000);
    }
  }
  return ret;
}

bool CWksItemDelegate::helpEventItem(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                     const QStyleOptionViewItem& opt, const IWksItem* item) {
  bool ret = false;
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectChanged] = getRectanglesItem(opt);

  if (rectName.contains(pos)) {
    QToolTip::showText(posGlobal, item->getToolTipName(), view);
    ret = true;
  } else if (rectChanged.contains(pos)) {
    if (item->isChanged()) {
      QToolTip::showText(posGlobal, trRichText("Item is changed and needs to be saved."), view, {}, 3000);
      ret = true;
    }
  }
  return ret;
}

bool CWksItemDelegate::helpEventGeoSearch(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                          const QStyleOptionViewItem& opt, const IWksItem* item) {
  auto [QFont, rectIcon, rectSetup, rectLineEdit, rectWptIcon, rectVisible] = getRectanglesGeoSearch(opt);
  if (rectSetup.contains(pos)) {
    QToolTip::showText(posGlobal, trRichText("Setup Search"), view, {}, 3000);
    return true;
  }

  return false;
}

bool CWksItemDelegate::helpEventGeoSearchError(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                                               const QStyleOptionViewItem& opt, const IWksItem* item) {
  QToolTip::showText(posGlobal, "<div>" + item->getToolTipName() + "</div>", view);
  return true;
}

QWidget* CWksItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& opt,
                                        const QModelIndex& index) const {
  CGeoSearch* search = dynamic_cast<CGeoSearch*>(indexToItem(index));
  if (search == nullptr || !search->isInputEnabled()) {
    return nullptr;
  }

  QLineEdit* editor = new QLineEdit(search->getLastAddress(), parent);
  editor->setPlaceholderText(tr("Enter address..."));

  return editor;
}

void CWksItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& opt,
                                            const QModelIndex& index) const {
  auto [QFont, rectIcon, rectSetup, rectLineEdit, rectWptIcon, rectVisible] = getRectanglesGeoSearch(opt);
  editor->setGeometry(rectLineEdit);
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
