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

#ifndef CFILTERENERGYCYCLE_H
#define CFILTERENERGYCYCLE_H

#include "ui_IFilterEnergyCycle.h"
#include <QWidget>

class CGisItemTrk;

class CFilterEnergyCycle : public QWidget, private Ui::IFilterEnergyCycle
{
    Q_OBJECT

public:

//    enum update_visual_e
//    {
//        eUpdateNone        = 0
//        , eUpdateByFilterCycle = 0x01
//        , eUpdateByFilterOthers   = 0x02
//    };

    struct energy_set_t
    {
        QString nameOfSet;
        qreal driverWeight;
        qreal bikeWeight;
        qreal airDensity;
        qint32 windSpeedIndex;
        qreal windSpeed;
        qint32 windPositionIndex;
        qreal frontalArea;
        qreal windDragCoeff;
        qint32 groundIndex;
        qreal rollingCoeff;
        qreal pedalCadence;
        qreal airResistForce;
        qreal gravitySlopeForce;
        qreal rollResistForce;
        qreal sumForce;
        qreal powerMovingTime;
        qreal power;
        qreal positivePower;
        qreal powerMovingTimeRatio;
        qreal energyKJoule;
        qreal energyKcal;
        qreal positivePedalForce;
    };

    CFilterEnergyCycle(CGisItemTrk &trk, QWidget *parent = 0);
    virtual ~CFilterEnergyCycle() = default;

    void updateUi();

private slots:
    void slotApply();
    void slotSetSetting(int set);
    void slotRemove(bool);
    void slotEditParameter(bool);

private:
    void loadSettings();
    void saveSettings();

    const qint32 noOfSets = 5;
    qint32 currentSet = 0;
    CGisItemTrk &trk;
    const energy_set_t energyDefaultSet;
    QList <energy_set_t> energySets;
};

#endif // CFILTERENERGYCYCLE_H
