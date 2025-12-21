/**********************************************************************************************
    Copyright (C) 2025 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CPOITREEWIDGETCATEGORIES_H
#define CPOITREEWIDGETCATEGORIES_H

#include <QScrollBar>
#include <QTreeWidget>

class CPoiTreeWidgetCategories : public QTreeWidget {
  Q_OBJECT
 public:
  CPoiTreeWidgetCategories(QWidget *parent);
  virtual ~CPoiTreeWidgetCategories() = default;

 protected:
  void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;
};

#endif  // CPOITREEWIDGETCATEGORIES_H
