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

#ifndef CGISITEMDELEGATE_H
#define CGISITEMDELEGATE_H

#include <QPointer>
#include <QStyledItemDelegate>

class CGisListWks;
class IWksItem;

class CWksItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  CWksItemDelegate(CGisListWks* parent);
  virtual ~CWksItemDelegate() = default;

  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                   const QModelIndex& index) override;
  bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                 const QModelIndex& index) override;
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
 signals:
  void sigUpdateCanvas();

 private:
  static inline QString trRichText(const char* msg) { return "<div>" + tr(msg) + "</div>"; }
  IWksItem* indexToItem(const QModelIndex& index) const;
  static std::tuple<QFont, QFont, QRect, QRect, QRect, QRect, QRect, QRect, QRect> getRectanglesProject(
      const QStyleOptionViewItem& opt);
  static std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> getRectanglesDevice(const QStyleOptionViewItem& opt);
  static std::tuple<QFont, QRect, QRect, QRect, QRect, QRect> getRectanglesGeoSearch(const QStyleOptionViewItem& opt);
  static std::tuple<QFont, QRect, QRect> getRectanglesGeoSearchError(const QStyleOptionViewItem& opt);

  static std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> getRectanglesItem(const QStyleOptionViewItem& opt);
  static void drawToolButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect, const QIcon& icon,
                             bool enabled, bool pressed);

  void paintProject(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IWksItem* item) const;
  void paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IWksItem* item) const;
  void paintDevice(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IWksItem* item) const;
  void paintGeoSearch(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                      const IWksItem* item) const;
  void paintGeoSearchError(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                           const IWksItem* item) const;

  bool mousePressProject(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem* item);
  bool mousePressDevice(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem* item);
  bool mousePressGeoSearch(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem* item);

  bool helpEventProject(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                        const QStyleOptionViewItem& opt, const IWksItem* item);

  bool helpEventItem(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                     const QStyleOptionViewItem& opt, const IWksItem* item);

  bool helpEventGeoSearch(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                          const QStyleOptionViewItem& opt, const IWksItem* item);

  bool helpEventGeoSearchError(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                               const QStyleOptionViewItem& opt, const IWksItem* item);

  QPointer<CGisListWks> treeWidget;
};

#endif  // CGISITEMDELEGATE_H
