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
#include "units/IUnit.h"
#include "helpers/Signals.h"

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

    slotPropertiesChanged();

    connect(sliderOpacity,     &QSlider::valueChanged,          demfile, &IDem::slotSetOpacity);
    connect(sliderOpacity,     &QSlider::valueChanged,          dem,     &CDemDraw::emitSigCanvasUpdate);
    connect(dem,               &CDemDraw::sigScaleChanged,      this,    &CDemPropSetup::slotScaleChanged);
    connect(toolSetMinScale,   &QToolButton::toggled,           this,    &CDemPropSetup::slotSetMinScale);
    connect(toolSetMaxScale,   &QToolButton::toggled,           this,    &CDemPropSetup::slotSetMaxScale);

    connect(checkHillshading,  &QCheckBox::toggled,             demfile, &IDem::slotSetHillshading);
    connect(checkHillshading,  &QCheckBox::clicked,             dem,     &CDemDraw::emitSigCanvasUpdate);
    connect(sliderHillshading, &QSlider::valueChanged,          demfile, &IDem::slotSetFactorHillshade);
    connect(sliderHillshading, &QSlider::valueChanged,          dem,     &CDemDraw::emitSigCanvasUpdate);

    connect(checkSlopeColor,   &QCheckBox::toggled,             demfile, &IDem::slotSetSlopeColor);
    connect(checkSlopeColor,   &QCheckBox::clicked,             dem,     &CDemDraw::emitSigCanvasUpdate);


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

void CDemPropSetup::resizeEvent(QResizeEvent * e)
{
    IDemProp::resizeEvent(e);
    updateScaleLabel();
}

void CDemPropSetup::slotPropertiesChanged()
{
    X______________BlockAllSignals______________X(this);

    sliderOpacity->setValue(demfile->getOpacity());

    toolSetMinScale->setChecked( demfile->getMinScale() != NOFLOAT );
    toolSetMaxScale->setChecked( demfile->getMaxScale() != NOFLOAT );

    updateScaleLabel();

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

