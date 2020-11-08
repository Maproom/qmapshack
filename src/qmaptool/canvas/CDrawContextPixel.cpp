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

#include "canvas/CDrawContextPixel.h"
#include "helpers/CDraw.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QtWidgets>

CDrawContextPixel::CDrawContextPixel(CCanvas *canvas, QObject * parent)
    : IDrawContext(canvas, parent)
    , CGdalFile(CGdalFile::eTypePixel)
{
    scale = QPointF(1.0, 1.0);
}

CDrawContextPixel::~CDrawContextPixel()
{
    unload();
}

void CDrawContextPixel::convertMap2Coord(QPointF &pt) const
{
    pt = trFwd.map(pt);
}

void CDrawContextPixel::convertCoord2Map(QPointF &pt) const
{
    pt = trInv.map(pt);
}

void CDrawContextPixel::convertMap2Proj(QPointF &pt) const
{
    pt = trFwdProj.map(pt);
}

void CDrawContextPixel::convertProj2Map(QPointF &pt) const
{
    pt = trInvProj.map(pt);
}

void CDrawContextPixel::setSourceFile(const QString& filename, bool resetContext)
{
    unload();

    if(resetContext)
    {
        focus = QPointF(0, 0);
        zoom(6);
    }

    if(filename.isEmpty())
    {
        return;
    }

    load(filename);

    intNeedsRedraw = true;
}

void CDrawContextPixel::drawt(buffer_t& buf)
{
    QPainter p(&buf.image);

    if(needsRedraw() || (dataset == nullptr) || (isValid == false))
    {
        CDraw::text(tr("Failed to load"), p, canvas->rect(), Qt::black);
        return;
    }

    // calculate area to read from file
    QPointF pt1 = buf.ref1;
    QPointF pt2 = buf.ref2;
    QPointF pt4 = buf.ref4;

    pt1.rx() = qMax(pt1.x(), 0.0);
    pt1.rx() = qMin(pt1.x(), xsize_px);

    pt2.rx() = qMax(pt2.x(), 0.0);
    pt2.rx() = qMin(pt2.x(), xsize_px);

    pt1.ry() = qMax(pt1.y(), 0.0);
    pt1.ry() = qMin(pt1.y(), ysize_px);

    pt4.ry() = qMax(pt4.y(), 0.0);
    pt4.ry() = qMin(pt4.y(), ysize_px);

    qint32 mapWidth  = qRound(pt2.x() - pt1.x());
    qint32 mapHeight = qRound(pt4.y() - pt1.y());
    QPointF mapOff   = pt1;

    convertMap2Screen(pt1);
    convertMap2Screen(pt2);
    convertMap2Screen(pt4);

    qint32 screenWidth  = qRound(pt2.x() - pt1.x()) & 0xFFFFFFFC;
    qint32 screenHeight = qRound(pt4.y() - pt1.y());
    QPointF screenOff   = pt1;


    // start to draw the map
    QImage img;
    QVector<quint8> buffer(screenWidth * screenHeight, 0);

    CPLErr err = CE_Failure;

    if(rasterBandCount == 1)
    {
        GDALRasterBand * pBand;
        pBand = dataset->GetRasterBand(1);

        img = QImage(screenWidth, screenHeight, QImage::Format_Indexed8);
        img.setColorTable(colortable);

        mutex.lock();
        err = pBand->RasterIO(GF_Read, mapOff.x(), mapOff.y(), mapWidth, mapHeight, img.bits(), screenWidth, screenHeight, GDT_Byte, 0, 0);
        mutex.unlock();
    }
    else
    {
        const QRgb testPix = qRgba(GCI_RedBand, GCI_GreenBand, GCI_BlueBand, GCI_AlphaBand);
        img = QImage(screenWidth, screenHeight, QImage::Format_ARGB32);
        // fill alpha channel of image buffer
        img.fill(Qt::white);

        // read map band by band and copy color values into the image buffer
        for(int b = 1; b <= rasterBandCount; ++b)
        {
            GDALRasterBand * pBand;
            pBand = dataset->GetRasterBand(b);

            mutex.lock();
            err = pBand->RasterIO(GF_Read, mapOff.x(), mapOff.y(), mapWidth, mapHeight, buffer.data(), screenWidth, screenHeight, GDT_Byte, 0, 0);
            mutex.unlock();
            if(err == CE_None)
            {
                int pbandColour = pBand->GetColorInterpretation();
                unsigned int offset;

                for (offset = 0; offset < sizeof(testPix) && *(((quint8 *)&testPix) + offset) != pbandColour; offset++)
                {
                }
                if(offset < sizeof(testPix))
                {
                    quint8 * pTar   = img.bits() + offset;
                    quint8 * pSrc   = buffer.data();
                    const int size  = buffer.size();

                    for(int i = 0; i < size; ++i)
                    {
                        *pTar = *pSrc;
                        pTar += sizeof(testPix);
                        pSrc += 1;
                    }
                }
            }
        }
    }

    p.drawImage(screenOff, img);
}
