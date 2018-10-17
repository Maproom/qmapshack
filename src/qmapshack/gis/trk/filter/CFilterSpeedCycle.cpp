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
    , energySetDefaults
    {
    {75, 17, 1.2, 5, 0, 1, 0.70, 1.10, 3, 0.0050, 23, 75, 70, 175}, // City
    {75, 15, 1.2, 5, 0, 2, 0.65, 1.00, 4, 0.0070, 23, 75, 70, 175}, // Trekking
    {75,  9, 1.2, 5, 0, 4, 0.48, 0.50, 2, 0.0040, 23, 75, 70, 175}, // Sportive
    {75, 10, 1.2, 5, 0, 6, 0.60, 0.90, 5, 0.0150, 23, 75, 70, 175}, // Mountain
    {75, 15, 1.2, 5, 0, 2, 0.65, 1.00, 4, 0.0070, 23, 75, 70, 175}, // Custom 0
    {75, 15, 1.2, 5, 0, 2, 0.65, 1.00, 4, 0.0070, 23, 75, 70, 175}, // Custom 1
    {75, 15, 1.2, 5, 0, 2, 0.65, 1.00, 4, 0.0070, 23, 75, 70, 175}  // Custom 2
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

    energy_set_t energySet;
    for (int i = 0; i < noOfFixTypes; ++i)
    {
        const energy_set_t &energySetDefault = energySetDefaults[i];
        energySet.driverWeight = energySetDefault.driverWeight;
        energySet.bikeWeight = energySetDefault.bikeWeight;
        energySet.airDensity = energySetDefault.airDensity;
        energySet.windSpeedIndex = energySetDefault.windSpeedIndex;
        energySet.windSpeed = energySetDefault.windSpeed;
        energySet.windPositionIndex = energySetDefault.windPositionIndex;
        energySet.frontalArea = energySetDefault.frontalArea;
        energySet.windDragCoeff = energySetDefault.windDragCoeff;
        energySet.groundIndex = energySetDefault.groundIndex;
        energySet.rollingCoeff = energySetDefault.rollingCoeff;
        energySet.muscleEff = energySetDefault.muscleEff;
        energySet.pedalCadence = energySetDefault.pedalCadence;
        energySet.pedalRange = energySetDefault.pedalRange;
        energySet.crankLength = energySetDefault.crankLength;
        energySets << energySet;
    }

    connect(comboCyclingType, SIGNAL(activated(int)), this, SLOT(slotSetCyclingType(int)));
    connect(spinPlainSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetPlainSpeed(double)));
    connect(spinMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMinSpeed(double)));
    connect(spinSlopeAtMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMinSpeed(double)));
    connect(spinMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMaxSpeed(double)));
    connect(spinSlopeAtMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMaxSpeed(double)));
    connect(pushSetMinMaxSlope, SIGNAL(clicked(bool)), this, SLOT(slotSetMinMaxSlopes(bool)));

    connect(toolApplyEnergy, &QToolButton::clicked, this, &CFilterSpeedCycle::slotApplyEnergy);
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

    energy_set_t energySet;
    cfg.beginReadArray("CustomEnergySets");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const energy_set_t &energyDefault = energySetDefaults[noOfFixTypes + i];
        cfg.setArrayIndex(i);

        energySet.driverWeight = cfg.value("personalWeight", energyDefault.driverWeight).toDouble();
        energySet.bikeWeight = cfg.value("bikeWeight", energyDefault.bikeWeight).toDouble();
        energySet.airDensity = cfg.value("airDensity", energyDefault.airDensity).toDouble();
        energySet.windSpeedIndex = cfg.value("windSpeedIndex", energyDefault.windSpeedIndex).toInt();
        energySet.windSpeed = cfg.value("windSpeed", energyDefault.windSpeed).toDouble();
        energySet.windPositionIndex = cfg.value("windPositionIndex", energyDefault.windPositionIndex).toInt();
        energySet.frontalArea = cfg.value("frontalArea", energyDefault.frontalArea).toDouble();
        energySet.windDragCoeff = cfg.value("windDragCoeff", energyDefault.windDragCoeff).toDouble();
        energySet.groundIndex = cfg.value("groundIndex", energyDefault.groundIndex).toInt();
        energySet.rollingCoeff = cfg.value("rollingCoeff", energyDefault.rollingCoeff).toDouble();
        energySet.muscleEff = cfg.value("muscleEff", energyDefault.muscleEff).toDouble();
        energySet.pedalCadence = cfg.value("pedalCadence", energyDefault.pedalCadence).toDouble();
        energySet.pedalRange = cfg.value("pedalRange", energyDefault.pedalRange).toDouble();
        energySet.crankLength = cfg.value("crankLength", energyDefault.crankLength).toDouble();

        energySets << energySet;
    }
    cfg.endArray();

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

    cfg.beginWriteArray("CustomEnergySets");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const energy_set_t &energySet = energySets[noOfFixTypes + i];
        cfg.setArrayIndex(i);
        cfg.setValue("personalWeight", energySet.driverWeight);
        cfg.setValue("bikeWeight", energySet.bikeWeight);
        cfg.setValue("airDensity", energySet.airDensity);
        cfg.setValue("windSpeedIndex", energySet.windSpeedIndex);
        cfg.setValue("windSpeed", energySet.windSpeed);
        cfg.setValue("windPositionIndex", energySet.windPositionIndex);
        cfg.setValue("frontalArea", energySet.frontalArea);
        cfg.setValue("windDragCoeff", energySet.windDragCoeff);
        cfg.setValue("groundIndex", energySet.groundIndex);
        cfg.setValue("rollingCoeff", energySet.rollingCoeff);
        cfg.setValue("muscleEff", energySet.muscleEff);
        cfg.setValue("pedalCadence", energySet.pedalCadence);
        cfg.setValue("pedalRange", energySet.pedalRange);
        cfg.setValue("crankLength", energySet.crankLength);
    }
    cfg.endArray();
}

void CFilterSpeedCycle::apply(CGisItemTrk& trk)
{
    trk.filterSpeed(cyclingTypes[comboCyclingType->currentIndex()]);
    updateUi();
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

    updateUi();
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

void CFilterSpeedCycle::slotApplyEnergy()
{
    qint32 type = comboCyclingType->currentIndex();
    CFilterSpeedCycleEnergy energyDlg(this, trk, energySets[type]);
    energyDlg.setWindowTitle(tr("Energy Parameter Set for Cycling Type ")
                             + cyclingTypes[comboCyclingType->currentIndex()].name);

    if((energyDlg.exec() == QDialog::Accepted))
    {
        insertEnergy(true);
    }
}

void CFilterSpeedCycle::updateUi()
{
    qint32 type = comboCyclingType->currentIndex();
    if (trk.isTrkTimeValid())
    {
        trk.filterSpeedCycleEnergy(energySets[type]);
        insertEnergy(true);
    }
    else
    {
        insertEnergy(false);
    }

    if (trk.isTrkTimeValid() && type >= noOfFixTypes)
    {
        toolApplyEnergy->setEnabled(true);
    }
    else
    {
        toolApplyEnergy->setEnabled(false);
    }
}

void CFilterSpeedCycle::insertEnergy(bool valid)
{
    QString energyStr = QString(tr("Energy Use"));
    if(valid)
    {
        qint32 type = comboCyclingType->currentIndex();
        energyStr += QString(" %1kcal").arg(energySets[type].totalEnergyKcal, 0, 'f', 0);
    }
    else
    {
        energyStr += " ?";
    }
    labelEnergy->setText(energyStr);
}
