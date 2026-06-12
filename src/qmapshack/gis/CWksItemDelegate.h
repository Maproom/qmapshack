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

#ifndef CWKSITEMDELEGATE_H
#define CWKSITEMDELEGATE_H

#include <QPointer>
#include <QStyledItemDelegate>
#include <QtCompilerDetection>

class CGisListWks;
class IWksItem;

/**
 * @brief QStyledItemDelegate for the workspace (GIS) tree view.
 *
 * Renders five distinct row types — Project, Device, GeoSearch, GeoSearchError,
 * and GIS Item — each with its own getRectangles*() layout helper. The paint*(),
 * mousePress*(), and helpEvent*() methods use the same layout to keep hit-testing
 * consistent with rendering.
 *
 * The optional status line below the item name is driven by item_status_ctrl_t,
 * whose flags are persisted in QSettings under Workspace/Decorations and can be
 * changed at runtime via setStatusItemsControl().
 */
class CWksItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  CWksItemDelegate(CGisListWks* parent);
  virtual ~CWksItemDelegate() = default;

#ifdef Q_CC_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif  // Q_CC_GNU

  /** @brief Bit flags controlling which statistics appear in the project status line. */
  union prj_t {
    struct {
      quint32 keywords : 1; /**< Show project keywords. */
      quint32 gisStats : 1; /**< Show item counts (tracks, waypoints, routes, areas). */
      quint32 distance : 1; /**< Show total distance. */
      quint32 ascent : 1;   /**< Show total ascent. */
      quint32 descent : 1;  /**< Show total descent. */
    };
    quint32 flags;
  };

  /** @brief Bit flags controlling which statistics appear in the track status line. */
  union trk_t {
    struct {
      quint32 rating : 1;   /**< Show star rating. */
      quint32 tags : 1;     /**< Show comma-separated tags. */
      quint32 distance : 1; /**< Show total distance. */
      quint32 ascent : 1;   /**< Show total ascent. */
      quint32 descent : 1;  /**< Show total descent. */
    };
    quint32 flags;
  };

  /** @brief Bit flags controlling which statistics appear in the waypoint status line. */
  union wpt_t {
    struct {
      quint32 rating : 1;    /**< Show star rating. */
      quint32 tags : 1;      /**< Show comma-separated tags. */
      quint32 elevation : 1; /**< Show elevation. */
    };
    quint32 flags;
  };

  /** @brief Bit flags controlling which statistics appear in the route status line. */
  union rte_t {
    struct {
      quint32 rating : 1; /**< Show star rating. */
      quint32 tags : 1;   /**< Show comma-separated tags. */
    };
    quint32 flags;
  };

  /** @brief Bit flags controlling which statistics appear in the area status line. */
  union area_t {
    struct {
      quint32 rating : 1; /**< Show star rating. */
      quint32 tags : 1;   /**< Show comma-separated tags. */
    };
    quint32 flags;
  };

#ifdef Q_CC_GNU
#pragma GCC diagnostic pop
#endif  // Q_CC_GNU

  /**
   * @brief Controls which fields appear in the status line and at what font size.
   *
   * The statusSize* fields specify how many points smaller than the name font
   * the status font should be. A value of -1 (kFontSizeInvalid) suppresses the
   * status line entirely for that row type.
   */
  struct item_status_ctrl_t {
    prj_t prj = {.flags = 0};
    trk_t trk = {.flags = 0};
    wpt_t wpt = {.flags = 0};
    rte_t rte = {.flags = 0};
    area_t area = {.flags = 0};
    qint32 statusSizePrj;  /**< Point-size reduction for the project status font; -1 hides the line. */
    qint32 statusSizeItem; /**< Point-size reduction for the item status font; -1 hides the line. */
  };

  /// Current state of the status line control flags
  const item_status_ctrl_t& getStatusItemsControl() const { return itemStatusControl; }
  /// Update status line control flags and persist them to QSettings. Triggers a layout change.
  void setStatusItemsControl(const item_status_ctrl_t& settings);

  /**
   * @brief Paint a row according to its base type (Project, Item, Device, GeoSearch, etc.).
   */
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  /**
   * @brief Handle mouse-button presses on tool buttons embedded in the row.
   *
   * Delegates to mousePressProject(), mousePressDevice(), or mousePressGeoSearch()
   * depending on the item's base type. Returns true to consume the event when a
   * tool button is hit.
   */
  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& opt,
                   const QModelIndex& index) override;

  /**
   * @brief Show a tooltip for the hovered region of the row.
   *
   * Hit-tests the cursor position against each named rect in the row's layout
   * and shows a context-appropriate tooltip.
   */
  bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& opt,
                 const QModelIndex& index) override;

  /**
   * @brief Return the preferred row height for the given item type.
   *
   * Height is computed from CRowBuilder::rowHeight() using the name and status
   * font metrics, with a minimum of 22 px.
   */
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  /**
   * @brief Create an inline QLineEdit for the geo-search address input.
   *
   * Returns nullptr for every item type other than GeoSearch, or when the
   * geo-search item has input disabled.
   */
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  /**
   * @brief Position the inline editor (geo-search line edit or project filter) within the row.
   */
  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

  /**
   * @brief Commit the line-edit text to the geo-search item, triggering a new search.
   */
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

  /**
   * @brief Pre-populate the line edit with the geo-search's most recent address.
   */
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;

 signals:
  void sigUpdateCanvas();

 private:
  /** @brief Cast the model index to an IWksItem; returns nullptr if the index is not an IWksItem. */
  IWksItem* indexToItem(const QModelIndex& index) const;

  /// Rectangles and fonts used to paint a project row
  struct ProjectLayout {
    QFont fontName;
    QFont fontStatus;
    QRect rectIcon;
    QRect rectName;
    QRect rectStatus;
    QRect rectProgress;
    QRect rectVisible;
    QRect rectSave;
    QRect rectActiveProject;
    QRect rectAutoSyncDev;
  };

  /// Rectangles and fonts used to paint a device row
  struct DeviceLayout {
    QFont fontName;
    QFont fontStatus;
    QRect rectIcon;
    QRect rectName;
    QRect rectStatus;
    QRect rectProgress;
    QRect rectVisible;
  };

  /// Rectangles and fonts used to paint a geo search row
  struct GeoSearchLayout {
    QFont fontSearch;
    QFont fontStatus;
    QRect rectIcon;
    QRect rectSetup;
    QRect rectLineEdit;
    QRect rectStatus;
    QRect rectWptIcon;
    QRect rectVisible;
  };

  /// Rectangle and font used to paint a geo search error row
  struct GeoSearchErrorLayout {
    QFont font;
    QRect rectIcon;
    QRect rectName;
  };

  /// Rectangles and fonts used to paint a GIS item row
  struct ItemLayout {
    QFont fontName;
    QFont fontStatus;
    QRect rectIcon;
    QRect rectName;
    QRect rectStatus;
    QRect rectChanged;
  };

  /** @brief Compute the layout rects for a project row; optional buttons are only valid when visible. */
  ProjectLayout getRectanglesProject(const QStyleOptionViewItem& opt, IWksItem& item) const;
  /** @brief Compute the layout rects for a device row. */
  DeviceLayout getRectanglesDevice(const QStyleOptionViewItem& opt, const IWksItem& item) const;
  /** @brief Compute the layout rects for the geo-search input row. */
  GeoSearchLayout getRectanglesGeoSearch(const QStyleOptionViewItem& opt) const;
  /** @brief Compute the layout rects for a geo-search error row (single icon + text). */
  GeoSearchErrorLayout getRectanglesGeoSearchError(const QStyleOptionViewItem& opt) const;
  /** @brief Compute the layout rects for a GIS item row; the changed-button rect is invalid when not needed. */
  ItemLayout getRectanglesItem(const QStyleOptionViewItem& opt, const IWksItem& item) const;

  /** @brief Draw a two-tone horizontal progress bar (white shadow, green fill) inside @p rect. */
  static void drawProgressBar(QPainter* p, const QRect& rect, qreal progress);

  /**
   * @brief Build the status text showing distance (↦), ascent (↗), and descent (↘).
   *
   * Each component is only included when the corresponding show* flag is set and
   * the value is non-zero and not NOFLOAT. Elevation components are suppressed when
   * @p elevationInvalid is true.
   */
  static QString distanceAscentDescentStatus(qreal distance, qreal ascent, qreal descent, bool elevationInvalid,
                                             bool showDistance, bool showAscent, bool showDescent);

  /**
   * @brief Draw N star icons at the left of @p rectStatus and advance its left edge past them.
   *
   * @note Modifies @p rectStatus in-place so subsequent text is drawn to the right of the stars.
   */
  void drawRatingStars(qreal rating, QPainter* p, QIcon::Mode iconMode, QRect& rectStatus) const;

  /** @brief Paint a project row including icon, optional tool buttons, name, and status line. */
  void paintProject(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item) const;
  /** @brief Paint a GIS item row (track, waypoint, route, area) with icon and optional status line. */
  void paintItem(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IWksItem& item) const;
  /** @brief Paint a device row with icon, visibility button, and status/progress line. */
  void paintDevice(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index, const IWksItem& item) const;
  /** @brief Paint the geo-search row with address text, service name, and icon buttons. */
  void paintGeoSearch(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                      const IWksItem& item) const;
  /** @brief Paint a geo-search error row (error icon + message text). */
  void paintGeoSearchError(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index,
                           const IWksItem& item) const;

  /** @brief Handle button clicks in a project row; returns true when a button was hit. */
  bool mousePressProject(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item);
  /** @brief Handle button clicks in a device row; returns true when a button was hit. */
  bool mousePressDevice(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item);
  /** @brief Handle button clicks in the geo-search row; returns false on the line-edit zone
   *         so the view's default edit activation still fires. */
  bool mousePressGeoSearch(QMouseEvent* me, const QStyleOptionViewItem& opt, const QModelIndex& index, IWksItem& item);

  /** @brief Show tooltips for the hovered zone of a project row. */
  bool helpEventProject(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                        const QStyleOptionViewItem& opt, IWksItem& item);

  /** @brief Show tooltips for the hovered zone of a GIS item row. */
  bool helpEventItem(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                     const QStyleOptionViewItem& opt, const IWksItem& item);

  /** @brief Show tooltips for the hovered zone of the geo-search row. */
  bool helpEventGeoSearch(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                          const QStyleOptionViewItem& opt, const IWksItem& item);

  /** @brief Show the full error message tooltip for a geo-search error row. */
  bool helpEventGeoSearchError(const QPoint& pos, const QPoint& posGlobal, QAbstractItemView* view,
                               const QStyleOptionViewItem& opt, const IWksItem& item);

  QPointer<CGisListWks> treeWidget;

  item_status_ctrl_t itemStatusControl;
};

#endif  // CWKSITEMDELEGATE_H
