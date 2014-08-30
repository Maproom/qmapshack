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

#ifndef CMAPWMTS_H
#define CMAPWMTS_H
#include "map/IMap.h"
#include <QMap>

class CMapDraw;

class CMapWMTS  : public IMap
{
    public:
        CMapWMTS(const QString& filename, CMapDraw *parent);
        virtual ~CMapWMTS();

        void draw(IDrawContext::buffer_t& buf);

    private:
        struct layer_t
        {
            QString     title;
            QStringList styles;
            QString     tileMatrixSet;
            QRectF      boundingBox;
            QString     resourceURL;
        };

        QList<layer_t> layers;

        struct  tilematrix_t
        {
            QPointF topLeft;
            qreal scale;
            qint32  tileWidth;
            qint32  tileHeight;
            qint32  matrixWidth;
            qint32  matrixHeight;
        };

        struct tileset_t
        {
            tileset_t() : pjsrc(0) {}
            ~tileset_t() { if(pjsrc) pj_free(pjsrc); }

            projPJ  pjsrc;
            QMap<QString,tilematrix_t> tilematrix;
        };

        QMap<QString,tileset_t> tilesets;
};

#endif //CMAPWMTS_H

