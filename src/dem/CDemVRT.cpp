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

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QtWidgets>

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

    qreal  x = left;
    qreal  y = top;
    qreal dx = right - left;
    qreal dy = bottom - top;

    QPointF p1(left,  top);
    QPointF p2(right, top);
    pj_transform(pjsrc, pjtar, 1, 0, &p1.rx(), &p1.ry(), 0);
    pj_transform(pjsrc, pjtar, 1, 0, &p2.rx(), &p2.ry(), 0);
    qreal cellsize = GPS_Math_Distance(p1.x(), p1.y(), p2.x(), p2.y()) * 3 / dx;

    dx = qRound(dx) & 0xFFFFFFFC;
    dy = qRound(dy);

    if(dx*dy > 50000000)
    {
        return;
    }

    QVector<qint16> data(dx*dy);
    CPLErr err = dataset->RasterIO(GF_Read, x, y, dx, dy, data.data(), dx, dy, GDT_Int16, 1, 0, 0, 0, 0);
    if(err == CE_Failure)
    {
        return;
    }

    QImage image(dx,dy,QImage::Format_Indexed8);
    image.setColorTable(graytable);

#define GET_VALUE(D, X, Y) D[(X) + (Y) * dx]
#define FILL_WINDOW(D, X, Y, W) \
    W[0] = GET_VALUE(D, X-1, Y-1); \
    W[1] = GET_VALUE(D, X  , Y-1); \
    W[2] = GET_VALUE(D, X+1, Y-1); \
    W[3] = GET_VALUE(D, X-1, Y  ); \
    W[4] = GET_VALUE(D, X  , Y  ); \
    W[5] = GET_VALUE(D, X+1, Y  ); \
    W[6] = GET_VALUE(D, X-1, Y+1); \
    W[7] = GET_VALUE(D, X  , Y+1); \
    W[8] = GET_VALUE(D, X+1, Y+1);

    qint16 w[9];
    qreal zenith  = (90 - 45)*DEG_TO_RAD;
    qreal azimuth = (360.0 - 0 + 90)*DEG_TO_RAD;
    qreal dz_dx, dz_dy, slope, aspect;

    // 0 1 2
    // a b c

    // 3 4 5
    // d e f

    // 6 7 8
    // g h i

    for(int m = 1; m < (dy-1); m++)
    {
        for(int n = 1; n < (dx-1); n++)
        {
            FILL_WINDOW(data,n,m,w);
            dz_dx = ((w[2] + 2*w[5] + w[8]) - (w[0] + 2*w[3] + w[6])) / (8 * cellsize);
            dz_dy = ((w[6] + 2*w[7] + w[8]) - (w[0] + 2*w[1] + w[2])) / (8 * cellsize);

            slope = atan( 1.0 * sqrt( dz_dx*dz_dx + dz_dy*dz_dy) );

            if(dz_dx != 0)
            {
                aspect = atan2(dz_dy, -dz_dx);

                if(aspect < 0)
                {
                    aspect = 2*M_PI + aspect;
                }
            }
            else
            {

                if(dz_dy > 0)
                {
                    aspect = M_PI/2;
                }
                else if(dz_dy < 0)
                {

                    aspect = 1.5*M_PI;
                }
                else
                {
                    aspect = 0;
                }
            }
            quint8 c = qRound(255.0 * ( ( cos(zenith) * cos(slope) ) + ( sin(zenith) * sin(slope) * cos(azimuth - aspect) ) ));
            image.setPixel(n,m,c);

        }
    }

    image.save("hillshade.png");

//    hillshade = 255.0 * ( ( cos(zenith) * cos(slope) ) + ( sin(zenith) * sin(slope) * cos(azimuth - aspect) ) );



}


///************************************************************************/
///*                         GDALHillshade()                              */
///************************************************************************/

//typedef struct
//{
//    double nsres;
//    double ewres;
//    double sin_altRadians;
//    double cos_altRadians_mul_z_scale_factor;
//    double azRadians;
//    double square_z_scale_factor;
//    double square_M_PI_2;
//} GDALHillshadeAlgData;

///* Unoptimized formulas are :
//    x = psData->z*((afWin[0] + afWin[3] + afWin[3] + afWin[6]) -
//        (afWin[2] + afWin[5] + afWin[5] + afWin[8])) /
//        (8.0 * psData->ewres * psData->scale);

//    y = psData->z*((afWin[6] + afWin[7] + afWin[7] + afWin[8]) -
//        (afWin[0] + afWin[1] + afWin[1] + afWin[2])) /
//        (8.0 * psData->nsres * psData->scale);

//    slope = M_PI / 2 - atan(sqrt(x*x + y*y));

//    aspect = atan2(y,x);

//    cang = sin(alt * degreesToRadians) * sin(slope) +
//           cos(alt * degreesToRadians) * cos(slope) *
//           cos(az * degreesToRadians - M_PI/2 - aspect);
//*/

//void*  GDALCreateHillshadeData(double* adfGeoTransform,
//                               double z,
//                               double scale,
//                               double alt,
//                               double az,
//                               int bZevenbergenThorne)
//{
//    GDALHillshadeAlgData* pData =
//        (GDALHillshadeAlgData*)CPLMalloc(sizeof(GDALHillshadeAlgData));

//    const double degreesToRadians = M_PI / 180.0;
//    pData->nsres = adfGeoTransform[5];
//    pData->ewres = adfGeoTransform[1];
//    pData->sin_altRadians = sin(alt * degreesToRadians);
//    pData->azRadians = az * degreesToRadians;
//    double z_scale_factor = z / (((bZevenbergenThorne) ? 2 : 8) * scale);
//    pData->cos_altRadians_mul_z_scale_factor =
//        cos(alt * degreesToRadians) * z_scale_factor;
//    pData->square_z_scale_factor = z_scale_factor * z_scale_factor;
//    pData->square_M_PI_2 = (M_PI*M_PI)/4;
//    return pData;
//}

//float GDALHillshadeAlg (float* afWin, float fDstNoDataValue, void* pData)
//{
//    GDALHillshadeAlgData* psData = (GDALHillshadeAlgData*)pData;
//    double x, y, aspect, xx_plus_yy, cang;

//    // First Slope ...
//    x = ((afWin[0] + afWin[3] + afWin[3] + afWin[6]) -
//        (afWin[2] + afWin[5] + afWin[5] + afWin[8])) / psData->ewres;

//    y = ((afWin[6] + afWin[7] + afWin[7] + afWin[8]) -
//        (afWin[0] + afWin[1] + afWin[1] + afWin[2])) / psData->nsres;

//    xx_plus_yy = x * x + y * y;

//    // ... then aspect...
//    aspect = atan2(y,x);

//    // ... then the shade value
//    cang = (psData->sin_altRadians -
//           psData->cos_altRadians_mul_z_scale_factor * sqrt(xx_plus_yy) *
//           sin(aspect - psData->azRadians)) /
//           sqrt(1 + psData->square_z_scale_factor * xx_plus_yy);

//    if (cang <= 0.0)
//        cang = 1.0;
//    else
//        cang = 1.0 + (254.0 * cang);

//    return (float) cang;
//}
