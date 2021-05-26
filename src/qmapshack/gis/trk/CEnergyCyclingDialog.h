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

#ifndef CENERGYCYCLINGDIALOG_H
#define CENERGYCYCLINGDIALOG_H

#include "ui_IEnergyCyclingDialog.h"

class CEnergyCycling;
class CGisItemTrk;

/** @brief GUI Dialog class to modify the CEnergyCycling parameter set
 */
class CEnergyCyclingDialog : public QDialog, private Ui::IEnergyCyclingDialog
{
    Q_OBJECT

public:
    explicit CEnergyCyclingDialog(CEnergyCycling& energyCycling, QWidget* parent);
    ~CEnergyCyclingDialog();

private slots:
    void slotOk(bool);
    void slotApply(bool);
    void slotLoadFromSettings(bool);
    void slotRemove(bool);
    void slotSetDriverWeight(qreal weight);
    void slotSetBikeWeight(qreal weight);
    void slotSetComboWindSpeed(qint32 windSpeedIndex);
    void slotSetWindSpeed(qreal windSpeed);
    void slotSetAirDensity(qreal airDensity);
    void slotSetComboWindPosition(qint32 windPositionIndex);
    void slotSetFrontalAreaSpin(qreal frontalArea);
    void slotSetWindDragCoeffSpin(qreal windDragCoeff);
    void checkWindPositionSpins();
    void slotSetComboGround(qint32 groundIndex);
    void slotSetRollingCoeff(qreal rollingCoeff);
    void slotSetPedalCadence(qreal pedalCadence);
    void slotShowHelp();

private:
    struct wind_speed_t
    {
        QString name;
        qreal windSpeed;
    };
    const QList <wind_speed_t> windSpeeds =
    {
        {tr("User defined"), NOFLOAT},               // name, windSpeed km/h ==> m/s
        {"-4 " + tr("Tailwind"), -24.0 / 3.6},
        {"-3 " + tr("Tailwind"), -15.5 / 3.6},
        {"-2 " + tr("Tailwind"), -8.5 / 3.6},
        {"-1 " + tr("Tailwind"), -3.0 / 3.6},
        {"0 " + tr("No Wind"), 0         },
        {"+1 " + tr("Headwind"), +3.0 / 3.6},
        {"+2 " + tr("Headwind"), +8.5 / 3.6},
        {"+3 " + tr("Headwind"), +15.5 / 3.6},
        {"+4 " + tr("Headwind"), +24.0 / 3.6}
    };

    struct wind_position_t
    {
        QString name;
        qreal frontalArea;
        qreal windDragCoeff;
    };
    const QList <wind_position_t> windPositions =
    {
        {tr("User defined"), NOFLOAT, NOFLOAT},        // name, frontalArea, windDragCoeff
        {tr("City"), 0.70, 1.10},
        {tr("Trekking"), 0.65, 1.00},
        {tr("Trekking w. Bags"), 0.80, 1.10},
        {tr("Sportive"), 0.48, 0.50},
        {tr("Race"), 0.32, 0.30},
        {tr("Mountain"), 0.60, 0.90}
    };

    struct ground_condition_t
    {
        QString name;
        qreal rollingCoeff;
    };
    const QList <ground_condition_t> grounds =
    {
        {tr("User defined"), NOFLOAT},            // name, rollingCoeff
        {tr("Cycling race track"), 0.002},
        {tr("Asphalt, small tyres"), 0.004},
        {tr("Asphalt, normal tyres"), 0.005},
        {tr("Mix Asphalt+Forest"), 0.007},
        {tr("Mountain Bike Trail"), 0.015},
        {tr("Hard Forest Path"), 0.010},
        {tr("Soft Forest Path"), 0.050},
        {tr("Sand"), 0.300}
    };

    CEnergyCycling& energyCycling;   //!< Reference to tracks CEnergyCycling object
    CEnergyCycling::energy_set_t energyTmpSet;  //!< Ttemporarily parameter set to be used in the dialog

    void updateUi();
};

#endif // CENERGYCYCLINGDIALOG_H
