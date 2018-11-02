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

#ifndef CFILTERENERGYCYCLEDLG_H
#define CFILTERENERGYCYCLEDLG_H

#include "ui_IFilterEnergyCycleDlg.h"
#include "gis/trk/filter/CFilterEnergyCycle.h"
#include <QDialog>

class CGisItemTrk;

class CFilterEnergyCycleDlg : public QDialog, private Ui::IFilterEnergyCycleDlg
{
    Q_OBJECT

public:
    explicit CFilterEnergyCycleDlg(QWidget *parent, /*const*/ CGisItemTrk &trk, CFilterEnergyCycle::energy_set_t &tmpEnergySet);
    ~CFilterEnergyCycleDlg();

private slots:
    void slotOk(bool);
    void slotApply(bool);
    void slotSetNameOfSet();
    void slotSetWeight(qreal);
    void slotSetComboWindSpeed(qint32 windSpeedIndex);
    void slotSetWindSpeed(qreal windSpeed);
    void slotSetAirDensity(qreal airDensity);
    void slotSetComboWindPosition(qint32 windPositionIndex);
    void slotSetWindPositionSpins(qreal);
    void slotSetComboGround(qint32 groundIndex);
    void slotSetRollingCoeff(qreal rollingCoeff);
    void slotSetPedalCadence(qreal pedalCadence);

private:
    struct wind_speed_t
    {
        QString name;
        qreal windSpeed;
    };
    const QList <wind_speed_t> windSpeeds;

    struct wind_position_t
    {
        QString name;
        qreal frontalArea;
        qreal windDragCoeff;
    };
    const QList <wind_position_t> windPositions;

    struct ground_condition_t
    {
        QString name;
        qreal rollingCoeff;
    };
    const QList <ground_condition_t> grounds;

    CGisItemTrk &trk;
    CFilterEnergyCycle::energy_set_t &energySet;
    CFilterEnergyCycle::energy_set_t tmpEnergySet;
};

#endif // CFILTERENERGYCYCLEDLG_H
