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

#ifndef CPOICATEGORY_H
#define CPOICATEGORY_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

class CPoiCategory : public QTreeWidgetItem
{
public:
    CPoiCategory(const QString& categoryName, quint64 categoryID, Qt::CheckState checkedState, CPoiCategory* parent);
    CPoiCategory(const QString& categoryName, quint64 categoryID, QTreeWidget* parent);

    const QString& getCategory() const
    {
        return categoryName;
    }

    quint64 getId() const
    {
        return categoryID;
    }

    Qt::CheckState checkState() const;

private:
    const QString categoryName;
    const quint64 categoryID;
};

#endif // CPOICATEGORY_H
