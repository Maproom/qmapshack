/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CDRAWCONTEXTPIXEL_H
#define CDRAWCONTEXTPIXEL_H

#include "canvas/IDrawContext.h"

class GDALDataset;


class CDrawContextPixel : public IDrawContext
{
    Q_OBJECT
public:
    CDrawContextPixel(CCanvas *canvas, QObject *parent);
    virtual ~CDrawContextPixel();

    void setSourceFile(const QString& filename, bool resetContext) override;


    void unload() override;

    bool getIsValid() const
    {
        return isValid;
    }

    const QString& getProjection() const override
    {
        return projcs;
    }

    bool getNoData() const override
    {
        return hasNoData != -1;
    }

    int getRasterBandCount() const override
    {
        return rasterBandCount;
    }

    QString getInfo() const override;

    bool is32BitRgb() const override
    {
        return rasterBandCount >= 3;
    }


    QRectF getMapArea() const override
    {
        return QRectF(0,0, xsize_px, ysize_px);
    }

    void convertMap2Coord(QPointF &pt) const override;
    void convertCoord2Map(QPointF &pt) const override;

protected:
    void drawt(buffer_t& buf) override;

    GDALDataset * dataset = nullptr;

    /// number of color bands used by the *vrt
    int rasterBandCount = 0;
    /// QT representation of the vrt's color table
    QVector<QRgb> colortable;

    // true if the map file has overviews
    qint32 hasOverviews = -1;
    qint32 hasNoData    = -1;


    /// true if the map file could be loaded
    bool isValid = false;

    /// width in number of px
    qreal xsize_px = 0;
    /// height in number of px
    qreal ysize_px = 0;

    /// scale [px/m]
    qreal xscale = 0;
    /// scale [px/m]
    qreal yscale = 0;

    qreal xrot = 0;
    qreal yrot = 0;

    QPointF ref1;
    QPointF ref2;
    QPointF ref3;
    QPointF ref4;

    QTransform trFwd;
    QTransform trInv;

    QString projcs;
};

#endif //CDRAWCONTEXTPIXEL_H

