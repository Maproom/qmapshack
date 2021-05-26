/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#include "poi/CPoiCategory.h"
#include "poi/CPoiPropSetup.h"

CPoiCategory::CPoiCategory(const QString& categoryName, quint64 categoryID, Qt::CheckState checkedState, CPoiCategory* parent)
    : QTreeWidgetItem(parent), categoryName(categoryName), categoryID(categoryID)
{
    setText(CPoiPropSetup::eTreeColumnDisplayName, categoryName);
    setCheckState(CPoiPropSetup::eTreeColumnCheckbox, checkedState);
    setFlags(flags() | Qt::ItemIsAutoTristate);
}

CPoiCategory::CPoiCategory(const QString& categoryName, quint64 categoryID, QTreeWidget* parent)
    : QTreeWidgetItem(parent), categoryName(categoryName), categoryID(categoryID)
{
    setText(CPoiPropSetup::eTreeColumnDisplayName, categoryName);
    setCheckState(CPoiPropSetup::eTreeColumnCheckbox, Qt::Unchecked);
    setFlags(flags() | Qt::ItemIsAutoTristate);
}

Qt::CheckState CPoiCategory::checkState() const
{
    return QTreeWidgetItem::checkState(CPoiPropSetup::eTreeColumnCheckbox);
}
