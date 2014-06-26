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

#include <QtWidgets>

CMapPropSetup::CMapPropSetup(IMap * mapfile, CMap *map)
    : mapfile(mapfile)
    , map(map)
{
    setupUi(this);

    sliderOpacity->setValue(mapfile->getOpacity());
    connect(sliderOpacity, SIGNAL(valueChanged(int)), mapfile, SLOT(slotSetOpacity(int)));
    connect(sliderOpacity, SIGNAL(valueChanged(int)), map, SLOT(emitSigCanvasUpdate()));
    connect(map, SIGNAL(sigScaleChanged(QPointF)), this, SLOT(slotScaleChanged(QPointF)));
}

CMapPropSetup::~CMapPropSetup()
{

}

#define BAR_HEIGHT 6
#define HOR_MARGIN 3
void CMapPropSetup::slotScaleChanged(const QPointF& scale)
{
    int w = labelScale->width();
    int h = labelScale->height();

    QPixmap pix(w,h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);

    int x = HOR_MARGIN;
    int y = (h - BAR_HEIGHT) / 2;

    QRect bar(x,y, w-2*HOR_MARGIN, BAR_HEIGHT);
    p.setPen(Qt::darkBlue);
    p.setBrush(Qt::white);
    p.drawRect(bar);

    w = bar.width();

    x  = HOR_MARGIN + qRound(w * (1+log10(scale.x())) / 5) - 2;
    y -= 1;

    QRect indicator(x,y,4, BAR_HEIGHT + 2);

    p.setPen(Qt::darkBlue);
    p.setBrush(Qt::darkBlue);
    p.drawRect(indicator);

    labelScale->setPixmap(pix);
}
