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

#include <QPointer>
#include <QTreeWidget>
#include <QWidget>

class CDemItem;
class QMenu;
class CCanvas;
class QTabWidget;

class CDemTreeWidget : public QTreeWidget {
  Q_OBJECT
 public:
  CDemTreeWidget(QWidget* parent);

 signals:
  void sigChanged();

 protected:
  void dragEnterEvent(QDragEnterEvent* e) override;
  void dragLeaveEvent(QDragLeaveEvent* e) override;
  void dropEvent(QDropEvent* event) override;

 private slots:
  void slotUpdateItem(const QString& key);
};

#include "ui_IDemList.h"

class CDemList : public QWidget, private Ui::IDemsList {
  Q_OBJECT
 public:
  CDemList(CCanvas* parent);
  virtual ~CDemList();

  /**
   * @brief Get the key of the canvas this list is attached to
   * @return The key as string if a canvas is attached. If not, an empty string.
   */
  QString getCanvasKey() const;

  /**
   * @brief Add this dem list to the given tab widget
   * @param widget  add list to this tab widget
   */
  void addToTabWidget(QTabWidget* widget);

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
  QPointer<CCanvas> canvas;
  QPointer<QTabWidget> tabWidget;
};

#endif  // CDEMLIST_H
