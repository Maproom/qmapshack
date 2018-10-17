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

#ifndef CFILTERSPEEDCYCLE_H
#define CFILTERSPEEDCYCLE_H

#include "ui_IFilterSpeedCycle.h"
//#include "gis/trk/filter/CFilterSpeedCycleEnergy.h"

class CGisItemTrk;
class QSettings;

class CFilterSpeedCycle : public QWidget, private Ui::IFilterSpeedCycle
{
    Q_OBJECT

public:
    struct cycling_type_t
    {
        QString name;
        qreal plainSpeed;
        qreal minSpeed;
        qreal slopeAtMinSpeed;
        qreal maxSpeed;
        qreal slopeAtMaxSpeed;
    };

    struct energy_set_t
    {
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
        qreal muscleEff;
        qreal pedalCadence;
        qreal pedalRange;
        qreal crankLength;
        qreal windResistanceForce;
        qreal gravitySlopeForce;
        qreal rollingResistanceForce;
        qreal sumForce;
        qreal totalPowerTime;
        qreal totalPower;
        qreal totalPositivePower;
        qreal powerTimeMovingRatio;
        qreal totalEnergyKJoule;
        qreal totalEnergyKcal;
        qreal totalPositivePedalForce;
    };

    CFilterSpeedCycle(QWidget *parent, CGisItemTrk &trk);
    virtual ~CFilterSpeedCycle() = default;

    void loadSettings(QSettings &cfg);
    void saveSettings(QSettings &cfg);
    void apply(CGisItemTrk& trk);
    void updateUi();

private slots:
    void slotSetCyclingType(int type);
    void slotSetPlainSpeed(double speed);
    void slotSetMinSpeed(double speed);
    void slotSetSlopeAtMinSpeed(double speed);
    void slotSetMaxSpeed(double speed);
    void slotSetSlopeAtMaxSpeed(double slope);
    void slotSetMinMaxSlopes(bool);
    void slotApplyEnergy();

private:
    void insertEnergy(bool valid);

    /*const*/ CGisItemTrk &trk; // Needed for filterGetSlopeLimits
    const qint32 noOfFixTypes;
    const qint32 noOfCustomTypes;
    const QList<cycling_type_t> cyclingTypeDefaults;
    const QList <energy_set_t> energySetDefaults;

    QList <cycling_type_t> cyclingTypes;
    QList <energy_set_t> energySets;
};

#endif //CFILTERSPEEDCYCLE_H
