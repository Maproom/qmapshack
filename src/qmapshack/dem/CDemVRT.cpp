/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
                  2019 Johannes Zellner johannes@zellner.org

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

#include "CMainWindow.h"
#include "dem/CDemDraw.h"
#include "dem/CDemVRT.h"
#include "GeoMath.h"
#include "helpers/CDraw.h"
#include "units/IUnit.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QtWidgets>

#define TILELIMIT 30000
#define TILESIZEX 64
#define TILESIZEY 64

CDemVRT::CDemVRT(const QString &filename, CDemDraw *parent)
    : IDem(parent)
    , filename(filename)
{
    qDebug() << "------------------------------";
    qDebug() << "VRT: try to open" << filename;

    dataset = (GDALDataset*)GDALOpen(filename.toUtf8(), GA_ReadOnly);
    if(nullptr == dataset)
    {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to load file: %1").arg(filename));
        return;
    }

    if(dataset->GetRasterCount() != 1)
    {
        GDALClose(dataset);
        dataset = nullptr;
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("DEM must have one band with 16bit or 32bit data."));
        return;
    }

    GDALRasterBand *pBand = dataset->GetRasterBand(1);
    if(nullptr == pBand)
    {
        GDALClose(dataset);
        dataset = nullptr;
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("DEM must have one band with 16bit or 32bit data."));
        return;
    }

    hasOverviews = pBand->GetOverviewCount() != 0;
    qDebug() << "has overviews" << hasOverviews;

    noData = pBand->GetNoDataValue(&hasNoData);
    qDebug() << "no data:" << hasNoData << noData;

    // ------- setup projection ---------------
    char str[1025] = {0};
    if(dataset->GetProjectionRef())
    {
        strncpy(str, dataset->GetProjectionRef(), sizeof(str) - 1);
    }
    OGRSpatialReference oSRS;
    const char *wkt = str;
    oSRS.importFromWkt(&wkt);

    char *proj4 = nullptr;
    oSRS.exportToProj4(&proj4);
    pjsrc = pj_init_plus(proj4);
    free(proj4);

    if(pjsrc == 0)
    {
        GDALClose(dataset);
        dataset = nullptr;
        QMessageBox::warning(0, tr("Error..."), tr("No georeference information found."));
        return;
    }

    xsize_px = dataset->GetRasterXSize();
    ysize_px = dataset->GetRasterYSize();

    qreal adfGeoTransform[6];
    dataset->GetGeoTransform( adfGeoTransform );

    xscale  = adfGeoTransform[1];
    yscale  = adfGeoTransform[5];

    trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
    trFwd.scale(adfGeoTransform[1], adfGeoTransform[5]);

    if(adfGeoTransform[4] != 0.0)
    {
        trFwd.rotate(qAtan(adfGeoTransform[2] / adfGeoTransform[4]));
    }

    if(pj_is_latlong(pjsrc))
    {
        xscale *= 111120;
        yscale *= 111120;
        // convert to RAD to match internal notations
        trFwd = trFwd * DEG_TO_RAD;
    }

    trInv = trFwd.inverted();

    ref1 = trFwd.map(QPointF(0, 0));
    ref2 = trFwd.map(QPointF(xsize_px, 0));
    ref3 = trFwd.map(QPointF(xsize_px, ysize_px));
    ref4 = trFwd.map(QPointF(0, ysize_px));

    qDebug() << ref1 << ref2 << ref3 << ref4;
    boundingBox = QRectF(ref1, ref3);

    qDebug() << "FF" << trFwd;
    qDebug() << "RR" << trInv;

    isActivated = true;
}

CDemVRT::~CDemVRT()
{
    GDALClose(dataset);
}

qreal CDemVRT::getElevationAt(const QPointF& pos, bool checkScale)
{
    if(pjsrc == 0 || (checkScale && outOfScale))
    {
        return NOFLOAT;
    }

    qint16 e[4];
    QPointF pt = pos;

    pj_transform(pjtar, pjsrc, 1, 0, &pt.rx(), &pt.ry(), 0);

    if(!boundingBox.contains(pt))
    {
        return NOFLOAT;
    }

    pt = trInv.map(pt);

    qreal x    = pt.x() - qFloor(pt.x());
    qreal y    = pt.y() - qFloor(pt.y());

    mutex.lock();
    CPLErr err = dataset->RasterIO(GF_Read, qFloor(pt.x()), qFloor(pt.y()), 2, 2, &e, 2, 2, GDT_Int16, 1, 0, 0, 0, 0);
    mutex.unlock();
    if(err == CE_Failure)
    {
        return NOFLOAT;
    }

    if(hasNoData && ((e[0] == noData) || (e[1] == noData) || (e[2] == noData) || (e[3] == noData)))
    {
        return NOFLOAT;
    }

    qreal b1 = e[0];
    qreal b2 = e[1] - e[0];
    qreal b3 = e[2] - e[0];
    qreal b4 = e[0] - e[1] - e[2] + e[3];

    qreal ele = b1 + b2 * x + b3 * y + b4 * x * y;

    return ele;
}

qreal CDemVRT::getSlopeAt(const QPointF& pos, bool checkScale)
{
    if(pjsrc == 0 || (checkScale && outOfScale))
    {
        return NOFLOAT;
    }

    QPointF pt = pos;

    pj_transform(pjtar, pjsrc, 1, 0, &pt.rx(), &pt.ry(), 0);

    if(!boundingBox.contains(pt))
    {
        return NOFLOAT;
    }

    pt = trInv.map(pt);

    qreal x    = pt.x() - qFloor(pt.x());
    qreal y    = pt.y() - qFloor(pt.y());

    qint16 win[eWinsize4x4];
    mutex.lock();
    CPLErr err = dataset->RasterIO(GF_Read, qFloor(pt.x()) - 1, qFloor(pt.y()) - 1, 4, 4, &win, 4, 4, GDT_Int16, 1, 0, 0, 0, 0);
    mutex.unlock();
    if(err == CE_Failure)
    {
        return NOFLOAT;
    }
    for(int i = 0; i < eWinsize4x4; i++)
    {
        if(hasNoData && win[i] == noData)
        {
            return NOFLOAT;
        }
    }

    qreal slope = slopeOfWindowInterp(win, eWinsize4x4, x, y);
    return slope;
}


void CDemVRT::draw(IDrawContext::buffer_t& buf)
{
    if(dem->needsRedraw())
    {
        return;
    }

    QPointF bufferScale = buf.scale * buf.zoomFactor;
    outOfScale = isOutOfScale(bufferScale);

    if(outOfScale || (!doHillshading() && !doSlopeColor() && !doElevationLimit()))
    {
        QThread::msleep(100);
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

    pj_transform(pjtar, pjsrc, 1, 0, &pt1.rx(), &pt1.ry(), 0);
    pj_transform(pjtar, pjsrc, 1, 0, &pt2.rx(), &pt2.ry(), 0);
    pj_transform(pjtar, pjsrc, 1, 0, &pt3.rx(), &pt3.ry(), 0);
    pj_transform(pjtar, pjsrc, 1, 0, &pt4.rx(), &pt4.ry(), 0);

    pt1 = trInv.map(pt1);
    pt2 = trInv.map(pt2);
    pt3 = trInv.map(pt3);
    pt4 = trInv.map(pt4);

    qreal left, right, top, bottom;
    left     = qRound(pt1.x() < pt4.x() ? pt1.x() : pt4.x());
    right    = qRound(pt2.x() > pt3.x() ? pt2.x() : pt3.x());
    top      = qRound(pt1.y() < pt2.y() ? pt1.y() : pt2.y());
    bottom   = qRound(pt4.y() > pt3.y() ? pt4.y() : pt3.y());

    if(left <= 0)
    {
        left = 1;
    }
    if(left >= xsize_px)
    {
        left = xsize_px - 1;
    }

    if(top <= 0)
    {
        top  = 1;
    }
    if(top >= ysize_px)
    {
        top = ysize_px - 1;
    }

    if(right >= xsize_px)
    {
        right = xsize_px - 1;
    }
    if(right <= 0)
    {
        right = 1;
    }

    if(bottom >= ysize_px)
    {
        bottom = ysize_px - 1;
    }
    if(bottom <= 0)
    {
        bottom = 1;
    }

    qreal imgw = TILESIZEX;
    qreal imgh = TILESIZEY;
    qreal w =  imgw;
    qreal h =  imgh;

    /*
        As the 3x3 window will create a border of one pixel
        more data is read than displayed to compensate.
     */
    int wp2 = w + 2;
    int hp2 = h + 2;

    // start to draw the map
    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p, true);
    p.translate(-pp);

    qreal o1 = getOpacity() / 100.0;
    qreal o2 = ((o1 + 0.4) >= 1.0) ? o1 : (o1 + 0.4);
    p.setOpacity(o1);

    qreal nTiles = ((right - left) * (bottom - top) / (w * h));
    if(nTiles < TILELIMIT)
    {
        for(qreal y = top - 1; y < bottom; y += h)
        {
            if(dem->needsRedraw())
            {
                break;
            }

            for(qreal x = left - 1; x < right; x += w)
            {
                if(dem->needsRedraw())
                {
                    break;
                }

                CPLErr err = CE_Failure;

                qreal wp2_used = wp2;
                qreal hp2_used = hp2;
                qreal w_used   = w;
                qreal h_used   = h;

                if((x + wp2) > xsize_px)
                {
                    wp2_used = xsize_px - x;
                    w_used   = wp2_used - 2;
                    if(w_used < 2)
                    {
                        continue;
                    }
                }

                if((y + hp2) > ysize_px)
                {
                    hp2_used = ysize_px - y;
                    h_used   = hp2_used - 2;
                    if(h_used < 2)
                    {
                        continue;
                    }
                }

                QVector<qint16> data(wp2_used * hp2_used);
                mutex.lock();
                err = dataset->RasterIO(GF_Read, x, y, wp2_used, hp2_used, data.data(), wp2_used, hp2_used, GDT_Int16, 1, 0, 0, 0, 0);
                mutex.unlock();

                if(err)
                {
                    continue;
                }


                QPolygonF l(4);
                l[0] = QPointF(x + 1, y + 1);
                l[1] = QPointF(x + 1 + w_used, y + 1);
                l[2] = QPointF(x + 1 + w_used, y + 1 + h_used);
                l[3] = QPointF(x + 1, y + 1 + h_used);
                l = trFwd.map(l);
                pj_transform(pjsrc, pjtar, 1, 0, &l[0].rx(), &l[0].ry(), 0);
                pj_transform(pjsrc, pjtar, 1, 0, &l[1].rx(), &l[1].ry(), 0);
                pj_transform(pjsrc, pjtar, 1, 0, &l[2].rx(), &l[2].ry(), 0);
                pj_transform(pjsrc, pjtar, 1, 0, &l[3].rx(), &l[3].ry(), 0);

                if(doHillshading())
                {
                    QPolygonF r = l;

                    QImage img(w_used, h_used, QImage::Format_Indexed8);
                    img.setColorTable(graytable);

                    hillshading(data, w_used, h_used, img);

                    drawTile(img, r, p);
                }

                if(doSlopeColor())
                {
                    QPolygonF r = l;

                    QImage img(w_used, h_used, QImage::Format_Indexed8);
                    img.setColorTable(slopetable);

                    slopecolor(data, w_used, h_used, img);

                    p.setOpacity(o2);
                    drawTile(img, r, p);
                    p.setOpacity(o1);
                }

                if(doElevationLimit())
                {
                    QPolygonF r = l;

                    QImage img(w_used, h_used, QImage::Format_Indexed8);
                    img.setColorTable(elevationtable);

                    elevationLimit(data, w_used, h_used, img);

                    p.setOpacity(o2);
                    drawTile(img, r, p);
                    p.setOpacity(o1);
                }
            }
        }
    }
}


