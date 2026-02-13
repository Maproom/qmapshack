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
#include <QToolTip>

#include "gis/IDBItem.h"
#include "helpers/CDraw.h"

constexpr int kMargin = 1;
constexpr int kFontSizeDiffItem = 3;

CDBItemDelegate::CDBItemDelegate(QTreeWidget* parent) : QStyledItemDelegate(parent), treeWidget(parent) {}

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
  QFont font1 = opt.font;
  font1.setBold(true);
  QFontMetrics fm1(font1);

  QFont font2 = opt.font;
  font2.setPointSize(font2.pointSize() - kFontSizeDiffItem);
  QFontMetrics fm2(font2);

  return QSize(opt.rect.width(), std::max(22, 7 * kMargin + fm1.height() + fm2.height()));
}

static std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> getRectangles(const QStyleOptionViewItem& opt,
                                                                          const IDBItem& item) {
  const QFont fontName = opt.font;
  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffItem);

  const QFontMetrics fmName(fontName);
  const QFontMetrics fmStatus(fontStatus);

  const QRect& r = opt.rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
  const QRect rectIcon(r.left(), r.top(), r.height(), r.height());
  QRect rectButton;
  // Checkbox button only for project or other folders and only for items
  if (item.type() > IDBItem::eTypeGroup || item.type() == IDBItem::eTypeLostFound) {
    // If the item is part of the lost-and-found folder -> no button
    // Yeah. The logic is a bit weird.
    if (!(item.parent() && item.parent()->type() == IDBItem::eTypeLostFound)) {
      rectButton.setRect(r.right() - r.height(), r.top(), r.height(), r.height());
    }
  }
  const QRect rectName(rectIcon.right() + 4 * kMargin, r.top() + kMargin,
                       r.width() - rectIcon.width() - rectButton.width() - 6 * kMargin, fmName.height());

  const QRect rectStatus(rectIcon.right() + 4 * kMargin, r.bottom() - fmStatus.height() - kMargin,
                         r.width() - rectIcon.width() - rectButton.width() - 6 * kMargin, fmName.height());

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

  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectangles(opt, *item);

  QIcon(item->getIcon()).paint(p, rectIcon, Qt::AlignCenter);

  if (rectButton.isValid()) {
    switch (item->getCheckState()) {
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

  QColor color = opt.palette.color(QPalette::Active,
                                   opt.state & QStyle::State_Selected ? QPalette::BrightText : QPalette::WindowText);

  IDBItem* parent = dynamic_cast<IDBItem*>(item->parent());
  if (item->type() == IDBItem::eTypeItem) {
    if (parent && parent->type() == IDBItem::eTypeLostFound) {
      color = opt.palette.color(parent->getCheckState() != Qt::Unchecked ? QPalette::Active : QPalette::Disabled,
                                opt.state & QStyle::State_Selected ? QPalette::BrightText : QPalette::WindowText);
    } else {
      color = opt.palette.color(item->getCheckState() != Qt::Unchecked ? QPalette::Active : QPalette::Disabled,
                                opt.state & QStyle::State_Selected ? QPalette::BrightText : QPalette::WindowText);
    }
  } else if (item->type() > IDBItem::eTypeGroup) {
    fontName.setBold(item->getCheckState() != Qt::Unchecked);
    color = opt.palette.color(item->getCheckState() != Qt::PartiallyChecked ? QPalette::Active : QPalette::Disabled,
                              opt.state & QStyle::State_Selected ? QPalette::BrightText : QPalette::WindowText);
  }

  p->setPen(color);
  p->setFont(fontName);
  p->drawText(rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, item->getName());

  if (parent && parent->type() == IDBItem::eTypeLostFound) {
    p->setFont(fontStatus);
    QString status;
    quint64 diff = QDateTime::currentDateTimeUtc().toSecsSinceEpoch() - item->getAge().toSecsSinceEpoch();
    if (diff < (60 * 60)) {
      status = tr("since: ") + tr("%1 min.").arg(diff / 60);
    } else if (diff < (60 * 60 * 24)) {
      status = tr("since: ") + tr("%1 h").arg(diff / (60 * 60));
    } else {
      status = tr("since: ") + tr("%1 days").arg(diff / (60 * 60 * 24));
    }
    p->drawText(rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignTop, status);
  }

  p->restore();
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

  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectangles(opt, *item);

  if (event->type() == QEvent::MouseButtonDblClick) {
    if (item->type() == IDBItem::eTypeItem) {
      toggleCheckState(*item);
      return true;
    }
  } else if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);
    if (rectButton.contains(me->pos())) {
      toggleCheckState(*item);
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

  const QPoint& pos = event->pos();
  const QPoint& posGlobal = event->globalPos();
  auto [fontName, fontStatus, rectIcon, rectName, rectStatus, rectButton] = getRectangles(opt, *item);
  if (rectButton.contains(pos)) {
    if (item->getCheckState() == Qt::Unchecked) {
      QToolTip::showText(posGlobal,
                         item->type() == IDBItem::eTypeItem ? trRichText("Load item into workspace.")
                                                            : trRichText("Load project into workspace"),
                         view, {}, 3000);
    } else {
      QToolTip::showText(posGlobal,
                         item->type() == IDBItem::eTypeItem ? trRichText("Remove item from workspace.")
                                                            : trRichText("Remove project from workspace"),
                         view, {}, 3000);
    }
  } else {
    const QString& toolTip = item->getToolTip();
    if (!toolTip.isEmpty()) {
      QToolTip::showText(posGlobal, "<div>" + toolTip + "</div>", view, {}, 3000);
    } else {
      return QStyledItemDelegate::helpEvent(event, view, opt, index);
    }
  }
  return true;
}
