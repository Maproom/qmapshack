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

#ifndef IDEM_H
#define IDEM_H

#include "canvas/IDrawObject.h"
#include <QObject>
#include <QPointer>
#include <proj_api.h>

#define CUSTOM_SLOPE_COLORTABLE ( -1 )

class CDemDraw;
class IDemProp;
class QSettings;

struct SlopePresets
{
    const char  *name;
    const qreal steps[5];
};

class IDem : public IDrawObject
{
    Q_OBJECT
public:
    IDem(CDemDraw * parent);
    virtual ~IDem();

    void saveConfig(QSettings& cfg) override;

    void loadConfig(QSettings& cfg) override;

    virtual void draw(IDrawContext::buffer_t& buf) = 0;

    virtual qreal getElevationAt(const QPointF& pos) = 0;
    virtual qreal getSlopeAt(const QPointF& pos) = 0;

    bool activated()
    {
        return isActivated;
    }

    /**
       @brief Get the dem's setup widget.

       As default an instance of CDemPropSetup is used. For other setups you have
       to override this method.

       @return A pointer to the widget. Use a smart pointer to store as the widget can be destroyed at any time
     */
    virtual IDemProp * getSetup();

    bool doHillshading()
    {
        return bHillshading;
    }

    int getFactorHillshading();

    bool doSlopeColor()
    {
        return bSlopeColor;
    }

    const QVector<QRgb> getSlopeColorTable()
    {
        return slopetable;
    }

    static const struct SlopePresets slopePresets[7];
    static const size_t slopePresetCount = sizeof(IDem::slopePresets) / sizeof(IDem::slopePresets[0]);

    const qreal* getCurrentSlopeStepTable();

    int getSlopeStepTableIndex()
    {
        return gradeSlopeColor;
    }

    void setSlopeStepTable(int idx);
    void setSlopeStepTableCustomValue(int idx, int val);

public slots:
    void slotSetHillshading(bool yes)
    {
        bHillshading = yes;
    }

    void slotSetFactorHillshade(int f);

    void slotSetSlopeColor(bool yes)
    {
        bSlopeColor = yes;
    }

protected:

    void hillshading(QVector<qint16>& data, qreal w, qreal h, QImage &img);

    void slopecolor(QVector<qint16>& data, qreal w, qreal h, QImage &img);

    /**
     @brief Slope in degrees based on a window. Origin is at point (1,1), counting from zero.
     @param win2  window data
     @param size  size of window (9: 3x3 window, 16: 4x4 window)
     @param x     Fractional value (0..1) for interpolation in x (4x4 window only)
     @param y     Fractional value (0..1) for interpolation in y (4x4 window only)
     @return      Slope in degrees
     */
    qreal slopeOfWindowInterp(qint16* win2, qint32 size, qreal x, qreal y);

    /**
       @brief Reproject (translate, rotate, scale) tile before drawing it.
       @param img   the tile as QImage
       @param l     a 4 point polygon to fit the tile in
       @param p     the QPainter used to paint the tile
     */
    void drawTile(QImage& img, QPolygonF& l, QPainter& p);

    CDemDraw * dem;

    /// source projection of the current map file
    /**
        Has to be set by subclass. Destruction has to be
        handled by subclass.
     */
    projPJ pjsrc = nullptr;
    /// target projection
    /**
        Is set by IMap() to WGS84. Will be freed by ~IMap()
     */
    projPJ pjtar = nullptr;

    /// width in number of px
    quint32 xsize_px = 0;
    /// height in number of px
    quint32 ysize_px = 0;

    /// scale [px/m]
    qreal xscale = 1.0;
    /// scale [px/m]
    qreal yscale = 1.0;


    /**
       @brief True if map was loaded successfully
     */
    bool isActivated = false;

    /// the setup dialog. Use getSetup() for access
    QPointer<IDemProp> setup;

    QVector<QRgb> graytable;

    QVector<QRgb> slopetable;

    int hasNoData = 0;

    double noData = 0;

private:
    bool bHillshading = false;
    qreal factorHillshading = 0.1666666716337204;

    bool bSlopeColor = false;
    int gradeSlopeColor = 0;
    qreal slopeCustomStepTable[5];
};

#endif //IDEM_H

