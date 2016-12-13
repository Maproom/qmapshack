/**********************************************************************************************
    Copyright (C) 2016 Peter Schumann peter.schumann@jeepxj.de

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

#ifndef CMAPGEMF_H
#define CMAPGEMF_H

#include "IMap.h"

class CMapGEMF : public IMap
{
    Q_OBJECT
public:
    CMapGEMF(const QString& filename, CMapDraw *parent);
    void draw(IDrawContext::buffer_t& buf) override;

private:
    const quint32 MAX_ZOOM_LEVEL = 21;
    const quint32 MIN_ZOOM_LEVEL = 0;

    QImage getTile(const quint32 col, const quint32 row,const quint32 z);
    quint64 getFilenameFromAddress(const quint64 offset, QString &filename );

    struct source_t
    {
        quint32 index;
        QString name;
    };

    struct gemffile_t
    {
        QString filename;
        quint64 size;
    };
    struct range_t
    {
        quint32 zoomlevel;
        quint32 minX;
        quint32 maxX;
        quint32 minY;
        quint32 maxY;
        quint32 sourceIdx;
        quint64 offset;
    };

    QString filename;
    quint32 version;
    quint32 tileSize;
    quint32 sourceNr;
    quint32 rangeNum;
    quint32 minZoom;
    quint32 maxZoom;
    QList< source_t> sources;
    QList<gemffile_t> files;
    QHash<quint32, QList<range_t> > rangesByZoom;
};

#endif // CMAPGEMF_H
