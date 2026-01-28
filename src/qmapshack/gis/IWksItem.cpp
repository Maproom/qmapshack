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

IWksItem::IWksItem(QTreeWidgetItem* parent, int type) : QTreeWidgetItem(parent, type) {}
IWksItem::IWksItem(QTreeWidget* parent, int type) : QTreeWidgetItem(parent, type) {}

IWksItem::eBaseType IWksItem::getBaseType() const {
  if (type() >= eTypeGarmin) {
    return eBaseType::Device;
  } else if (type() >= eTypeQms) {
    return eBaseType::Project;
  } else if (type() == eTypeGeoSearch) {
    return eBaseType::GeoSearch;
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

void IWksItem::updateItem() { treeWidget()->viewport()->update(); }
