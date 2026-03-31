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

void checkForThread() {
  if (!QThread::isMainThread()) {
    qFatal() << "Called from a thread other than the maiUI thread! This will not work!";
  }
}

IWksItem::IWksItem(QTreeWidgetItem* parent, int type) : QTreeWidgetItem(parent, type) { setupAnimations(); }
IWksItem::IWksItem(QTreeWidget* parent, int type) : QTreeWidgetItem(parent, type) { setupAnimations(); }

void IWksItem::setupAnimations() {
  animationOpacityOfFocusBasedItems = new QVariantAnimation(treeWidget());
  animationOpacityOfFocusBasedItems->setDuration(250);
  animationOpacityOfFocusBasedItems->setEasingCurve(QEasingCurve::InOutQuad);

  QObject::connect(animationOpacityOfFocusBasedItems, &QVariantAnimation::valueChanged,
                   animationOpacityOfFocusBasedItems, [this](QVariant v) {
                     opacityOfFocusBasedItems = v.toFloat();
                     treeWidget()->viewport()->update(treeWidget()->visualItemRect(this));
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
  checkForThread();
  flagsDecoration |= enable;
  flagsDecoration &= ~disable;
  updateItem();
}

void IWksItem::updateItem() {
  checkForThread();
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
  checkForThread();
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
  checkForThread();
  this->visible = visible;
  updateItem();
}

void IWksItem::setAutoSave(bool on) {
  checkForThread();
  autoSave = on;
  updateItem();
}

void IWksItem::setAutoSyncToDev(bool on) {
  checkForThread();
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
  checkForThread();
  QMutexLocker lock(&IGisItem::mutexItems);
  const qreal progress = (100.0 * countProgress) / totalProgress;
  return {countProgress != totalProgress, progress};
}
