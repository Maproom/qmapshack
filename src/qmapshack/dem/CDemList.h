/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CDEMLIST_H
#define CDEMLIST_H

#include <QTreeWidget>
#include <QWidget>

class CDemItem;
class QMenu;

class CDemTreeWidget : public QTreeWidget {
  Q_OBJECT
 public:
  CDemTreeWidget(QWidget* parent);

 signals:
  void sigChanged();

 protected:
  void dragEnterEvent(QDragEnterEvent* e) override;
  void dropEvent(QDropEvent* event) override;
};

#include "ui_IDemList.h"

class CDemList : public QWidget, private Ui::IDemsList {
  Q_OBJECT
 public:
  CDemList(QWidget* parent);
  virtual ~CDemList();

  void clear();
  int count();
  CDemItem* item(int i);
  operator QTreeWidget*() { return treeWidget; }

  /**
   * @brief Show/hide hints depending on the DEM list's state
   */
  void updateHelpText();

  /**
   * @brief Add DEM item to the list
   *
   * Add the item as top level item and set the item's widget. Connect
   * the item's signals to properly handle changes.
   *
   * @param dem
   */
  void addDem(CDemItem* dem);

  /**
   * @brief Move dem item to top of list
   * @param dem
   */
  void moveDemToTop(CDemItem* dem);

 signals:
  void sigChanged();

 private slots:
  void slotMoveUp();
  void slotMoveDown();
  void slotRemove();
  void slotReloadDem();
  void slotContextMenu(const QPoint& point);
  void slotFilter(const QString& str);

 private:
  QMenu* menu;
};

#endif  // CDEMLIST_H
