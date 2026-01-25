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

#ifndef IWKSITEM_H
#define IWKSITEM_H

#include <QTreeWidgetItem>

class IWksItem : public QTreeWidgetItem {
 public:
  IWksItem(QTreeWidgetItem* parent, int type = Type) : QTreeWidgetItem(parent, type) {}
  IWksItem(QTreeWidget* parent, int type = Type) : QTreeWidgetItem(parent, type) {}
  virtual ~IWksItem() = default;

  virtual const QString& getName() const { return name; }
  virtual const QPixmap& getIcon() const { return icon; }

 protected:
  QString name;
  QPixmap icon;
};

#endif //IWKSITEM_H

