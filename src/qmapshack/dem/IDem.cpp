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

#include "dem/CDemDraw.h"
#include "dem/CDemPropSetup.h"
#include "dem/IDem.h"


#include <QtWidgets>

inline qint16 getValue(QVector<qint16>& data, int x, int y, int dx)
{
    return data[x + y * dx];
}

inline void fillWindow(QVector<qint16>& data, int x, int y, int dx, qint16* w)
{
    w[0] = getValue(data, x - 1, y - 1, dx);
    w[1] = getValue(data, x, y - 1, dx);
    w[2] = getValue(data, x + 1, y - 1, dx);
    w[3] = getValue(data, x - 1, y, dx);
    w[4] = getValue(data, x, y, dx);
    w[5] = getValue(data, x + 1, y, dx);
    w[6] = getValue(data, x - 1, y + 1, dx);
    w[7] = getValue(data, x, y + 1, dx);
    w[8] = getValue(data, x + 1, y + 1, dx);
}

inline void fillWindow4x4(QVector<qint16>& data, qreal x, qreal y, int dx, qint16* w)
{
    x = qFloor(x);
    y = qFloor(y);

    w[0] = getValue(data, x - 1, y - 1, dx);
    w[1] = getValue(data, x, y - 1, dx);
    w[2] = getValue(data, x + 1, y - 1, dx);
    w[3] = getValue(data, x + 2, y - 1, dx);
    w[4] = getValue(data, x - 1, y, dx);
    w[5] = getValue(data, x, y, dx);
    w[6] = getValue(data, x + 1, y, dx);
    w[7] = getValue(data, x + 2, y, dx);
    w[8] = getValue(data, x - 1, y + 1, dx);
    w[9] = getValue(data, x, y + 1, dx);
    w[10] = getValue(data, x + 1, y + 1, dx);
    w[11] = getValue(data, x + 2, y + 1, dx);
    w[12] = getValue(data, x - 1, y + 2, dx);
    w[13] = getValue(data, x, y + 2, dx);
    w[14] = getValue(data, x + 1, y + 2, dx);
    w[15] = getValue(data, x + 2, y + 2, dx);
}

const struct SlopePresets IDem::slopePresets[7]
{
    /* http://www.alpenverein.de/bergsport/sicherheit/skitouren-schneeschuh-sicher-im-schnee/dav-snowcard_aid_10619.html */
    { "Grade 1 (DAV Snowcard)", {27.0, 31.0, 34.0, 39.0, 50.0}},
    { "Grade 2 (DAV Snowcard)", {27.0, 30.0, 32.0, 35.0, 39.0}},
    { "Grade 3 (DAV Snowcard)", {27.0, 29.0, 30.0, 31.0, 34.0}},
    { "Grade 4 (DAV Snowcard)", {23.0, 25.0, 27.0, 28.0, 30.0}},

    { "level country", { 3.0, 6.0, 8.0, 12.0, 15.0}},
    { "secondary mountain", { 4.0, 7.0, 10.0, 15.0, 20.0}},
    { "lofty mountain", {10.0, 15.0, 20.0, 30.0, 50.0}}
};

IDem::IDem(CDemDraw* parent)
    : IDrawObject(parent)
    , dem(parent)
{
    slotSetOpacity(17);

    graytable.resize(256);
    for(int i = 0; i < 255; i++)
    {
        graytable[i] = qRgba(i, i, i, 255);
    }
    graytable[255] = qRgba(0, 0, 0, 0);

    slopetable << qRgba(0, 0, 0, 0);
    slopetable << qRgba(0, 128, 0, 100);
    slopetable << qRgba(0, 255, 0, 100);
    slopetable << qRgba(255, 255, 0, 100);
    slopetable << qRgba(255, 128, 0, 100);
    slopetable << qRgba(255, 0, 0, 100);

    elevationtable << qRgba(0, 0, 0, 0); // transparent
    elevationtable << qRgba(255, 0, 255, 128); // magenta
}

IDem::~IDem()
{
}

void IDem::saveConfig(QSettings& cfg)
{
    IDrawObject::saveConfig(cfg);

    cfg.setValue("doHillshading", bHillshading);
    cfg.setValue("factorHillshading", factorHillshading);
    cfg.setValue("doSlopeColor", bSlopeColor);

    cfg.setValue("gradeSlopeColor", gradeSlopeColor);
    cfg.setValue("slopeCustomValue0", slopeCustomStepTable[0]);
    cfg.setValue("slopeCustomValue1", slopeCustomStepTable[1]);
    cfg.setValue("slopeCustomValue2", slopeCustomStepTable[2]);
    cfg.setValue("slopeCustomValue3", slopeCustomStepTable[3]);
    cfg.setValue("slopeCustomValue4", slopeCustomStepTable[4]);

    cfg.setValue("doElevationLimit", bElevationLimit);
    cfg.setValue("elevationValue", elevationValue);
}

void IDem::loadConfig(QSettings& cfg)
{
    IDrawObject::loadConfig(cfg);

    bHillshading = cfg.value("doHillshading", bHillshading     ).toBool();
    factorHillshading = cfg.value("factorHillshading", factorHillshading).toFloat();
    bSlopeColor = cfg.value("doSlopeColor", bSlopeColor      ).toBool();
    gradeSlopeColor = cfg.value("gradeSlopeColor", gradeSlopeColor  ).toInt();

    slopeCustomStepTable[0] = cfg.value("slopeCustomValue0", 5.).toFloat();
    slopeCustomStepTable[1] = cfg.value("slopeCustomValue1", 10.).toFloat();
    slopeCustomStepTable[2] = cfg.value("slopeCustomValue2", 15.).toFloat();
    slopeCustomStepTable[3] = cfg.value("slopeCustomValue3", 20.).toFloat();
    slopeCustomStepTable[4] = cfg.value("slopeCustomValue4", 25.).toFloat();

    bElevationLimit = cfg.value("doElevationLimit", bElevationLimit  ).toBool();
    elevationValue = cfg.value("elevationValue", 0).toInt();
}

IDemProp* IDem::getSetup()
{
    if(setup.isNull())
    {
        setup = new CDemPropSetup(this, dem);
    }

    return setup;
}

void IDem::slotSetFactorHillshade(int f)
{
    if(f == 0)
    {
        factorHillshading = 1.0;
    }
    else if(f < 0)
    {
        factorHillshading = -1.0 / f;
    }
    else
    {
        factorHillshading = f;
    }
}

void IDem::setSlopeStepTableCustomValue(int idx, int val)
{
    slopeCustomStepTable[idx] = (qreal) val;
}

void IDem::setElevationLimit(int val)
{
    elevationValue = val;
}

void IDem::setSlopeStepTable(int idx)
{
    gradeSlopeColor = idx;
    dem->emitSigCanvasUpdate();
}

const qreal* IDem::getCurrentSlopeStepTable()
{
    if(CUSTOM_SLOPE_COLORTABLE == gradeSlopeColor)
    {
        return slopeCustomStepTable;
    }
    else
    {
        return slopePresets[gradeSlopeColor].steps;
    }
}

int IDem::getFactorHillshading()
{
    if(factorHillshading == 1.0)
    {
        return 0;
    }
    else if(factorHillshading < 1)
    {
        return -1.0 / factorHillshading;
    }
    else
    {
        return factorHillshading;
    }
}

void IDem::hillshading(QVector<qint16>& data, qreal w, qreal h, QImage& img)
{
    int wp2 = w + 2;

#define ZFACT           0.125
#define ZFACT_BY_ZFACT  (ZFACT * ZFACT)
#define SIN_ALT         (qSin(45 * DEG_TO_RAD))
#define ZFACT_COS_ALT   (ZFACT * qCos(45 * DEG_TO_RAD))
#define AZ              (315 * DEG_TO_RAD)
    for(unsigned int m = 1; m <= h; m++)
    {
        unsigned char* scan = img.scanLine(m - 1);
        for(unsigned int n = 1; n <= w; n++)
        {
            qint16 win[eWinsize3x3];
            fillWindow(data, n, m, wp2, win);

            if(hasNoData && win[4] == noData)
            {
                scan[n - 1] = 255;
                continue;
            }

            qreal dx = ((win[0] + win[3] + win[3] + win[6]) - (win[2] + win[5] + win[5] + win[8])) / (xscale * factorHillshading);
            qreal dy = ((win[6] + win[7] + win[7] + win[8]) - (win[0] + win[1] + win[1] + win[2])) / (yscale * factorHillshading);
            qreal aspect = qAtan2(dy, dx);
            qreal xx_plus_yy = dx * dx + dy * dy;
            qreal cang = (SIN_ALT - ZFACT_COS_ALT * qSqrt(xx_plus_yy) * qSin(aspect - AZ)) / qSqrt(1 + ZFACT_BY_ZFACT * xx_plus_yy);

            if (cang <= 0.0)
            {
                cang = 1.0;
            }
            else
            {
                cang = 1.0 + (254.0 * cang);
            }

            scan[n - 1] = cang;
        }
    }
}

qreal IDem::slopeOfWindowInterp(qint16* win2, winsize_e size, qreal x, qreal y)
{
    for(int i = 0; i < size; i++)
    {
        if(hasNoData && win2[i] == noData)
        {
            return NOFLOAT;
        }
    }

    qreal win[eWinsize3x3];
    switch(size)
    {
    case eWinsize3x3:
        for(int i = 0; i < 9; i++)
        {
            win[i] = win2[i];
        }
        break;

    case eWinsize4x4:
        win[0] = win2[0] + x * (win2[1] - win2[0]) + y * (win2[4] - win2[0]) + x * y * (win2[0] - win2[1] - win2[4] + win2[5]);
        win[1] = win2[1] + x * (win2[2] - win2[1]) + y * (win2[5] - win2[1]) + x * y * (win2[1] - win2[2] - win2[5] + win2[6]);
        win[2] = win2[2] + x * (win2[3] - win2[2]) + y * (win2[6] - win2[2]) + x * y * (win2[2] - win2[3] - win2[6] + win2[7]);

        win[3] = win2[4] + x * (win2[5] - win2[4]) + y * (win2[8] - win2[4]) + x * y * (win2[4] - win2[5] - win2[8] + win2[9]);
        win[4] = win2[5] + x * (win2[6] - win2[5]) + y * (win2[9] - win2[5]) + x * y * (win2[5] - win2[6] - win2[9] + win2[10]);
        win[5] = win2[6] + x * (win2[7] - win2[6]) + y * (win2[10] - win2[6]) + x * y * (win2[6] - win2[7] - win2[10] + win2[11]);

        win[6] = win2[8] + x * (win2[9] - win2[8]) + y * (win2[12] - win2[8]) + x * y * (win2[8] - win2[9] - win2[12] + win2[13]);
        win[7] = win2[9] + x * (win2[10] - win2[9]) + y * (win2[13] - win2[9]) + x * y * (win2[9] - win2[10] - win2[13] + win2[14]);
        win[8] = win2[10] + x * (win2[11] - win2[10]) + y * (win2[14] - win2[10]) + x * y * (win2[10] - win2[11] - win2[14] + win2[15]);
        break;

    default:
        return NOFLOAT;
    }

    qreal dx = ((win[0] + win[3] + win[3] + win[6]) - (win[2] + win[5] + win[5] + win[8])) / (xscale);
    qreal dy = ((win[6] + win[7] + win[7] + win[8]) - (win[0] + win[1] + win[1] + win[2])) / (yscale);
    qreal k = dx * dx + dy * dy;
    qreal slope = qAtan(qSqrt(k) / (8 * 1.0)) * 180.0 / M_PI;

    return slope;
}

void IDem::slopecolor(QVector<qint16>& data, qreal w, qreal h, QImage& img)
{
    int wp2 = w + 2;

    for(unsigned int m = 1; m <= h; m++)
    {
        unsigned char* scan = img.scanLine(m - 1);
        for(unsigned int n = 1; n <= w; n++)
        {
            qint16 win[eWinsize3x3];
            fillWindow(data, n, m, wp2, win);
            qreal slope = slopeOfWindowInterp(win, eWinsize3x3, 0, 0);

            const qreal* currentSlopeStepTable = getCurrentSlopeStepTable();

            if(slope > currentSlopeStepTable[4])
            {
                scan[n - 1] = 5;
            }
            else if(slope > currentSlopeStepTable[3])
            {
                scan[n - 1] = 4;
            }
            else if(slope > currentSlopeStepTable[2])
            {
                scan[n - 1] = 3;
            }
            else if(slope > currentSlopeStepTable[1])
            {
                scan[n - 1] = 2;
            }
            else if(slope > currentSlopeStepTable[0])
            {
                scan[n - 1] = 1;
            }
            else
            {
                scan[n - 1] = 0;
            }
        }
    }
}

void IDem::elevationLimit(QVector<qint16>& data, qreal w, qreal h, QImage& img)
{
    int wp2 = w + 2;

    for(unsigned int m = 1; m <= h; m++)
    {
        unsigned char* scan = img.scanLine(m - 1);
        for(unsigned int n = 1; n <= w; n++)
        {
            qint16 win[eWinsize3x3];
            fillWindow(data, n, m, wp2, win);

            // get maximum of window (_not_ mean)
            //
            qreal meters = -2.0;
            for(unsigned int i = 0; i < eWinsize3x3; i++)
            {
                if(win[i] != noData && win[i] > meters)
                {
                    meters = win[i];
                }
            }

            qreal elevation; // elevation in the units set by the user
            QString unit; // result not used
            IUnit::self().meter2elevation(meters, elevation, unit);
            if(elevation >= getElevationLimit())
            {
                scan[n - 1] = 1;
            }
            else
            {
                scan[n - 1] = 0;
            }
        }
    }
}

void IDem::drawTile(QImage& img, QPolygonF& l, QPainter& p)
{
    drawTileLQ(img, l, p, *dem, proj);
}
