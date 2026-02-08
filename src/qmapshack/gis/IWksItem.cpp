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

#include <QVariantAnimation>

IWksItem::IWksItem(QTreeWidgetItem* parent, int type) : QTreeWidgetItem(parent, type) { setupAnimations(); }
IWksItem::IWksItem(QTreeWidget* parent, int type) : QTreeWidgetItem(parent, type) { setupAnimations(); }

void IWksItem::setupAnimations() {
  animationOpacityOfFocusBasedItems = std::make_shared<QVariantAnimation>();
  animationOpacityOfFocusBasedItems->setDuration(250);
  animationOpacityOfFocusBasedItems->setEasingCurve(QEasingCurve::InOutQuad);

  animationOpacityOfFocusBasedItems->connect(animationOpacityOfFocusBasedItems.get(), &QVariantAnimation::valueChanged,
                                             [this](QVariant v) {
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
  flagsDecoration |= enable;
  flagsDecoration &= ~disable;
  updateItem();
}

void IWksItem::updateItem() {
  QTreeWidget* tree = treeWidget();
  if (tree == nullptr) {
    return;
  }
  QWidget* viewport = tree->viewport();
  if (viewport == nullptr) {
    return;
  }
  viewport->update();
}

bool IWksItem::holdUiFocus(const QStyleOptionViewItem& opt) {
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
