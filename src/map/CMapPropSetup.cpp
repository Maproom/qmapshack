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

#include "map/CMapPropSetup.h"
#include "map/IMap.h"
#include "map/CMap.h"
#include "units/IUnit.h"

#include <QtWidgets>

CMapPropSetup::CMapPropSetup(IMap * mapfile, CMap *map)
    : IMapPropSetup(mapfile, map)
{
    setupUi(this);

    slotPropertiesChanged();

    connect(sliderOpacity, SIGNAL(valueChanged(int)), mapfile, SLOT(slotSetOpacity(int)));
    connect(sliderOpacity, SIGNAL(valueChanged(int)), map, SLOT(emitSigCanvasUpdate()));
    connect(map, SIGNAL(sigScaleChanged(QPointF)), this, SLOT(slotScaleChanged(QPointF)));
    connect(toolSetMinScale, SIGNAL(toggled(bool)), this, SLOT(slotSetMinScale(bool)));
    connect(toolSetMaxScale, SIGNAL(toggled(bool)), this, SLOT(slotSetMaxScale(bool)));
}

CMapPropSetup::~CMapPropSetup()
{

}

void CMapPropSetup::slotPropertiesChanged()
{
    sliderOpacity->setValue(mapfile->getOpacity());
    qreal minScale = mapfile->getMinScale();
    toolSetMinScale->setChecked(minScale != NOFLOAT);
    qreal maxScale = mapfile->getMaxScale();
    toolSetMaxScale->setChecked(maxScale != NOFLOAT);
}

void CMapPropSetup::slotScaleChanged(const QPointF& s)
{
    scale = s;

    qreal minScale = mapfile->getMinScale();
    qreal maxScale = mapfile->getMaxScale();

    if((minScale != NOFLOAT) && (s.x() < minScale))
    {
        toolSetMinScale->setEnabled(false);
    }
    else
    {
        toolSetMinScale->setEnabled(true);
    }

    if((maxScale != NOFLOAT) && (s.x() > maxScale))
    {
        toolSetMaxScale->setEnabled(false);
    }
    else
    {
        toolSetMaxScale->setEnabled(true);
    }

    updateScaleLabel();
}

void CMapPropSetup::slotSetMinScale(bool checked)
{
//    if(checked)
//    {
//        mapfile->setMinScale(scale.x());
//    }
//    else
//    {
//        mapfile->setMinScale(NOFLOAT);
//    }
    updateScaleLabel();
}

void CMapPropSetup::slotSetMaxScale(bool checked)
{
//    if(checked)
//    {
//        mapfile->setMaxScale(scale.x());
//    }
//    else
//    {
//        mapfile->setMaxScale(NOFLOAT);
//    }
    updateScaleLabel();
}


#define BAR_HEIGHT 6
#define HOR_MARGIN 3

void CMapPropSetup::updateScaleLabel()
{
    int w = labelScale->width();
    int h = labelScale->height();

    QPixmap pix(w,h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    // draw bar background
    int xBar = HOR_MARGIN;
    int yBar = (h - BAR_HEIGHT) / 2;

    QRect bar(xBar, yBar, w-2*HOR_MARGIN, BAR_HEIGHT);
    p.setPen(Qt::darkBlue);
    p.setBrush(Qt::white);
    p.drawRect(bar);

    // draw current scale range
    qreal minScale = mapfile->getMinScale();
    qreal maxScale = mapfile->getMaxScale();
    if((minScale != NOFLOAT) || (maxScale != NOFLOAT))
    {
        int x1Range = minScale != NOFLOAT ? HOR_MARGIN + qRound(bar.width() * (1 + log10(minScale)) / 5) - 2 : bar.left();
        int x2Range = maxScale != NOFLOAT ? HOR_MARGIN + qRound(bar.width() * (1 + log10(maxScale)) / 5) - 2 : bar.right();
        int yRange  = yBar;

        QRect range(x1Range, yRange, x2Range - x1Range, BAR_HEIGHT);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::red);
        p.drawRect(range);

    }

    // draw scale indicator
    int xInd  = HOR_MARGIN + qRound(bar.width() * (1 + log10(scale.x())) / 5) - 2;
    int yInd  = yBar - 1;

    QRect ind(xInd, yInd, 4, BAR_HEIGHT + 2);
    p.setPen(Qt::darkBlue);
    p.setBrush(Qt::darkBlue);
    p.drawRect(ind);



    labelScale->setPixmap(pix);
}
