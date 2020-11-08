/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CCanvas.h"
#include "units/IUnit.h"

#include <QMutex>
#include <QThread>
class QPainter;
class QSettings;

class IDrawContext : public QThread
{
    Q_OBJECT
public:
    IDrawContext(CCanvas *canvas, QObject *parent);
    virtual ~IDrawContext() = default;

    virtual QString getInfo() const = 0;

    virtual bool is32BitRgb() const = 0;

    virtual int getRasterBandCount() const = 0;

    virtual bool getNoData() const = 0;

    virtual const QString& getProjection() const = 0;

    virtual const QTransform& getTrFwd() const = 0;

    virtual QRectF getMapArea() const = 0;

    virtual void setSourceFile(const QString& filename, bool resetContext) = 0;

    virtual void unload() = 0;

    virtual bool getIsValid() const = 0;

    virtual void saveSettings(QSettings& cfg) const;

    virtual void loadSettings(QSettings& cfg);

    void move(const QPointF& delta);

    void zoom(bool in, const QPointF& pt);

    bool needsRedraw() const;

    void convertScreen2Map(QPointF &pt) const;
    void convertMap2Screen(QPointF& pt) const;
    void convertMap2Screen(QPolygonF& line) const;
    void convertMap2Screen(QRectF &rect) const;

    /**
       @brief Convert point in map to coordinates

        Depending on the type and the reference information
        of the map the result can be:

        * CGdalFile::eTypePixel: Pixel coordinates
        * CGdalFile::eTypeProj: Geo coordinates, if the file
          is referenced else pixel coordinates
     */
    virtual void convertMap2Coord(QPointF &pt) const = 0;
    virtual void convertCoord2Map(QPointF &pt) const = 0;

    /** Convert point in map to coordinates

      Depending on the reference information of the map
      the result can be:

      * Referenced map: Geo coordinates
      * Un-referenced map: Pixel coordinates

    */
    virtual void convertMap2Proj(QPointF &pt) const
    {
        convertMap2Coord(pt);
    }

    virtual void convertProj2Map(QPointF &pt) const
    {
        convertCoord2Map(pt);
    }

    /**
       @brief draw
       @param p
       @param needsRedraw
     */
    void draw(QPainter& p, CCanvas::redraw_e needsRedraw);

    void triggerCompleteUpdate(CCanvas::redraw_e flags) const
    {
        canvas->slotTriggerCompleteUpdate(flags);
    }

    const CCanvas * getCanvas() const
    {
        return canvas;
    }

signals:
    void sigDraw(QPainter& p);

protected slots:
    void slotResize(const QSize& size);

protected:
    CCanvas *canvas;

    struct buffer_t
    {
        QImage image;

        QPointF zoomFactor {1.0, 1.0}; //< the zoomfactor used to draw the canvas
        QPointF scale {1.0, 1.0}; //< the scale of the global viewport

        QPointF ref1;  //< top left corner
        QPointF ref2;  //< top right corner
        QPointF ref3;  //< bottom right corner
        QPointF ref4;  //< bottom left corner
        QPointF focus; //< point of focus
    };

    void run() override;

    virtual void drawt(buffer_t& currentBuffer) = 0;

    void zoom(int idx);

    static QMutex mutex;

    /// internal needs redraw flag
    bool intNeedsRedraw = true;

    const CCanvas::redraw_e maskRedraw = CCanvas::eRedrawMap;

    /// map canvas twin buffer
    buffer_t buffer[2];
    /// the main threads currently used map canvas buffer
    bool bufIndex = false;

    qreal bufWidth   = 100; //< buffer width [px]
    qreal bufHeight  = 100; //< buffer height [px]
    qreal viewWidth  = 100; //< the viewports width [px]
    qreal viewHeight = 100; //< the viewports height [px]

    QPointF focus {0, 0};

    /// the basic scale of the map canvas
    QPointF scale = QPoint(1.0, 1.0);

private:
    void init();

    static const qreal scales[];
    qint32 zoomFactorIdx = 6;

    /// the actual used scaleFactor
    QPointF zoomFactor {scales[zoomFactorIdx], scales[zoomFactorIdx]};

    QPointF ref1; //< top left corner of next buffer
    QPointF ref2; //< top right corner of next buffer
    QPointF ref3; //< bottom right corner of next buffer
    QPointF ref4; //< bottom left corner of next buffer
};

extern QPointF operator*(const QPointF& p1, const QPointF& p2);

extern QPointF operator/(const QPointF& p1, const QPointF& p2);


#endif //IDRAWCONTEXT_H

