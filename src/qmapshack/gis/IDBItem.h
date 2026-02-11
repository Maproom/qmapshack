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

#ifndef IDBITEM_H
#define IDBITEM_H

#include <QCoreApplication>
#include <QDateTime>
#include <QTreeWidgetItem>

class IDBFolder;

class IDBItem : public QTreeWidgetItem {
 public:
  IDBItem(QTreeWidget* parent, int type);
  IDBItem(QTreeWidgetItem* parent, int type);
  virtual ~IDBItem() {};

  enum column_e { eColumn };

  enum type_e {
    eTypeLostFound = 1,
    eTypeDatabase = 2,
    eTypeGroup = 3,
    eTypeProject = 4,
    eTypeOther = 5,
    eTypeItem = 6
  };

  virtual void setAge(const QDateTime& timestamp) { ageTimestamp = timestamp; }
  virtual const QDateTime& getAge() const { return ageTimestamp; }

  virtual void setName(const QString& name);
  virtual const QString& getName() const { return name; }

  virtual void setToolTip(const QString& tip) { toolTip = tip; }
  virtual const QString& getToolTip() const { return toolTip; }

  virtual void setIcon(const QPixmap& icon) { this->icon = icon; }
  virtual const QPixmap& getIcon() const { return icon; }

  virtual void setCheckState(Qt::CheckState state);
  virtual Qt::CheckState getCheckState() const { return checkState; }

 private:
  QDateTime ageTimestamp;
  QString name;
  QString toolTip;
  QPixmap icon;
  Qt::CheckState checkState;
};

#endif //IDBITEM_H

