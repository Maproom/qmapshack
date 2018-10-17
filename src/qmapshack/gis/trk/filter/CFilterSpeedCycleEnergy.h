#ifndef CFILTERSPEEDCYCLEENERGY_H
#define CFILTERSPEEDCYCLEENERGY_H

#include "ui_IFilterSpeedCycleEnergy.h"

#include "gis/trk/filter/CFilterSpeedCycle.h"

#include <QDialog>

class CGisItemTrk;

class CFilterSpeedCycleEnergy : public QDialog, private Ui::CFilterSpeedCycleEnergy
{
    Q_OBJECT

public:
    explicit CFilterSpeedCycleEnergy(QWidget *parent, CGisItemTrk &trk, CFilterSpeedCycle::energy_set_t &tmpEnergySet);
    ~CFilterSpeedCycleEnergy();

private slots:
    void slotOk(bool);
    void slotApply(bool);
    void slotSetWeight(double);
    void slotSetComboWindSpeed(qint32 index);
    void slotSetWindSpeed(double windSpeed);
    void slotSetAirDensity(double airDensity);
    void slotSetComboWindPosition(qint32 index);
    void slotSetWindPositionSpins(double);
    void slotSetComboGround(qint32 index);
    void slotSetRollingCoeff(double rollingCoeff);
    void slotSetMuscleEff(double muscleEff);
    void slotSetPedalCadence(double pedalCadence);
    void slotSetPedalRange(double pedalRange);
    void slotSetCrankLength(double crankLength);

private:
//    void compute();

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

    /*const*/ CGisItemTrk &trk;
    CFilterSpeedCycle::energy_set_t &energySet;
    CFilterSpeedCycle::energy_set_t tmpEnergySet;
};

#endif // CFILTERSPEEDCYCLEENERGY_H
