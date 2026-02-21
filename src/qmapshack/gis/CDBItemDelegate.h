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

  void toggleCheckState(IDBItem& item);

  QPointer<QTreeWidget> treeWidget;
};

#endif  // CDBITEMDELEGATE_H
