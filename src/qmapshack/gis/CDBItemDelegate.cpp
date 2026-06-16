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
#include "helpers/CRowBuilder.h"
#include "helpers/CSettings.h"
#include "misc.h"

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
  return dynamic_cast<IDBItem*>(treeWidget->itemFromIndex(index));
}

void CDBItemDelegate::drawCheckStateButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect,
                                           Qt::CheckState state) {
  switch (state) {
    case Qt::Unchecked:
      CDraw::drawToolButton(p, opt, rect, QIcon(":/icons/32x32/ToWksUnchecked.png"), true, false);
      break;
    case Qt::PartiallyChecked:
      CDraw::drawToolButton(p, opt, rect, QIcon(":/icons/32x32/ToWksPartially.png"), true, true);
      break;
    case Qt::Checked:
      CDraw::drawToolButton(p, opt, rect, QIcon(":/icons/32x32/ToWksChecked.png"), true, true);
      break;
  }
}

void CDBItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {}

QSize CDBItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  const IDBItem* item = indexToItem(index);
  if (item == nullptr) {
    return QSize(opt.rect.width(), 22);
  }

  const QFontMetrics fmName(opt.font);

  QFont fontStatusFolder = opt.font;
  fontStatusFolder.setPointSize(opt.font.pointSize() - itemStatusControl.statusSizeFolder);
  const QFontMetrics fmStatusFolder(fontStatusFolder);

  QFont fontStatusItem = opt.font;
  fontStatusItem.setPointSize(opt.font.pointSize() - itemStatusControl.statusSizeItem);
  const QFontMetrics fmStatusItem(fontStatusItem);

  switch (item->type()) {
    case IDBItem::eTypeLostFound:
    case IDBItem::eTypeDatabase:
    case IDBItem::eTypeGroup:
    case IDBItem::eTypeProject:
    case IDBItem::eTypeOther: {
      const int statusH = (itemStatusControl.statusSizeFolder != kFontSizeInvalid) ? fmStatusFolder.height() : 0;
      return QSize(opt.rect.width(), std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height(), statusH)));
    }

    case IDBItem::eTypeItem: {
      const int statusH = (itemStatusControl.statusSizeItem != kFontSizeInvalid) ? fmStatusItem.height() : 0;
      return QSize(opt.rect.width(), std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height(), statusH)));
    }
  }

  return QSize(opt.rect.width(), std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height())));
}

CDBItemDelegate::ItemLayout CDBItemDelegate::getRectanglesFolder(const QStyleOptionViewItem& opt,
                                                                 const IDBItem& item) const {
  const QFont fontName = opt.font;
  const QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - itemStatusControl.statusSizeFolder);
  const QFontMetrics fmStatus(fontStatus);

  CRowBuilder row(opt.rect, kCellPad, kInnerGap);
  const QRect rectIcon = row.takeLeft(row.height());
  row.markStatusColumn();

  QRect rectButton;
  if (item.type() > IDBItem::eTypeGroup || item.type() == IDBItem::eTypeLostFound) {
    rectButton = row.takeButton(fmName.height());
  }

  const QRect rectName = row.nameSlice(fmName.height());

  QRect rectStatus;
  if (itemStatusControl.statusSizeFolder != kFontSizeInvalid) {
    rectStatus = row.fullStatusSlice(fmStatus.height());
  }

  return {fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton};
}

CDBItemDelegate::ItemLayout CDBItemDelegate::getRectanglesItem(const QStyleOptionViewItem& opt,
                                                               const IDBItem& item) const {
  const QFont fontName = opt.font;
  const QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - itemStatusControl.statusSizeItem);
  const QFontMetrics fmStatus(fontStatus);

  CRowBuilder row(opt.rect, kCellPad, kInnerGap);
  const QRect rectIcon = row.takeLeft(row.height());
  row.markStatusColumn();

  QRect rectButton;
  // Items inside the lost-and-found folder have no load/unload button.
  if (!(item.parent() && item.parent()->type() == IDBItem::eTypeLostFound)) {
    rectButton = row.takeButton(fmName.height());
  }

  const QRect rectName = row.nameSlice(fmName.height());

  QRect rectStatus;
  if (itemStatusControl.statusSizeItem != kFontSizeInvalid) {
    rectStatus = row.fullStatusSlice(fmStatus.height());
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
  auto layout = getRectanglesFolder(opt, item);

  const QPixmap& icon = item.getIcon().scaled(layout.rectIcon.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter);

  if (layout.rectButton.isValid()) {
    drawCheckStateButton(p, opt, layout.rectButton, item.getCheckState());
  }

  const bool isSelected = (opt.state & QStyle::State_Selected) != 0;
  const bool hasFocus = (opt.state & QStyle::State_HasFocus) != 0;

  const QPalette::ColorRole colorRole = (isSelected && hasFocus) ? QPalette::HighlightedText : QPalette::WindowText;
  QPalette::ColorGroup colorGroup = hasFocus ? QPalette::Active : QPalette::Inactive;

  if (item.type() > IDBItem::eTypeGroup) {
    layout.fontName.setBold(item.getCheckState() != Qt::Unchecked);
    colorGroup = item.getCheckState() != Qt::PartiallyChecked ? colorGroup : QPalette::Disabled;
  }
  const QColor& color = opt.palette.color(colorGroup, colorRole);

  p->setPen(color);
  p->setFont(layout.fontName);
  p->drawText(layout.rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item.getName());

  if (layout.rectStatus.isValid()) {
    p->setFont(layout.fontStatus);
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

    p->drawText(layout.rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
  }
}

void CDBItemDelegate::paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                                const IDBItem& item) const {
  const auto& layout = getRectanglesItem(opt, item);

  const QPixmap& icon = item.getIcon().scaled(layout.rectIcon.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon, Qt::AlignCenter);

  if (layout.rectButton.isValid()) {
    drawCheckStateButton(p, opt, layout.rectButton, item.getCheckState());
  }

  const bool isSelected = (opt.state & QStyle::State_Selected) != 0;
  const bool hasFocus = (opt.state & QStyle::State_HasFocus) != 0;

  const QPalette::ColorRole colorRole = (isSelected && hasFocus) ? QPalette::HighlightedText : QPalette::WindowText;
  QPalette::ColorGroup colorGroup = hasFocus ? QPalette::Active : QPalette::Inactive;

  IDBItem* parent = dynamic_cast<IDBItem*>(item.parent());

  if (parent && parent->type() == IDBItem::eTypeLostFound) {
    colorGroup = parent->getCheckState() != Qt::Unchecked ? colorGroup : QPalette::Disabled;
  } else {
    colorGroup = item.getCheckState() != Qt::Unchecked ? colorGroup : QPalette::Disabled;
  }
  const QColor& color = opt.palette.color(colorGroup, colorRole);

  p->setPen(color);
  p->setFont(layout.fontName);
  p->drawText(layout.rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item.getName());

  p->setFont(layout.fontStatus);
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
    p->drawText(layout.rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
  } else if (itemStatusControl.item.infoText) {
    QTextDocument doc;
    doc.setHtml(item.getToolTip());
    const QStringList lines = doc.toPlainText().split("\n");
    if (lines.count() > 1) {
      p->drawText(layout.rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, lines[1]);
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
  const auto& layout = getRectanglesFolder(opt, item);

  if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);
    if (layout.rectButton.contains(me->pos())) {
      toggleCheckState(item);
      return true;
    }
  }

  return QStyledItemDelegate::editorEvent(event, model, opt, index);
}

bool CDBItemDelegate::editorEventItem(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                                      const QModelIndex& index, IDBItem& item) {
  const auto& layout = getRectanglesItem(opt, item);

  if (event->type() == QEvent::MouseButtonDblClick) {
    toggleCheckState(item);
    return true;
  } else if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);
    if (layout.rectButton.contains(me->pos())) {
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
  const auto& layout = getRectanglesFolder(opt, item);
  if (layout.rectButton.contains(pos)) {
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
  const auto& layout = getRectanglesItem(opt, item);
  if (layout.rectButton.contains(pos)) {
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
