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

#include "helpers/Signals.h"
#include "map/CMapDraw.h"
#include "map/CMapPropSetup.h"
#include "map/IMap.h"
#include "units/IUnit.h"

#include <QtWidgets>
QPointF CMapPropSetup::scale;

CMapPropSetup::CMapPropSetup(IMap * mapfile, CMapDraw *map)
    : IMapProp(mapfile, map)
{
    setupUi(this);

    slotPropertiesChanged();

    connect(sliderOpacity,       &QSlider::valueChanged,     mapfile, &IMap::slotSetOpacity);
    connect(sliderOpacity,       &QSlider::valueChanged,     map,     &CMapDraw::emitSigCanvasUpdate);
    connect(map,                 &CMapDraw::sigScaleChanged, this,    &CMapPropSetup::slotScaleChanged);
    connect(toolSetMinScale,     &QToolButton::toggled,      this,    &CMapPropSetup::slotSetMinScale);
    connect(toolSetMaxScale,     &QToolButton::toggled,      this,    &CMapPropSetup::slotSetMaxScale);

    connect(checkPolygons,       &QCheckBox::toggled,        mapfile, &IMap::slotSetShowPolygons);
    connect(checkPolylines,      &QCheckBox::toggled,        mapfile, &IMap::slotSetShowPolylines);
    connect(checkPoints,         &QCheckBox::toggled,        mapfile, &IMap::slotSetShowPOIs);
    connect(checkPolygons,       &QCheckBox::clicked,        map,     &CMapDraw::emitSigCanvasUpdate);
    connect(checkPolylines,      &QCheckBox::clicked,        map,     &CMapDraw::emitSigCanvasUpdate);
    connect(checkPoints,         &QCheckBox::clicked,        map,     &CMapDraw::emitSigCanvasUpdate);

    connect(spinCacheSize,       static_cast<void (QSpinBox::*)(int) >(&QSpinBox::valueChanged), mapfile, &IMap::slotSetCacheSize);
    connect(spinCacheExpiration, static_cast<void (QSpinBox::*)(int) >(&QSpinBox::valueChanged), mapfile, &IMap::slotSetCacheExpiration);

    if(mapfile->hasFeatureVectorItems())
    {
        frameVectorItems->show();
    }
    else
    {
        frameVectorItems->hide();
    }

    if(mapfile->hasFeatureTileCache())
    {
        frameTileCache->show();
    }
    else
    {
        frameTileCache->hide();
    }

    if(mapfile->hasFeatureLayers())
    {
        frameLayers->show();
        mapfile->getLayers(*listLayers);
    }
    else
    {
        frameLayers->hide();
    }
}

CMapPropSetup::~CMapPropSetup()
{
}

void CMapPropSetup::resizeEvent(QResizeEvent * e) /* override */
{
    IMapProp::resizeEvent(e);
    updateScaleLabel();
}

void CMapPropSetup::slotPropertiesChanged() /* override */
{
    X______________BlockAllSignals______________X(this);
    // block all signals to avoid retrigger

    // opacity and visibility settings
    sliderOpacity->setValue(mapfile->getOpacity());
    qreal minScale = mapfile->getMinScale();
    toolSetMinScale->setChecked(minScale != NOFLOAT);
    qreal maxScale = mapfile->getMaxScale();
    toolSetMaxScale->setChecked(maxScale != NOFLOAT);
    updateScaleLabel();

    // vector maps properties
    checkPolygons->setChecked(mapfile->getShowPolygons());
    checkPolylines->setChecked(mapfile->getShowPolylines());
    checkPoints->setChecked(mapfile->getShowPOIs());

    // streaming map properties
    QString lbl = mapfile->getCachePath();
    labelCachePath->setText(lbl);
    labelCachePath->setToolTip(lbl);
    spinCacheSize->setValue(mapfile->getCacheSize());
    spinCacheExpiration->setValue(mapfile->getCacheExpiration());

    // unblock all signals
    X_____________UnBlockAllSignals_____________X(this);
}

void CMapPropSetup::slotScaleChanged(const QPointF& s)
{
    scale = s;
    slotPropertiesChanged();
}

void CMapPropSetup::slotSetMinScale(bool checked)
{
    mapfile->setMinScale(checked ? scale.x() : NOFLOAT);
    slotPropertiesChanged();
}

void CMapPropSetup::slotSetMaxScale(bool checked)
{
    mapfile->setMaxScale(checked ? scale.x() : NOFLOAT);
    slotPropertiesChanged();
}


#define BAR_HEIGHT 6
#define HOR_MARGIN 3

void CMapPropSetup::updateScaleLabel()
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
    qreal minScale = mapfile->getMinScale();
    qreal maxScale = mapfile->getMaxScale();
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
