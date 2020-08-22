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

#include <proj_api.h>
#include <QMap>
#include <QPainter>
#include <QPointer>
#include <QWidget>

#include "gis/IGisItem.h"

class IDrawContext;
class CMapDraw;
class CGrid;
class CDemDraw;
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
struct poi_t;
class CTableTrkInfo;

class CCanvas : public QWidget
{
    Q_OBJECT
public:
    CCanvas(QWidget * parent, const QString& name);
    virtual ~CCanvas();

    static void setOverrideCursor(const QCursor &cursor, const QString&src);
    static void restoreOverrideCursor(const QString &src);
    static void changeOverrideCursor(const QCursor& cursor, const QString &src);


    void saveConfig(QSettings& cfg);
    void loadConfig(QSettings& cfg);

    void setupGrid();
    void convertGridPos2Str(const QPointF& pos, QString& str, bool simple);
    void convertRad2Px(QPointF &pos) const;
    void convertPx2Rad(QPointF& pos) const;

    void setupBackgroundColor();

    void setup();
    QString getProjection();
    void  setProjection(const QString& proj);

    enum scales_type_e
    {
        eScalesDefault
        , eScalesSquare
    };

    void  setScales(const scales_type_e type);
    scales_type_e getScalesType();

    qreal getElevationAt(const QPointF &pos) const;
    void  getElevationAt(const QPolygonF& pos, QPolygonF &ele) const;
    void  getElevationAt(SGisLine &line) const;

    qreal getSlopeAt(const QPointF &pos) const;
    void getSlopeAt(const QPolygonF& pos, QPolygonF& slope) const;

    void moveMap(const QPointF &delta);
    void zoomTo(const QRectF& rect);
    void displayInfo(const QPoint& px);
    poi_t findPOICloseBy(const QPoint& px) const;

    enum redraw_e
    {
        eRedrawNone = 0
        , eRedrawMap = 0x01
        , eRedrawDem = 0x02
        , eRedrawGis = 0x04
        , eRedrawMouse = 0x08
        , eRedrawRt = 0x10
        , eRedrawAll = 0xFFFFFFFF
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

    void showProfileAsWindow(bool yes);
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

    void print(QPainter &p, const QRectF& area, const QPointF &focus, bool printScale = true);

    /**
       @brief Set a single map file to be shown on the canvas

       @param filename   the map's file path, can be a resource, too
     */
    void setMap(const QString& filename);

    void followPosition(const QPointF& pos);

    /// Allows showing the track overlays if they are set in CMainWindow
    void allowShowTrackOverlays(bool show)
    {
        showTrackOverlays = show;
    }

    /// save the size of the track profile if it is in window mode
    void saveSizeTrackProfile();

    static qreal gisLayerOpacity;

signals:
    void sigMousePosition(const QPointF& pos, qreal ele, qreal slope);
    void sigZoom();
    void sigMove();
    void sigResize(const QSize& size);

public slots:
    void slotTriggerCompleteUpdate(CCanvas::redraw_e flags);
    void slotUpdateTrackInfo(bool updateVisuals);
    void slotCheckTrackOnFocus();

protected:
    bool event(QEvent *) override;
    bool gestureEvent(QGestureEvent *e);
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent  *e) override;
    void mousePressEvent(QMouseEvent  *e) override;
    void mouseMoveEvent(QMouseEvent  *e) override;
    void mouseReleaseEvent(QMouseEvent  *e) override;
    void mouseDoubleClickEvent(QMouseEvent  *e) override;
    void wheelEvent(QWheelEvent  *e) override;
    void enterEvent(QEvent       *e) override;
    void leaveEvent(QEvent       *e) override;
    void keyPressEvent(QKeyEvent    *e) override;


private slots:
    void slotToolTip();

private:
    void drawStatusMessages(QPainter& p);
    void drawTrackStatistic(QPainter& p);
    void drawScale(QPainter& p, QRectF drawRect);
    void drawScale(QPainter& p)//Default use, drawRect is introduced for correct printing
    {
        drawScale(p, rect());
    }
    void setZoom(bool in, redraw_e & needsRedraw);
    void setSizeTrackProfile();
    /**
       @brief Resize all registered drwa context objects

       @param s     the new size

       @return Return false if one of the draw conext could not be resized
               because it's thread is running and blocking access to the data
     */
    bool setDrawContextSize(const QSize& s);

    bool isShowMinMaxSummary() const;
    bool isShowTrackSummary() const;
    bool isShowTrackInfoTable() const;
    bool isShowTrackInfoPoints() const;
    bool isShowTrackProfile() const;
    bool isShowTrackHighlight() const;

    bool showTrackOverlays = true;

    QColor backColor = "#FFFFBF";       //< the background color used in case of missing map tiles
    redraw_e needsRedraw = eRedrawAll;  //< set true to initiate a complete redraw of the screen content
    CMapDraw * map;                     //< the map object attached to this canvas
    CDemDraw * dem;                     //< the elevation data layer attached to this canvas
    CGisDraw * gis;                     //< the GIS data layer attached to this canvas
    CRtDraw * rt;                       //< the real time data layer attached to this canvas
    CGrid * grid;                       //< the grid attached to this canvas

    QList<IDrawContext*> allDrawContext;

    /// the current point of focus (usually the canvas center)
    QPointF posFocus {12.00 * DEG_TO_RAD, 49.00 * DEG_TO_RAD};

    /// the mouse handler
    CMouseAdapter * mouse;

    /// tool tip timer for vector map tool tips
    QTimer * timerToolTip;
    /// the position of the tool tip
    QPoint posToolTip;

    /// load indicator for maps
    QMovie * loadIndicator1;
    QLabel * mapLoadIndicator;

    /// load indicator for DEM
    QMovie * loadIndicator2;
    QLabel * demLoadIndicator;

    QPointer<CColorLegend> colorLegend;

    /// current accumulated angleDelta, used/required for zooming on trackpads
    int zoomAngleDelta = 0;

    /// timer to poll for track gaining/loosing focus
    QTimer * timerTrackOnFocus;
    /// the key of the currently focused track
    IGisItem::key_t keyTrackOnFocus;
    /// the track profile plot
    QPointer<IPlot>  plotTrackProfile;
    /// a label with a track
    QLabel * labelTrackStatistic;

    QLabel * labelTrackInfo;

    QTextBrowser * textStatusMessages;
    QMap<QString, QString> statusMessages;

    QMutex mousePressMutex;
    bool mouseLost = false;

    QTextBrowser * labelHelp = nullptr;
};

class CCanvasCursorLock
{
public:
    CCanvasCursorLock(const QCursor &cursor, const QString& src)
        : src(src)
    {
        CCanvas::setOverrideCursor(cursor, src);
    }

    ~CCanvasCursorLock()
    {
        CCanvas::restoreOverrideCursor(src);
    }

private:
    const QString src;
};


Q_DECLARE_METATYPE(CCanvas*)

#endif //CCANVAS_H

