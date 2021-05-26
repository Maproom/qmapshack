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
#include "gis/trk/filter/CFilterSpeedCycle.h"

#include <QtWidgets>

CFilterSpeedCycle::CFilterSpeedCycle(QWidget* parent, CGisItemTrk& trk)
    : QWidget(parent), trk(trk), noOfFixTypes(4), noOfCustomTypes(3)
    // 4 fix and 3 custom cycling types has be defined as default
    , cyclingTypeDefaults
    {
    {
        tr("City")                // Fix: name
        , 15                      // spinPlainSpeed
        , 5                       // spinMinSpeed
        , 5                       // spinSlopeAtMinSpeed
        , 35                      // spinMaxSpeed
        , -5                      // spinSlopeAtMaxSpeed
    },
    {
        tr("Trekking"), 20, 5, 8, 40, -5         // Fix
    },
    {
        tr("Sportive"), 27, 7, 10, 60, -8        // Fix
    },
    {
        tr("Mountain"), 15, 4, 15, 40, -15       // Fix
    },
    {
        tr("Custom 0"), 20, 5, 8, 40, -5       // Custom 0
    },
    {
        tr("Custom 1"), 20, 5, 8, 40, -5       // Custom 1
    },
    {
        tr("Custom 2"), 20, 5, 8, 40, -5       // Custom 2
    }
    }
{
    setupUi(this);

    spinPlainSpeed->setSuffix("km/h");
    spinMinSpeed->setSuffix("km/h");
    spinMaxSpeed->setSuffix("km/h");

    QString val, slopeUnit;
    IUnit::self().slope2string(0, val, slopeUnit);
    spinSlopeAtMaxSpeed->setSuffix(slopeUnit);
    spinSlopeAtMinSpeed->setSuffix(slopeUnit);

    cycling_type_t cyclingType;
    for (int i = 0; i < noOfFixTypes; ++i)
    {
        const cycling_type_t& cyclingTypeDefault = cyclingTypeDefaults[i];
        cyclingType.name = cyclingTypeDefault.name;
        cyclingType.plainSpeed = cyclingTypeDefault.plainSpeed;
        cyclingType.minSpeed = cyclingTypeDefault.minSpeed;
        cyclingType.slopeAtMinSpeed = cyclingTypeDefault.slopeAtMinSpeed;
        cyclingType.maxSpeed = cyclingTypeDefault.maxSpeed;
        cyclingType.slopeAtMaxSpeed = cyclingTypeDefault.slopeAtMaxSpeed;

        comboCyclingType->addItem(cyclingType.name);
        cyclingTypes << cyclingType;
    }

    connect(comboCyclingType, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CFilterSpeedCycle::slotSetCyclingType);
    connect(spinPlainSpeed, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedCycle::slotSetPlainSpeed);
    connect(spinMinSpeed, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedCycle::slotSetMinSpeed);
    connect(spinSlopeAtMinSpeed, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedCycle::slotSetSlopeAtMinSpeed);
    connect(spinMaxSpeed, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedCycle::slotSetMaxSpeed);
    connect(spinSlopeAtMaxSpeed, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CFilterSpeedCycle::slotSetSlopeAtMaxSpeed);
    connect(pushSetMinMaxSlope, &QPushButton::clicked, this, &CFilterSpeedCycle::slotSetMinMaxSlopes);
}

void CFilterSpeedCycle::loadSettings(QSettings& cfg)
{
    cycling_type_t cyclingType;
    cfg.beginReadArray("CustomCyclingTypes");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const cycling_type_t& cyclingTypeDefault = cyclingTypeDefaults[noOfFixTypes + i];
        cfg.setArrayIndex(i);
        cyclingType.name = cyclingTypeDefault.name;
        cyclingType.plainSpeed = cfg.value("plainSpeed", cyclingTypeDefault.plainSpeed).toDouble();
        cyclingType.minSpeed = cfg.value("minSpeed", cyclingTypeDefault.minSpeed).toDouble();
        cyclingType.slopeAtMinSpeed = cfg.value("slopeAtMinSpeed", cyclingTypeDefault.slopeAtMinSpeed).toDouble();
        cyclingType.maxSpeed = cfg.value("maxSpeed", cyclingTypeDefault.maxSpeed).toDouble();
        cyclingType.slopeAtMaxSpeed = cfg.value("slopeAtMaxSpeed", cyclingTypeDefault.slopeAtMaxSpeed).toDouble();

        comboCyclingType->addItem(cyclingType.name);
        cyclingTypes << cyclingType;
    }
    cfg.endArray();
    comboCyclingType->setCurrentIndex(cfg.value("cyclingType", 0).toInt());
    slotSetCyclingType(comboCyclingType->currentIndex());
}

void CFilterSpeedCycle::saveSettings(QSettings& cfg)
{
    cfg.setValue("cyclingType", comboCyclingType->currentIndex());

    cfg.beginWriteArray("CustomCyclingTypes");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const cycling_type_t& cyclingType = cyclingTypes[noOfFixTypes + i];
        cfg.setArrayIndex(i);
        cfg.setValue("name", cyclingType.name);
        cfg.setValue("plainSpeed", cyclingType.plainSpeed);
        cfg.setValue("minSpeed", cyclingType.minSpeed);
        cfg.setValue("slopeAtMinSpeed", cyclingType.slopeAtMinSpeed);
        cfg.setValue("maxSpeed", cyclingType.maxSpeed);
        cfg.setValue("slopeAtMaxSpeed", cyclingType.slopeAtMaxSpeed);
    }
    cfg.endArray();
}

void CFilterSpeedCycle::apply(CGisItemTrk& trk)
{
    trk.filterSpeed(cyclingTypes[comboCyclingType->currentIndex()]);
}

void CFilterSpeedCycle::slotSetCyclingType(int type)
{
    const cycling_type_t& cyclingType = cyclingTypes[type];

    spinPlainSpeed->setValue(cyclingType.plainSpeed);
    spinMinSpeed->setValue(cyclingType.minSpeed);
    spinMaxSpeed->setValue(cyclingType.maxSpeed);

    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingType.slopeAtMinSpeed);
        spinSlopeAtMinSpeed->setValue(val);
        val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingType.slopeAtMaxSpeed);
        spinSlopeAtMaxSpeed->setValue(val);
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        spinSlopeAtMinSpeed->setValue(cyclingType.slopeAtMinSpeed);
        spinSlopeAtMaxSpeed->setValue(cyclingType.slopeAtMaxSpeed);
    }

    if (type < noOfFixTypes)
    {
        frameCycling->setEnabled(false);
    }
    else
    {
        frameCycling->setEnabled(true);
    }
}

void CFilterSpeedCycle::slotSetPlainSpeed(double speed)
{
    cyclingTypes[comboCyclingType->currentIndex()].plainSpeed = speed;
}

void CFilterSpeedCycle::slotSetMinSpeed(double speed)
{
    cyclingTypes[comboCyclingType->currentIndex()].minSpeed = speed;
}

void CFilterSpeedCycle::slotSetSlopeAtMinSpeed(double slope)
{
    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, slope);
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMinSpeed = val;
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMinSpeed = slope;
    }
}

void CFilterSpeedCycle::slotSetMaxSpeed(double speed)
{
    cyclingTypes[comboCyclingType->currentIndex()].maxSpeed = speed;
}

void CFilterSpeedCycle::slotSetSlopeAtMaxSpeed(double slope)
{
    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, slope);
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMaxSpeed = val;
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMaxSpeed = slope;
    }
}

void CFilterSpeedCycle::slotSetMinMaxSlopes(bool)
{
    qreal minSlope, maxSlope;
    trk.filterGetSlopeLimits(minSlope, maxSlope);

    if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, maxSlope);
        spinSlopeAtMinSpeed->setValue(val);
        val = IUnit::slopeConvert(IUnit::eSlopeDegrees, minSlope);
        spinSlopeAtMaxSpeed->setValue(val);
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        spinSlopeAtMinSpeed->setValue(maxSlope);
        spinSlopeAtMaxSpeed->setValue(minSlope);
    }
}
