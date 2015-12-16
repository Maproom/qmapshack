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

#ifndef CMAPRMAP_H
#define CMAPRMAP_H

#include "IMap.h"

class CMapDraw;

class CMapRMAP : public IMap
{
    Q_OBJECT
public:
    CMapRMAP(const QString& filename, CMapDraw *parent);

    virtual void draw(IDrawContext::buffer_t& buf) override;

private:
    struct level_t
    {
        level_t() : offsetLevel(0), width(0), height(0), xTiles(0), yTiles(0), xscale(0), yscale(0)
        {
        }
        quint64 offsetLevel;
        qint32 width;
        qint32 height;
        qint32 xTiles;
        qint32 yTiles;
        QVector<quint64> offsetJpegs;

        quint64 getOffsetJpeg(quint32 x, quint32 y)
        {
            qint32 idx = y * xTiles + x;
            return idx < offsetJpegs.size() ? offsetJpegs[idx] : 0;
        }

        qreal xscale;
        qreal yscale;
    };

    bool setProjection(const QString& projection, const QString& datum);
    level_t& findBestLevel(const QPointF &s);

    QString filename;

    /// total width in number of px
    qint32 xsize_px = 0;
    /// total height in number of px
    qint32 ysize_px = 0;
    /// width of a tile in number of px
    quint32 tileSizeX = 0;
    /// height of a tile in number of px
    quint32 tileSizeY = 0;
    /// all pre-scaled levels
    QList<level_t> levels;
    /// reference point [m] or [°] (left hand side of map)
    qreal xref1 = 0;
    /// reference point [m] or [°] (top of map)
    qreal yref1 = 0;
    /// reference point [m] or [°] (right hand side of map)
    qreal xref2 = 0;
    /// reference point [m] or [°] (bottom of map)
    qreal yref2 = 0;

    QPointF scale;
};

#endif // CMAPRMAP_H
