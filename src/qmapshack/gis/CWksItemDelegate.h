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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  union prj_t {
    struct {
      quint32 keywords : 1;
      quint32 gisStats : 1;
      quint32 distance : 1;
      quint32 ascent : 1;
      quint32 descent : 1;
    };
    quint32 flags;
  };

  union trk_t {
    struct {
      quint32 rating : 1;
      quint32 tags : 1;
      quint32 distance : 1;
      quint32 ascent : 1;
      quint32 descent : 1;
    };
    quint32 flags;
  };

  union wpt_t {
    struct {
      quint32 rating : 1;
      quint32 tags : 1;
      quint32 elevation : 1;
    };
    quint32 flags;
  };

  union rte_t {
    struct {
      quint32 rating : 1;
      quint32 tags : 1;
    };
    quint32 flags;
  };

  union area_t {
    struct {
      quint32 rating : 1;
      quint32 tags : 1;
    };
    quint32 flags;
  };
#pragma GCC diagnostic pop

  /**
   * @brief The control flags for the project's and GIS item's status line
   */
  struct item_status_ctrl_t {
    prj_t prj = {.flags = 0};
    trk_t trk = {.flags = 0};
    wpt_t wpt = {.flags = 0};
    rte_t rte = {.flags = 0};
    area_t area = {.flags = 0};
    qint32 statusSizePrj;
    qint32 statusSizeItem;
  };

  /// Current state of the status line control flags
  const item_status_ctrl_t& getStatusItemsControl() const { return itemStatusControl; }
  /// Update status line control flags. This will update the tree widget
  void setStatusItemsControl(const item_status_ctrl_t& settings);

  /**
   * @brief Paint an item in the tree widget
   */
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  /**
   * @brief Handle mouse press events for the item's tool buttons   *
   */
  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                   const QModelIndex& index) override;
  /**
   * @brief Show tool tips
   */
  bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                 const QModelIndex& index) override;
  /**
   * @brief Define the item size depending on the item type
   */
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  /**
   * @brief Create a line edit for the geo serach
   */
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  /**
   * @brief Resize the line edit of the geo search and the project filter (!)
   */
  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
  /**
   * @brief Copy text from the line edit into the geo search
   */
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
  /**
   * @brief Setup the line edit with the last address search of the geo search
   */
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;

 signals:
  void sigUpdateCanvas();

 private:
  IWksItem* indexToItem(const QModelIndex& index) const;

  std::tuple<QFont, QFont, QRect, QRect, QRect, QRect, QRect, QRect, QRect> getRectanglesProject(
      const QStyleOptionViewItem& opt, IWksItem& item) const;
  std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> getRectanglesDevice(const QStyleOptionViewItem& opt,
                                                                           const IWksItem& item) const;
  std::tuple<QFont, QFont, QRect, QRect, QRect, QRect, QRect, QRect> getRectanglesGeoSearch(
      const QStyleOptionViewItem& opt) const;
  std::tuple<QFont, QRect, QRect> getRectanglesGeoSearchError(const QStyleOptionViewItem& opt) const;
  std::tuple<QFont, QFont, QRect, QRect, QRect, QRect> getRectanglesItem(const QStyleOptionViewItem& opt,
                                                                         const IWksItem& item) const;

  static void drawToolButton(QPainter* p, const QStyleOptionViewItem& opt, const QRect& rect, const QIcon& icon,
                             bool enabled, bool pressed);

  void drawRatingStars(qreal rating, QPainter* p, QIcon::Mode iconMode, QRect& rectStatus) const;

  void paintProject(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item) const;
  void paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IWksItem& item) const;
  void paintDevice(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IWksItem& item) const;
  void paintGeoSearch(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                      const IWksItem& item) const;
  void paintGeoSearchError(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                           const IWksItem& item) const;

  bool mousePressProject(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item);
  bool mousePressDevice(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item);
  bool mousePressGeoSearch(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item);

  bool helpEventProject(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                        const QStyleOptionViewItem& opt, IWksItem& item);

  bool helpEventItem(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                     const QStyleOptionViewItem& opt, const IWksItem& item);

  bool helpEventGeoSearch(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                          const QStyleOptionViewItem& opt, const IWksItem& item);

  bool helpEventGeoSearchError(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                               const QStyleOptionViewItem& opt, const IWksItem& item);

  QPointer<CGisListWks> treeWidget;

  item_status_ctrl_t itemStatusControl;
};

#endif  // CGISITEMDELEGATE_H
