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

#ifndef CDBITEMDELEGATE_H
#define CDBITEMDELEGATE_H

#include <QPointer>
#include <QStyledItemDelegate>
#include <utility>

class QTreeWidget;
class IDBItem;

/**
 * @brief QStyledItemDelegate for the database tree view.
 *
 * Renders two row types: folders (databases, groups, projects, lost-and-found)
 * and GIS items. Each row has a load/unload check-state button that is hit-tested
 * via editorEvent() using the same layout as paintFolder() / paintItem().
 *
 * The optional status line is driven by item_status_ctrl_t, persisted in QSettings
 * under Database/Decorations and changeable at runtime via setStatusItemsControl().
 */
class CDBItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  CDBItemDelegate(QTreeWidget* parent);
  virtual ~CDBItemDelegate() = default;

#ifdef Q_CC_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif  // Q_CC_GNU

  /** @brief Bit flags controlling which statistics appear in a folder's status line. */
  union folder_t {
    struct {
      quint32 countFolders : 1; /**< Show sub-folder counts (groups, projects, other). */
      quint32 countItems : 1;   /**< Show GIS item counts (W/T/R/A). */
    };
    quint32 flags;
  };

  /** @brief Bit flags controlling which statistics appear in a GIS item's status line. */
  union item_t {
    struct {
      quint32 infoText : 1; /**< Show the first line of the item's tooltip text. */
    };
    quint32 flags;
  };

#ifdef Q_CC_GNU
#pragma GCC diagnostic pop
#endif  // Q_CC_GNU

  /**
   * @brief Controls which fields appear in the status line and at what font size.
   *
   * The statusSize* fields specify how many points smaller than the name font the
   * status font should be. A value of -1 (kFontSizeInvalid) suppresses the status
   * line entirely for that row type.
   */
  struct item_status_ctrl_t {
    folder_t folder = {.countFolders = 1, .countItems = 1};
    item_t item = {.infoText = 1};
    qint32 statusSizeFolder = 3; /**< Point-size reduction for the folder status font; -1 hides the line. */
    qint32 statusSizeItem = 3;   /**< Point-size reduction for the item status font; -1 hides the line. */
  };

  /// Current state of the status line control flags
  const item_status_ctrl_t& getStatusItemsControl() const { return itemStatusControl; }
  /// Update status line control flags and persist them to QSettings. Triggers a layout change.
  void setStatusItemsControl(const item_status_ctrl_t& settings);

  /**
   * @brief Suppress Qt's default icon decoration so paint() can draw the icon itself.
   *
   * Intentionally left empty; icon drawing is handled entirely inside paint().
   */
  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

  /**
   * @brief Return the preferred row height for the given item type.
   *
   * Height is computed from CRowBuilder::rowHeight() using the name and status
   * font metrics, with a minimum of 22 px.
   */
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  /**
   * @brief Paint a row according to its type (folder or GIS item).
   */
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  /**
   * @brief Handle mouse events on the check-state button embedded in each row.
   *
   * A single click on the button or a double-click anywhere on an item row
   * toggles the load/unload state. Returns true to consume the event.
   */
  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                   const QModelIndex& index) override;

  /**
   * @brief Show a tooltip for the hovered region of the row.
   */
  bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                 const QModelIndex& index) override;

 private:
  /** @brief Cast the model index to an IDBItem; returns nullptr if the index is not an IDBItem. */
  IDBItem* indexToItem(const QModelIndex& index) const;

  /** @brief Draw the three-state load/unload button (unchecked, partially checked, checked). */
  static void drawCheckStateButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect,
                                   Qt::CheckState state);

  /**
   * @brief Resolve the initial color group and role from the view's selection and focus state.
   *
   * Returns {colorGroup, colorRole}. Callers may further adjust colorGroup based on the item's
   * check state before passing both to QPalette::color().
   */
  static std::pair<QPalette::ColorGroup, QPalette::ColorRole> resolveColorState(const QStyleOptionViewItem& opt);

  /// Rectangles and fonts used to paint a folder or item row
  struct ItemLayout {
    QFont fontName;
    QFont fontStatus;
    QRect rectIcon;
    QRect rectName;
    QRect rectStatus;
    QRect rectButton; /**< Load/unload button; invalid (default-constructed) for rows that have none. */
  };

  /** @brief Compute the layout rects for a folder row; rectButton is valid for projects and lost-and-found. */
  ItemLayout getRectanglesFolder(const QStyleOptionViewItem& opt, const IDBItem& item) const;
  /** @brief Compute the layout rects for a GIS item row; rectButton is invalid for lost-and-found children. */
  ItemLayout getRectanglesItem(const QStyleOptionViewItem& opt, const IDBItem& item) const;

  /** @brief Paint a folder row (database, group, project) with icon, check-state button, name, and status. */
  void paintFolder(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IDBItem& item) const;
  /** @brief Paint a GIS item row with icon, optional load button, name, and status/age text. */
  void paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IDBItem& item) const;

  /** @brief Handle mouse events on a folder row's check-state button. */
  bool editorEventFolder(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                         const QModelIndex& index, IDBItem& item);
  /** @brief Handle mouse events on a GIS item row (single click on button, or double-click anywhere). */
  bool editorEventItem(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                       const QModelIndex& index, IDBItem& item);

  /** @brief Show tooltip for a hovered folder row zone. */
  bool helpEventFolder(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                       const QModelIndex& index, const IDBItem& item);
  /** @brief Show tooltip for a hovered item row zone. */
  bool helpEventItem(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                     const QModelIndex& index, const IDBItem& item);

  /**
   * @brief Toggle the item between Unchecked and Checked, then emit itemChanged.
   *
   * For lost-and-found folders this also repaints all child items, since their
   * visual state depends on the parent's check state.
   */
  void toggleCheckState(IDBItem& item);

  QPointer<QTreeWidget> treeWidget;
  item_status_ctrl_t itemStatusControl;
};

#endif  // CDBITEMDELEGATE_H
