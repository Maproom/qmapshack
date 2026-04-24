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

class QTreeWidget;
class IDBItem;

class CDBItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  CDBItemDelegate(QTreeWidget* parent);
  virtual ~CDBItemDelegate() = default;

#ifdef Q_CC_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif  // Q_CC_GNU

  union folder_t {
    struct {
      quint32 countFolders : 1;
      quint32 countItems : 1;
    };
    quint32 flags;
  };

  union item_t {
    struct {
      quint32 infoText : 1;
    };
    quint32 flags;
  };

#ifdef Q_CC_GNU
#pragma GCC diagnostic pop
#endif  // Q_CC_GNU

  /**
   * @brief The control flags for the project's and GIS item's status line
   */
  struct item_status_ctrl_t {
    folder_t folder = {.countFolders = 1, .countItems = 1};
    item_t item = {.infoText = 1};
    qint32 statusSizeFolder = 3;
    qint32 statusSizeItem = 3;
  };

  /// Current state of the status line control flags
  const item_status_ctrl_t& getStatusItemsControl() const { return itemStatusControl; }
  /// Update status line control flags. This will update the tree widget
  void setStatusItemsControl(const item_status_ctrl_t& settings);

  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                   const QModelIndex& index) override;
  bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                 const QModelIndex& index) override;

 private:
  IDBItem* indexToItem(const QModelIndex& index) const;
  static void drawToolButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect, const QIcon& icon,
                             bool enabled, bool pressed);
  std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> getRectanglesFolder(const QStyleOptionViewItem& opt,
                                                                           const IDBItem& item) const;
  std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> getRectanglesItem(const QStyleOptionViewItem& opt,
                                                                         const IDBItem& item) const;
  void paintFolder(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IDBItem& item) const;
  void paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IDBItem& item) const;

  bool editorEventFolder(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                         const QModelIndex& index, IDBItem& item);
  bool editorEventItem(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                       const QModelIndex& index, IDBItem& item);

  bool helpEventFolder(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                       const QModelIndex& index, const IDBItem& item);
  bool helpEventItem(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                     const QModelIndex& index, const IDBItem& item);

  void toggleCheckState(IDBItem& item);

  QPointer<QTreeWidget> treeWidget;
  item_status_ctrl_t itemStatusControl;
};

#endif  // CDBITEMDELEGATE_H
