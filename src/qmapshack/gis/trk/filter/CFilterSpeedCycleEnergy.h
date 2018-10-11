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
    explicit CFilterSpeedCycleEnergy(QWidget *parent, const CGisItemTrk &trk, CFilterSpeedCycle::cycling_energy_t &energy);
    ~CFilterSpeedCycleEnergy();

private slots:
    void slotApply(bool);
    void slotSetWeight(double);


private:

    struct wind_speed_t
    {
        QString bft;
        qreal kmh;
    };
    const QList <wind_speed_t> windSpeeds;

    const CGisItemTrk &trk;
    CFilterSpeedCycle::cycling_energy_t &energy;
};

#endif // CFILTERSPEEDCYCLEENERGY_H
