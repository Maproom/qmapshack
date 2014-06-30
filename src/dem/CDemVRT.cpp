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
        QMessageBox::warning(0, tr("Error..."), tr("DEM must have one band with 16bit data."));
        return;
    }

    GDALRasterBand * pBand;
    pBand = dataset->GetRasterBand(1);
    if(pBand == 0)
    {
        delete dataset; dataset = 0;
        QMessageBox::warning(0, tr("Error..."), tr("Failed to load file: %1").arg(filename));
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
        trFwd = trFwd * DEG_TO_RAD;
    }

    trInv = trFwd.inverted();

    ref1 = trFwd.map(QPointF(0,0));
    ref2 = trFwd.map(QPointF(xsize_px,0));
    ref3 = trFwd.map(QPointF(xsize_px,ysize_px));
    ref4 = trFwd.map(QPointF(0,ysize_px));

    qDebug() << "FF" << trFwd;
    qDebug() << "RR" << trInv;

    qDebug() << ref1 << ref2 << ref3 << ref4;


    isActivated = true;
}

CDemVRT::~CDemVRT()
{
    delete dataset;
}

void CDemVRT::draw(IDrawContext::buffer_t& buf)
{

}
