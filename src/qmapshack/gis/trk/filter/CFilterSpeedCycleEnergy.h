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
    explicit CFilterSpeedCycleEnergy(QWidget *parent, const CGisItemTrk &trk, CFilterSpeedCycle::energy_set_t &tmpEnergySet);
    ~CFilterSpeedCycleEnergy();

private slots:
    void slotOk(bool);
    void slotApply(bool);
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

    const CGisItemTrk &trk;
    CFilterSpeedCycle::energy_set_t &energySet;
    CFilterSpeedCycle::energy_set_t tmpEnergySet;
};

#endif // CFILTERSPEEDCYCLEENERGY_H
