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

#ifndef CDRAWCONTEXTPIXEL_H
#define CDRAWCONTEXTPIXEL_H

#include "canvas/IDrawContext.h"
#include "helpers/CGdalFile.h"

class GDALDataset;


class CDrawContextPixel : public IDrawContext, public CGdalFile
{
    Q_OBJECT
public:
    CDrawContextPixel(CCanvas *canvas, QObject *parent);
    virtual ~CDrawContextPixel();

    void setSourceFile(const QString& filename, bool resetContext) override;

    void unload() override
    {
        CGdalFile::unload();
    }

    bool getIsValid() const override
    {
        return isValid;
    }

    const QString& getProjection() const override
    {
        return proj4str;
    }

    const QTransform& getTrFwd() const override
    {
        return trFwd;
    }

    bool getNoData() const override
    {
        return hasNoData != -1;
    }

    int getRasterBandCount() const override
    {
        return rasterBandCount;
    }

    QString getInfo() const override
    {
        return CGdalFile::getInfo();
    }

    bool is32BitRgb() const override
    {
        return rasterBandCount >= 3;
    }


    QRectF getMapArea() const override
    {
        return QRectF(0, 0, xsize_px, ysize_px);
    }

    void convertMap2Coord(QPointF &pt) const override;
    void convertCoord2Map(QPointF &pt) const override;
    void convertMap2Proj(QPointF &pt) const override;
    void convertProj2Map(QPointF &pt) const override;

protected:
    void drawt(buffer_t& buf) override;
};

#endif //CDRAWCONTEXTPIXEL_H

