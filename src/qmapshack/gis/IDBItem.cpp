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

IDBItem::IDBItem(QTreeWidget* parent, int type) : QTreeWidgetItem(parent, type) {}
IDBItem::IDBItem(QTreeWidgetItem* parent, int type) : QTreeWidgetItem(parent, type) {}

void IDBItem::updateItem() {
  QTreeWidget* widget = dynamic_cast<QTreeWidget*>(treeWidget());
  if (widget != nullptr) {
    widget->viewport()->update(widget->visualItemRect(this));
  }
}

void IDBItem::setCheckState(Qt::CheckState state) {
  checkState = state;
  updateItem();
}

void IDBItem::setName(const QString& name) {
  setText(eColumn, name);
  this->name = name;
}

void IDBItem::setFolderCount(quint32 group, quint32 project, quint32 other) {
  countGroup = group;
  countProject = project;
  countOther = other;
  updateItem();
}

std::tuple<quint32, quint32, quint32> IDBItem::getFolderCount() const { return {countGroup, countProject, countOther}; }

void IDBItem::setItemCount(quint32 wpt, quint32 trk, quint32 rte, quint32 area) {
  countWpt = wpt;
  countTrk = trk;
  countRte = rte;
  countArea = area;
  updateItem();
}

std::tuple<quint32, quint32, quint32, quint32> IDBItem::getItemCount() const {
  return {countWpt, countTrk, countRte, countArea};
}
