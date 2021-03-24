/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterSpeedHike.h"

#include <QtWidgets>

CFilterSpeedHike::CFilterSpeedHike(QWidget *parent)
    : QWidget(parent), noOfFixTypes(4), noOfCustomTypes(3)
    // 4 fix and 3 custom hiking types has be defined as default
    // Based on carloscoi analysis
    //  - Road or Track:               5.5 / 600 / 850
    //  - Path:                        4.5 / 500 / 700
    //  - Difficult Path:              3.6 / 400 / 560
    //  - Off Path or Rocky Terrain:   2.9 / 320 / 450
    , hikingTypeDefaults
    {
    {
        tr("Road or Track")                  // Fix: name
        , 5.5                       // Flat Speed in actual speedunit
        , 600                       // Ascending in fix m/h
        , 850                       // Descending in fix m/h
    },
    {
        tr("Path"), 4.5, 500, 700,                             // Fix
    },
    {
        tr("Difficult Path"), 3.6, 400, 560,                   // Fix
    },
    {
        tr("Off Path or Rocky Terrain"), 2.9, 320, 450,        // Fix
    },
    {
        tr("Custom 0"), 4.5, 500, 700             // Custom 0
    },
    {
        tr("Custom 1"), 4.5, 500, 700             // Custom 1
    },
    {
        tr("Custom 2"), 4.5, 500, 700             // Custom 2
    }
    }
{
    setupUi(this);

    spinPlainSpeed->setSuffix("km/h");
    spinAscending->setSuffix("m/h");
    spinDescending->setSuffix("m/h");

    hiking_type_t hikingType;
    for (int i = 0; i < noOfFixTypes; ++i)
    {
        const hiking_type_t &hikingTypeDefault = hikingTypeDefaults[i];
        hikingType.name = hikingTypeDefault.name;
        hikingType.plainSpeed = hikingTypeDefault.plainSpeed;
        hikingType.ascending = hikingTypeDefault.ascending;
        hikingType.descending = hikingTypeDefault.descending;

        comboHikingType->addItem(hikingType.name);
        hikingTypes << hikingType;
    }

    connect(comboHikingType, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CFilterSpeedHike::slotSetHikingType);
    connect(spinPlainSpeed, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedHike::slotSetPlainSpeed);
    connect(spinAscending, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedHike::slotSetAscending);
    connect(spinDescending, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedHike::slotSetDescending);
}

void CFilterSpeedHike::loadSettings(QSettings& cfg)
{
    hiking_type_t hikingType;
    cfg.beginReadArray("CustomHikingTypes");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const hiking_type_t &hikingTypeDefault = hikingTypeDefaults[noOfFixTypes + i];
        cfg.setArrayIndex(i);
        hikingType.name = hikingTypeDefault.name;
        hikingType.plainSpeed = cfg.value("plainSpeed", hikingTypeDefault.plainSpeed).toDouble();
        hikingType.ascending = cfg.value("ascending", hikingTypeDefault.ascending).toDouble();
        hikingType.descending = cfg.value("descending", hikingTypeDefault.descending).toDouble();

        comboHikingType->addItem(hikingType.name);
        hikingTypes << hikingType;
    }
    cfg.endArray();
    comboHikingType->setCurrentIndex(cfg.value("hikingType", 0).toInt());
    slotSetHikingType(comboHikingType->currentIndex());
}

void CFilterSpeedHike::saveSettings(QSettings& cfg)
{
    cfg.setValue("hikingType", comboHikingType->currentIndex());

    cfg.beginWriteArray("CustomHikingTypes");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const hiking_type_t &hikingType = hikingTypes[noOfFixTypes + i];
        cfg.setArrayIndex(i);
        cfg.setValue("name", hikingType.name);
        cfg.setValue("plainSpeed", hikingType.plainSpeed);
        cfg.setValue("ascending", hikingType.ascending);
        cfg.setValue("descending", hikingType.descending);
    }
    cfg.endArray();
}

void CFilterSpeedHike::apply(CGisItemTrk& trk)
{
    trk.filterSpeed(hikingTypes[comboHikingType->currentIndex()]);
}

void CFilterSpeedHike::slotSetHikingType(int type)
{
    const hiking_type_t &hikingType = hikingTypes[type];

    spinPlainSpeed->setValue(hikingType.plainSpeed);
    spinAscending->setValue(hikingType.ascending);
    spinDescending->setValue(hikingType.descending);

    if (type < noOfFixTypes)
    {
        frameHiking->setEnabled(false);
    }
    else
    {
        frameHiking->setEnabled(true);
    }
}

void CFilterSpeedHike::slotSetPlainSpeed(double speed)
{
    hikingTypes[comboHikingType->currentIndex()].plainSpeed = speed;
}

void CFilterSpeedHike::slotSetAscending(double ascending)
{
    hikingTypes[comboHikingType->currentIndex()].ascending = ascending;
}

void CFilterSpeedHike::slotSetDescending(double descending)
{
    hikingTypes[comboHikingType->currentIndex()].descending = descending;
}
