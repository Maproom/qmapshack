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

#ifndef IPLOT_H
#define IPLOT_H

#include "gis/trk/CGisItemTrk.h"
#include "mouse/CMouseDummy.h"
#include "plot/CPlotData.h"
#include <QWidget>

class QMenu;
class CScrOptRangeTrk;

class IPlot : public QWidget, public INotifyTrk
{
    Q_OBJECT
public:
    enum mode_e {eModeNormal, eModeIcon, eModeWindow};
    enum mouse_click_state_e
    {
        eMouseClickIdle
        , eMouseClick1st
        , eMouseClick2nd
    };

    IPlot(CGisItemTrk * trk, CPlotData::axistype_e type, mode_e mode, QWidget * parent);
    virtual ~IPlot();

    virtual void setMouseRangeFocus(const CGisItemTrk::trkpt_t * ptRange1, const CGisItemTrk::trkpt_t * ptRange2) override;
    virtual void setMouseClickFocus(const CGisItemTrk::trkpt_t * pt) override {}

    void save(QImage& image);
    void setSolid(bool yes)
    {
        solid = yes;
    }

    bool isZoomed() const;

signals:
    void sigMouseClickState(int);

private slots:
    void slotContextMenu(const QPoint & point);
    void slotSave();
    void slotHidePoints();
    void slotShowPoints();
    void slotActivity();
    void slotCopy();
    void slotStopRange();
    void slotResetZoom();


protected:
    void setYLabel(const QString& str);
    void setXLabel(const QString& str);
    void clear();
    void newLine(const QPolygonF& line, const QString& label);
    void addLine(const QPolygonF& line, const QString& label);
    void setLimits();
    void resetZoom();

    virtual void paintEvent     (QPaintEvent  *e) override;
    virtual void resizeEvent    (QResizeEvent *e) override;
    virtual void leaveEvent     (QEvent       *e) override;
    virtual void enterEvent     (QEvent       *e) override;
    virtual void keyPressEvent  (QKeyEvent    *e) override;
    virtual void mouseMoveEvent (QMouseEvent  *e) override;
    virtual void mousePressEvent(QMouseEvent  *e) override;
    virtual void wheelEvent     (QWheelEvent  *e) override;


    void setSizes();
    void setLRTB();
    void setSizeIconArea();
    void setSizeXLabel();
    void setSizeYLabel();
    void setSizeTrackInfo();
    void setSizeDrawArea();

    QPointF getBasePoint(int ptx) const;

    void draw(QPainter& p);
    void draw();
    void drawData(QPainter& p);
    void drawLabels( QPainter &p );
    void drawXScale( QPainter &p );
    void drawYScale( QPainter &p );
    void drawGridX( QPainter &p );
    void drawGridY( QPainter &p );
    void drawXTic( QPainter &p );
    void drawYTic( QPainter &p );
    void drawLegend(QPainter& p);
    void drawDecoration( QPainter &p );
    void drawTags(QPainter& p);
    void drawActivities(QPainter& p);

    static int cnt;

    // different draw modes
    mode_e mode;
    // buffer needs update
    bool needsRedraw = true;

    bool showScale = true;
    bool thinLine  = false;
    bool solid     = false;

    QImage buffer;
    QPoint posMouse = NOPOINT;

    /**
       @brief The track this plot is attached to

       @note It is save to store the pointer to the track item because
             the plot objects registers/unregisters with the track during
             construction and destruction.

             See CGisItem::registeredPlots for details.
     */
    CGisItemTrk * trk;
    CPlotData * data;

    QFontMetrics fm;

    int left   = 0;
    int right  = 0;
    int top    = 0;
    int bottom = 0;

    int deadAreaX = 0;
    int deadAreaY = 0;

    int fontWidth    = 0;
    int fontHeight   = 0;
    int scaleWidthX1 = 0;
    int scaleWidthY1 = 0;

    QRect rectX1Label;
    QRect rectY1Label;
    QRect rectGraphArea;
    QRect rectIconArea;
    QRect rectTrackInfo;

    static const QPen pens[];
    static const QPen pensThin[];
    static const QColor colors[];

    QMenu * menu;
    QAction * actionResetZoom;
    QAction * actionPrint;
    QAction * actionStopRange;

    qint32 idxSel1 = NOIDX;
    qint32 idxSel2 = NOIDX;

    mouse_click_state_e mouseClickState = eMouseClickIdle;

    QPointer<CScrOptRangeTrk> scrOptRange;

    CMouseDummy dummyMouse;

private:
    void setMouseFocus(qreal pos, enum CGisItemTrk::focusmode_e fm);
    QPolygonF getVisiblePolygon(const QPolygonF &polyline, QPolygonF &line) const;
};

#endif //IPLOT_H

