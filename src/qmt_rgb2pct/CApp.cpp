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

#include "CApp.h"

#include <gdal_alg.h>
#include <gdal_priv.h>
#include <iostream>

const GDALColorEntry CApp::noColor = {255, 255, 255, 0};

void printStdoutQString(const QString& str)
{
    QByteArray array = str.toUtf8();
    printf("%s", array.data());
    printf("\n");
}

void printStderrQString(const QString& str)
{
    QByteArray array = str.toUtf8();
    fprintf(stderr, "%s", array.data());
    fprintf(stderr, "\n");
}



CApp::CApp(qint32 ncolors, const QString& pctFilename, const QString& sctFilename, const QString& srcFilename, const QString& tarFilename)
    : ncolors(ncolors)
    , pctFilename(pctFilename)
    , sctFilename(sctFilename)
    , srcFilename(srcFilename)
    , tarFilename(tarFilename)
{
    GDALAllRegister();
}

qint32 CApp::exec()
{
    qint32 res = 0;
    GDALColorTable* ct = nullptr;
    GDALDataset* dsSrc = nullptr;
    try
    {
        dsSrc = (GDALDataset*)GDALOpenShared(srcFilename.toUtf8(), GA_ReadOnly);
        if(dsSrc == nullptr)
        {
            throw tr("Failed to open source file.");
        }

        if(dsSrc->GetRasterCount() < 3 || dsSrc->GetRasterCount() > 4)
        {
            throw tr("Raster band count of source file must be either 3 or 4.");
        }

        if(QFile(tarFilename).exists())
        {
            QFile::remove(tarFilename);
        }

        ct = createColorTable(ncolors, pctFilename, dsSrc);
        saveColorTable(ct, sctFilename);
        ditherMap(dsSrc, tarFilename, ct);
    }
    catch(const QString& msg)
    {
        printStderrQString(msg);
        res = -1;
    }


    GDALClose(dsSrc);
    delete ct;
    return res;
}

GDALColorTable* CApp::createColorTable(qint32 ncolors, const QString& pctFilename, GDALDataset* dataset)
{
    GDALColorTable* ct = nullptr;
    try
    {
        if(pctFilename.isEmpty())
        {
            ct = (GDALColorTable*)GDALCreateColorTable(GPI_RGB);

            printStdoutQString(tr("Calculate optimal color table from source file"));

            int ok = GDALComputeMedianCutPCT(dataset->GetRasterBand(1),
                                             dataset->GetRasterBand(2),
                                             dataset->GetRasterBand(3),
                                             nullptr,
                                             ncolors,
                                             ct,
                                             GDALTermProgress,
                                             0
                                             );

            if(ok != CE_None)
            {
                throw tr("Failed to create color table.");
            }
        }
        else
        {
            GDALDataset* dsPct = (GDALDataset*)GDALOpenShared(pctFilename.toUtf8(), GA_ReadOnly);
            if(dsPct == nullptr)
            {
                throw tr("Failed to open file with palette.");
            }

            GDALRasterBand* band = (GDALRasterBand*)dsPct->GetRasterBand(1);

            if((dsPct->GetRasterCount() != 1) || (band->GetColorInterpretation() != GCI_PaletteIndex))
            {
                GDALClose(dsPct);
                throw tr("Palette file does not have a single band with a color table");
            }

            int ok = 0;
            band->GetNoDataValue(&ok);

            if(ok || band->GetColorTable()->GetColorEntryCount() > 255)
            {
                GDALClose(dsPct);
                throw tr("The color table must not contain a \"no data\" value and it's size must not exceed 255 colors.");
            }

            ct = dsPct->GetRasterBand(1)->GetColorTable()->Clone();
        }
    }
    catch(const QString& msg)
    {
        delete ct;
        throw msg;
    }
    return ct;
}

void CApp::saveColorTable(GDALColorTable* ct, QString& sctFilename)
{
    if(sctFilename.isEmpty())
    {
        return;
    }

    if(!sctFilename.endsWith(".vrt"))
    {
        sctFilename += ".vrt";
    }

    QByteArray buf = sctFilename.toUtf8();
    printStdoutQString(tr("Save color table to: %1").arg(buf.data()));

    GDALDriverManager* drvman = GetGDALDriverManager();
    GDALDriver* driver = drvman->GetDriverByName("VRT");
    GDALDataset* dataset = driver->Create(sctFilename.toUtf8(), 1, 1, 1, GDT_Byte, {});

    dataset->GetRasterBand(1)->SetColorInterpretation(GCI_PaletteIndex);
    dataset->GetRasterBand(1)->SetColorTable(ct);

    dataset->FlushCache();
    GDALClose(dataset);
}

void CApp::ditherMap(GDALDataset* dsSrc, const QString& tarFilename, GDALColorTable* ct)
{
    if(tarFilename.isEmpty())
    {
        return;
    }

    qint32 xsize = dsSrc->GetRasterBand(1)->GetXSize();
    qint32 ysize = dsSrc->GetRasterBand(1)->GetYSize();

    GDALDriverManager* drvman = nullptr;
    GDALDriver* driver = nullptr;
    GDALDataset* dataset = nullptr;

    try
    {
        const char* cargs[] = {"TILED=YES", "COMPRESS=LZW", 0};
        drvman = GetGDALDriverManager();
        driver = drvman->GetDriverByName("GTiff");
        dataset = driver->Create(tarFilename.toUtf8(), xsize, ysize, 1, GDT_Byte, (char**)cargs);

        if(dataset == nullptr)
        {
            throw tr("Failed to create target file.");
        }

        dataset->GetRasterBand(1)->SetColorTable(ct);
        dataset->GetRasterBand(1)->SetNoDataValue(ct->GetColorEntryCount());
        dataset->SetProjection(dsSrc->GetProjectionRef());

        double adfGeoTransform[6] = {0};
        dsSrc->GetGeoTransform(adfGeoTransform);
        dataset->SetGeoTransform(adfGeoTransform);

        printStdoutQString(tr("Dither source file to target file"));
        int res = GDALDitherRGB2PCT(dsSrc->GetRasterBand(1),
                                    dsSrc->GetRasterBand(2),
                                    dsSrc->GetRasterBand(3),
                                    dataset->GetRasterBand(1),
                                    ct,
                                    GDALTermProgress,
                                    0
                                    );
        if(res != CE_None)
        {
            throw tr("Failed to dither file.");
        }

        if(dsSrc->GetRasterCount() == 3)
        {
            return;
        }

        GDALRasterBand* alpha = dsSrc->GetRasterBand(4);
        GDALRasterBand* band = dataset->GetRasterBand(1);

        QByteArray buffer1(xsize, 0);
        QByteArray buffer2(xsize, 0);

        quint8 nodata = band->GetNoDataValue();
        printStdoutQString(tr("Apply alpha channel as no data value to target file"));
        for(int y = 0; y < ysize; y++)
        {
            GDALTermProgress(double(xsize * y) / (xsize * ysize), 0, 0);
            res = alpha->RasterIO(GF_Read, 0, y, xsize, 1, buffer1.data(), xsize, 1, GDT_Byte, 0, 0);
            if(res != CE_None)
            {
                throw tr("Failed to read from alpha channel.");
            }

            res = band->RasterIO(GF_Read, 0, y, xsize, 1, buffer2.data(), xsize, 1, GDT_Byte, 0, 0);
            if(res != CE_None)
            {
                throw tr("Failed to read from target file.");
            }

            for(int x = 0; x < xsize; x++)
            {
                if(buffer1[x] != char(0xFF))
                {
                    buffer2[x] = nodata;
                }
            }

            res = band->RasterIO(GF_Write, 0, y, xsize, 1, buffer2.data(), xsize, 1, GDT_Byte, 0, 0);
            if(res != CE_None)
            {
                throw tr("Failed to write to target file.");
            }
        }
        GDALTermProgress(1.0, 0, 0);
    }
    catch(const QString& msg)
    {
        GDALClose(dataset);
        throw msg;
    }

    dataset->FlushCache();
    GDALClose(dataset);
}
