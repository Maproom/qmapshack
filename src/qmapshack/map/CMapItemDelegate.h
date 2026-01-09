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

#ifndef CMAPITEMDELEGATE_H
#define CMAPITEMDELEGATE_H

#include <QHash>
#include <QPointer>
#include <QStyledItemDelegate>

class QTreeWidget;
class QVariantAnimation;
class IMapItem;

class CMapItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  CMapItemDelegate(QTreeWidget* parent);

  void setColor(const QModelIndex& index, const QColor& c);
  void setProcessing(const QModelIndex& index, bool on);
  void setAccess(const QModelIndex& index, const QString& info);

  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                   const QModelIndex& index) override;

  bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                 const QModelIndex& index) override;

  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& idx) const override;

  void reset() { data.clear(); };

 signals:
  void sigUpdateItem(const QString& key);
  // signal used internally to decouple threads
  void sigSetProcessing(const QModelIndex, bool);

 private slots:
  void slotSetProcessing(const QModelIndex& index, bool on);

 private:
  void hideIndicator(const QModelIndex& index);
  void showIndicator(const QModelIndex& index);

  IMapItem* indexToItem(const QModelIndex& index) const;
  QString keyFromIndex(const QModelIndex& index) const;

  std::tuple<QFont, QRect, QRect, QRect, QRect, QRect> getRectangles(const QStyleOptionViewItem& opt,
                                                                     bool isActive) const;

  struct animations_t;
  animations_t& getAnimations(const QModelIndex& index);
  const animations_t& getAnimations(const QModelIndex& index) const;

  QPointer<QTreeWidget> treeWidget;

  struct animations_t {
    QPointer<QVariantAnimation> animColorIndicator;
    QPointer<QVariantAnimation> animOpacityIndicator;
    QPointer<QVariantAnimation> animFlashingIndicator;
    QPointer<QVariantAnimation> animAccessInfo;
    qreal opacityIndicator = 0.0;
    QColor colorIndicator = Qt::lightGray;
    QString accessInfo;
    qreal opacityAccessInfo = 0.0;
  };

  struct item_data_t {
    QIcon icon;
    animations_t animations;
  };

  mutable QHash<QString, item_data_t> data;
};

#endif  // CMAPITEMDELEGATE_H
