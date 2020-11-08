/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "helpers/CGdalFile.h"

#include <gdal_priv.h>
#include <ogr_spatialref.h>

#include <QtWidgets>

CGdalFile::CGdalFile(type_e type)
    : type(type)
{
}


void CGdalFile::unload()
{
    isValid = false;
    if(dataset != nullptr)
    {
        GDALClose(dataset);
    }
    dataset = nullptr;
}


void CGdalFile::load(const QString& filename)
{
    qDebug() << filename;
    CCanvas * canvas = CMainWindow::self().getCanvas();

    dataset = (GDALDataset*)GDALOpenShared(filename.toUtf8(), GA_ReadOnly);

    if(nullptr == dataset)
    {
        QMessageBox::warning(canvas, tr("Error..."), tr("Failed to load file: %1").arg(filename));
        return;
    }

    char str[1025] = {0};
    if(dataset->GetProjectionRef())
    {
        strncpy(str, dataset->GetProjectionRef(), sizeof(str) - 1);
    }

    {
        OGRSpatialReference oSRS;
        const char *wkt = str;
        oSRS.importFromWkt(&wkt);

        char *proj4 = nullptr;
        oSRS.exportToProj4(&proj4);
        proj4str = proj4;
        pjsrc = pj_init_plus(proj4);
        free(proj4);
    }

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
            for(int i = 0; i < pct->GetColorEntryCount(); ++i)
            {
                const GDALColorEntry& e = *pct->GetColorEntry(i);
                colortable << qRgba(e.c1, e.c2, e.c3, e.c4);
            }
        }
        else if(pBand->GetColorInterpretation() ==  GCI_GrayIndex )
        {
            for(int i = 0; i < 256; ++i)
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
            if((idx >= 0) && (idx < colortable.size()))
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

    qreal adfGeoTransform[6] = {0, 1, 0, 0, 0, 1};
    if(type == eTypeProj)
    {
        dataset->GetGeoTransform( adfGeoTransform );
    }

    xscale  = adfGeoTransform[1];
    yscale  = adfGeoTransform[5];
    xrot    = adfGeoTransform[4];
    yrot    = adfGeoTransform[2];

    // Setup the basic transformation. Depending on the type this
    // can be the transformation into pixel coordinates or into
    // geo coordinates if the file is referenced.
    trFwd = QTransform();
    trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
    trFwd.scale(xscale, yscale);

    if(adfGeoTransform[4] != 0.0)
    {
        trFwd.rotate(qAtan(xrot / yrot));
    }

    trInv = trFwd.inverted();

    ref1 = trFwd.map(QPointF(0, 0));
    ref2 = trFwd.map(QPointF(xsize_px, 0));
    ref3 = trFwd.map(QPointF(xsize_px, ysize_px));
    ref4 = trFwd.map(QPointF(0, ysize_px));

    {
        // Setup the geo transformation. If the file is referenced
        // this will be the transformation between points and geo
        // coordinates. If not this will be point to pixel transformation
        qreal adfGeoTransform[6] = {0, 1, 0, 0, 0, 1};
        dataset->GetGeoTransform( adfGeoTransform );

        trFwdProj = QTransform();
        trFwdProj.translate(adfGeoTransform[0], adfGeoTransform[3]);
        trFwdProj.scale(adfGeoTransform[1], adfGeoTransform[5]);

        if(adfGeoTransform[4] != 0.0)
        {
            trFwdProj.rotate(qAtan(adfGeoTransform[4] / adfGeoTransform[2]));
        }

        trInvProj = trFwdProj.inverted();
    }

    isValid = true;
}

QString CGdalFile::getProjection() const
{
    return proj4str;
}

QString CGdalFile::getInfo() const
{
    QString str;
    QTextStream out(&str);

    if(proj4str.isEmpty())
    {
        out << "no projection" << endl;
    }
    else
    {
        out << getProjection() << endl;
        if(pj_is_latlong(pjsrc))
        {
            out << "xscale: " << xscale << "px/rad\tyscale: " << yscale << "px/rad" << endl;
        }
        else
        {
            out << "xscale: " << xscale << "px/m\tyscale: " << yscale << "px/m" << endl;
        }
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
