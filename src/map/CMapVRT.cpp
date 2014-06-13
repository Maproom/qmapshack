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
#include "map/CMap.h"
#include "CCanvas.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QtWidgets>

CMapVRT::CMapVRT(const QString &filename, CMap *parent)
    : IMap(parent)
    , map(parent)
    , filename(filename)
    , rasterBandCount(0)
{
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

    if (pj_is_latlong(pjsrc))
    {
        xscale  = adfGeoTransform[1] * DEG_TO_RAD;
        yscale  = adfGeoTransform[5] * DEG_TO_RAD;

        xref1   = adfGeoTransform[0] * DEG_TO_RAD;
        yref1   = adfGeoTransform[3] * DEG_TO_RAD;
    }
    else
    {
        xscale  = adfGeoTransform[1];
        yscale  = adfGeoTransform[5];

        xref1   = adfGeoTransform[0];
        yref1   = adfGeoTransform[3];
    }

    xref2   = xref1 + xsize_px * xscale;
    yref2   = yref1 + ysize_px * yscale;

    qDebug() << "xscale" << xscale << "yscale" << yscale;
    qDebug() << xref1 << yref1 << xref2 << yref2;

    isActivated = true;
}

CMapVRT::~CMapVRT()
{
    if(dataset) delete dataset;
}

void CMapVRT::draw(buffer_t& buf)
{
    if(map->needsRedraw())
    {
        return;
    }

    qDebug() << "---------------------";
    // convert top left buffer corner
    // into buffer's coordinate system
    QPointF pp = buf.ref1;
    pj_transform(pjtar,buf.pjsrc,1,0,&pp.rx(),&pp.ry(),0);

    QPointF p1 = buf.ref1;
    QPointF p2 = buf.ref2;
    QPointF p3 = buf.ref3;
    QPointF p4 = buf.ref4;
    pj_transform(pjtar,buf.pjsrc, 1, 0, &p1.rx(), &p1.ry(), 0);
    pj_transform(pjtar,buf.pjsrc, 1, 0, &p2.rx(), &p2.ry(), 0);
    pj_transform(pjtar,buf.pjsrc, 1, 0, &p3.rx(), &p3.ry(), 0);
    pj_transform(pjtar,buf.pjsrc, 1, 0, &p4.rx(), &p4.ry(), 0);

    qreal dx1 = sqrt((p2.x() - p1.x())*(p2.x() - p1.x()) + (p2.y() - p1.y())*(p2.y() - p1.y()));
    qreal dy1 = sqrt((p4.x() - p1.x())*(p4.x() - p1.x()) + (p4.y() - p1.y())*(p4.y() - p1.y()));

    QPointF pt1 = buf.ref1;
    QPointF pt2 = buf.ref2;
    QPointF pt3 = buf.ref3;
    QPointF pt4 = buf.ref4;

    pj_transform(pjtar,pjsrc, 1, 0, &pt1.rx(), &pt1.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt2.rx(), &pt2.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt3.rx(), &pt3.ry(), 0);
    pj_transform(pjtar,pjsrc, 1, 0, &pt4.rx(), &pt4.ry(), 0);

    qreal dx2 = sqrt((pt2.x() - pt1.x())*(pt2.x() - pt1.x()) + (pt2.y() - pt1.y())*(pt2.y() - pt1.y()));
    qreal dy2 = sqrt((pt4.x() - pt1.x())*(pt4.x() - pt1.x()) + (pt4.y() - pt1.y())*(pt4.y() - pt1.y()));

    qDebug() << dx1 << dx2 << dx1/dx2;
    qDebug() << dy1 << dy2 << dy1/dy2;

    qreal u1 = pt1.x() < pt4.x() ? pt1.x() : pt4.x();
    qreal u2 = pt2.x() > pt3.x() ? pt2.x() : pt3.x();
    qreal v1 = pt1.y() > pt2.y() ? pt1.y() : pt2.y();
    qreal v2 = pt4.y() < pt3.y() ? pt4.y() : pt3.y();

    qDebug() << pt1 << pt2 << pt3 << pt4;
    qDebug() << u1 << v1 << u2 << v2;

    QRectF viewport(u1,v1, u2 - u1, v2 - v1);
    QRectF maparea(QPointF(xref1, yref1), QPointF(xref2, yref2));
    QRectF intersect = viewport.intersected(maparea);

    qDebug() << viewport << maparea << intersect;

    if(!intersect.isValid())
    {
        return;
    }
    QPointF bufferScale = buf.scale * buf.zoomFactor * QPointF(dx2/dx1, dy2/dy1);


    // x/y offset [pixel] into file matrix
    qint32 xoff = (intersect.left()   - xref1) / xscale;
    qint32 yoff = (intersect.bottom() - yref1) / yscale;

    // number of x/y pixel to read
    qint32 pxx  =   (qint32)(intersect.width()  / xscale);
    qint32 pxy  =  -(qint32)(intersect.height() / yscale);

    // the final image width and height in pixel
    qint32 w    =   (qint32)(pxx / bufferScale.x()) & 0xFFFFFFFC;
    qint32 h    =  -(qint32)(pxy / bufferScale.y());

    // correct pxx by truncation
    pxx         =   (qint32)(w * bufferScale.x());

    qDebug() << xoff << yoff << pxx << pxy << w << h;
    if(w > 0 && h > 0)
    {

        CPLErr err = CE_Failure;

        QImage img;
        if(rasterBandCount == 1)
        {
            GDALRasterBand * pBand;
            pBand = dataset->GetRasterBand(1);

            img = QImage(QSize(w,h),QImage::Format_Indexed8);
            img.setColorTable(colortable);

            err = pBand->RasterIO(GF_Read
                ,(int)xoff,(int)yoff
                ,pxx,pxy
                ,img.bits()
                ,w,h
                ,GDT_Byte,0,0);
        }

        if(err)
        {
            return;
        }

        pt1 = intersect.bottomLeft();
        pt2 = intersect.bottomRight();
        pt3 = intersect.topRight();
        pt4 = intersect.topLeft();

        // transform the tile's corner coordinate from map's projection into buffer's projecton
        pj_transform(pjsrc,buf.pjsrc,1,0,&pt1.rx(),&pt1.ry(),0);
        pj_transform(pjsrc,buf.pjsrc,1,0,&pt2.rx(),&pt2.ry(),0);
        pj_transform(pjsrc,buf.pjsrc,1,0,&pt3.rx(),&pt3.ry(),0);
        pj_transform(pjsrc,buf.pjsrc,1,0,&pt4.rx(),&pt4.ry(),0);

        // adjust the tiles width and height to fit the buffer's scale
        qreal dx1   = pt1.x() - pt2.x();
        qreal dy1   = pt1.y() - pt2.y();
        qreal dx2   = pt1.x() - pt4.x();
        qreal dy2   = pt1.y() - pt4.y();
        qreal w    = ceil( sqrt(dx1*dx1 + dy1*dy1) / bufferScale.x());
        qreal h    = ceil(-sqrt(dx2*dx2 + dy2*dy2) / bufferScale.y());

        qDebug() << pp << pt1;
        // calculate offset into buffer
        pt1 = (pt1 - pp) / (bufferScale);
        // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
        qreal a = atan(dy1/dx1) * RAD_TO_DEG;

        QPainter p(&buf.image);
        USE_ANTI_ALIASING(p,true);

        qDebug() << "angle" << a << "offset" << pt1;
        // finally scale, rotate and draw tile
        p.translate(pt1);
        p.rotate(-a);
        p.drawImage(0,0,img);
        p.resetTransform();


    }
}
