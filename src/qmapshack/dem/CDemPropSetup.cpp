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
#include "helpers/Signals.h"
#include "units/IUnit.h"

#include <QtWidgets>
QPointF CDemPropSetup::scale;

CDemPropSetup::CDemPropSetup(IDem * demfile, CDemDraw *dem)
    : IDemProp(demfile, dem)
{
    setupUi(this);

    slopeSpins[0] = spinSlope0;
    slopeSpins[1] = spinSlope1;
    slopeSpins[2] = spinSlope2;
    slopeSpins[3] = spinSlope3;
    slopeSpins[4] = spinSlope4;

    CDemPropSetup::slotPropertiesChanged();

    connect(sliderOpacity,         &QSlider::valueChanged,      demfile, &IDem::slotSetOpacity);
    connect(sliderOpacity,         &QSlider::valueChanged,      dem,     &CDemDraw::emitSigCanvasUpdate);
    connect(dem,                   &CDemDraw::sigScaleChanged,  this,    &CDemPropSetup::slotScaleChanged);
    connect(toolSetMinScale,       &QToolButton::toggled,       this,    &CDemPropSetup::slotSetMinScale);
    connect(toolSetMaxScale,       &QToolButton::toggled,       this,    &CDemPropSetup::slotSetMaxScale);

    connect(checkHillshading,      &QCheckBox::toggled,         demfile, &IDem::slotSetHillshading);
    connect(checkHillshading,      &QCheckBox::clicked,         dem,     &CDemDraw::emitSigCanvasUpdate);
    connect(sliderHillshading,     &QSlider::valueChanged,      demfile, &IDem::slotSetFactorHillshade);
    connect(sliderHillshading,     &QSlider::valueChanged,      dem,     &CDemDraw::emitSigCanvasUpdate);

    connect(checkSlopeColor,       &QCheckBox::toggled,         demfile, &IDem::slotSetSlopeColor);
    connect(checkSlopeColor,       &QCheckBox::clicked,         dem,     &CDemDraw::emitSigCanvasUpdate);

    // elevation color
    //
    connect(checkElevationLimit,   &QCheckBox::toggled,         demfile, &IDem::slotSetElevationLimit);
    connect(checkElevationLimit,   &QCheckBox::clicked,         dem,     &CDemDraw::emitSigCanvasUpdate);
    connect(spinBoxElevationLimit, QOverload<int>::of(&QSpinBox::valueChanged), this, &CDemPropSetup::slotElevationAfterEdit);

    for(size_t i = 0; i < SLOPE_LEVELS; i++)
    {
        slopeSpins[i]->setProperty("level", (uint) i);
        connect(slopeSpins[i], &CTinySpinBox::editingFinished,    this, &CDemPropSetup::slotSlopeValiddateAfterEdit);
        connect(slopeSpins[i], &CTinySpinBox::valueChangedByStep, this, &CDemPropSetup::slotSlopeValiddateAfterEdit);
    }


    for(size_t i = 0; i < IDem::slopePresetCount; i++)
    {
        comboGrades->addItem(IDem::slopePresets[i].name);
    }
    comboGrades->addItem("custom");
    connect(comboGrades, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDemPropSetup::slotGradeIndex);


    const QVector<QRgb>& colortable = demfile->getSlopeColorTable();
    QPixmap pixmap(20, 10);
    pixmap.fill(colortable[5]);
    labelColor5->setPixmap(pixmap);
    pixmap.fill(colortable[4]);
    labelColor4->setPixmap(pixmap);
    pixmap.fill(colortable[3]);
    labelColor3->setPixmap(pixmap);
    pixmap.fill(colortable[2]);
    labelColor2->setPixmap(pixmap);
    pixmap.fill(colortable[1]);
    labelColor1->setPixmap(pixmap);
}

CDemPropSetup::~CDemPropSetup()
{
}


void CDemPropSetup::slotPropertiesChanged()
{
    X______________BlockAllSignals______________X(this);

    sliderOpacity->setValue(demfile->getOpacity());

    toolSetMinScale->setChecked( demfile->getMinScale() != NOFLOAT );
    toolSetMaxScale->setChecked( demfile->getMaxScale() != NOFLOAT );

    labelScale->setValue(demfile->getMinScale(), scale.x(), demfile->getMaxScale());

    checkHillshading->setChecked(demfile->doHillshading());
    sliderHillshading->setValue(demfile->getFactorHillshading());
    checkSlopeColor->setChecked(demfile->doSlopeColor());

    bool spinsReadonly = true;
    // calculate the index of the element, that should be selected in comboGrades
    // -1 indicates the `custom` entry (this allows adding DEM presets migration
    // of configuration during update
    int expectedComboGradesIndex = demfile->getSlopeStepTableIndex();
    if(-1 == expectedComboGradesIndex)
    {
        spinsReadonly = false;
        expectedComboGradesIndex = comboGrades->count() - 1;
    }
    comboGrades->setCurrentIndex(expectedComboGradesIndex);

    for(size_t i = 0; i < SLOPE_LEVELS; i++)
    {
        slopeSpins[i]->setReadOnly(spinsReadonly);
        slopeSpins[i]->setValue(demfile->getCurrentSlopeStepTable()[i]);
    }

    checkElevationLimit->setChecked(demfile->doElevationLimit());
    spinBoxElevationLimit->setValue(demfile->getElevationLimit());
    spinBoxElevationLimit->setSuffix(IUnit::self().elevationUnit);

    dem->emitSigCanvasUpdate();

    X_____________UnBlockAllSignals_____________X(this);
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

void CDemPropSetup::slotSlopeChanged(int val)
{
    uint level = QObject::sender()->property("level").toUInt();
    demfile->setSlopeStepTableCustomValue(level, val);
}

void CDemPropSetup::slotSlopeValiddateAfterEdit()
{
    uint level = QObject::sender()->property("level").toUInt();
    demfile->setSlopeStepTableCustomValue(level, slopeSpins[level]->value());

    if(comboGrades->count() - 1 == comboGrades->currentIndex())
    {
        // ensure the levels below/above the current level are
        // less/greater than the current level

        for(uint l = level; l < SLOPE_LEVELS - 1; l++)
        {
            int val = slopeSpins[l]->value();
            if(slopeSpins[l + 1]->value() <= val)
            {
                slopeSpins[l + 1]->setValue(val + 1);
                demfile->setSlopeStepTableCustomValue(l + 1, val + 1);
            }
        }
        for(uint l = level; l >= 1; l--)
        {
            int val = slopeSpins[l]->value();
            if(slopeSpins[l - 1]->value() >= val)
            {
                slopeSpins[l - 1]->setValue(val - 1);
                demfile->setSlopeStepTableCustomValue(l - 1, val - 1);
            }
        }
    }

    dem->emitSigCanvasUpdate();
}

void CDemPropSetup::slotGradeIndex(int idx)
{
    // enable the spins if the selected entry is `custom`
    demfile->setSlopeStepTable(idx == IDem::slopePresetCount ? -1 : idx);
    slotPropertiesChanged();
}

void CDemPropSetup::slotElevationAfterEdit()
{
    demfile->setElevationLimit(spinBoxElevationLimit->value());

    dem->emitSigCanvasUpdate();
}

