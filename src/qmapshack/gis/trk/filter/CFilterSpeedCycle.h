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

    struct cycling_energy_t
    {
        qreal totalWeight;
        qreal windDragCoeff;
        qreal frontalArea;
        qreal airDensity;
        qreal rollingCoeff;
        qreal windSpeed;
        qreal muscleCoeff;
        qreal crankLength;
        qreal pedalCadence;
        qreal pedalRangeEff;
        qreal totalEnergyKcal;
    };

    CFilterSpeedCycle(QWidget *parent, CGisItemTrk &trk);
    virtual ~CFilterSpeedCycle() = default;

    void loadSettings(QSettings &cfg);
    void saveSettings(QSettings &cfg);
    void apply(CGisItemTrk& trk);

private slots:
    void slotSetCyclingType(int type);
    void slotSetPlainSpeed(double speed);
    void slotSetMinSpeed(double speed);
    void slotSetSlopeAtMinSpeed(double speed);
    void slotSetMaxSpeed(double speed);
    void slotSetSlopeAtMaxSpeed(double slope);
    void slotSetMinMaxSlopes(bool);
    void slotEnergy();

private:
    void insertEnergy();

    const CGisItemTrk &trk; // Needed for filterGetSlopeLimits
    const qint32 noOfFixTypes;
    const qint32 noOfCustomTypes;
    const QList<cycling_type_t> cyclingTypeDefaults;
    QList <cycling_type_t> cyclingTypes;

    cycling_energy_t energy;
};

#endif //CFILTERSPEEDCYCLE_H

