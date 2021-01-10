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

/** @brief Constructor - Load parameter from SETTINGS to the track's parameter set

   @param trk The track for which the "Energy Use Cycling" value should computed
 */
CEnergyCycling::CEnergyCycling(CGisItemTrk &trk) :
    trk(trk)
{
    loadSettings(energyTrkSet);
}

/** @brief Loads parameters from SETTINGS

   On initial start (no parameters are saved in SETTINGS in file QMapShack.conf) the default parameters from header file be used.

   When modifying the parameters and clicking on button "Ok" parameters SETTINGS be saved TO SETTINGS. When loading a track
   (ex. from GPS device) with Energy Use = NOFLOAT the parameters from SETTING will be used and shown in the dialog.

   When modifying and clicking on "Ok" the parameters will be saved back to SETTINGS file QMapShack.conf.

   SETTINGS will be load either in the energyTrkSet belonging to the track at at initiation
   or in a energyTmpSet belonging to the Dialog. By clicking "Ok" in the dialog energyTmpSet will be saved back to track’s energyTrkSet.

   By clicking on buttons "Cancel" or "Remove" no changes will be made in track’s energyTrkSet and no SETTINGS will be saved.

   The button “Load previous Set” can be used for the following uses cases:
   - You load an old track with a computed Energy Use value and you will replace
     all the parameter with your current active parameter set
   - You have to “play” to much with all these parameters in the dialog and you
     are a bit confused, so you can load your last save parameter set
   - When clicking in history and you will use an older track to start again with modifying
     you can you also use your latest save parameter set
   - When you have some tracks in edit mode and you would like to update them all
     with the same parameter set, edit the first one and use “Load previous Set” for all the others

   @param energySet The parameter set in which the SETTINGS parameter will be load
 */
void CEnergyCycling::loadSettings(CEnergyCycling::energy_set_t &energySet)
{
    SETTINGS;
    cfg.beginGroup("EnergyCycling");

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

    cfg.endGroup();
}

/** @brief Saves parameters to SETTINGS

   @note Will be triggered by clicking on "ok" button in the dialog
 */
void CEnergyCycling::saveSettings()
{
    SETTINGS;
    cfg.beginGroup("EnergyCycling");

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

    cfg.endGroup();
}

/** @brief Computes the "Energy Use Cycling" value

    @note
      Computes only when "Energy Use Cycling" != NOFLOAT, which indicates a no-show in the info panel
      Computation based directly on the track parameter set
 */
void CEnergyCycling::compute()
{
    if (energyTrkSet.energyKcal != NOFLOAT)
    {
        compute(energyTrkSet);
    }
}

/**  @brief Computes the "Energy Use Cycling" value
   @note
   Used by dialog to compute temporarily parameter set in all cases

   Related information for the computation process and used formulas:
    - http://www.blog.ultracycle.net/2010/05/cycling-power-calculations (English)
    - http://www.cptips.com/energy.htm  (English)
    - http://www.tribology-abc.com/calculators/cycling.htm (English)
    - http://www.kreuzotter.de/deutsch/speed.htm (German)
    - http://horst-online.com/physik-des-fahrrads/index.html (German)
    - http://www.helpster.de/wirkungsgrad-vom-mensch-erklaerung_198168 (German)
    - http://www.wolfgang-menn.de/motion_d.htm (German)
    - http://www.msporting.com/planung/5_3_6%20Aerodynamik.htm (German)

   @param energySet The parameter set used for the computation algorithm (input and output values)
 */
void CEnergyCycling::compute(CEnergyCycling::energy_set_t &energySet)
{
    if(!isValid())
    {
        return;
    }

    // Input values
    constexpr qreal joule2Calor = 4.1868;
    constexpr qreal gravityAccel = 9.81;    // kg * m / s2
    constexpr qreal muscleCoeff = 23;       // %
    constexpr qreal pedalRange = 70;        // Degree °
    constexpr qreal crankLength = 175;      // mm

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

    qint32 cntPowerPoints = 0;            // Count the moving track points
    qint32 cntPositivePowerPoints = 0;    // Count the moving track point and positive force to the pedal

    qreal pedalSpeed = crankLength * pedalCadence * 2 * M_PI / 60 / 1000;

    const CTrackData::trkpt_t *lastTrkpt  = nullptr;

    for(const CTrackData::trkpt_t &pt : trk.getTrackData())
    {
        if(pt.isHidden())
        {
            continue;
        }

        if(lastTrkpt != nullptr)     // First track point will not considered
        {
            qreal deltaTime = (pt.time.toMSecsSinceEpoch() - lastTrkpt->time.toMSecsSinceEpoch()) / 1000.0;
            if(deltaTime > 0 && ((pt.deltaDistance / deltaTime) <= 0.2)) // 0.2 ==> to be synchron with deriveSecondaryData()
            {
                lastTrkpt = &pt;
                continue;            // Standstill - no moving, track point will not considered
            }

            qreal slope = pt.slope2;
            qreal speed = pt.speed;

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
            energySet.power += power; // Positive and negative power

            cntPowerPoints++;
            if (power > 0)
            {
                energySet.powerMovingTime += deltaTime;
                energySet.positivePower += power;  // Positive power only
                energySet.energyKJoule += power * deltaTime / muscleCoeff / 1000 * 100;
                energySet.positivePedalForce += power / pedalSpeed * 180 / pedalRange;
                cntPositivePowerPoints++;
            }
        }
        lastTrkpt = &pt;
    }

    if (cntPowerPoints)  // For all moving points
    {
        energySet.airResistForce /= cntPowerPoints;
        energySet.gravitySlopeForce /= cntPowerPoints;
        energySet.sumForce /= cntPowerPoints;
        energySet.power /= cntPowerPoints;
    }

    qreal totalElapsedSecondsMoving = trk.getTotalElapsedSecondsMoving(); // The track moving time
    if(totalElapsedSecondsMoving)
    {
        energySet.powerMovingTimeRatio = (quint32)energySet.powerMovingTime / totalElapsedSecondsMoving;
    }

    if(cntPositivePowerPoints) // For the moving points with positive force to the pedal
    {
        energySet.positivePedalForce /= cntPositivePowerPoints;
        energySet.positivePower /= cntPositivePowerPoints;
    }
    energySet.energyKcal = energySet.energyKJoule / joule2Calor;  // The final energy use cycling value to show in the info panel
}

/** @brief Set the "Energy Use Cycling" value to NOFLOAT which indicates a remove

   Updates the info panel to noshow the "Energy Use Cycling" value
 */
void CEnergyCycling::remove()
{
    energyTrkSet.energyKcal = NOFLOAT;
    trk.updateHistory(CGisItemTrk::eVisualDetails);
}

/** @brief Verifies whether the track is valid to compute the "Energy Use Cycling" value

   @note
   Time, elevation and slope needs to be valid for computation algorithm. On invaild toolButton in status panel will be disabled.
 */
bool CEnergyCycling::isValid() const
{
    if(!trk.isTrkTimeValid() || trk.isTrkElevationInvalid() || trk.isTrkSlopeInvalid())
    {
        return false;
    }
    return true;
}

/** @brief Sets a parameter set to the track

   @note
   Will be used:
    - by serialization to load the parameter from the stream to the track
    - by the dialog when clicking Ok, saving temporarily parameters back to the track

   @param energyTrkSet The parameter set which will be set to the track
   @param updateHistory Set to true to trigger an update of history and info panel
 */
void CEnergyCycling::setEnergyTrkSet(const energy_set_t &energyTrkSet, bool updateHistory)
{
    this->energyTrkSet = energyTrkSet;
    if (updateHistory)
    {
        trk.updateHistory(CGisItemTrk::eVisualDetails);
    }
}
