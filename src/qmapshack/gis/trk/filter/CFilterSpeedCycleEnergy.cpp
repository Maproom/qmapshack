#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterSpeedCycleEnergy.h"
#include "gis/trk/filter/CFilterSpeedCycle.h"


CFilterSpeedCycleEnergy::CFilterSpeedCycleEnergy(QWidget *parent, const CGisItemTrk &trk, CFilterSpeedCycle::cycling_energy_t &energy) :
    QDialog(parent),
    trk(trk),
    energy(energy)
{
    setupUi(this);

    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)), this, SLOT(slotApply(bool)));

}

CFilterSpeedCycleEnergy::~CFilterSpeedCycleEnergy()
{
}

void CFilterSpeedCycleEnergy::slotApply(bool)
{
    qDebug() << "Apply Pushed";
    trk.filterSpeedCycleEnergy(energy);
}
