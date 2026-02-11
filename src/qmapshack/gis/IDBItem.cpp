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

#include "gis/IDBItem.h"

#include "gis/CGisListDB.h"

IDBItem::IDBItem(QTreeWidget* parent, int type) : QTreeWidgetItem(parent, type) {}
IDBItem::IDBItem(QTreeWidgetItem* parent, int type) : QTreeWidgetItem(parent, type) {}

void IDBItem::setCheckState(Qt::CheckState state) {
  checkState = state;
  CGisListDB* widget = dynamic_cast<CGisListDB*>(treeWidget());
  if (widget != nullptr) {
    widget->slotItemChanged(this, 0);
  }
}

void IDBItem::setName(const QString& name) {
  setText(eColumn, name);
  this->name = name;
}
