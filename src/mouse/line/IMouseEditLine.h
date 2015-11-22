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

#ifndef IMOUSEEDITLINE_H
#define IMOUSEEDITLINE_H

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"
#include "mouse/IMouse.h"
#include <QDebug>
#include <QPointer>
#include <QPolygonF>

class CGisDraw;
class CCanvas;
class IGisLine;
class CScrOptEditLine;
class ILineOp;

class IMouseEditLine : public IMouse
{
    Q_OBJECT
public:
    enum features_e
    {
        eFeatureSnapToLines    = 0x01
        ,eFeatureRouting        = 0x02
    };

    /**
       @brief Start to create a new track with given point as first track point
       @param point     the starting point
       @param gis       the draw context to use
       @param parent    the canvas to use
     */
    IMouseEditLine(const IGisItem::key_t& key, const QPointF& point, bool enableStatus, const QString& type, CGisDraw * gis, CCanvas * parent);
    /**
       @brief Edit an existing track
       @param trk       the track to edit
       @param gis       the draw context to use
       @param parent    the canvas to use
     */
    IMouseEditLine(const IGisItem::key_t &key, IGisLine &src, bool enableStatus, const QString& type, CGisDraw * gis, CCanvas * parent);
    virtual ~IMouseEditLine();

    void draw(QPainter& p,  CCanvas::redraw_e needsRedraw, const QRect &rect);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent * e);
    void keyPressEvent(QKeyEvent * e);

    void abortStep();

    bool useAutoRouting();
    bool useVectorRouting();

    void setCanvasPanning(bool enable)
    {
        doCanvasPanning = enable;
    }

    void storeToHistory(const SGisLine& line);
    void restoreFromHistory(SGisLine& line);

    virtual void updateStatus();

protected slots:
    /**
       @brief Delete the selected point
     */
    void slotDeletePoint();
    /**
       @brief Start to select a range of points
     */
    void slotSelectRange();
    /**
       @brief Move selected point
     */
    void slotMovePoint();
    /**
       @brief Add points in direction start of track (eStateAddPointBwd)
     */
    void slotAddPoint();

    void slotNoRouting();
    void slotAutoRouting();
    void slotVectorRouting();


    virtual void slotAbort() = 0;
    void slotAbortEx(bool showMB);
    virtual void slotCopyToOrig();
    virtual void slotCopyToNew() = 0;

    void slotUndo();
    void slotRedo();

    void slotPanCanvas();

protected:
    virtual void drawLine(const QPolygonF& l, const QColor color, int width, QPainter& p);
    /**
       @brief Get access to the IGisLine object a subclass of IMouseEditLine is handling.
       @return A valid pointer or 0.
     */
    virtual IGisLine * getGisLine() = 0;

    virtual void startNewLine(const QPointF &point);

    /// shadow cursor needed to restore cursor after some actions providing their own cursor.
    QCursor cursor1;

    /// the abstract line object to edit
    SGisLine points;

    /// undo/redo history
    QList<SGisLine> history;
    qint32 idxHistory = NOIDX;


    /// the on screen buttons
    CScrOptEditLine * scrOptEditLine;

    /// the key of the GIS item to edit
    IGisItem::key_t key;

private:
    void commonSetup();
    void changeCursor();

    /// flag to enable/disable canvas/map panning
    bool doCanvasPanning = false;

    QPolygonF pixelLine;
    QPolygonF pixelPts;
    QPolygonF pixelSubs;

    /// the current active line operation (move, add, delete...)
    ILineOp  * lineOp = 0;

    bool enableStatus;

    QString type;
};

#endif //IMOUSEEDITLINE_H


