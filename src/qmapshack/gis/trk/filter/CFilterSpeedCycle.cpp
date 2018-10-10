/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/trk/filter/CFilterSpeedCycleEnergy.h"

CFilterSpeedCycle::CFilterSpeedCycle(QWidget *parent, CGisItemTrk &trk)
    : QWidget(parent), trk(trk), noOfFixTypes(4), noOfCustomTypes(3)
    // 4 fix and 3 custom cycling types has be defined as default
    ,cyclingTypeDefaults
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

    spinPlainSpeed->setSuffix(IUnit::self().speedunit);
    spinMinSpeed->setSuffix(IUnit::self().speedunit);
    spinMaxSpeed->setSuffix(IUnit::self().speedunit);

    QString val, slopeUnit;
    IUnit::self().slope2string(0, val, slopeUnit);
    spinSlopeAtMaxSpeed->setSuffix(slopeUnit);
    spinSlopeAtMinSpeed->setSuffix(slopeUnit);

    cycling_type_t cyclingType;
    for (int i = 0; i < noOfFixTypes; ++i)
    {
        const cycling_type_t &cyclingTypeDefault = cyclingTypeDefaults[i];
        cyclingType.name = cyclingTypeDefault.name;
        cyclingType.plainSpeed = cyclingTypeDefault.plainSpeed;
        cyclingType.minSpeed = cyclingTypeDefault.minSpeed;
        cyclingType.slopeAtMinSpeed = cyclingTypeDefault.slopeAtMinSpeed;
        cyclingType.maxSpeed = cyclingTypeDefault.maxSpeed;
        cyclingType.slopeAtMaxSpeed = cyclingTypeDefault.slopeAtMaxSpeed;

        comboCyclingType->addItem(cyclingType.name);
        cyclingTypes << cyclingType;
    }

    connect(comboCyclingType, SIGNAL(activated(int)), this, SLOT(slotSetCyclingType(int)));
    connect(spinPlainSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetPlainSpeed(double)));
    connect(spinMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMinSpeed(double)));
    connect(spinSlopeAtMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMinSpeed(double)));
    connect(spinMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMaxSpeed(double)));
    connect(spinSlopeAtMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMaxSpeed(double)));
    connect(pushSetMinMaxSlope, SIGNAL(clicked(bool)), this, SLOT(slotSetMinMaxSlopes(bool)));

    connect(toolApplyEnergy, &QToolButton::clicked, this, &CFilterSpeedCycle::slotEnergy);

    energy.totalWeight = 100;
    energy.windDragCoeff = 0.85;
    energy.frontalArea = 0.42;
    energy.airDensity = 1.2;
    energy.rollingCoeff = 0.005;
    energy.windSpeed = 0;
    energy.muscleCoeff = 0.23;
    energy.crankLength = 170;
    energy.pedalCadence = 75;
    energy.pedalRangeEff = 70;
    energy.totalEnergyKcal = 0;
    trk.filterSpeedCycleEnergy(energy);
    insertEnergy();
}

void CFilterSpeedCycle::loadSettings(QSettings& cfg)
{
    cycling_type_t cyclingType;
    cfg.beginReadArray("CustomCyclingTypes");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const cycling_type_t &cyclingTypeDefault = cyclingTypeDefaults[noOfFixTypes + i];
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
        const cycling_type_t &cyclingType = cyclingTypes[noOfFixTypes + i];
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

    trk.filterSpeedCycleEnergy(energy);
    insertEnergy();
}

void CFilterSpeedCycle::slotSetCyclingType(int type)
{
    const cycling_type_t &cyclingType = cyclingTypes[type];

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

void CFilterSpeedCycle::slotEnergy()
{
    if (comboCyclingType->currentIndex() < noOfFixTypes)
    {
        trk.filterSpeedCycleEnergy(energy);
        insertEnergy();
    }
    else
    {
        CFilterSpeedCycleEnergy energyDlg(this, trk, energy);
        if((energyDlg.exec() == QDialog::Accepted))
        {
            insertEnergy();
        }
    }
}

void CFilterSpeedCycle::insertEnergy()
{
    QString energyStr = QString(tr("Energy=%1kcal")).arg(energy.totalEnergyKcal, 0, 'f', 0);
    labelEnergy->setText(energyStr);
    if (comboCyclingType->currentIndex() < noOfFixTypes)
    {
        toolApplyEnergy->setDisabled(true);
    }
    else
    {
        toolApplyEnergy->setDisabled(false);
    }
}
