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

#include <QTreeWidget>

#include "CWksItemDelegate.h"
#include "helpers/CDraw.h"

CWksItemDelegate::CWksItemDelegate(QTreeWidget* parent) : QStyledItemDelegate(parent), treeWidget(parent) {}

void CWksItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
  QStyledItemDelegate::initStyleOption(option, index);

  // option->features &= ~QStyleOptionViewItem::HasDecoration;
}

void CWksItemDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
  // IMapItem* item = indexToItem(index);
  // if (item == nullptr) {
  //   return;
  // }

  p->save();
  USE_ANTI_ALIASING(*p, true);
  // Standard background & selection
  QStyledItemDelegate::paint(p, opt, index);

  p->restore();
}
