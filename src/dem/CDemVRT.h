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

#ifndef CDEMVRT_H
#define CDEMVRT_H

#include "dem/IDem.h"

class CDemDraw;
class GDALDataset;

class CDemVRT : public IDem
{
    public:
        CDemVRT(const QString& filename, CDemDraw *parent);
        virtual ~CDemVRT();

        void draw(IDrawContext::buffer_t& buf);

        qreal getElevation(const QPointF& pos);

    private:
        QString filename;
        /// instance of GDAL dataset
        GDALDataset * dataset;

        /// width in number of px
        quint32 xsize_px;
        /// height in number of px
        quint32 ysize_px;

        /// scale [px/m]
        double xscale;
        /// scale [px/m]
        double yscale;

        double xrot;
        double yrot;

        QPointF ref1;
        QPointF ref2;
        QPointF ref3;
        QPointF ref4;

        QTransform trFwd;
        QTransform trInv;

        bool hasOverviews;

        QRectF boundingBox;

        QVector<QRgb> graytable;




};

#endif //CDEMVRT_H

