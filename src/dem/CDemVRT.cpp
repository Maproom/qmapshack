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

#include "dem/CDemVRT.h"
#include "dem/CDemDraw.h"
#include "GeoMath.h"
#include "CCanvas.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QtWidgets>

#define TILELIMIT 30000

CDemVRT::CDemVRT(const QString &filename, CDemDraw *parent)
    : IDem(parent)
    , filename(filename)
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

    if(dataset->GetRasterCount() != 1)
    {
        delete dataset; dataset = 0;
        QMessageBox::warning(0, tr("Error..."), tr("DEM must have one band with 16bit or 32bit data."));
        return;
    }

    GDALRasterBand * pBand;
    pBand = dataset->GetRasterBand(1);
    if(pBand == 0)
    {
        delete dataset; dataset = 0;
        QMessageBox::warning(0, tr("Error..."), tr("DEM must have one band with 16bit or 32bit data."));
        return;
    }

    hasOverviews = pBand->GetOverviewCount() != 0;
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

    graytable.resize(256);
    for(int i = 0; i < 256; i++)
    {
        graytable[i] = qRgb(i,i,i);
    }

    isActivated = true;
}

CDemVRT::~CDemVRT()
{
    delete dataset;
}

//#define GET_VALUE(D, X, Y) D[(X) + (Y) * dx]
inline qint16 getValue(QVector<qint16>& data, int x, int y, int dx)
{
    return data[x + y * dx];
}

inline void fillWindow(QVector<qint16>& data, int x, int y, int dx, qint16 * w)
{
    w[0] = getValue(data, x - 1, y - 1, dx);
    w[1] = getValue(data, x    , y - 1, dx);
    w[2] = getValue(data, x + 1, y - 1, dx);
    w[3] = getValue(data, x - 1, y    , dx);
    w[4] = getValue(data, x    , y    , dx);
    w[5] = getValue(data, x + 1, y    , dx);
    w[6] = getValue(data, x - 1, y + 1, dx);
    w[7] = getValue(data, x    , y + 1, dx);
    w[8] = getValue(data, x + 1, y + 1, dx);
}



void CDemVRT::draw(IDrawContext::buffer_t& buf)
{
    if(dem->needsRedraw())
    {
        return;
    }

    // get pixel offset of top left buffer corner
    QPointF pp = buf.ref1;
    dem->convertRad2Px(pp);

    // calculate area to read from file
    QPointF pt1 = buf.ref1;
    QPointF pt2 = buf.ref2;
    QPointF pt3 = buf.ref3;
    QPointF pt4 = buf.ref4;

    pj_transform(pjtar,pjsrc, 1, 0, &pt1.rx(), &pt1.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt2.rx(), &pt2.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt3.rx(), &pt3.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt4.rx(), &pt4.ry(), 0);

    pt1 = trInv.map(pt1);
    pt2 = trInv.map(pt2);
    pt3 = trInv.map(pt3);
    pt4 = trInv.map(pt4);

    qreal left, right, top, bottom;
    left     = qRound(pt1.x() < pt4.x() ? pt1.x() : pt4.x());
    right    = qRound(pt2.x() > pt3.x() ? pt2.x() : pt3.x());
    top      = qRound(pt1.y() < pt2.y() ? pt1.y() : pt2.y());
    bottom   = qRound(pt4.y() > pt3.y() ? pt4.y() : pt3.y());

    if(left <= 0) left = 1;
    if(left >= xsize_px) left = xsize_px - 1;

    if(top <= 0) top  = 1;
    if(top >= ysize_px) top = ysize_px - 1;

    if(right >= xsize_px) right = xsize_px - 1;
    if(right <= 0) right = 1;

    if(bottom >= ysize_px) bottom = ysize_px - 1;
    if(bottom <= 0) bottom = 1;

    qreal imgw = 64;
    qreal imgh = 64;
    qreal dx =  imgw;
    qreal dy =  imgh;

    int w = dx + 2;
    int h = dy + 2;

    // start to draw the map
    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p,true);
    p.setOpacity(0.25);
    p.translate(-pp);

    double nTiles = ((right - left) * (bottom - top) / (dx*dy));
    qDebug() << nTiles;
    if(nTiles < TILELIMIT)
    {
        for(qreal y = top - 1; y < bottom; y += dy)
        {
            if(dem->needsRedraw())
            {
                break;
            }

            for(qreal x = left - 1; x < right; x += dx)
            {
                if(dem->needsRedraw())
                {
                    break;
                }

                CPLErr err = CE_Failure;

                QVector<qint16> data(w*h);
                err = dataset->RasterIO(GF_Read, x, y, w, h, data.data(), w, h, GDT_Int16, 1, 0, 0, 0, 0);

                if(err)
                {
                    continue;
                }

                QImage img(imgw,imgh,QImage::Format_Indexed8);
                img.setColorTable(graytable);

                qint16 win[9];
                qreal dz_dx, dz_dy, aspect, xx_plus_yy, cang;

#define ZFACT           0.5
#define ZFACT_BY_ZFACT  0.25
#define SIN_ALT         0.70711
#define ZFACT_COS_ALT   0.35355
#define AZ              5.4978
                for(int m = 1; m < (dy+1); m++)
                {
                    for(int n = 1; n < (dx+1); n++)
                    {
                        fillWindow(data, n, m, w, win);
                        dz_dx       = ((win[0] + win[3] + win[3] + win[6]) - (win[2] + win[5] + win[5] + win[8])) / (xscale);
                        dz_dy       = ((win[6] + win[7] + win[7] + win[8]) - (win[0] + win[1] + win[1] + win[2])) / (yscale);
                        aspect      = atan2(dz_dy, dz_dx);
                        xx_plus_yy  = dz_dx * dz_dx + dz_dy * dz_dy;

                        cang = (SIN_ALT - ZFACT_COS_ALT * sqrt(xx_plus_yy) * sin(aspect - AZ)) / sqrt(1+ZFACT_BY_ZFACT*xx_plus_yy);

                        if (cang <= 0.0)
                        {
                            cang = 1.0;
                        }
                        else
                        {
                            cang = 1.0 + (254.0 * cang);
                        }

                        img.setPixel(n - 1, m - 1, cang);
                    }
                }


                QPolygonF l;
                l << QPointF(x + 1, y + 1) << QPointF(x + 1 + dx, y + 1) << QPointF(x + 1 + dx, y + 1 + dy) << QPointF(x + 1, y + 1 + dy);
                l = trFwd.map(l);

                pj_transform(pjsrc,pjtar, 1, 0, &l[0].rx(), &l[0].ry(), 0);
                pj_transform(pjsrc,pjtar, 1, 0, &l[1].rx(), &l[1].ry(), 0);
                pj_transform(pjsrc,pjtar, 1, 0, &l[2].rx(), &l[2].ry(), 0);
                pj_transform(pjsrc,pjtar, 1, 0, &l[3].rx(), &l[3].ry(), 0);

                drawTile(img, l, p);

            }
        }
    }
}


