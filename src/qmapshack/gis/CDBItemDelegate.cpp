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

#include "gis/CDBItemDelegate.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTextDocument>
#include <QToolTip>

#include "gis/IDBItem.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "misc.h"

constexpr int kMargin = 1;
constexpr int kFontSizeDiffItem = 3;
constexpr int kFontSizeInvalid = -1;

CDBItemDelegate::CDBItemDelegate(QTreeWidget* parent) : QStyledItemDelegate(parent), treeWidget(parent) {
  SETTINGS;
  cfg.beginGroup("Database");
  cfg.beginGroup("Decorations");
  itemStatusControl.folder.flags = cfg.value("folder", itemStatusControl.folder.flags).toUInt();
  itemStatusControl.item.flags = cfg.value("item", itemStatusControl.item.flags).toUInt();
  itemStatusControl.statusSizeFolder = cfg.value("statusSizeFolder", kFontSizeDiffItem).toInt();
  itemStatusControl.statusSizeItem = cfg.value("statusSizeItem", kFontSizeDiffItem).toInt();
  cfg.endGroup();  // Decorations
  cfg.endGroup();  // Database
}

void CDBItemDelegate::setStatusItemsControl(const item_status_ctrl_t& settings) {
  itemStatusControl = settings;

  SETTINGS;
  cfg.beginGroup("Database");
  cfg.beginGroup("Decorations");
  cfg.setValue("folder", itemStatusControl.folder.flags);
  cfg.setValue("item", itemStatusControl.item.flags);
  cfg.setValue("statusSizeFolder", itemStatusControl.statusSizeFolder);
  cfg.setValue("statusSizeItem", itemStatusControl.statusSizeItem);
  cfg.endGroup();  // Decorations
  cfg.endGroup();  // Database

  emit treeWidget->model()->layoutChanged();
}

IDBItem* CDBItemDelegate::indexToItem(const QModelIndex& index) const {
  IDBItem* item = dynamic_cast<IDBItem*>(treeWidget->itemFromIndex(index));
  return item;
}

void CDBItemDelegate::drawToolButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect, const QIcon& icon,
                                     bool enabled, bool pressed) {
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

void CDBItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {}

QSize CDBItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  const IDBItem* item = indexToItem(index);
  if (item == nullptr) {
    return QSize(opt.rect.width(), 22);
  }

  QFont fontName = opt.font;
  fontName.setBold(true);
  QFontMetrics fmName(fontName);

  QFont fontStatusFolder = opt.font;
  fontStatusFolder.setPointSize(fontName.pointSize() - itemStatusControl.statusSizeFolder);
  QFontMetrics fmStatusFolder(fontStatusFolder);

  QFont fontStatusItem = opt.font;
  fontStatusItem.setPointSize(fontName.pointSize() - itemStatusControl.statusSizeItem);
  QFontMetrics fmStatusItem(fontStatusItem);

  switch (item->type()) {
    case IDBItem::eTypeLostFound:
    case IDBItem::eTypeDatabase:
    case IDBItem::eTypeGroup:
    case IDBItem::eTypeProject:
    case IDBItem::eTypeOther:
      if (itemStatusControl.statusSizeFolder != kFontSizeInvalid) {
        return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height() + fmStatusFolder.height()));
      } else {
        return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height()));
      }

    case IDBItem::eTypeItem:
      if (itemStatusControl.statusSizeItem != kFontSizeInvalid) {
        return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height() + fmStatusItem.height()));
      } else {
        return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height()));
      }
  }

  return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fmName.height()));
}

std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> CDBItemDelegate::getRectanglesFolder(
    const QStyleOptionViewItem& opt, const IDBItem& item) const {
  const QFont fontName = opt.font;
  const QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - itemStatusControl.statusSizeFolder);
  const QFontMetrics fmStatus(fontStatus);

  const quint32 heightButton = fmName.height() + 3 * kMargin;

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());
  QRect rectButton;
  if (item.type() > IDBItem::eTypeGroup || item.type() == IDBItem::eTypeLostFound) {
    rectButton.setRect(r.right() - heightButton, r.top(), heightButton, heightButton);
  }
  const QRect rectName(rectIcon.right() + 4 * kMargin, r.top() + kMargin,
                       r.width() - rectIcon.width() - rectButton.width() - 6 * kMargin, fmName.height());

  QRect rectStatus;
  if (itemStatusControl.statusSizeFolder != kFontSizeInvalid) {
    rectStatus.setRect(rectIcon.right() + 4 * kMargin, r.bottom() - fmStatus.height() - kMargin,
                       r.width() - rectIcon.width() - rectButton.width() - 6 * kMargin, fmName.height());
  }

  return {fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton};
}

std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> CDBItemDelegate::getRectanglesItem(const QStyleOptionViewItem& opt,
                                                                                        const IDBItem& item) const {
  const QFont fontName = opt.font;
  const QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - itemStatusControl.statusSizeItem);
  const QFontMetrics fmStatus(fontStatus);

  const quint32 heightButton = fmName.height() + 3 * kMargin;

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());
  QRect rectButton;
  // If the item is part of the lost-and-found folder -> no button
  // Yeah. The logic is a bit weird.
  if (!(item.parent() && item.parent()->type() == IDBItem::eTypeLostFound)) {
    rectButton.setRect(r.right() - heightButton, r.top(), heightButton, heightButton);
  }

  const QRect rectName(rectIcon.right() + 4 * kMargin, r.top() + kMargin,
                       r.width() - rectIcon.width() - rectButton.width() - 6 * kMargin, fmName.height());

  QRect rectStatus;
  if (itemStatusControl.statusSizeItem != kFontSizeInvalid) {
    rectStatus.setRect(rectIcon.right() + 4 * kMargin, r.bottom() - fmStatus.height() - kMargin,
                       r.width() - rectIcon.width() - rectButton.width() - 6 * kMargin, fmName.height());
  }

  return {fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton};
}

void CDBItemDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  IDBItem* item = indexToItem(index);
  if (item == nullptr) {
    return QStyledItemDelegate::paint(p, opt, index);
  }

  p->save();
  USE_ANTI_ALIASING(*p, true);
  // Standard background & selection
  QStyledItemDelegate::paint(p, opt, index);

  switch (item->type()) {
    case IDBItem::eTypeLostFound:
    case IDBItem::eTypeDatabase:
    case IDBItem::eTypeGroup:
    case IDBItem::eTypeProject:
    case IDBItem::eTypeOther:
      paintFolder(p, opt, index, *item);
      break;
    case IDBItem::eTypeItem:
      paintItem(p, opt, index, *item);
      break;
  }

  p->restore();
}

void CDBItemDelegate::paintFolder(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                  const IDBItem& item) const {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectanglesFolder(opt, item);

  const QPixmap& icon = item.getIcon().scaled(rectIcon.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, rectIcon, Qt::AlignCenter);

  if (rectButton.isValid()) {
    switch (item.getCheckState()) {
      case Qt::Unchecked:
        drawToolButton(p, opt, rectButton, QIcon(":/icons/32x32/ToWksUnchecked.png"), true, false);
        break;
      case Qt::PartiallyChecked:
        drawToolButton(p, opt, rectButton, QIcon(":/icons/32x32/ToWksPartially.png"), true, true);
        break;
      case Qt::Checked:
        drawToolButton(p, opt, rectButton, QIcon(":/icons/32x32/ToWksChecked.png"), true, true);
        break;
    }
  }

  QPalette::ColorGroup colorGroup = (opt.state & QStyle::State_HasFocus) ? QPalette::Active : QPalette::Inactive;
  QColor color = opt.palette.color(
      colorGroup, opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::WindowText);

  if (item.type() > IDBItem::eTypeGroup) {
    fontName.setBold(item.getCheckState() != Qt::Unchecked);
    color = opt.palette.color(item.getCheckState() != Qt::PartiallyChecked ? colorGroup : QPalette::Disabled,
                              opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::WindowText);
  }

  p->setPen(color);
  p->setFont(fontName);
  p->drawText(rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item.getName());

  if (rectStatus.isValid()) {
    p->setFont(fontStatus);
    QString status;
    if (itemStatusControl.folder.countFolders) {
      auto [cntGroup, cntProject, cntOther] = item.getFolderCount();
      if (cntGroup != 0) {
        status += tr("Groups: %1 ").arg(cntGroup);
      }
      if (cntProject != 0) {
        status += tr("Projects: %1 ").arg(cntProject);
      }
      if (cntOther != 0) {
        status += tr("Other: %1 ").arg(cntOther);
      }
    }

    if (itemStatusControl.folder.countItems) {
      auto [cntWpt, cntTrk, cntRte, cntArea] = item.getItemCount();
      if (cntWpt != 0) {
        status += tr("W: %1 ").arg(cntWpt);
      }
      if (cntTrk != 0) {
        status += tr("T: %1 ").arg(cntTrk);
      }
      if (cntRte != 0) {
        status += tr("R: %1 ").arg(cntRte);
      }
      if (cntArea != 0) {
        status += tr("A: %1 ").arg(cntArea);
      }
    }

    p->drawText(rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
  }
}

void CDBItemDelegate::paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                const IDBItem& item) const {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectanglesItem(opt, item);

  const QPixmap& icon = item.getIcon().scaled(rectIcon.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, rectIcon, Qt::AlignCenter);

  if (rectButton.isValid()) {
    switch (item.getCheckState()) {
      case Qt::Unchecked:
        drawToolButton(p, opt, rectButton, QIcon(":/icons/32x32/ToWksUnchecked.png"), true, false);
        break;
      case Qt::PartiallyChecked:
        drawToolButton(p, opt, rectButton, QIcon(":/icons/32x32/ToWksPartially.png"), true, true);
        break;
      case Qt::Checked:
        drawToolButton(p, opt, rectButton, QIcon(":/icons/32x32/ToWksChecked.png"), true, true);
        break;
    }
  }

  QPalette::ColorGroup colorGroup = (opt.state & QStyle::State_HasFocus) ? QPalette::Active : QPalette::Inactive;
  QColor color = opt.palette.color(
      colorGroup, opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::WindowText);

  IDBItem* parent = dynamic_cast<IDBItem*>(item.parent());

  if (parent && parent->type() == IDBItem::eTypeLostFound) {
    color = opt.palette.color(parent->getCheckState() != Qt::Unchecked ? colorGroup : QPalette::Disabled,
                              opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::WindowText);
  } else {
    color = opt.palette.color(item.getCheckState() != Qt::Unchecked ? colorGroup : QPalette::Disabled,
                              opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::WindowText);
  }

  p->setPen(color);
  p->setFont(fontName);
  p->drawText(rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item.getName());

  p->setFont(fontStatus);
  if (parent && parent->type() == IDBItem::eTypeLostFound) {
    QString status;
    quint64 diff = QDateTime::currentDateTimeUtc().toSecsSinceEpoch() - item.getAge().toSecsSinceEpoch();
    if (diff < (60 * 60)) {
      status = tr("since: ") + tr("%1 min.").arg(diff / 60);
    } else if (diff < (60 * 60 * 24)) {
      status = tr("since: ") + tr("%1 h").arg(diff / (60 * 60));
    } else {
      status = tr("since: ") + tr("%1 days").arg(diff / (60 * 60 * 24));
    }
    p->drawText(rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
  } else if (itemStatusControl.item.infoText) {
    QTextDocument doc;
    doc.setHtml(item.getToolTip());
    const QStringList lines = doc.toPlainText().split("\n");
    if (lines.count() > 1) {
      p->drawText(rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, lines[1]);
    }
  }
}

void CDBItemDelegate::toggleCheckState(IDBItem& item) {
  item.setCheckState(item.getCheckState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
  emit treeWidget->itemChanged(&item, IDBItem::eColumn);

  if (item.type() == IDBItem::eTypeLostFound) {
    // Lex lost-n-found: we need to update all child items, too.
    const int N = item.childCount();
    QWidget* viewport = treeWidget->viewport();
    for (int n = 0; n < N; n++) {
      viewport->update(treeWidget->visualItemRect(item.child(n)));
    }
  }
}

bool CDBItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                                  const QModelIndex& index) {
  IDBItem* item = indexToItem(index);
  if (item == nullptr) {
    return QStyledItemDelegate::editorEvent(event, model, opt, index);
  }

  switch (item->type()) {
    case IDBItem::eTypeLostFound:
    case IDBItem::eTypeDatabase:
    case IDBItem::eTypeGroup:
    case IDBItem::eTypeProject:
    case IDBItem::eTypeOther:
      return editorEventFolder(event, model, opt, index, *item);

    case IDBItem::eTypeItem:
      return editorEventItem(event, model, opt, index, *item);
  }

  return QStyledItemDelegate::editorEvent(event, model, opt, index);
}

bool CDBItemDelegate::editorEventFolder(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                                        const QModelIndex& index, IDBItem& item) {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectanglesFolder(opt, item);

  if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);
    if (rectButton.contains(me->pos())) {
      toggleCheckState(item);
      return true;
    }
  }

  return QStyledItemDelegate::editorEvent(event, model, opt, index);
}

bool CDBItemDelegate::editorEventItem(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                                      const QModelIndex& index, IDBItem& item) {
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectanglesItem(opt, item);

  if (event->type() == QEvent::MouseButtonDblClick) {
    toggleCheckState(item);
    return true;
  } else if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);
    if (rectButton.contains(me->pos())) {
      toggleCheckState(item);
      return true;
    }
  }

  return QStyledItemDelegate::editorEvent(event, model, opt, index);
}

bool CDBItemDelegate::helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                                const QModelIndex& index) {
  if (!event || !view) {
    return QStyledItemDelegate::helpEvent(event, view, opt, index);
  }

  IDBItem* item = indexToItem(index);
  if (item == nullptr) {
    return QStyledItemDelegate::helpEvent(event, view, opt, index);
  }

  switch (item->type()) {
    case IDBItem::eTypeLostFound:
    case IDBItem::eTypeDatabase:
    case IDBItem::eTypeGroup:
    case IDBItem::eTypeProject:
    case IDBItem::eTypeOther:
      return helpEventFolder(event, view, opt, index, *item);

    case IDBItem::eTypeItem:
      return helpEventItem(event, view, opt, index, *item);
  }

  return QStyledItemDelegate::helpEvent(event, view, opt, index);
}

bool CDBItemDelegate::helpEventFolder(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                                      const QModelIndex& index, const IDBItem& item) {
  const QPoint& pos = event->pos();
  const QPoint& posGlobal = event->globalPos();
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectanglesFolder(opt, item);
  if (rectButton.contains(pos)) {
    if (item.getCheckState() == Qt::Unchecked) {
      QToolTip::showText(posGlobal, toRichText(tr("Load project into workspace")), view, {}, 3000);
    } else {
      QToolTip::showText(posGlobal, toRichText(tr("Remove project from workspace")), view, {}, 3000);
    }
  } else {
    const QString& toolTip = item.getToolTip();
    if (!toolTip.isEmpty()) {
      QToolTip::showText(posGlobal, "<div>" + toolTip + "</div>", view, {}, 3000);
    } else {
      return QStyledItemDelegate::helpEvent(event, view, opt, index);
    }
  }
  return true;
}

bool CDBItemDelegate::helpEventItem(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                                    const QModelIndex& index, const IDBItem& item) {
  const QPoint& pos = event->pos();
  const QPoint& posGlobal = event->globalPos();
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectanglesItem(opt, item);
  if (rectButton.contains(pos)) {
    if (item.getCheckState() == Qt::Unchecked) {
      QToolTip::showText(posGlobal, toRichText(tr("Load item into workspace.")), view, {}, 3000);
    } else {
      QToolTip::showText(posGlobal, toRichText(tr("Remove item from workspace.")), view, {}, 3000);
    }
  } else {
    const QString& toolTip = item.getToolTip();
    if (!toolTip.isEmpty()) {
      QToolTip::showText(posGlobal, "<div>" + toolTip + "</div>", view, {}, 3000);
    } else {
      return QStyledItemDelegate::helpEvent(event, view, opt, index);
    }
  }
  return true;
}
