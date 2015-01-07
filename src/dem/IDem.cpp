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

#include "dem/IDem.h"
#include "dem/CDemDraw.h"
#include "dem/CDemPropSetup.h"

#include <QtWidgets>

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

const qreal IDem::tblGrade[5][6] =
{
     {0,0,0,0,0,0}
    ,{0.0,27.0,31.0,34.0,39.0,50.0}
    ,{0.0,27.0,30.0,32.0,35.0,39.0}
    ,{0.0,27.0,29.0,30.0,31.0,34.0}
    ,{0.0,23.0,25.0,27.0,28.0,30.0}
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
    , gradeSlopeColor(1)
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

    cfg.setValue("doHillshading",bHillshading);
    cfg.setValue("factorHillshading", factorHillshading);
    cfg.setValue("doSlopeColor",bSlopeColor);
    cfg.setValue("gradeSlopeColor", gradeSlopeColor);
}

void IDem::loadConfig(QSettings& cfg)
{
    IDrawObject::loadConfig(cfg);

    bHillshading = cfg.value("doHillshading",bHillshading).toBool();
    factorHillshading = cfg.value("factorHillshading", factorHillshading).toFloat();
    bSlopeColor = cfg.value("doSlopeColor", bSlopeColor).toBool();
    gradeSlopeColor = cfg.value("gradeSlopeColor", gradeSlopeColor).toInt();
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
        factorHillshading = - 1.0/f;
    }
    else
    {
        factorHillshading = f;
    }
}

void IDem::slotSetGradeSlopeColor(int g)
{
    /// @todo check range
    gradeSlopeColor = g;
}

int IDem::getFactorHillshading()
{
    if(factorHillshading == 1.0)
    {
        return 0;
    }
    else if(factorHillshading < 1)
    {
        return - 1.0/factorHillshading;
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
#define SIN_ALT         (sin(45*DEG_TO_RAD))
#define ZFACT_COS_ALT   (ZFACT*cos(45*DEG_TO_RAD))
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
            aspect      = atan2(dy, dx);
            xx_plus_yy  = dx * dx + dy * dy;
            cang        = (SIN_ALT - ZFACT_COS_ALT * sqrt(xx_plus_yy) * sin(aspect - AZ)) / sqrt(1+ZFACT_BY_ZFACT*xx_plus_yy);

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
            slope =  atan(sqrt(k) / (8 * 1.0)) * 180.0 / M_PI;

            if(slope > tblGrade[gradeSlopeColor][5])
            {
                img.setPixel(n - 1, m - 1, 5);
            }
            else if(slope > tblGrade[gradeSlopeColor][4])
            {
                img.setPixel(n - 1, m - 1, 4);
            }
            else if(slope > tblGrade[gradeSlopeColor][3])
            {
                img.setPixel(n - 1, m - 1, 3);
            }
            else if(slope > tblGrade[gradeSlopeColor][2])
            {
                img.setPixel(n - 1, m - 1, 2);
            }
            else if(slope > tblGrade[gradeSlopeColor][1])
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
    qreal w    = ceil( sqrt(dx1*dx1 + dy1*dy1));
    qreal h    = ceil( sqrt(dx2*dx2 + dy2*dy2));

    // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
    qreal a = atan(dy1/dx1) * RAD_TO_DEG;

    // finally translate, scale, rotate and draw tile
    p.save();
    p.translate(l[0]);
    p.scale(w/img.width(), h/img.height());
    p.rotate(a);
    p.drawImage(0,0,img);
    p.restore();

}
