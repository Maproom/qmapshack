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

/**
 * @brief QStyledItemDelegate for the map-source tree view.
 *
 * Each row shows a map-source icon, name, status text, a full-height activate/deactivate
 * button, and a thin animated indicator bar. Four QVariantAnimation objects per row handle
 * the indicator color, indicator opacity, processing pulse, and access-info fade-out.
 *
 * Animation state is stored in a per-item animations_t keyed by IMapItem::getKey() inside
 * the mutable @c data hash. getAnimations() lazily initialises animations on first access.
 *
 * setProcessing() may be called from a draw thread; it emits sigSetProcessing(), which is
 * connected to slotSetProcessing() via a queued connection to keep all animation state
 * changes on the main thread.
 */
class CMapItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  CMapItemDelegate(QTreeWidget* parent);

  /**
   * @brief Smoothly animate the indicator bar to a new color.
   *
   * No-op if the color is already set to @p c.
   */
  void setColor(const QModelIndex& index, const QColor& c);

  /**
   * @brief Start or stop the processing-pulse animation on the indicator bar.
   *
   * Thread-safe: delegates to slotSetProcessing() via a queued signal.
   * When @p on is false and the item is active, the indicator fades in; otherwise it fades out.
   */
  void setProcessing(const QModelIndex& index, bool on);

  /**
   * @brief Display a transient access-info string that fades out over 2 seconds.
   */
  void setAccess(const QModelIndex& index, const QString& info);

  /**
   * @brief Capture the item's icon pixmap and suppress Qt's default decoration rendering.
   *
   * Stores the 48×48 pixmap in @c data[key].icon, then clears @p option's icon and sets
   * decorationSize to zero so the base-class paint() leaves the icon zone empty. paint()
   * then draws the cached pixmap at the exact size and position computed by getRectangles().
   */
  void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

  /**
   * @brief Paint the map-item row: icon, name, status, activate button, indicator, access-info.
   */
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  /**
   * @brief Handle clicks on the activate/deactivate button; returns true to consume the event.
   */
  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                   const QModelIndex& index) override;

  /**
   * @brief Show tooltips for the button, indicator bar, and (when truncated) the name.
   */
  bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                 const QModelIndex& index) override;

  /**
   * @brief Return the row height calculated from CRowBuilder::rowHeight() with a 22 px minimum.
   */
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& idx) const override;

  /** @brief Clear all per-item cached icons and animation state. */
  void reset() { data.clear(); }

 signals:
  /** Emitted whenever animation state changes so the view repaints the affected item. */
  void sigUpdateItem(const QString& key);
  /** Internal: relays setProcessing() calls from non-main threads to slotSetProcessing(). */
  void sigSetProcessing(const QModelIndex, bool);

 private slots:
  /** @brief Receives sigSetProcessing() on the main thread and drives the animation accordingly. */
  void slotSetProcessing(const QModelIndex& index, bool on);

 private:
  /** @brief Fade the indicator bar out to zero opacity. No-op if already hidden. */
  void hideIndicator(const QModelIndex& index);
  /** @brief Fade the indicator bar in to full opacity. No-op if already fully visible. */
  void showIndicator(const QModelIndex& index);

  /** @brief Cast the model index to an IMapItem; returns nullptr if the index is not an IMapItem. */
  IMapItem* indexToItem(const QModelIndex& index) const;
  /** @brief Return the string key used to look up per-item data; empty string if index is invalid. */
  QString keyFromIndex(const QModelIndex& index) const;

  /// Rectangles and fonts used to paint a map item row
  struct MapItemLayout {
    QFont fontName;
    QFont fontStatus;
    QRect rectIcon;
    QRect rectButton;    /**< Full-height activate/deactivate toggle button on the right. */
    QRect rectIndicator; /**< Thin vertical bar to the left of the button; animated by setColor/setProcessing. */
    QRect rectName;
    QRect rectStatus;
  };

  /** @brief Compute the layout rects from @p opt.rect using CRowBuilder. */
  MapItemLayout getRectangles(const QStyleOptionViewItem& opt) const;

  struct animations_t;  // defined below; forward-declared so getAnimations() can reference it

  /**
   * @brief Return the animations_t for @p index, lazily constructing all four animations on first access.
   */
  animations_t& getAnimations(const QModelIndex& index);
  const animations_t& getAnimations(const QModelIndex& index) const;

  QPointer<QTreeWidget> treeWidget;

  /** @brief Per-item animation state for the indicator bar and access-info overlay. */
  struct animations_t {
    QPointer<QVariantAnimation> animColorIndicator;    /**< Smooth color transition of the indicator bar. */
    QPointer<QVariantAnimation> animOpacityIndicator;  /**< Fade-in / fade-out of the indicator bar. */
    QPointer<QVariantAnimation> animFlashingIndicator; /**< Looping opacity pulse while the map is being processed. */
    QPointer<QVariantAnimation> animAccessInfo;        /**< Fade-out of the transient access-info text over 2 s. */
    qreal opacityIndicator = 0.0;
    QColor colorIndicator = Qt::lightGray;
    QString accessInfo;
    qreal opacityAccessInfo = 0.0;
  };

  /** @brief Cached icon pixmap and animation state for one map item. */
  struct item_data_t {
    QPixmap icon; /**< 48×48 pixmap captured by initStyleOption(); drawn by paint(). */
    animations_t animations;
  };

  mutable QHash<QString, item_data_t> data;
};

#endif  // CMAPITEMDELEGATE_H
