/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CSettings.h"
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

    CMapPropSetup::slotPropertiesChanged();

    connect(sliderOpacity,       &QSlider::valueChanged,     mapfile, &IMap::slotSetOpacity);
    connect(sliderOpacity,       &QSlider::valueChanged,     map,     &CMapDraw::emitSigCanvasUpdate);
    connect(map,                 &CMapDraw::sigScaleChanged, this,    &CMapPropSetup::slotScaleChanged);
    connect(toolSetMinScale,     &QToolButton::toggled,      this,    &CMapPropSetup::slotSetMinScale);
    connect(toolSetMaxScale,     &QToolButton::toggled,      this,    &CMapPropSetup::slotSetMaxScale);

    connect(checkPolygons,       &QCheckBox::toggled,        mapfile, &IMap::slotSetShowPolygons);
    connect(checkPolylines,      &QCheckBox::toggled,        mapfile, &IMap::slotSetShowPolylines);
    connect(checkPoints,         &QCheckBox::toggled,        mapfile, &IMap::slotSetShowPOIs);
    connect(spinAdjustDetails,   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), mapfile, &IMap::slotSetAdjustDetailLevel);
    connect(checkPolygons,       &QCheckBox::clicked,        map,     &CMapDraw::emitSigCanvasUpdate);
    connect(checkPolylines,      &QCheckBox::clicked,        map,     &CMapDraw::emitSigCanvasUpdate);
    connect(checkPoints,         &QCheckBox::clicked,        map,     &CMapDraw::emitSigCanvasUpdate);
    connect(spinAdjustDetails,   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), map, &CMapDraw::emitSigCanvasUpdate);

    connect(spinCacheSize,       static_cast<void (QSpinBox::*)(int) >(&QSpinBox::valueChanged), mapfile, &IMap::slotSetCacheSize);
    connect(spinCacheExpiration, static_cast<void (QSpinBox::*)(int) >(&QSpinBox::valueChanged), mapfile, &IMap::slotSetCacheExpiration);

    connect(toolOpenTypFile,    &QToolButton::pressed,      this,      &CMapPropSetup::slotLoadTypeFile);
    connect(toolClearTypFile,   &QToolButton::pressed,      this,      &CMapPropSetup::slotClearTypeFile);

    frameVectorItems->setVisible( mapfile->hasFeatureVectorItems() );
    frameTileCache->setVisible( mapfile->hasFeatureTileCache() );

    if(mapfile->hasFeatureLayers())
    {
        frameLayers->show();
        mapfile->getLayers(*listLayers);
    }
    else
    {
        frameLayers->hide();
    }

    frameTypFile->setVisible(mapfile->hasFeatureTypFile());
}

CMapPropSetup::~CMapPropSetup()
{
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

    labelScale->setValue(mapfile->getMinScale(), scale.x(), mapfile->getMaxScale());

    // vector maps properties
    checkPolygons->setChecked(mapfile->getShowPolygons());
    checkPolylines->setChecked(mapfile->getShowPolylines());
    checkPoints->setChecked(mapfile->getShowPOIs());
    spinAdjustDetails->setValue(mapfile->getAdjustDetailLevel());

    // streaming map properties
    QString lbl = mapfile->getCachePath();
    labelCachePath->setText(lbl.size() < 20 ? lbl : "..." + lbl.right(17));
    labelCachePath->setToolTip(lbl);
    spinCacheSize->setValue(mapfile->getCacheSize());
    spinCacheExpiration->setValue(mapfile->getCacheExpiration());

    // type file
    QFileInfo fi(mapfile->getTypeFile());
    labelTypeFile->setText(fi.completeBaseName());
    labelTypeFile->setToolTip(fi.absoluteFilePath());
    toolClearTypFile->setEnabled(!labelTypeFile->text().isEmpty());

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




void CMapPropSetup::slotLoadTypeFile()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastTypePath", QDir::homePath()).toString();
    QString filename = QFileDialog::getOpenFileName(this, tr("Select type file..."), path, "Garmin type file (*.typ)");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    cfg.setValue("Paths/lastTypePath", fi.absolutePath());

    mapfile->slotSetTypeFile(filename);
    slotPropertiesChanged();
}

void CMapPropSetup::slotClearTypeFile()
{
    mapfile->slotSetTypeFile("");
    slotPropertiesChanged();
}
