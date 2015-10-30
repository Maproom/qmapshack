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

#include "dem/CDemDraw.h"
#include "dem/CDemPropSetup.h"
#include "dem/IDem.h"


#include <QtWidgets>

inline qint16 getValue(QVector<qint16>& data, int x, int y, int dx)
{
    return data[x + y * dx];
}

inline void fillWindow(QVector<qint16>& data, int x, int y, int dx, qint16 * w)
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

const struct SlopePresets IDem::slopePresets[7]
{
    /* http://www.alpenverein.de/bergsport/sicherheit/skitouren-schneeschuh-sicher-im-schnee/dav-snowcard_aid_10619.html */
    { "Grade 1 (DAV Snowcard)", {27.0, 31.0, 34.0, 39.0, 50.0}
    },
    { "Grade 2 (DAV Snowcard)", {27.0, 30.0, 32.0, 35.0, 39.0}
    },
    { "Grade 3 (DAV Snowcard)", {27.0, 29.0, 30.0, 31.0, 34.0}
    },
    { "Grade 4 (DAV Snowcard)", {23.0, 25.0, 27.0, 28.0, 30.0}
    },

    { "level country",        { 3.0,  6.0,  8.0, 12.0, 15.0}
    },
    { "secondary mountain",   { 4.0,  7.0, 10.0, 15.0, 20.0}
    },
    { "lofty mountain",       {10.0, 15.0, 20.0, 30.0, 50.0}
    }
};

IDem::IDem(CDemDraw *parent)
    : IDrawObject(parent)
    , dem(parent)
    , pjsrc(0)
    , isActivated(false)
    , hasNoData(0)
    , noData(0)
    , bHillshading(false)
    , factorHillshading(1.0)
    , bSlopeColor(false)
    , gradeSlopeColor(0)
{
    slotSetOpacity(50);
    pjtar = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");

    graytable.resize(256);
    for(int i = 0; i < 255; i++)
    {
        graytable[i] = qRgba(i,i,i,255);
    }
    graytable[255] = qRgba(0,0,0,0);

    slopetable << qRgba(0,0,0,0);
    slopetable << qRgba(0,128,0,100);
    slopetable << qRgba(0,255,0,100);
    slopetable << qRgba(255,255,0,100);
    slopetable << qRgba(255,128,0,100);
    slopetable << qRgba(255,0,0,100);
}

IDem::~IDem()
{
    pj_free(pjtar);
    pj_free(pjsrc);
}

void IDem::saveConfig(QSettings& cfg)
{
    IDrawObject::saveConfig(cfg);

    cfg.setValue("doHillshading",     bHillshading);
    cfg.setValue("factorHillshading", factorHillshading);
    cfg.setValue("doSlopeColor",      bSlopeColor);

    cfg.setValue("gradeSlopeColor",   gradeSlopeColor);
    cfg.setValue("slopeCustomValue0", slopeCustomStepTable[0]);
    cfg.setValue("slopeCustomValue1", slopeCustomStepTable[1]);
    cfg.setValue("slopeCustomValue2", slopeCustomStepTable[2]);
    cfg.setValue("slopeCustomValue3", slopeCustomStepTable[3]);
    cfg.setValue("slopeCustomValue4", slopeCustomStepTable[4]);
}

void IDem::loadConfig(QSettings& cfg)
{
    IDrawObject::loadConfig(cfg);

    bHillshading      = cfg.value("doHillshading",     bHillshading     ).toBool();
    factorHillshading = cfg.value("factorHillshading", factorHillshading).toFloat();
    bSlopeColor       = cfg.value("doSlopeColor",      bSlopeColor      ).toBool();
    gradeSlopeColor   = cfg.value("gradeSlopeColor",   gradeSlopeColor  ).toInt();

    slopeCustomStepTable[0] = cfg.value("slopeCustomValue0",  5.).toFloat();
    slopeCustomStepTable[1] = cfg.value("slopeCustomValue1", 10.).toFloat();
    slopeCustomStepTable[2] = cfg.value("slopeCustomValue2", 15.).toFloat();
    slopeCustomStepTable[3] = cfg.value("slopeCustomValue3", 20.).toFloat();
    slopeCustomStepTable[4] = cfg.value("slopeCustomValue4", 25.).toFloat();
}

IDemProp * IDem::getSetup()
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
        factorHillshading = -1.0/f;
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
        return -1.0/factorHillshading;
    }
    else
    {
        return factorHillshading;
    }
}

void IDem::hillshading(QVector<qint16>& data, qreal w, qreal h, QImage& img)
{
    int wp2 = w + 2;
    qint16 win[9];
    qreal dx, dy, aspect, xx_plus_yy, cang;

#define ZFACT           0.125
#define ZFACT_BY_ZFACT  (ZFACT*ZFACT)
#define SIN_ALT         (qSin(45*DEG_TO_RAD))
#define ZFACT_COS_ALT   (ZFACT*qCos(45*DEG_TO_RAD))
#define AZ              (315 * DEG_TO_RAD)
    for(int m = 1; m <= h; m++)
    {
        for(int n = 1; n <= w; n++)
        {
            fillWindow(data, n, m, wp2, win);

            if(hasNoData && win[4] == noData)
            {
                img.setPixel(n - 1, m - 1, 255);
                continue;
            }

            dx          = ((win[0] + win[3] + win[3] + win[6]) - (win[2] + win[5] + win[5] + win[8])) / (xscale*factorHillshading);
            dy          = ((win[6] + win[7] + win[7] + win[8]) - (win[0] + win[1] + win[1] + win[2])) / (yscale*factorHillshading);
            aspect      = qAtan2(dy, dx);
            xx_plus_yy  = dx * dx + dy * dy;
            cang        = (SIN_ALT - ZFACT_COS_ALT * qSqrt(xx_plus_yy) * qSin(aspect - AZ)) / qSqrt(1+ZFACT_BY_ZFACT*xx_plus_yy);

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
}

void IDem::slopecolor(QVector<qint16>& data, qreal w, qreal h, QImage &img)
{
    int wp2 = w + 2;
    qint16 win[9];
    qreal dx, dy, k, slope;

    for(int m = 1; m <= h; m++)
    {
        for(int n = 1; n <= w; n++)
        {
            fillWindow(data, n, m, wp2, win);
            dx  = ((win[0] + win[3] + win[3] + win[6]) - (win[2] + win[5] + win[5] + win[8])) / (xscale);
            dy  = ((win[6] + win[7] + win[7] + win[8]) - (win[0] + win[1] + win[1] + win[2])) / (yscale);
            k   = (dx * dx + dy * dy);
            slope =  qAtan(qSqrt(k) / (8 * 1.0)) * 180.0 / M_PI;

            const qreal *currentSlopeStepTable = getCurrentSlopeStepTable();

            if(slope > currentSlopeStepTable[4])
            {
                img.setPixel(n - 1, m - 1, 5);
            }
            else if(slope > currentSlopeStepTable[3])
            {
                img.setPixel(n - 1, m - 1, 4);
            }
            else if(slope > currentSlopeStepTable[2])
            {
                img.setPixel(n - 1, m - 1, 3);
            }
            else if(slope > currentSlopeStepTable[1])
            {
                img.setPixel(n - 1, m - 1, 2);
            }
            else if(slope > currentSlopeStepTable[0])
            {
                img.setPixel(n - 1, m - 1, 1);
            }
            else
            {
                img.setPixel(n - 1, m - 1, 0);
            }
        }
    }
}


void IDem::drawTile(QImage& img, QPolygonF& l, QPainter& p)
{
    dem->convertRad2Px(l);

    // adjust the tiles width and height to fit the buffer's scale
    qreal dx1   = l[0].x() - l[1].x();
    qreal dy1   = l[0].y() - l[1].y();
    qreal dx2   = l[0].x() - l[3].x();
    qreal dy2   = l[0].y() - l[3].y();
    qreal w    = qCeil( qSqrt(dx1*dx1 + dy1*dy1));
    qreal h    = qCeil( qSqrt(dx2*dx2 + dy2*dy2));

    // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
    qreal a = qAtan(dy1/dx1) * RAD_TO_DEG;

    // finally translate, scale, rotate and draw tile
    p.save();
    p.translate(l[0]);
    p.scale(w/img.width(), h/img.height());
    p.rotate(a);
    p.drawImage(0,0,img);
    p.restore();
}
