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

#include "canvas/CDrawContextPixel.h"
#include "helpers/CDraw.h"

#include <QtWidgets>
#include <gdal_priv.h>
#include <ogr_spatialref.h>

CDrawContextPixel::CDrawContextPixel(CCanvas *canvas, QObject * parent)
    : IDrawContext(canvas, parent)
{
}

CDrawContextPixel::~CDrawContextPixel()
{
    if(dataset)
    {
        GDALClose(dataset);
    }
}

void CDrawContextPixel::convertMap2Coord(QPointF &pt) const
{
    pt = trFwd.map(pt);
}

void CDrawContextPixel::convertCoord2Map(QPointF &pt) const
{
    pt = trInv.map(pt);
}

void CDrawContextPixel::unload()
{
    isValid = false;
    if(dataset != nullptr)
    {
        GDALClose(dataset);
    }
    dataset = nullptr;
}

void CDrawContextPixel::setSourceFile(const QString& filename, bool resetContext)
{
    unload();

    if(resetContext)
    {
        focus = QPointF(0,0);
        zoom(6);
    }

    if(filename.isEmpty())
    {
        return;
    }

    dataset = (GDALDataset*)GDALOpenShared(filename.toUtf8(),GA_ReadOnly);

    if(nullptr == dataset)
    {
        QMessageBox::warning(canvas, tr("Error..."), tr("Failed to load file: %1").arg(filename));
        return;
    }

    projcs = dataset->GetProjectionRef();
//    qDebug() << projcs;

    GDALRasterBand *pBand = dataset->GetRasterBand(1);

    if(nullptr == pBand)
    {
        GDALClose(dataset);
        dataset = nullptr;
        QMessageBox::warning(canvas, tr("Error..."), tr("Failed to load file: %1").arg(filename));
        return;
    }
    hasOverviews = pBand->GetOverviewCount();
    qDebug() << "hasOverviews" << hasOverviews;

    // ------- setup color table ---------
    rasterBandCount = dataset->GetRasterCount();
    if(rasterBandCount == 1)
    {
        if(pBand->GetColorInterpretation() ==  GCI_PaletteIndex )
        {
            GDALColorTable * pct = pBand->GetColorTable();
            for(int i=0; i < pct->GetColorEntryCount(); ++i)
            {
                const GDALColorEntry& e = *pct->GetColorEntry(i);
                colortable << qRgba(e.c1, e.c2, e.c3, e.c4);
            }
        }
        else if(pBand->GetColorInterpretation() ==  GCI_GrayIndex )
        {
            for(int i=0; i < 256; ++i)
            {
                colortable << qRgba(i, i, i, 255);
            }
        }
        else
        {
            GDALClose(dataset);
            dataset = nullptr;
            QMessageBox::warning(canvas, tr("Error..."), tr("File must be 8 bit palette or gray indexed."));
            return;
        }

        int success = 0;
        qreal idx = pBand->GetNoDataValue(&success);
        if(success)
        {
            if((idx > 0) && (idx < colortable.size()))
            {
                QColor tmp(colortable[idx]);
                tmp.setAlpha(0);
                colortable[idx] = tmp.rgba();
                hasNoData = idx;
            }
            else
            {
                qDebug() << "Index for no data value is out of bound";
                return;
            }
        }
    }
    qDebug() << "hasNoData" << hasNoData;

    xsize_px = dataset->GetRasterXSize();
    ysize_px = dataset->GetRasterYSize();

    qreal adfGeoTransform[6];
    dataset->GetGeoTransform( adfGeoTransform );

    xscale  = adfGeoTransform[1];
    yscale  = adfGeoTransform[5];
    xrot    = adfGeoTransform[4];
    yrot    = adfGeoTransform[2];

    trFwd = QTransform();
    trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
    trFwd.scale(adfGeoTransform[1], adfGeoTransform[5]);

    if(adfGeoTransform[4] != 0.0)
    {
        trFwd.rotate(qAtan(adfGeoTransform[2]/adfGeoTransform[4]));
    }

    trInv = trFwd.inverted();

    QPointF pt1(0,0);
    pt1 = trFwd.map(pt1);
    pt1 = trInv.map(pt1);

    intNeedsRedraw = true;
    isValid = true;
}

QString CDrawContextPixel::getInfo() const
{
    QString str;
    QTextStream out(&str);

    if(projcs.isEmpty())
    {
        out << "no projection" << endl;
    }
    else
    {
        char str[1025] = {0};
        strncpy(str, projcs.toLatin1().data(), sizeof(str) - 1);
        OGRSpatialReference oSRS;
        char *wkt = str;
        oSRS.importFromWkt(&wkt);

        char *proj4 = nullptr;
        oSRS.exportToProj4(&proj4);
        out << proj4 << endl;
        free(proj4);
    }

    out << "num. bands:\t" << rasterBandCount << " ";
    switch(rasterBandCount)
    {
    case 1:
        out << tr("(color table)");
        break;

    case 3:
        out << tr("(RGB)");
        break;

    case 4:
        out << tr("(RGBA)");
        break;

    default:
        out << tr("(unknown)");
    }

    out << endl;

    out << "has overviews:\t" << hasOverviews << endl;

    if((rasterBandCount != 4) && (hasNoData != -1))
    {
        out << "has no data:\t" << hasNoData << endl;
    }


    return str;
}

void CDrawContextPixel::drawt(buffer_t& buf)
{
    QPainter p(&buf.image);

    if(needsRedraw() || (dataset == nullptr) || (isValid == false))
    {
        CDraw::text(tr("Failed to load'"), p, canvas->rect(), Qt::black);
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

        img = QImage(screenWidth,screenHeight,QImage::Format_Indexed8);
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
