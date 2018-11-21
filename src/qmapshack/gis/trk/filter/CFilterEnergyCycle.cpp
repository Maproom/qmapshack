/**********************************************************************************************

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

#include "canvas/CCanvas.h"
#include "CFilterEnergyCycle.h"
#include "CFilterEnergyCycleDlg.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"

CFilterEnergyCycle::CFilterEnergyCycle(CGisItemTrk &trk, QWidget *parent) :
    QWidget(parent)
    , trk(trk)
    , energyDefaultSet{"", 75, 15, 1.2, 5, 0, 2, 0.65, 1.00, 4, 0.0070, 75}
{
    setupUi(this);

    loadSettings();

    connect(comboBox, SIGNAL(activated(int)), this, SLOT(slotSetSetting(int)));
    connect(pushButtonRemove, SIGNAL(clicked(bool)), this, SLOT(slotRemove(bool)));
    connect(pushButtonEdit, SIGNAL(clicked(bool)), this, SLOT(slotEditParameter(bool)));
    connect(toolApply, &QToolButton::clicked, this, &CFilterEnergyCycle::slotApply);

}

void CFilterEnergyCycle::loadSettings()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/EnergyCycle/");

    energy_set_t energySet;

    cfg.beginReadArray("Set");
    for (int i = 0; i < noOfSets; ++i)
    {
        cfg.setArrayIndex(i);

        energySet.nameOfSet = cfg.value("nameOfSet", QString(tr("Set %1")).arg(i)).toString();
        energySet.driverWeight = cfg.value("personalWeight", energyDefaultSet.driverWeight).toDouble();
        energySet.bikeWeight = cfg.value("bikeWeight", energyDefaultSet.bikeWeight).toDouble();
        energySet.airDensity = cfg.value("airDensity", energyDefaultSet.airDensity).toDouble();
        energySet.windSpeedIndex = cfg.value("windSpeedIndex", energyDefaultSet.windSpeedIndex).toInt();
        energySet.windSpeed = cfg.value("windSpeed", energyDefaultSet.windSpeed).toDouble();
        energySet.windPositionIndex = cfg.value("windPositionIndex", energyDefaultSet.windPositionIndex).toInt();
        energySet.frontalArea = cfg.value("frontalArea", energyDefaultSet.frontalArea).toDouble();
        energySet.windDragCoeff = cfg.value("windDragCoeff", energyDefaultSet.windDragCoeff).toDouble();
        energySet.groundIndex = cfg.value("groundIndex", energyDefaultSet.groundIndex).toInt();
        energySet.rollingCoeff = cfg.value("rollingCoeff", energyDefaultSet.rollingCoeff).toDouble();
        energySet.pedalCadence = cfg.value("pedalCadence", energyDefaultSet.pedalCadence).toDouble();

        energySets << energySet;
        comboBox->addItem(energySet.nameOfSet);
    }
    cfg.endArray();
    currentSet = cfg.value("currentSet", 0).toInt();
    comboBox->setCurrentIndex(currentSet);
    cfg.endGroup();
}

void CFilterEnergyCycle::saveSettings()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/EnergyCycle/");

    cfg.setValue("currentSet", currentSet);

    cfg.beginWriteArray("Set");
    for (int i = 0; i < noOfSets; ++i)
    {
        const energy_set_t &energySet = energySets[i];
        cfg.setArrayIndex(i);
        cfg.setValue("nameOfSet", energySet.nameOfSet);
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
        cfg.setValue("pedalCadence", energySet.pedalCadence);
    }
    cfg.endArray();
    cfg.endGroup();
}

void CFilterEnergyCycle::slotApply()
{
    updateUi();

//    energy_set_t &energySet = energySets[currentSet];
//    trk.filterEnergyCycle(energySet);
//    trk.setEnergyUse(energySet.energyKcal); // Save to track property
//    trk.updateVisuals(CGisItemTrk::eVisualDetails, "filterEnergyCycle");
//    toolApply->setEnabled(false);
}

void CFilterEnergyCycle::slotSetSetting(int set)
{
    currentSet = set;
    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/EnergyCycle/");
    cfg.setValue("currentSet", currentSet);
    cfg.endGroup();
    toolApply->setEnabled(true);
}

void CFilterEnergyCycle::slotRemove(bool)
{
    trk.setEnergyUse(NOFLOAT);
    trk.updateVisuals(CGisItemTrk::eVisualDetails, "filterEnergyCycle");
    toolApply->setEnabled(true);
}

void CFilterEnergyCycle::slotEditParameter(bool)
{
    energy_set_t &energySet = energySets[currentSet];

    CFilterEnergyCycleDlg energyDlg(this, trk, energySet, energyDefaultSet);
    if(energyDlg.exec() == QDialog::Accepted)
    {
        updateUi();
//        if(!qFuzzyCompare(trk.getEnergyUse() + 1, energySet.energyKcal + 1))
//        {
//            trk.setEnergyUse(energySet.energyKcal);
//            trk.updateVisuals(CGisItemTrk::eVisualDetails, "filterEnergyCycle");
//            toolApply->setEnabled(false);
//        }
        if(comboBox->currentText() != energySet.nameOfSet)
        {
            comboBox->setItemText(currentSet, energySet.nameOfSet);
            saveSettings();
        }
    }
}

void CFilterEnergyCycle::updateUi()
{
    energy_set_t &energySet = energySets[currentSet];
    trk.filterEnergyCycle(energySet);
    if(!qFuzzyCompare(trk.getEnergyUse() + 1, energySet.energyKcal + 1))
    {
//        trk.filterEnergyCycle(energySet);
        trk.setEnergyUse(energySet.energyKcal);
        toolApply->setEnabled(false);
        trk.updateVisuals(CGisItemTrk::eVisualDetails, "filterEnergyCycle");
    }
}

void CFilterEnergyCycle::updateUi1()
{
    energy_set_t &energySet = energySets[currentSet];
    trk.filterEnergyCycle(energySet);
    if(!qFuzzyCompare(trk.getEnergyUse() + 1, energySet.energyKcal + 1))
    {
        trk.setEnergyUse(energySet.energyKcal);
        toolApply->setEnabled(false);
        trk.updateVisuals(CGisItemTrk::eVisualDetails, "filterEnergyCycle");
    }
}

void CFilterEnergyCycle::energy_set_t::compute(CGisItemTrk &trk)
{
    // Input values
    const qreal joule2Calor = 4.1868;
    const qreal gravityAccel = 9.81;    // kg * m / s2
    const qreal muscleCoeff = 23;       // %
    const qreal pedalRange = 70;        // Degree °
    const qreal crankLength = 175;      // mm
    const qreal totalWeight = driverWeight + bikeWeight;

    //    qreal airDensity = energySet.airDensity;
//    qreal windSpeed = energySet.windSpeed;
//    qreal pedalCadence = energySet.pedalCadence;
//    qreal frontalArea = energySet.frontalArea;
//    qreal windDragCoeff = energySet.windDragCoeff;
//    qreal rollingCoeff = energySet.rollingCoeff;

    // Output values
    airResistForce = 0;
    rollResistForce = totalWeight * gravityAccel * rollingCoeff;
    gravitySlopeForce = 0;
    sumForce = 0;
    positivePedalForce = 0;
    power = 0;
    positivePower = 0;
    powerMovingTime = 0;
    powerMovingTimeRatio = 0;
    energyKJoule = 0;

    qint32 cntPositivePowerPoints = 0;

    qreal pedalSpeed = crankLength * pedalCadence * 2 * M_PI / 60 / 1000;

//    CTrackData::trkpt_t const * lastTrkpt  = nullptr;

    for(const CTrackData::trkpt_t &pt : trk.getTrackData())
    {
        if(pt.isHidden())
        {
            continue;
        }

// --- Alternative approach to calculate exact speed, instead using smoothed trkPts speed
// --- using smoothed trkPts speed will not provide exact power moving time in case of uphill only
// --- ratio will be then > 100%
//        qreal speed = 0;
//        if(lastTrkpt != nullptr)
//        {
//            qreal deltaTime = (pt.time.toMSecsSinceEpoch() - lastTrkpt->time.toMSecsSinceEpoch()) / 1000.0;
//            speed = pt.deltaDistance / deltaTime;
//            lastTrkpt = &pt;
//        }
//        else
//        {
//            lastTrkpt = &pt;
//            continue;
//        }
//        if (speed != pt.speed)
//        {
//            qDebug() << "pt.speed=" << pt.speed * 3.6 << "speed=" << speed * 3.6 << "speed delta=" << (speed - pt.speed) * 3.6;
//        }

        qreal speed = pt.speed;
        if (speed <= 0.2) // 0.2 ==> to be synchron with deriveSecondaryData()
        {
            continue;
        }

        qreal slope = pt.slope2;
//        slope = 0;

        qreal airResistForcePt = 0.5 * windDragCoeff * frontalArea * airDensity * qPow(speed + windSpeed, 2);
        qreal gravitySlopeForcePt = totalWeight * gravityAccel * slope / 100;
        airResistForce += airResistForcePt;
        gravitySlopeForce += gravitySlopeForcePt;
        sumForce += airResistForcePt + gravitySlopeForcePt + rollResistForce;

        qreal powerPt = (airResistForcePt * (speed + windSpeed)) + ((rollResistForce + gravitySlopeForcePt) * speed);
        power += powerPt;

        if (powerPt > 0)
        {
            qreal deltaPowerTime = pt.deltaDistance / speed;
            powerMovingTime += deltaPowerTime;
            positivePower += powerPt;
            energyKJoule += powerPt * deltaPowerTime / muscleCoeff / 1000 * 100;
            positivePedalForce += powerPt / pedalSpeed * 180 / pedalRange;
            cntPositivePowerPoints++;
        }
    }

    qint32 cntVisiblePoints = trk.getNumberOfVisiblePoints();
    qDebug() << "struct cntVisiblePoints=" << cntVisiblePoints;
    if (cntVisiblePoints)
    {
        airResistForce /= cntVisiblePoints;
        gravitySlopeForce /= cntVisiblePoints;
        sumForce /= cntVisiblePoints;
        power /= cntVisiblePoints;
    }

    quint32 totalElapsedSecondsMoving = trk.getTotalElapsedSecondsMoving();
    qDebug() << "struct totalElapsedSecondsMoving=" << totalElapsedSecondsMoving;
    if(totalElapsedSecondsMoving)
    {
        powerMovingTimeRatio = powerMovingTime / totalElapsedSecondsMoving;
    }
    if(cntPositivePowerPoints)
    {
        positivePedalForce /= cntPositivePowerPoints;
        positivePower /= cntPositivePowerPoints;
    }
    energyKcal = energyKJoule / joule2Calor;

//    qDebug() <<
//                QString("name;totalWeight;airDensity;windSpeed;pedalCadence;frontalArea;windDragCoeff;rollingCoeff;") +
//                QString("airResistForce;rollResistForce;gravitySlopeForce;sumForce;positivePedalForce;power;positivePower;") +
//                QString("powerMovingTime;powerMovingTimeRatio;energyKJoule;energyKcal");
//    qDebug()
//            << getName() << ";"
//            << totalWeight << ";"
//            << energy.airDensity << ";"
//            << energy.windSpeed << ";"
//            << energy.pedalCadence << ";"
//            << energy.frontalArea << ";"
//            << energy.windDragCoeff << ";"
//            << energy.rollingCoeff << ";"
//            << energy.airResistForce << ";"
//            << energy.rollResistForce << ";"
//            << energy.gravitySlopeForce << ";"
//            << energy.sumForce << ";"
//            << energy.positivePedalForce << ";"
//            << energy.power << ";"
//            << energy.positivePower << ";"
//            << energy.powerMovingTime << ";"
//            << energy.powerMovingTimeRatio << ";"
//            << energy.energyKJoule << ";"
//            << energy.energyKcal;
}
