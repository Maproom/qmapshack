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

class CEnergyCycling
{
public:
    struct energy_set_t                         // set of parameters (input and output) to compute "Energy Use Cylcing" value
    {
        qreal driverWeight          = 75;
        qreal bikeWeight            = 15;
        qreal airDensity            = 1.2;
        qint32 windSpeedIndex       = 5;
        qreal windSpeed             = 0;
        qint32 windPositionIndex    = 2;
        qreal frontalArea           = 0.65;
        qreal windDragCoeff         = 1.0;
        qint32 groundIndex          = 3;
        qreal rollingCoeff          = 0.005;
        qreal pedalCadence          = 75;
        qreal airResistForce        = NOFLOAT;
        qreal gravitySlopeForce     = NOFLOAT;
        qreal rollResistForce       = NOFLOAT;
        qreal sumForce              = NOFLOAT;
        qreal powerMovingTime       = NOFLOAT;
        qreal power                 = NOFLOAT;
        qreal positivePower         = NOFLOAT;
        qreal powerMovingTimeRatio  = NOFLOAT;
        qreal energyKJoule          = NOFLOAT;
        qreal energyKcal            = NOFLOAT;
        qreal positivePedalForce    = NOFLOAT;
    };

    CEnergyCycling(CGisItemTrk &trk);
    virtual ~CEnergyCycling() = default;

    const energy_set_t &getEnergyTrkSet() const { return energyTrkSet; }
    void setEnergyTrkSet(const energy_set_t &energyTrkSet, bool updateHistory); // Used to saved the temporarily modified parameter set back to the track

    qreal getEnergyUseCycling() const { return energyTrkSet.energyKcal; }  // Returns the "Energy Use Cylcing" value shown in status panel

    void compute();                             // Computes "Energy Use Cylcing" and more output values based on the input shown in the dialog
    void compute(energy_set_t &energySet);
    void remove();                              // Remove the "Energy Use Cylcing" value from status panel

    bool isValid() const;

    void loadSettings(energy_set_t &energySet); // Load the setting directly in the track or temporarily in the dialog to modify first
    void saveSettings();

private:
    energy_set_t energyTrkSet;                  // The set of parameters belongig to the track

    CGisItemTrk &trk;                           // reference to the track, non-const, due to update of the parameter set belonging to the track
};

#endif // CENERGYCYCLING_H
