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

#include "gis/IWksItem.h"

#include <QThread>
#include <QVariantAnimation>

#include "gis/IGisItem.h"

IWksItem::IWksItem(QTreeWidgetItem* parent, int type) : QTreeWidgetItem(parent, type) { setupAnimations(); }
IWksItem::IWksItem(QTreeWidget* parent, int type) : QTreeWidgetItem(parent, type) { setupAnimations(); }

IWksItem::~IWksItem() {
  // IWksItem is not a QObject. Therefore the animation's parent
  // is the tree widget owning the IWksItem. If the IWksItem is
  // destroyed the animation survives and might call via signal
  // into the delete IWksItem instance. Therefore it has to be
  // destroyed explicitly with the IWksItem.
  delete animationOpacityOfFocusBasedItems;
}

void IWksItem::setupAnimations() {
  animationOpacityOfFocusBasedItems = new QVariantAnimation(treeWidget());
  animationOpacityOfFocusBasedItems->setDuration(250);
  animationOpacityOfFocusBasedItems->setEasingCurve(QEasingCurve::InOutQuad);

  QObject::connect(animationOpacityOfFocusBasedItems, &QVariantAnimation::valueChanged,
                   animationOpacityOfFocusBasedItems, [this](QVariant v) {
                     opacityOfFocusBasedItems = v.toFloat();
                     QTreeWidget* widget = treeWidget();
                     if (widget != nullptr) {
                       widget->viewport()->update(treeWidget()->visualItemRect(this));
                     }
                   });
}

IWksItem::eBaseType IWksItem::getBaseType() const {
  if (type() == eTypeGeoSearch) {
    return eBaseType::GeoSearch;
  } else if (type() == eTypeGeoSearchError) {
    return eBaseType::GeoSearchError;
  } else if (type() >= eTypeGarmin) {
    return eBaseType::Device;
  } else if (type() >= eTypeQms) {
    return eBaseType::Project;
  } else if (type() >= eTypeWpt) {
    return eBaseType::Item;
  }
  return eBaseType::Unknown;
}

void IWksItem::updateDecoration(quint32 enable, quint32 disable) {
  if (!QThread::isMainThread()) return;
  flagsDecoration |= enable;
  flagsDecoration &= ~disable;
  updateItem();
}

void IWksItem::updateItem() {
  if (!QThread::isMainThread()) return;
  QPointer<QTreeWidget> tree = treeWidget();
  if (tree == nullptr) {
    return;
  }

  if (tree == nullptr) {
    return;
  }
  emit tree->itemChanged(this, 0);
  QWidget* viewport = tree->viewport();
  if (viewport == nullptr) {
    return;
  }
  viewport->update();
}

bool IWksItem::holdUiFocus(const QStyleOptionViewItem& opt) {
  if (!QThread::isMainThread()) return false;
  bool hasFocus = (opt.state & QStyle::State_HasFocus) != 0;
  if (hasFocus != lastFocusState) {
    float opacity = hasFocus ? 1.0 : 0.0;
    animationOpacityOfFocusBasedItems->stop();
    animationOpacityOfFocusBasedItems->setStartValue(opacityOfFocusBasedItems);
    animationOpacityOfFocusBasedItems->setEndValue(opacity);
    animationOpacityOfFocusBasedItems->start();

    lastFocusState = hasFocus;
  }

  return hasFocus || (animationOpacityOfFocusBasedItems->state() == QAbstractAnimation::Running);
}

void IWksItem::setVisibility(bool visible) {
  if (!QThread::isMainThread()) return;
  this->visible = visible;
  updateItem();
}

void IWksItem::setAutoSave(bool on) {
  if (!QThread::isMainThread()) return;
  autoSave = on;
  updateItem();
}

void IWksItem::setAutoSyncToDev(bool on) {
  if (!QThread::isMainThread()) return;
  autoSyncToDev = on;
  updateItem();
}

void IWksItem::setProgress(quint32 count, quint32 total) {
  // ok to call from any thread
  QMutexLocker lock(&IGisItem::mutexItems);
  countProgress = count;
  totalProgress = total;
  QMetaObject::invokeMethod(treeWidget(), [this]() { updateItem(); });
}

std::tuple<bool, qreal> IWksItem::getProgress() const {
  if (!QThread::isMainThread()) return {false, 0};
  QMutexLocker lock(&IGisItem::mutexItems);
  const qreal progress = (100.0 * countProgress) / totalProgress;
  return {countProgress != totalProgress, progress};
}
