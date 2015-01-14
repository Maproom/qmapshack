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

#ifndef IDRAWCONTEXT_H
#define IDRAWCONTEXT_H


#include <QImage>
#include <QMutex>
#include <QPointF>
#include <QThread>
#include <proj_api.h>


#include "canvas/CCanvas.h"

class IDrawContext : public QThread
{
    Q_OBJECT
public:
    IDrawContext(const QString &name, CCanvas::redraw_e maskRedraw, CCanvas *parent);
    virtual ~IDrawContext();

    struct buffer_t
    {
        buffer_t() : zoomFactor(1.0,1.0), scale(1.0,1.0)
        {
        }

        /// @note: all coordinate values are long/lat WGS84 [rad]

        /// the canvas buffer
        QImage image;
        /// the used projection
        projPJ pjsrc;
        /// the zoomfactor used to draw the canvas
        QPointF zoomFactor;
        /// the scale of the global viewport
        QPointF scale;
        /// top left corner
        QPointF ref1;
        /// top right corner
        QPointF ref2;
        /// bottom right corner
        QPointF ref3;
        /// bottom left corner
        QPointF ref4;
        /// point of focus
        QPointF focus;
    };

    /**
       @brief resize the internal buffer
       @param size  the new size of the canvas
     */
    void resize(const QSize& size);
    /**
       @brief Zoom in and out of the map by the scale factors defined in CMapDB::scales.
       @param in            set true to zoom in, and false to zoom out
       @param needsRedraw   if the zoom action makes a redraw nesseccary needsRedraw is set true
     */
    void zoom(bool in, CCanvas::redraw_e &needsRedraw);
    void zoom(int idx);
    void zoom(const QRectF& rect);
    int  zoom()
    {
        return zoomIndex;
    }

    /**
       @brief Convert a geo coordinate of format lon/lat WGS84 into the currently used coordinate/projection/datum system.
       @note  The unit is dependent on the currently used projection and must not nesseccarily be meter
       @param p             the point to convert
     */
    void convertRad2M(QPointF &p);
    /**
       @brief Convert a geo coordinate of the currently used projection/datum to lon/lat WGS84
       @note  The unit is dependent on the currently used projection and must not nesseccarily be meter
       @param p             the point to convert
     */
    void convertM2Rad(QPointF &p);
    /**
       @brief Convert a pixel coordinate from the viewport to a geo coordinate in [rad]
       @param p             the point to convert
     */
    void convertPx2Rad(QPointF& p);
    /**
       @brief Convert a geo coordinate in [rad] to a pixel coordinate of the viewport
       @param p             the point to convert
     */
    void convertRad2Px(QPointF& p);
    void convertRad2Px(QPolygonF& poly);

    /**
       @brief Check if the internal needs redraw flag is set
       @return intNeedsRedraw is returned
     */
    bool needsRedraw();

    /**
        @brief Draw the active map buffer to the painter
        @param p            the painte used to draw the map
        @param needsRedraw  set true to trigger a redraw in the background thread
        @param f            the point of focus in [°] that is drawn in the middle of the viewport.
     */
    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QPointF &f);

    /**
       @brief Get the projection string of this map object
       @return A proj4 string.
     */
    QString getProjection();

    /**
       @brief Set the projection of the draw context

       This will just create a new source projection object (pjsrc). Most likely you want to
       overide this method to:

       1) save what ever has to be saved
       2) call this method
       3) restore everything with the new projection

       @param proj a Proj4 projection string
     */
    virtual void setProjection(const QString& proj);

signals:
    void sigCanvasUpdate(CCanvas::redraw_e flags);
    void sigStartThread();
    void sigStopThread();
    void sigScaleChanged(const QPointF& scale);

public slots:
    void emitSigCanvasUpdate();

protected:
    void run();
    /**
       @brief The draw method called from the thread.

       That's where the actual drawing has to be done

       @param currentBuffer this is the current buffer reserved for the thread to draw on.
     */
    virtual void drawt(buffer_t& currentBuffer) = 0;

    /**
       @brief The global list of available scale factors
     */
    static const qreal scales[];

    /// the mutex to serialize access
    QMutex mutex;

    /// internal needs redraw flag
    bool intNeedsRedraw;

    /// the canvas this map object is attached to
    CCanvas * canvas;

    const CCanvas::redraw_e maskRedraw;
    /// map canvas twin buffer
    buffer_t buffer[2];
    /// the main threads currently used map canvas buffer
    bool bufIndex;
    /// buffer width [px]
    int bufWidth;
    /// buffer height [px]
    int bufHeight;
    /// the viewports width [px]
    int viewWidth;
    /// the viewports height [px]
    int viewHeight;
    /// the center of the viewport
    QPointF center;

    /// source projection should be the same for all maps
    projPJ pjsrc;
    /// target projection is always WGS84
    projPJ pjtar;

    /// the basic scale of the map canvas
    QPointF scale;
    /// index into scales table
    int zoomIndex;
    /// the actual used scaleFactor
    QPointF zoomFactor;

    /// the next point of focus that will be displayed right in the middle of the viewport
    QPointF focus;
    /// top left corner of next buffer
    QPointF ref1;
    /// top right corner of next buffer
    QPointF ref2;
    /// bottom right corner of next buffer
    QPointF ref3;
    /// bottom left corner of next buffer
    QPointF ref4;
};

extern QPointF operator*(const QPointF& p1, const QPointF& p2);

extern QPointF operator/(const QPointF& p1, const QPointF& p2);


#endif //IDRAWCONTEXT_H

