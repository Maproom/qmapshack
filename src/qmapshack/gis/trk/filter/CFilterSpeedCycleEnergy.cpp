#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterSpeedCycleEnergy.h"
#include "gis/trk/filter/CFilterSpeedCycle.h"


CFilterSpeedCycleEnergy::CFilterSpeedCycleEnergy(QWidget *parent, const CGisItemTrk &trk, CFilterSpeedCycle::cycling_energy_t &energy) :
    QDialog(parent), windSpeeds
    {
        {tr("-4 Tailwind"), -24},
        {tr("-3 Tailwind"), -15.5},
        {tr("-2 Tailwind"), -8.5},
        {tr("-1 Tailwind"), -3},
        {tr("0 No Wind"), 0},
        {tr("+1 Headwind"), 3},
        {tr("+2 Headwind"), 8.5},
        {tr("+3 Headwind"), 15.5},
        {tr("+4 Headwind"), 24}
    }
    , trk(trk)
    , energy(energy)
{
    setupUi(this);
    spinPersonalWeight->setValue(energy.personalWeight);
    spinBikeWeight->setValue(energy.bikeWeight);
    labelTotalWeightRes->setText(QString("<b>%1kg</b>").arg(energy.personalWeight + energy.bikeWeight, 0, 'f', 1));

    QList<wind_speed_t>::const_iterator i;
    for (i = windSpeeds.constBegin(); i != windSpeeds.constEnd(); ++i)
    {
        comboWindSpeed->addItem(i->bft);

    }

    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)), this, SLOT(slotApply(bool)));
    connect(spinPersonalWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));
    connect(spinBikeWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));

}

CFilterSpeedCycleEnergy::~CFilterSpeedCycleEnergy()
{
}

void CFilterSpeedCycleEnergy::slotApply(bool)
{
    qDebug() << "Apply Pushed";
    trk.filterSpeedCycleEnergy(energy);
}

void CFilterSpeedCycleEnergy::slotSetWeight(double)
{
    energy.personalWeight=spinPersonalWeight->value();
    energy.bikeWeight=spinBikeWeight->value();
    labelTotalWeightRes->setText(QString("<b>%1kg</b>").arg(energy.personalWeight + energy.bikeWeight, 0, 'f', 1));

}
