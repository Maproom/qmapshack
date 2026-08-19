/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CCANVAS_H
#define CCANVAS_H

#include <QMap>
#include <QPainter>
#include <QPointer>
#include <QWidget>
#include <optional>

#include "gis/IGisItem.h"
#include "theme/CUiTheme.h"

class IDrawContext;
class CMapDraw;
class CMapVRT;
class CGrid;
class CDemDraw;
class CDemVRT;
class CVrtAdvisoryDialog;
class CPoiDraw;
class QGestureEvent;
class CGisDraw;
class CRtDraw;
class CGisItemWpt;
class CGisItemTrk;
class CGisItemRte;
class CGisItemOvlArea;
class CColorLegend;
class CMouseAdapter;
class QSettings;
class QPointF;
class IMouse;
class QTimer;
class QMovie;
class QLabel;
class QTextBrowser;
class IPlot;
struct SGisLine;
struct IPoiItem;
class CTableTrkInfo;

class CCanvas : public QWidget {
  Q_OBJECT
 public:
  CCanvas(QWidget* parent, const QString& storedKey);
  virtual ~CCanvas();

  static void setOverrideCursor(const QCursor& cursor, const QString& src);
  static void restoreOverrideCursor(const QString& src);
  static void changeOverrideCursor(const QCursor& cursor, const QString& src);

  void saveConfig(QSettings& cfg);
  void loadConfig(QSettings& cfg);

  void setupGrid();
  void convertGridPos2Str(const QPointF& pos, QString& str, bool simple);
  void convertRad2Px(QPointF& pos) const;
  void convertPx2Rad(QPointF& pos) const;

  void setupBackgroundColor();

  void setup();
  QString getProjection();
  void setProjection(const QString& proj);

  enum scales_type_e { eScalesDefault, eScalesSquare };

  void setScales(const scales_type_e type);
  scales_type_e getScalesType();

  const QString& getKey() const { return _key; }
  const QString& getName() const { return _name; }
  void setName(const QString& newName);

  qreal getElevationAt(const QPointF& pos, bool checkScale) const;
  void getElevationAt(const QPolygonF& pos, QPolygonF& ele) const;
  void getElevationAt(SGisLine& line) const;

  qreal getSlopeAt(const QPointF& pos) const;
  void getSlopeAt(const QPolygonF& pos, QPolygonF& slope) const;

  void moveTo(const QPointF& newFocus);
  void moveMap(const QPointF& delta);
  void zoomTo(const QRectF& rect);
  void zoom(int index);
  void displayInfo(const QPoint& px);
  /// The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
  ///  Thus the location where to draw the highlight is separately given
  void findPoiCloseBy(const QPoint& px, QSet<IPoiItem>& poiItems, QList<QPointF>& posPoiHighlight) const;
  /// The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
  ///  Thus the location where to draw the highlight is separately given
  void findPoisIn(const QRectF& degRect, QSet<IPoiItem>& poiItems, QList<QPointF>& posPoiHighlight) const;

  enum redraw_e {
    eRedrawNone = 0,
    eRedrawMap = 0x01,
    eRedrawDem = 0x02,
    eRedrawGis = 0x04,
    eRedrawMouse = 0x08,
    eRedrawRt = 0x10,
    eRedrawPoi = 0x20,
    eRedrawAll = 0xFFFFFFFF
  };

  static void triggerCompleteUpdate(CCanvas::redraw_e flags);

  void abortMouse();
  void resetMouse();
  void mouseTrackingLost();
  void setMouseMoveWpt(CGisItemWpt& wpt);
  void setMouseRadiusWpt(CGisItemWpt& wpt);
  void setMouseEditTrk(CGisItemTrk& trk);
  void setMouseRangeTrk(CGisItemTrk& trk);
  void setMouseEditTrk(const QPointF& pt);
  void setMouseEditRte(const QPointF& pt);
  void setMouseEditRte(CGisItemRte& rte);
  void setMouseEditArea(CGisItemOvlArea& area);
  void setMouseEditArea(const QPointF& pt);
  void setMouseWptBubble(const IGisItem::key_t& key);
  void setMouseRuler();
  void setMousePrint();
  void setMouseSelect();

  void showProfileAsWindow();
  void showProfile(bool yes);

  void buildHelpText();

  /**
     @brief Add a message by key to be reported on the canvas

     Messages from various sources will be collected in a list and displayed in the top left corner
     of the widget.

     @note The object reporting has to take care to remove the message by reporting an empty string.

     @param key   the key to identify the reporting object
     @param msg   the message to report
   */
  void reportStatus(const QString& key, const QString& msg);

  /**
     @brief reportStatus() for a message that carries a status colour.

     Pass @p role instead of wrapping @p msg in CUiTheme::span() - the colour is applied when the
     panel is rendered, so the message follows a colour scheme change.
   */
  void reportStatus(const QString& key, CUiTheme::Role role, const QString& msg);

  /**
     @brief Find a matching street polyline

     The polyline must be close enough in terms of pixel to point 1 and 2. "Close enough" is defined by
     the threshold. The returned polyline uses lon/lat as coordinates.

     @param pt1           first point in [rad]
     @param pt2           second point in [rad]
     @param threshold     the "close enough" threshold in [pixel]
     @param polyline      the resulting polyline, if any, in [rad]
     @return              Return true if a line has been found.
   */
  bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline);

  void print(QPainter& p, const QRectF& area, const QPointF& focus, bool printScale = true);

  /**
     @brief Set a single map file to be shown on the canvas

     @param filename   the map's file path, can be a resource, too
   */
  void setMap(const QString& filename);

  void followPosition(const QPointF& pos);

  /// Allows showing the track overlays if they are set in CMainWindow
  void allowShowTrackOverlays(bool show) { showTrackOverlays = show; }

  /// save the size of the track profile if it is in window mode
  void saveSizeTrackProfile();

  static qreal gisLayerOpacity;

  void linkMapViewEnabled();

 signals:
  void sigMousePosition(const QPointF& pos, qreal ele, qreal slope);
  void sigMoveAndZoom(int index, const QPointF& focus);
  void sigZoom();
  void sigMove();
  void sigResize(const QSize& size);
  void sigNameChanged(const CCanvas& self);
  void sigCanvasIsCurrent();

 public slots:
  void slotTriggerCompleteUpdate(CCanvas::redraw_e flags);
  void slotUpdateTrackInfo(bool updateVisuals);
  void slotCheckTrackOnFocus();

 protected:
  bool event(QEvent*) override;
  bool gestureEvent(QGestureEvent* e);
  void changeEvent(QEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  void paintEvent(QPaintEvent* e) override;
  void mousePressEvent(QMouseEvent* e) override;
  void mouseMoveEvent(QMouseEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void mouseDoubleClickEvent(QMouseEvent* e) override;
  void wheelEvent(QWheelEvent* e) override;
  void enterEvent(QEnterEvent* e) override;
  void leaveEvent(QEvent* e) override;
  void keyPressEvent(QKeyEvent* e) override;

 private slots:
  void slotToolTip();

  /**
     @brief Apply the canvas' current size and device pixel ratio to all draw context objects

     Always reads the current geometry instead of taking it from an event: a retry runs long after
     the event that scheduled it, and the canvas may have been resized again meanwhile.

     If a draw thread blocks the request the retry timer takes over and nothing is applied, so the
     layers cannot end up disagreeing about the viewport.
   */
  void slotUpdateDrawContextViewport();

  /// @brief Show the overview advisory dialog for a CDemVRT that hit the render timeout
  /// with overviews missing/inadequate; wires the dialog's "don't show again" result back
  /// to source.
  void slotShowDemOverviewAdvisory(QPointer<CDemVRT> source);

  /// @brief Same as slotShowDemOverviewAdvisory(), for a CMapVRT.
  void slotShowMapOverviewAdvisory(QPointer<CMapVRT> source);

 private:
  /// @brief Shared implementation behind slotShowDemOverviewAdvisory()/slotShowMapOverviewAdvisory():
  /// only the source type differs between CDemVRT/CMapVRT, both of which expose the
  /// identical getFilename()/getOverviewAdvisory() the
  /// dialog needs.
  template <class T>
  CVrtAdvisoryDialog* showOverviewAdvisory(QPointer<T> source);

  static QString generateKey(int count);
  void drawStatusMessages(QPainter& p);
  void drawTrackStatistic(QPainter& p);
  void drawScale(QPainter& p, QRectF drawRect);
  void drawScale(QPainter& p)  // Default use, drawRect is introduced for correct printing
  {
    drawScale(p, rect());
  }
  void setZoom(bool in, redraw_e& needsRedraw);
  void setSizeTrackProfile();
  /// @brief True if every draw context is built for the canvas' current size and pixel ratio
  bool drawContextViewportIsCurrent() const;

  /**
     @brief Resize all registered draw context objects

     @param s     the new size

     @return Return false if one of the draw contexts could not be resized
             because it's thread is running and blocking access to the data
   */
  bool setDrawContextSize(const QSize& s);

  bool setDrawContextPixelRatio(qreal ratio);

  /// @brief Block until no draw thread is running and the buffers can be rebuilt
  void waitForDrawContexts();

  bool isShowMinMaxSummary() const;
  bool isShowTrackSummary() const;
  bool isShowTrackInfoTable() const;
  bool isShowTrackInfoPoints() const;
  bool isShowTrackProfile() const;
  bool isShowTrackHighlight() const;

  // only change in ctor
  QString _key;
  // only change via setName()
  QString _name;

  bool showTrackOverlays = true;

  QColor backColor = 0x00FFFFBF;      //< the background color used in case of missing map tiles
  redraw_e needsRedraw = eRedrawAll;  //< set true to initiate a complete redraw of the screen content
  CMapDraw* map;                      //< the map object attached to this canvas
  CDemDraw* dem;                      //< the elevation data layer attached to this canvas
  CPoiDraw* poi;                      //< the poi database attached to this canvas
  CGisDraw* gis;                      //< the GIS data layer attached to this canvas
  CRtDraw* rt;                        //< the real time data layer attached to this canvas
  CGrid* grid;                        //< the grid attached to this canvas

  QList<IDrawContext*> allDrawContext;

  /// retry timer for a viewport update a running draw thread refused
  QTimer* timerViewport;

  /// the current point of focus (usually the canvas center)
  QPointF posFocus{0.209439510239, 0.855211333477};

  /// the mouse handler
  CMouseAdapter* mouse;

  /// tool tip timer for vector map tool tips
  QTimer* timerToolTip;
  /// the position of the tool tip
  QPoint posToolTip;

  /// load indicator for maps
  QMovie* loadIndicator1;
  QLabel* mapLoadIndicator;

  /// load indicator for DEM
  QMovie* loadIndicator2;
  QLabel* demLoadIndicator;

  /// load indicator for POI
  QMovie* loadIndicator3;
  QLabel* poiLoadIndicator;

  QPointer<CColorLegend> colorLegend;

  /// current accumulated angleDelta, used/required for zooming on trackpads
  int zoomAngleDelta = 0;

  /// timer to poll for track gaining/loosing focus
  QTimer* timerTrackOnFocus;
  /// the key of the currently focused track
  IGisItem::key_t keyTrackOnFocus;
  /// the track profile plot
  QPointer<IPlot> plotTrackProfile;
  /// a label with a track
  QLabel* labelTrackStatistic;

  QLabel* labelTrackInfo;

  /** @brief A status panel entry. The role stays unresolved so the panel can be re-rendered. */
  struct status_message_t {
    QString msg;
    std::optional<CUiTheme::Role> role;
  };

  void setStatus(const QString& key, const QString& msg, std::optional<CUiTheme::Role> role);
  void renderStatusMessages();

  QTextBrowser* textStatusMessages;
  QMap<QString, status_message_t> statusMessages;

  QMutex mousePressMutex;
  bool mouseLost = false;

  QTextBrowser* labelHelp = nullptr;
};

class CCanvasCursorLock {
 public:
  CCanvasCursorLock(const QCursor& cursor, const QString& src) : src(src) { CCanvas::setOverrideCursor(cursor, src); }

  ~CCanvasCursorLock() { CCanvas::restoreOverrideCursor(src); }

 private:
  const QString src;
};

Q_DECLARE_METATYPE(CCanvas*)

#endif  // CCANVAS_H
