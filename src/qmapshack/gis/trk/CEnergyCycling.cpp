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

#include "CMainWindow.h"
#include "gis/trk/CEnergyCycling.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CSettings.h"

CEnergyCycling::CEnergyCycling(CGisItemTrk &trk) :
    trk(trk)
{
    loadSettings(energyTrkSet);
}

/* On initial start (no parameters are saved in setting file QMapShack.conf) the dafault parameters from header file be used.
* When modifying the parameters and clicking Ok parameters will be saved in QMapShack.conf. When loading a track
* (ex. from GPS device) with Energy Use = NOFLOAT the parameters from QMapShack.conf will be used and shown in the dialog.
* When modifiyng and clicking on Ok the parameters will be saved back in QMapShack.conf.
* Setting will be load either in the energyTrkSet belonging to the track at at initiation
* or in a energyTmpSet belonging to the Dialog. On Ok energyTmpSet will be saved back to track’s energyTrkSet.
* On Cancel or Remove no changes will be made in track’s energyTrkSet.
* The button “Load previous Set” can be used for the following uses cases:
*    - You load an old track with a computed Energy Use value and you will replace
*      all the parameter with your current active parameter set
*    - You have to “play” to much with all these parameters in the dialog and you
*      are a bit confused, so you can load your last save parameter set
*    - When clicking in history and you will use an older track to start again with modifying
*      you can you also use your latest save parameter set
*    - When you have some tracks in edit mode and you would like to update them all
*      with the same parameter set, edit the first one and use “Load previous Set” for all the others
*/
void CEnergyCycling::loadSettings(CEnergyCycling::energy_set_t &energySet)
{
    SETTINGS;

    energy_set_t energyDefaultSet; // to get the default values defined in header

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
}

void CEnergyCycling::saveSettings()
{
    SETTINGS;

    cfg.setValue("personalWeight", energyTrkSet.driverWeight);
    cfg.setValue("bikeWeight", energyTrkSet.bikeWeight);
    cfg.setValue("airDensity", energyTrkSet.airDensity);
    cfg.setValue("windSpeedIndex", energyTrkSet.windSpeedIndex);
    cfg.setValue("windSpeed", energyTrkSet.windSpeed);
    cfg.setValue("windPositionIndex", energyTrkSet.windPositionIndex);
    cfg.setValue("frontalArea", energyTrkSet.frontalArea);
    cfg.setValue("windDragCoeff", energyTrkSet.windDragCoeff);
    cfg.setValue("groundIndex", energyTrkSet.groundIndex);
    cfg.setValue("rollingCoeff", energyTrkSet.rollingCoeff);
    cfg.setValue("pedalCadence", energyTrkSet.pedalCadence);
}

/* To compute based directly on the track parameter set
 * only when "Energy Use Cycling" != NOFLOAT, which indicates noshow in info panel
*/
void CEnergyCycling::compute()
{
    if (energyTrkSet.energyKcal != NOFLOAT)
    {
        compute(energyTrkSet);
    }
}

/* To compute based on a given parameter set
 * Used by dialog to compute temporarily parameter set in all cases
 *
 * Related information for the computation process and used formulas:
 * http://www.blog.ultracycle.net/2010/05/cycling-power-calculations (English)
 * http://www.cptips.com/energy.htm  (English)
 * http://www.tribology-abc.com/calculators/cycling.htm (English)
 * http://www.kreuzotter.de/deutsch/speed.htm (German)
 * http://horst-online.com/physik-des-fahrrads/index.html (German)
 * http://www.helpster.de/wirkungsgrad-vom-mensch-erklaerung_198168 (German)
 * http://www.wolfgang-menn.de/motion_d.htm (German)
 * http://www.msporting.com/planung/5_3_6%20Aerodynamik.htm (German)
*/
void CEnergyCycling::compute(CEnergyCycling::energy_set_t &energySet)
{
    if(!isValid())
    {
        return;
    }

    // Input values
    const qreal joule2Calor = 4.1868;
    const qreal gravityAccel = 9.81;    // kg * m / s2
    const qreal muscleCoeff = 23;       // %
    const qreal pedalRange = 70;        // Degree °
    const qreal crankLength = 175;      // mm

    qreal totalWeight = energySet.driverWeight + energySet.bikeWeight;
    qreal airDensity = energySet.airDensity;
    qreal windSpeed = energySet.windSpeed;
    qreal pedalCadence = energySet.pedalCadence;
    qreal frontalArea = energySet.frontalArea;
    qreal windDragCoeff = energySet.windDragCoeff;
    qreal rollingCoeff = energySet.rollingCoeff;

    // Output values
    energySet.airResistForce = 0;
    energySet.rollResistForce = totalWeight * gravityAccel * rollingCoeff;
    energySet.gravitySlopeForce = 0;
    energySet.sumForce = 0;
    energySet.positivePedalForce = 0;
    energySet.power = 0;
    energySet.positivePower = 0;
    energySet.powerMovingTime = 0;
    energySet.powerMovingTimeRatio = 0;
    energySet.energyKJoule = 0;

    qint32 cntPositivePowerPoints = 0;

    qreal pedalSpeed = crankLength * pedalCadence * 2 * M_PI / 60 / 1000;

    for(const CTrackData::trkpt_t &pt : trk.getTrackData())
    {
        if(pt.isHidden())
        {
            continue;
        }

        qreal speed = pt.speed;
        if (speed <= 0.2)           // 0.2 ==> to be synchron with deriveSecondaryData()
        {
            continue;
        }

        qreal slope = pt.slope2;

        qreal airResistForce = 0.5 * windDragCoeff * frontalArea * airDensity * qPow(speed + windSpeed, 2);

        if ((speed + windSpeed) < 0)
        {
            airResistForce *= -1;
        }
        qreal gravitySlopeForce = totalWeight * gravityAccel * slope / 100;
        energySet.airResistForce += airResistForce;
        energySet.gravitySlopeForce += gravitySlopeForce;
        energySet.sumForce += airResistForce + gravitySlopeForce + energySet.rollResistForce;

        qreal power = (qAbs(airResistForce) * (speed + windSpeed)) + ((energySet.rollResistForce + gravitySlopeForce) * speed);
        energySet.power += power;

        if (power > 0)
        {
            qreal deltaPowerTime = pt.deltaDistance / speed;
            energySet.powerMovingTime += deltaPowerTime;
            energySet.positivePower += power;
            energySet.energyKJoule += power * deltaPowerTime / muscleCoeff / 1000 * 100;
            energySet.positivePedalForce += power / pedalSpeed * 180 / pedalRange;
            cntPositivePowerPoints++;
        }
    }

    qint32 cntVisiblePoints = trk.getNumberOfVisiblePoints();
    if (cntVisiblePoints)
    {
        energySet.airResistForce /= cntVisiblePoints;
        energySet.gravitySlopeForce /= cntVisiblePoints;
        energySet.sumForce /= cntVisiblePoints;
        energySet.power /= cntVisiblePoints;
    }

    qreal totalElapsedSecondsMoving = trk.getTotalElapsedSecondsMoving();
    if(totalElapsedSecondsMoving)
    {
        energySet.powerMovingTimeRatio = energySet.powerMovingTime / totalElapsedSecondsMoving;
    }

    if(cntPositivePowerPoints)
    {
        energySet.positivePedalForce /= cntPositivePowerPoints;
        energySet.positivePower /= cntPositivePowerPoints;
    }
    energySet.energyKcal = energySet.energyKJoule / joule2Calor;
}

/* Set the "Energy Use Cycling" value to NOFLOAT, indicates a remove
 * and update the info panel, to noshow the value there
*/
void CEnergyCycling::remove()
{
    energyTrkSet.energyKcal = NOFLOAT;
    trk.updateHistory(CGisItemTrk::eVisualDetails);
}

/*
 * time and elevation and slope needs to be valid for computation algo
 * Otherwise toolButton in status panel will be disabled
*/
bool CEnergyCycling::isValid() const
{
    if(!trk.isTrkTimeValid() || trk.isTrkElevationInvalid() || trk.isTrkSlopeInvalid())
    {
        return false;
    }
    return true;
}

/* Will be used:
 *    - by serialization to load the parameter from the stream to the track
 *    - by the dialog when clicking Ok, saving temporarily parameters to the track
*/
void CEnergyCycling::setEnergyTrkSet(const energy_set_t &energyTrkSet, bool updateHistory)
{
    this->energyTrkSet = energyTrkSet;
    if (updateHistory)
    {
        trk.updateHistory(CGisItemTrk::eVisualDetails);
    }
}
