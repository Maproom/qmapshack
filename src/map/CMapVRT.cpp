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

#include "map/CMapVRT.h"
#include "map/CMapDraw.h"
#include "units/IUnit.h"
#include "canvas/CCanvas.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QtWidgets>

#define TILELIMIT 5000

CMapVRT::CMapVRT(const QString &filename, CMapDraw *parent)
    : IMap(eFeatVisibility,parent)
    , filename(filename)
    , rasterBandCount(0)
    , hasOverviews(false)
{
    qDebug() << "------------------------------";
    qDebug() << "VRT: try to open" << filename;

    dataset = (GDALDataset*)GDALOpen(filename.toUtf8(),GA_ReadOnly);

    if(dataset == 0)
    {
        QMessageBox::warning(0, tr("Error..."), tr("Failed to load file: %1").arg(filename));
        return;
    }

    // ------- setup color table ---------
    rasterBandCount = dataset->GetRasterCount();
    if(rasterBandCount == 1)
    {
        GDALRasterBand * pBand;
        pBand = dataset->GetRasterBand(1);

        if(pBand == 0)
        {
            delete dataset; dataset = 0;
            QMessageBox::warning(0, tr("Error..."), tr("Failed to load file: %1").arg(filename));
            return;
        }


        hasOverviews = pBand->GetOverviewCount() != 0;

        //        qDebug() << pBand->GetColorInterpretation();

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
            delete dataset; dataset = 0;
            QMessageBox::warning(0, tr("Error..."), tr("File must be 8 bit palette or gray indexed."));
            return;
        }

        int success = 0;
        double idx = pBand->GetNoDataValue(&success);

        if(success)
        {
            QColor tmp(colortable[idx]);
            tmp.setAlpha(0);
            colortable[idx] = tmp.rgba();
        }
    }
    else
    {
        delete dataset; dataset = 0;
        QMessageBox::warning(0, tr("Error..."), tr("File must be 8 bit palette or gray indexed."));
        return;
    }

    qDebug() << "has overviews" << hasOverviews;


    // ------- setup projection ---------------
    char str[1024] = {0};
    if(dataset->GetProjectionRef())
    {
        strncpy(str,dataset->GetProjectionRef(),sizeof(str));
    }
    char * ptr = str;
    OGRSpatialReference oSRS;
    oSRS.importFromWkt(&ptr);
    oSRS.exportToProj4(&ptr);

    qDebug() << ptr;

    pjsrc = pj_init_plus(ptr);
    if(pjsrc == 0)
    {
        delete dataset; dataset = 0;
        QMessageBox::warning(0, tr("Error..."), tr("No georeference information found."));
        return;
    }

    xsize_px = dataset->GetRasterXSize();
    ysize_px = dataset->GetRasterYSize();


    double adfGeoTransform[6];
    dataset->GetGeoTransform( adfGeoTransform );

    xscale  = adfGeoTransform[1];
    yscale  = adfGeoTransform[5];
    xrot    = adfGeoTransform[4];
    yrot    = adfGeoTransform[2];

    trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
    trFwd.scale(adfGeoTransform[1],adfGeoTransform[5]);

    if(adfGeoTransform[4] != 0.0)
    {
        trFwd.rotate(atan(adfGeoTransform[2]/adfGeoTransform[4]));
    }

    if(pj_is_latlong(pjsrc))
    {
        // convert to RAD to match internal notations
        trFwd = trFwd * DEG_TO_RAD;
    }

    trInv = trFwd.inverted();

    ref1 = trFwd.map(QPointF(0,0));
    ref2 = trFwd.map(QPointF(xsize_px,0));
    ref3 = trFwd.map(QPointF(xsize_px,ysize_px));
    ref4 = trFwd.map(QPointF(0,ysize_px));

    qDebug() << "FF" << trFwd;
    qDebug() << "RR" << trInv;

    isActivated = true;
}

CMapVRT::~CMapVRT()
{
    delete dataset;
}

void CMapVRT::draw(IDrawContext::buffer_t& buf)
{
    if(map->needsRedraw())
    {
        return;
    }

    QPointF bufferScale = buf.scale * buf.zoomFactor;

    // calculate bounding box;
    QPointF pt1 = ref1;
    QPointF pt2 = ref2;
    QPointF pt3 = ref3;
    QPointF pt4 = ref4;

    pj_transform(pjsrc,pjtar, 1, 0, &pt1.rx(), &pt1.ry(), 0);
    pj_transform(pjsrc,pjtar, 1, 0, &pt2.rx(), &pt2.ry(), 0);
    pj_transform(pjsrc,pjtar, 1, 0, &pt3.rx(), &pt3.ry(), 0);
    pj_transform(pjsrc,pjtar, 1, 0, &pt4.rx(), &pt4.ry(), 0);

    QPolygonF boundingBox;
    boundingBox << pt1 << pt2 << pt3 << pt4;
    map->convertRad2Px(boundingBox);

    // get pixel offset of top left buffer corner
    QPointF pp = buf.ref1;
    map->convertRad2Px(pp);

    // calculate area to read from file
    pt1 = buf.ref1;
    pt2 = buf.ref2;
    pt3 = buf.ref3;
    pt4 = buf.ref4;

    pj_transform(pjtar,pjsrc, 1, 0, &pt1.rx(), &pt1.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt2.rx(), &pt2.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt3.rx(), &pt3.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt4.rx(), &pt4.ry(), 0);

    pt1 = trInv.map(pt1);
    pt2 = trInv.map(pt2);
    pt3 = trInv.map(pt3);
    pt4 = trInv.map(pt4);

    qreal left, right, top, bottom;
    left     = pt1.x() < pt4.x() ? pt1.x() : pt4.x();
    right    = pt2.x() > pt3.x() ? pt2.x() : pt3.x();
    top      = pt1.y() < pt2.y() ? pt1.y() : pt2.y();
    bottom   = pt4.y() > pt3.y() ? pt4.y() : pt3.y();

    if(left < 0) left = 0;
    if(left > xsize_px) left = xsize_px;

    if(top < 0) top  = 0;
    if(top > ysize_px) top  = ysize_px;

    if(right > xsize_px) right = xsize_px;
    if(right < 0) right = 0;

    if(bottom > ysize_px) bottom = ysize_px;
    if(bottom < 0) bottom = 0;

    qreal imgw = 64;
    qreal imgh = 64;
    qreal dx =  imgw;
    qreal dy =  imgh;


    // estimate number of tiles and use it as a limit if no
    // user defined limit is given
    double nTiles = ((right - left) * (bottom - top) / (dx*dy));
    if(hasOverviews)
    {
        // if there are overviews tiles canbe reduced by reading
        // with a scale factor from file. Increase amount of pixel
        // read until tile limit is met.
        while(nTiles > TILELIMIT)
        {
            dx *= 2;
            dy *= 2;
            nTiles /= 4;
        }
    }
    else
    {
        nTiles = getMaxScale() == NOFLOAT ? nTiles : 0;
    }

    // start to draw the map
    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p,true);
    p.setOpacity(getOpacity()/100.0);
    p.translate(-pp);


    qDebug() << imgw << dx;
    // limit number of tiles to keep performance
    if(!isOutOfScale(bufferScale) && (nTiles < TILELIMIT))
    {

        for(qreal y = top; y < bottom; y += dy)
        {
            if(map->needsRedraw())
            {
                break;
            }

            for(qreal x = left; x < right; x += dx)
            {
                if(map->needsRedraw())
                {
                    break;
                }

                // read tile from file
                CPLErr err = CE_Failure;

                // reduce tile size at the border of the file
                qreal dx_used   = dx;
                qreal dy_used   = dy;
                qreal imgw_used = imgw;
                qreal imgh_used = imgh;

                if((x + dx) > xsize_px)
                {
                    dx_used     = xsize_px - x;
                    imgw_used   = qRound(imgw * dx_used / dx) & 0xFFFFFFFC;
                    dx_used     = dx * imgw_used / imgw;
                }
                if((y + dy) > ysize_px)
                {
                    dy_used     = ysize_px - y;
                    imgh_used   = imgh * dy_used / dy;
                }

                QImage img;
                if(rasterBandCount == 1)
                {
                    GDALRasterBand * pBand;
                    pBand = dataset->GetRasterBand(1);

                    img = QImage(QSize(imgw_used,imgh_used),QImage::Format_Indexed8);
                    img.setColorTable(colortable);

                    err = pBand->RasterIO(GF_Read
                        ,qRound(x),qRound(y)
                        ,dx_used,dy_used
                        ,img.bits()
                        ,imgw_used,imgh_used
                        ,GDT_Byte,0,0);
                }

                if(err)
                {
                    continue;
                }


                QPolygonF l;
                l << QPointF(x,y) << QPointF(x+dx_used,y) << QPointF(x+dx_used,y+dy_used) << QPointF(x,y+dy_used);
                l = trFwd.map(l);

                pj_transform(pjsrc,pjtar, 1, 0, &l[0].rx(), &l[0].ry(), 0);
                pj_transform(pjsrc,pjtar, 1, 0, &l[1].rx(), &l[1].ry(), 0);
                pj_transform(pjsrc,pjtar, 1, 0, &l[2].rx(), &l[2].ry(), 0);
                pj_transform(pjsrc,pjtar, 1, 0, &l[3].rx(), &l[3].ry(), 0);

                drawTile(img, l, p);
            }
        }
    }

    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    p.drawPolygon(boundingBox);
}

