/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CMAPVRT_H
#define CMAPVRT_H

#include "map/IMap.h"


class CMapDraw;
class GDALDataset;

class CMapVRT : public IMap
{
    Q_OBJECT
public:
    CMapVRT(const QString& filename, CMapDraw *parent);
    virtual ~CMapVRT();

    void draw(IDrawContext::buffer_t& buf) override;

private:
    /**
       @brief Test subfiles of VRT for overviews
       @param filename The VRT filename to inspect
       @return Return true if all subfiles have overviews.
     */
    bool testForOverviews(const QString& filename);
    QString filename;
    /// instance of GDAL dataset
    GDALDataset * dataset;
    /// number of color bands used by the *vrt
    int rasterBandCount = 0;
    /// QT representation of the vrt's color table
    QVector<QRgb> colortable;

    /// width in number of px
    qint32 xsize_px = 0;
    /// height in number of px
    qint32 ysize_px = 0;

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

    bool hasOverviews = false;
};

#endif //CMAPVRT_H

