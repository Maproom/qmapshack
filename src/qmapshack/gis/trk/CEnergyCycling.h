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

#ifndef CENERGYCYCLING_H
#define CENERGYCYCLING_H

class CGisItemTrk;
class CTrackData;

/** @brief Class to compute the "Energy Use Cycling" value

   With this functionality your personal energy use (consumption) for a cycling tour can be computed.

   The computed value of "Energy Use Cycling" can be see as an indicator for the exertion of a cycling tour.

   The tour length, speed and slope values will be taken into account.

   To individualize your personal energy use the following input data are more needed:
    - Driver and bicyle weight
    - Air density, wind speed and position to the wind to consider the wind drag resistance
    - Ground situation (tyre and ground) to consider the rolling resistance
    - Average pedal cadence for the computation of pedal force

   The individualize data will be defined in its own dialog (see CEnergyCyclingDialog) and more computed values will be shown here.

   When loading older tracks or switching in history to tracks with a different parameter set compared to the previous saved parameter set, the shown parameter set in this dialog can be replaced by the previous saved parameter set."

   The energy use in unit "kcal" will be stored in the track and can be remove later on when no longer needed.

    For more information see tooltips on input and output values in the dialog.
 */
class CEnergyCycling
{
public:
/** @brief The parameter set structure (input and output values) to compute "Energy Use Cycling" value
 */
    struct energy_set_t
    {
        qreal driverWeight = 75;
        qreal bikeWeight = 15;
        qreal airDensity = 1.2;
        qint32 windSpeedIndex = 5;
        qreal windSpeed = 0;
        qint32 windPositionIndex = 2;
        qreal frontalArea = 0.65;
        qreal windDragCoeff = 1.0;
        qint32 groundIndex = 3;
        qreal rollingCoeff = 0.005;
        qreal pedalCadence = 75;
        qreal airResistForce = NOFLOAT;
        qreal gravitySlopeForce = NOFLOAT;
        qreal rollResistForce = NOFLOAT;
        qreal sumForce = NOFLOAT;
        qreal powerMovingTime = NOFLOAT;
        qreal power = NOFLOAT;
        qreal positivePower = NOFLOAT;
        qreal powerMovingTimeRatio = NOFLOAT;
        qreal energyKJoule = NOFLOAT;
        qreal energyKcal = NOFLOAT;
        qreal positivePedalForce = NOFLOAT;
    };

    CEnergyCycling(CGisItemTrk& trk);
    virtual ~CEnergyCycling() = default;

    /** @brief Get the track's parameter set

       @return The track's parameter set
     */
    const energy_set_t& getEnergyTrkSet() const { return energyTrkSet; }

    void setEnergyTrkSet(const energy_set_t& energyTrkSet, bool updateHistory);

    /** @brief Get the "Energy Use Cycling" value from the track's parameter set

       @return The "Energy Use Cycling" value
     */
    qreal getEnergyUseCycling() const { return energyTrkSet.energyKcal; }

    void compute();
    void compute(energy_set_t& energySet);
    void remove();

    bool isValid() const;

    void loadSettings(energy_set_t& energySet);
    void saveSettings();

private:
    energy_set_t energyTrkSet;      //!< The track's parameter set
    CGisItemTrk& trk;               //!< reference to the track, non-const, due to update of parameter set energyTrkSet
};

#endif // CENERGYCYCLING_H
