/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CDRAWCONTEXTPROJ_H
#define CDRAWCONTEXTPROJ_H

#include "canvas/IDrawContext.h"
#include "gis/proj_x.h"
#include "helpers/CGdalFile.h"

class CDrawContextProj : public IDrawContext, public CGdalFile
{
    Q_OBJECT
public:
    CDrawContextProj(CCanvas* canvas, QObject* parent);
    virtual ~CDrawContextProj() = default;

    void setSourceFile(const QString& filename, bool resetContext) override;

    void unload() override
    {
        CGdalFile::unload();
    }

    bool getIsValid() const override
    {
        return isValid;
    }

    QString getProjection() const override
    {
        return proj.getProjSrc();
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

    void convertMap2Coord(QPointF& pt) const override;
    void convertCoord2Map(QPointF& pt) const override;

    void drawt(buffer_t& buf) override;
};

#endif //CDRAWCONTEXTPROJ_H

