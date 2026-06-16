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

#include "map/CMapItemDelegate.h"

#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>
#include <QTreeWidget>
#include <QVariantAnimation>

#include "helpers/CDraw.h"
#include "helpers/CRowBuilder.h"
#include "map/IMapItem.h"

constexpr int kFontSizeDiffItem = 2;

CMapItemDelegate::CMapItemDelegate(QTreeWidget* parent) : QStyledItemDelegate(parent), treeWidget(parent) {
  // cross-thread signalling. Signal is triggered by draw thread, indicator animation has to be triggered in main thread
  connect(this, &CMapItemDelegate::sigSetProcessing, this, &CMapItemDelegate::slotSetProcessing);
}

IMapItem* CMapItemDelegate::indexToItem(const QModelIndex& index) const {
  return dynamic_cast<IMapItem*>(treeWidget->itemFromIndex(index));
}

QString CMapItemDelegate::keyFromIndex(const QModelIndex& index) const {
  IMapItem* item = indexToItem(index);
  if (item) {
    return item->getKey();
  }
  return "";
}

CMapItemDelegate::animations_t& CMapItemDelegate::getAnimations(const QModelIndex& index) {
  const QString& key = keyFromIndex(index);
  animations_t& anim = data[key].animations;

  if (anim.animColorIndicator.isNull()) {
    anim.animColorIndicator = new QVariantAnimation(this);
    anim.animColorIndicator->setDuration(250);
    anim.animColorIndicator->setEasingCurve(QEasingCurve::InOutQuad);

    connect(anim.animColorIndicator, &QVariantAnimation::valueChanged, this, [this, key](QVariant v) {
      data[key].animations.colorIndicator = v.value<QColor>();
      emit sigUpdateItem(key);
    });
  }

  if (anim.animOpacityIndicator.isNull()) {
    anim.animOpacityIndicator = new QVariantAnimation(this);
    anim.animOpacityIndicator->setDuration(250);
    anim.animOpacityIndicator->setEasingCurve(QEasingCurve::InOutQuad);

    connect(anim.animOpacityIndicator, &QVariantAnimation::valueChanged, this, [this, key](QVariant v) {
      data[key].animations.opacityIndicator = v.toFloat();
      emit sigUpdateItem(key);
    });
  }

  if (anim.animFlashingIndicator.isNull()) {
    anim.animFlashingIndicator = new QVariantAnimation(this);
    anim.animFlashingIndicator->setDuration(500);
    anim.animFlashingIndicator->setEasingCurve(QEasingCurve::InOutQuad);

    connect(anim.animFlashingIndicator, &QVariantAnimation::valueChanged, this, [this, key](QVariant v) {
      data[key].animations.opacityIndicator = v.toFloat();
      emit sigUpdateItem(key);
    });
  }

  if (anim.animAccessInfo.isNull()) {
    anim.animAccessInfo = new QVariantAnimation(this);
    anim.animAccessInfo->setDuration(2000);
    anim.animAccessInfo->setEasingCurve(QEasingCurve::InOutQuad);

    connect(anim.animAccessInfo, &QVariantAnimation::valueChanged, this, [this, key](QVariant v) {
      data[key].animations.opacityAccessInfo = v.toFloat();
      emit sigUpdateItem(key);
    });
  }

  return anim;
}

const CMapItemDelegate::animations_t& CMapItemDelegate::getAnimations(const QModelIndex& index) const {
  return data[keyFromIndex(index)].animations;
}

void CMapItemDelegate::setColor(const QModelIndex& index, const QColor& c) {
  animations_t& anim = getAnimations(index);
  if (anim.colorIndicator == c) {
    return;
  }

  anim.animColorIndicator->stop();
  anim.animColorIndicator->setStartValue(anim.colorIndicator);
  anim.animColorIndicator->setEndValue(c);
  anim.animColorIndicator->start();
}

void CMapItemDelegate::hideIndicator(const QModelIndex& index) {
  animations_t& anim = getAnimations(index);
  if (anim.opacityIndicator == 0.0) {
    return;
  }

  anim.animFlashingIndicator->stop();

  anim.animOpacityIndicator->stop();
  anim.animOpacityIndicator->setStartValue(anim.opacityIndicator);
  anim.animOpacityIndicator->setEndValue(0.0);
  anim.animOpacityIndicator->start();
}

void CMapItemDelegate::showIndicator(const QModelIndex& index) {
  animations_t& anim = getAnimations(index);
  if (anim.opacityIndicator == 1.0) {
    return;
  }

  anim.animFlashingIndicator->stop();

  anim.animOpacityIndicator->stop();
  anim.animOpacityIndicator->setStartValue(anim.opacityIndicator);
  anim.animOpacityIndicator->setEndValue(1.0);
  anim.animOpacityIndicator->start();
}

void CMapItemDelegate::setProcessing(const QModelIndex& index, bool on) {
  // use signal to decouple threads when setProcessing()
  // is called from a thread not the main application thread
  emit sigSetProcessing(index, on);
}

void CMapItemDelegate::slotSetProcessing(const QModelIndex& index, bool on) {
  animations_t& anim = getAnimations(index);

  anim.animOpacityIndicator->stop();

  anim.animFlashingIndicator->stop();
  if (on) {
    anim.animFlashingIndicator->setStartValue(1.0);
    anim.animFlashingIndicator->setEndValue(0.0);
    anim.animFlashingIndicator->setLoopCount(-1);
    anim.animFlashingIndicator->setDirection(QAbstractAnimation::Forward);
    anim.animFlashingIndicator->start();
  } else {
    IMapItem* item = indexToItem(index);
    if (item == nullptr || item->getStatus() != IMapItem::eStatus::Active) {
      hideIndicator(index);
    } else {
      showIndicator(index);
    }
  }
}

void CMapItemDelegate::setAccess(const QModelIndex& index, const QString& info) {
  animations_t& anim = getAnimations(index);

  anim.animAccessInfo->stop();
  anim.accessInfo = info;
  anim.opacityAccessInfo = 1.0;
  anim.animAccessInfo->setStartValue(anim.opacityAccessInfo);
  anim.animAccessInfo->setEndValue(0.0);
  anim.animAccessInfo->start();
}

void CMapItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
  QStyledItemDelegate::initStyleOption(option, index);

  const QString& key = keyFromIndex(index);
  if (!data.contains(key)) {
    data.insert(key, {});
  }
  data[key].icon = option->icon.pixmap({48, 48});
  option->icon = QIcon();
  option->decorationSize = QSize(0, 0);
}

CMapItemDelegate::MapItemLayout CMapItemDelegate::getRectangles(const QStyleOptionViewItem& opt) const {
  const QFont fontName = opt.font;
  const QFontMetrics fmName(fontName);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffItem);
  const QFontMetrics fmStatus(fontStatus);

  CRowBuilder row(opt.rect, kCellPad, kInnerGap);
  const QRect rectIcon = row.takeLeft(row.height());
  const QRect rectButton = row.takeRight(row.height());
  // Thin vertical indicator bar to the left of the button.
  const QRect rawIndicatorSlot = row.takeRight(6);
  const QRect rectIndicator = rawIndicatorSlot.adjusted(0, kMargin, 0, -kMargin);
  const QRect rectName = row.nameSlice(fmName.height());
  const QRect rectStatus = row.statusSlice(fmStatus.height());

  return {fontName, fontStatus, rectIcon, rectButton, rectIndicator, rectName, rectStatus};
}

void CMapItemDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  IMapItem* item = indexToItem(index);
  if (item == nullptr) {
    return;
  }

  p->save();
  USE_ANTI_ALIASING(*p, true);
  // Standard background & selection
  QStyledItemDelegate::paint(p, opt, index);

  const bool isActive = item->getStatus() == IMapItem::eStatus::Active;

  // derive strings colors
  const QColor colorName = CDraw::itemNameColor(opt, isActive);

  QColor colorStatus = colorName;
  QString status;
  switch (item->getStatus()) {
    case IMapItem::eStatus::Inactive:
      status = tr("inactive");
      break;
    case IMapItem::eStatus::Active:
      status = tr("active");
      break;
    case IMapItem::eStatus::Missing:
      status = "!!!" + tr("missing") + "!!!";
      colorStatus = Qt::red;
      break;
    case IMapItem::eStatus::Unused:
      status = tr("-");
      break;
  }

  // derive all rectangles to place visual elements
  auto layout = getRectangles(opt);
  layout.fontName.setBold(isActive);

  // draw name
  p->setPen(colorName);
  p->setFont(layout.fontName);
  p->drawText(layout.rectName.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignVCenter, item->getName());
  p->setClipping(false);

  // draw status
  p->setPen(colorStatus);
  p->setFont(layout.fontStatus);
  p->drawText(layout.rectStatus.adjusted(0, -1, 0, 1), Qt::AlignLeft | Qt::AlignVCenter, status);

  // draw icon
  const QPixmap& icon =
      data[keyFromIndex(index)].icon.scaled(layout.rectIcon.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QIcon(icon).paint(p, layout.rectIcon);

  // draw tool button to activate
  CDraw::drawToolButton(p, opt, layout.rectButton,
                        isActive ? QIcon(":/icons/32x32/ShowAll.png") : QIcon(":/icons/32x32/ShowNone.png"),
                        item->getStatus() != IMapItem::eStatus::Missing, isActive);

  // draw all elements that tinker with opacity
  // draw indicator
  const animations_t& anim = getAnimations(index);
  p->setOpacity(anim.opacityIndicator);
  p->setPen(Qt::NoPen);
  p->setBrush(anim.colorIndicator);
  p->drawRoundedRect(layout.rectIndicator, 4, 4);

  // draw access info
  p->setPen(colorName);
  p->setOpacity(anim.opacityAccessInfo);
  p->setFont(layout.fontStatus);
  p->drawText(layout.rectStatus, Qt::AlignRight | Qt::AlignVCenter, anim.accessInfo);

  p->restore();
}

bool CMapItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                                   const QModelIndex& index) {
  if (event->type() == QEvent::MouseButtonPress) {
    auto* me = static_cast<QMouseEvent*>(event);

    const auto& layout = getRectangles(opt);

    if (layout.rectButton.contains(me->pos())) {
      IMapItem* item = indexToItem(index);
      if (item == nullptr) {
        return false;
      }
      if (item->getStatus() != IMapItem::eStatus::Missing) {
        const bool activate = item->getStatus() != IMapItem::eStatus::Active;
        if (activate) {
          showIndicator(index);
        } else {
          hideIndicator(index);
        }
        item->activate(activate);
      }
      return true;
    }
  }

  return QStyledItemDelegate::editorEvent(event, model, opt, index);
}

bool CMapItemDelegate::helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                                 const QModelIndex& index) {
  if (!event || !view) {
    return false;
  }
  IMapItem* item = indexToItem(index);
  if (item == nullptr) {
    return false;
  }

  const bool isActive = item->getStatus() == IMapItem::eStatus::Active;
  const auto& layout = getRectangles(opt);

  if (layout.rectButton.contains(event->pos())) {
    const QString& tip = isActive ? tr("Deactivate %1").arg(item->getName()) : tr("Activate %1").arg(item->getName());
    QToolTip::showText(event->globalPos(), tip, view, {}, 3000);
  } else if (isActive && layout.rectIndicator.contains(event->pos())) {
    const bool outOfScale = item->isOutOfScale();
    const QString& tip = outOfScale ? tr("%1 is not visible at current scale").arg(item->getName())
                                    : tr("%1 is visible at current scale").arg(item->getName());
    QToolTip::showText(event->globalPos(), tip, view, {}, 3000);
  } else if (layout.rectName.contains(event->pos())) {
    const QFontMetrics fm(layout.fontName);
    const QRect& boundingRectName = fm.boundingRect(item->getName());
    QString toolTip;
    if (boundingRectName.width() > layout.rectName.width()) {
      toolTip = QString("<p>%1</p>").arg(item->getName());
    }
    if (!item->getToolTip().isEmpty()) {
      toolTip += QString("<p>%1</p>").arg(item->getToolTip());
    }
    if (!toolTip.isEmpty()) {
      QToolTip::showText(event->globalPos(), toolTip, view, {}, 5000);
    }
  }

  return true;
}

QSize CMapItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  const QFontMetrics fmName(opt.font);

  QFont fontStatus = opt.font;
  fontStatus.setPointSize(fontStatus.pointSize() - kFontSizeDiffItem);
  const QFontMetrics fmStatus(fontStatus);

  return QSize(opt.rect.width(), std::max(22, CRowBuilder::rowHeight(kCellPad, fmName.height(), fmStatus.height())));
}
