/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"

class CMapDraw;
class CGrid;
class CDemDraw;
class CGisDraw;
class CGisItemWpt;
class CGisItemTrk;
class CGisItemRte;
class CGisItemOvlArea;
class QSettings;
class QPointF;
class IMouse;
class QTimer;
class QMovie;
class QLabel;
class IPlot;

inline void USE_ANTI_ALIASING(QPainter& p, bool useAntiAliasing)
{
    p.setRenderHints(QPainter::TextAntialiasing|QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing, useAntiAliasing);
}

#define PROGRESS_SETUP(lbl, max, parent) \
    QProgressDialog progress(lbl, "Abort", 0, max, parent); \
    progress.setWindowModality(Qt::WindowModal); \

#define PROGRESS(x, cmd) \
    progress.setValue(x); \
    if (progress.wasCanceled()) { cmd; } \

#define PAINT_ROUNDED_RECT(p,r) p.drawRoundedRect(r,5,5)


class CCanvas : public QWidget
{
    Q_OBJECT
public:
    CCanvas(QWidget * parent, const QString& name);
    virtual ~CCanvas();

    void saveConfig(QSettings& cfg);
    void loadConfig(QSettings& cfg);

    void setupGrid();
    void convertGridPos2Str(const QPointF& pos, QString& str);

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

    qreal getElevationAt(const QPointF &pos);
    void  getElevationAt(const QPolygonF& pos, QPolygonF &ele);
    void  getElevationAt(SGisLine line);

    void moveMap(const QPointF &delta);
    void zoomTo(const QRectF& rect);
    void displayInfo(const QPoint& px);

    static void drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color);
    static void drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color, const QFont& font);
    static void drawText(const QString& str, QPainter& p, const QRect& r, const QColor& color);

    enum redraw_e
    {
        eRedrawNone = 0
        , eRedrawMap = 0x01
        , eRedrawDem = 0x02
        , eRedrawGis = 0x04
        , eRedrawMouse = 0x08
        , eRedrawAll = 0xFFFFFFFF
    };

    static QPen penBorderBlue;
    static QPen penBorderGray;
    static QPen penBorderBlack;
    static QBrush brushBackWhite;
    static QBrush brushBackYellow;

    void resetMouse();
    void setMouseMoveWpt(CGisItemWpt& wpt);
    void setMouseEditTrk(CGisItemTrk& trk);
    void setMouseRangeTrk(CGisItemTrk& trk);
    void setMouseEditTrk(const QPointF& pt);
    void setMouseEditRte(const QPointF& pt);
    void setMouseEditRte(CGisItemRte& rte);
    void setMouseEditArea(CGisItemOvlArea& area);
    void setMouseEditArea(const QPointF& pt);
    void setMouseWptBubble(const IGisItem::key_t& key);

    void showProfileAsWindow(bool yes);
    void showProfile(bool yes);


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
       the threshold. The returned poylline uses lon/lat as coordinates.

       @param pt1           first point in [rad]
       @param pt2           second point in [rad]
       @param threshold     the "close enough" threshold in [pixel]
       @param polyline      the resulting polyline, if any, in [rad]
       @return              Return true if a line has been found.
     */
    bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline);

signals:
    void sigMousePosition(const QPointF& pos, qreal ele);

public slots:
    void slotTriggerCompleteUpdate(CCanvas::redraw_e flags);

protected:
    void resizeEvent(QResizeEvent * e);
    void paintEvent(QPaintEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent * e);
    void wheelEvent(QWheelEvent * e);
    void enterEvent(QEvent * e);
    void leaveEvent(QEvent * e);
    void keyPressEvent(QKeyEvent * e);


private slots:
    void slotToolTip();
    void slotCheckTrackOnFocus();

private:
    void drawStatusMessages(QPainter& p);
    void drawScale(QPainter& p);
    void setZoom(bool in, redraw_e &needsRedraw);
    void setSizeTrackProfile();
    void saveSizeTrackProfile();

    /// set true to initiate a complete redraw of the screen content
    redraw_e needsRedraw;
    /// the map object attached to this canvas
    CMapDraw * map;
    /// the elevation data layer attached to this canvas
    CDemDraw * dem;
    /// the GIS data layer attached to this convas
    CGisDraw * gis;
    /// the grid attached to this canvas
    CGrid * grid;
    /// the current point of focus (usually the canvas center)
    QPointF posFocus;

    /// the current mouse handler
    IMouse * mouse;

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

    /// timer to poll for track gaining/loosing focus
    QTimer * timerTrackOnFocus;
    /// the key of the currentl focused track
    IGisItem::key_t keyTrackOnFocus;
    /// the track profile plot
    QPointer<IPlot>  plotTrackProfile;

    QLabel * labelStatusMessages;
    QMap<QString, QString> statusMessages;
};

#endif //CCANVAS_H

