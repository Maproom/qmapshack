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

#ifndef CMAPLIST_H
#define CMAPLIST_H

#include <QPointer>
#include <QTreeWidget>
#include <QWidget>

class CMapItem;
class QMenu;
class CCanvas;
class QTabWidget;

class CMapTreeWidget : public QTreeWidget {
  Q_OBJECT
 public:
  CMapTreeWidget(QWidget* parent);

 signals:
  void sigChanged();
  void sigMoveHome();
  void sigMoveUp();
  void sigMoveDown();
  void sigMoveEnd();

 protected:
  void keyPressEvent(QKeyEvent* e) override;
  void dragEnterEvent(QDragEnterEvent* e) override;
  void dragLeaveEvent(QDragLeaveEvent* e) override;
  void dropEvent(QDropEvent* e) override;
 private slots:
  void slotUpdateItem(const QString& key);
};

#include "ui_IMapList.h"

class CMapList : public QWidget, private Ui::IMapList {
  Q_OBJECT
 public:
  CMapList(CCanvas* parent);
  virtual ~CMapList();

  /**
   * @brief Get the key of the canvas this list is attached to
   * @return The key as string if a canvas is attached. If not, an empty string.
   */
  QString getCanvasKey() const;

  /**
   * @brief Add this map list to the given tab widget
   * @param widget  add list to this tab widget
   */
  void addToTabWidget(QTabWidget* widget);

  /**
   * @brief Remove all maps from the list
   */
  void clear();
  /**
   * @brief Number of maps in the list
   * @return
   */
  int count();
  /**
   * @brief Get map item at index
   * @param i
   * @return A pointer to the map item or nullptr if index is out of bound
   */
  CMapItem* item(int i);

  /**
   * @brief Cast operator to get the tree widget
   */
  operator QTreeWidget*() { return treeWidget; }

  /**
   * @brief Show/hide hints depending on the map list's state
   */
  void updateHelpText();

  /**
   * @brief Add map item to the list
   *
   * Add the item as top level item and set the item's widget. Connect
   * the item's signals to properly handle changes.
   *
   * @param map
   */
  void addMap(CMapItem* map);

  /**
   * @brief Move map item to top of list
   * @param map
   */
  void moveMapToTop(CMapItem* map);

 signals:
  void sigChanged();

 public slots:
  static void slotMapHonk();

 private slots:
  void slotMoveHome();
  void slotMoveUp();
  void slotMoveDown();
  void slotMoveEnd();
  void slotRemove();
  void slotReloadMaps();
  void slotOverviewInfo();
  void slotContextMenu(const QPoint& point);
  void slotFilter(const QString& str);

 private:
  /**
   * @brief Move map item within map list
   * @param map     list item
   * @param from    from row
   * @param to      to row
   */
  void moveMapItem(CMapItem* map, const int from, const int to);

  QMenu* menu;
  QPointer<CCanvas> canvas;
  QPointer<QTabWidget> tabWidget;
};

#endif  // CMAPLIST_H
