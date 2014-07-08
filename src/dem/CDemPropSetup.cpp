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

#include "dem/CDemPropSetup.h"
#include "dem/IDem.h"
#include "dem/CDemDraw.h"
#include "units/IUnit.h"

#include <QtWidgets>
QPointF CDemPropSetup::scale;

CDemPropSetup::CDemPropSetup(IDem * demfile, CDemDraw *dem)
    : IDemProp(demfile, dem)
{
    setupUi(this);

    slotPropertiesChanged();

    connect(sliderOpacity, SIGNAL(valueChanged(int)), demfile, SLOT(slotSetOpacity(int)));
    connect(sliderOpacity, SIGNAL(valueChanged(int)), dem, SLOT(emitSigCanvasUpdate()));
    connect(dem, SIGNAL(sigScaleChanged(QPointF)), this, SLOT(slotScaleChanged(QPointF)));
    connect(toolSetMinScale, SIGNAL(toggled(bool)), this, SLOT(slotSetMinScale(bool)));
    connect(toolSetMaxScale, SIGNAL(toggled(bool)), this, SLOT(slotSetMaxScale(bool)));
    connect(checkHillshading, SIGNAL(toggled(bool)), demfile, SLOT(slotSetHillshading(bool)));
    connect(checkHillshading, SIGNAL(clicked()), dem, SLOT(emitSigCanvasUpdate()));
}

CDemPropSetup::~CDemPropSetup()
{

}

void CDemPropSetup::resizeEvent(QResizeEvent * e)
{
    IDemProp::resizeEvent(e);
    updateScaleLabel();
}

void CDemPropSetup::slotPropertiesChanged()
{
    sliderOpacity->blockSignals(true);
    toolSetMaxScale->blockSignals(true);
    toolSetMinScale->blockSignals(true);
    checkHillshading->blockSignals(true);

    sliderOpacity->setValue(demfile->getOpacity());
    qreal minScale = demfile->getMinScale();
    toolSetMinScale->setChecked(minScale != NOFLOAT);
    qreal maxScale = demfile->getMaxScale();
    toolSetMaxScale->setChecked(maxScale != NOFLOAT);

    updateScaleLabel();

    checkHillshading->setChecked(demfile->doHillshading());

    sliderOpacity->blockSignals(false);
    toolSetMaxScale->blockSignals(false);
    toolSetMinScale->blockSignals(false);
    checkHillshading->blockSignals(false);
}

void CDemPropSetup::slotScaleChanged(const QPointF& s)
{
    scale = s;
    slotPropertiesChanged();
}

void CDemPropSetup::slotSetMinScale(bool checked)
{
    demfile->setMinScale(checked ? scale.x() : NOFLOAT);
    slotPropertiesChanged();
}

void CDemPropSetup::slotSetMaxScale(bool checked)
{
    demfile->setMaxScale(checked ? scale.x() : NOFLOAT);
    slotPropertiesChanged();
}


#define BAR_HEIGHT 6
#define HOR_MARGIN 3

void CDemPropSetup::updateScaleLabel()
{
    int w = labelScale->width();
    int h = labelScale->height();

    QPixmap pix(w,h);
    if(pix.isNull())
    {
        return;
    }

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
    qreal minScale = demfile->getMinScale();
    qreal maxScale = demfile->getMaxScale();
    if((minScale != NOFLOAT) || (maxScale != NOFLOAT))
    {
        int x1Range = minScale != NOFLOAT ? HOR_MARGIN + qRound(bar.width() * (1 + log10(minScale)) / 5) : bar.left();
        int x2Range = maxScale != NOFLOAT ? HOR_MARGIN + qRound(bar.width() * (1 + log10(maxScale)) / 5) : bar.right();
        int yRange  = yBar;

        QRect range(x1Range, yRange, x2Range - x1Range, BAR_HEIGHT);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::darkGreen);
        p.drawRect(range);

    }

    // draw scale indicator
    int xInd  = HOR_MARGIN + qRound(bar.width() * (1 + log10(scale.x())) / 5) - 3;
    int yInd  = yBar - 1;

    QRect ind(xInd, yInd, 5, BAR_HEIGHT + 2);
    p.setPen(Qt::darkBlue);
    p.setBrush(Qt::NoBrush);
    p.drawRect(ind);

    labelScale->setPixmap(pix);
}
