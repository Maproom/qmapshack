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

  enum mark_e { eMarkNone = 0, eMarkChanged = 0x00000001, eMarkNotPart = 0x00000002, eMarkNotInDB = 0x00000004 };

  virtual void setToolTipName(const QString& tip) { toolTipName = tip; }

  virtual const QString& getName() const { return name; }
  virtual const QPixmap& getIcon() const { return icon; }
  virtual const bool getVisibility() const { return visible; }

 protected:
  QString name;
  QString toolTipName;
  QPixmap icon;
  bool visible;
  bool autoSave = false;       ///< flag to show if auto save is on or off
  bool autoSyncToDev = false;  ///< if set true sync the project with every device connected

  quint32 flagsDecoration;
  QString toolTipDecoration;

  /// labeling the GisItems
  qreal rating = 0;
  QSet<QString> keywords;
};

#endif //IWKSITEM_H

