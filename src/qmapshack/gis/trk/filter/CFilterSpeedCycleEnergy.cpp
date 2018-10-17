#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterSpeedCycleEnergy.h"
#include "gis/trk/filter/CFilterSpeedCycle.h"


CFilterSpeedCycleEnergy::CFilterSpeedCycleEnergy(QWidget *parent, /*const*/ CGisItemTrk &trk, CFilterSpeedCycle::energy_set_t &energySet) :
    QDialog(parent)
  , windSpeeds
    {
        {tr("User defined"),        NOFLOAT},        // name, windSpeed km/h ==> m/s
        {"-4 " + tr("Tailwind"),    -24.0 / 3.6},
        {"-3 " + tr("Tailwind"),    -15.5 / 3.6},
        {"-2 " + tr("Tailwind"),     -8.5 / 3.6},
        {"-1 " + tr("Tailwind"),     -3.0 / 3.6},
        {tr("0 No Wind"), 0},
        {"+1 " + tr("Headwind"),     +3.0 / 3.6},
        {"+2 " + tr("Headwind"),     +8.5 / 3.6},
        {"+3 " + tr("Headwind"),    +15.5 / 3.6},
        {"+4 " + tr("Headwind"),    +24.0 / 3.6}
    }
  , windPositions
    {
        {tr("User defined"),        NOFLOAT, NOFLOAT}, // name, frontalArea, windDragCoeff
        {tr("City"),                0.70, 1.10},
        {tr("Trekking"),            0.65, 1.00},
        {tr("Trekking w. Bags"),    0.80, 1.10},
        {tr("Sportive"),            0.48, 0.50},
        {tr("Race"),                0.32, 0.30},
        {tr("Mountain"),            0.60, 0.90}
    }
  , grounds
    {
        {tr("User defined"),            NOFLOAT}, // name, rollingCoeff
        {tr("Cycling race track"),      0.002},
        {tr("Asphalt, small tire"),     0.004},
        {tr("Asphalt, normal tire"),    0.005},
        {tr("Mix Asphalt+Forest"),      0.007},
        {tr("Mountain Bike Trail"),     0.015},
        {tr("Hard Forest Path"),        0.010},
        {tr("Soft Forest Path"),        0.050},
        {tr("Sand"),                    0.300}
    }
  , trk(trk)
  , energySet(energySet)
{
    setupUi(this);

    tmpEnergySet = energySet; // Changes will be made first in tmpEnergySet before saving back on OK button

    spinPersonalWeight->setValue(tmpEnergySet.personalWeight);
    spinBikeWeight->setValue(tmpEnergySet.bikeWeight);
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.personalWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));

    for (QList<wind_speed_t>::const_iterator it = windSpeeds.constBegin(); it != windSpeeds.constEnd(); ++it)
    {
        comboWindSpeed->addItem(it->name);
    }
    comboWindSpeed->setCurrentIndex(tmpEnergySet.windSpeedIndex);
    if (tmpEnergySet.windSpeedIndex > 0)
    {
        spinWindSpeed->setValue(windSpeeds[tmpEnergySet.windSpeedIndex].windSpeed * 3.6); // m/s ==> km/h
    }
    else
    {
        spinWindSpeed->setValue(tmpEnergySet.windSpeed);
    }

    spinAirDensity->setValue(tmpEnergySet.airDensity);

    for (QList<wind_position_t>::const_iterator it = windPositions.constBegin(); it != windPositions.constEnd(); ++it)
    {
        comboWindPosition->addItem(it->name);
    }
    comboWindPosition->setCurrentIndex(tmpEnergySet.windPositionIndex);
    if (tmpEnergySet.windPositionIndex > 0)
    {
        spinFrontalArea->setValue(windPositions[tmpEnergySet.windPositionIndex].frontalArea);
        spinWindDragCoeff->setValue(windPositions[tmpEnergySet.windPositionIndex].windDragCoeff);
    }
    else
    {
        spinFrontalArea->setValue(tmpEnergySet.frontalArea);
        spinWindDragCoeff->setValue(tmpEnergySet.windDragCoeff);
    }

    for (QList<ground_condition_t>::const_iterator it = grounds.constBegin(); it != grounds.constEnd(); ++it)
    {
        comboGround->addItem(it->name);
    }
    comboGround->setCurrentIndex(tmpEnergySet.groundIndex);
    if (tmpEnergySet.groundIndex > 0)
    {
        spinRollingCoeff->setValue(grounds[tmpEnergySet.groundIndex].rollingCoeff);
    }
    else
    {
        spinRollingCoeff->setValue(tmpEnergySet.rollingCoeff);
    }

    spinMuscleEff->setValue(tmpEnergySet.muscleEff);
    spinPedalCadence->setValue(tmpEnergySet.pedalCadence);
    spinPedalRange->setValue(tmpEnergySet.pedalRange);
    spinCrankLength->setValue(tmpEnergySet.crankLength);

    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)), this, SLOT(slotApply(bool)));
    connect(spinPersonalWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));
    connect(spinBikeWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));
    connect(comboWindSpeed, SIGNAL(activated(int)), this, SLOT(slotSetComboWindSpeed(int)));
    connect(spinWindSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindSpeed(double)));
    connect(spinAirDensity, SIGNAL(valueChanged(double)), this, SLOT(slotSetAirDensity(double)));
    connect(comboWindPosition, SIGNAL(activated(int)), this, SLOT(slotSetComboWindPosition(int)));
    connect(spinFrontalArea, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindPositionSpins(double)));
    connect(spinWindDragCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindPositionSpins(double)));
    connect(comboGround, SIGNAL(activated(int)), this, SLOT(slotSetComboGround(int)));
    connect(spinRollingCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotSetRollingCoeff(double)));
    connect(spinMuscleEff, SIGNAL(valueChanged(double)), this, SLOT(slotSetMuscleEff(double)));
    connect(spinPedalCadence, SIGNAL(valueChanged(double)), this, SLOT(slotSetPedalCadence(double)));
    connect(spinPedalRange, SIGNAL(valueChanged(double)), this, SLOT(slotSetPedalRange(double)));
    connect(spinCrankLength, SIGNAL(valueChanged(double)), this, SLOT(slotSetCrankLength(double)));

    slotApply(true);
 }

CFilterSpeedCycleEnergy::~CFilterSpeedCycleEnergy()
{
}

void CFilterSpeedCycleEnergy::slotOk(bool)
{
    slotApply(true);
    energySet = tmpEnergySet; // Saving back
    accept();
}

void CFilterSpeedCycleEnergy::slotApply(bool)
{
    trk.filterSpeedCycleEnergy(tmpEnergySet);

    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.personalWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));

    labelWindResistForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.windResistanceForce, 0, 'f', 1));
    labelRollingResistForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.rollingResistanceForce, 0, 'f', 1));
    labelGravitySlopeForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.gravitySlopeForce, 0, 'f', 1));
    labelTotalForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.totalForce, 0, 'f', 1));

    QString val, unit;
    IUnit::self().seconds2time(tmpEnergySet.totalPowerTime, val, unit);
    labelTotalPowerTime->setText(QString("<b>%1%2</b>").arg(val).arg(unit));
    labelPowerTimeMovingRatio->setText(QString("<b>%1%</b>").arg(tmpEnergySet.powerTimeMovingRatio * 100, 0, 'f', 1));

    labelTotalPower->setText(QString("<b>%1W</b>").arg(tmpEnergySet.totalPower, 0, 'f', 1));
    labelTotalPositivePower->setText(QString("<b>%1W</b>").arg(tmpEnergySet.totalPositivePower, 0, 'f', 1));

    labelEnergyKJoule->setText(QString("<b>%1kJ</b>").arg(tmpEnergySet.totalEnergyKJoule, 0, 'f', 0));
    labelEnergyKcal->setText(QString("<b><u>%1kcal</u></b>").arg(tmpEnergySet.totalEnergyKcal, 0, 'f', 0));

    labelPositivePedalForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.totalPositivePedalForce, 0, 'f', 1));
}

void CFilterSpeedCycleEnergy::slotSetWeight(double)
{
    tmpEnergySet.personalWeight = spinPersonalWeight->value();
    tmpEnergySet.bikeWeight = spinBikeWeight->value();
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.personalWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));
}

void CFilterSpeedCycleEnergy::slotSetComboWindSpeed(qint32 index)
{
    tmpEnergySet.windSpeedIndex = index;
    if (index > 0)
    {
        spinWindSpeed->setValue(windSpeeds[index].windSpeed * 3.6); // m/s ==> km/h
        tmpEnergySet.windSpeed = windSpeeds[index].windSpeed;
    }
}

void CFilterSpeedCycleEnergy::slotSetWindSpeed(double windSpeed)
{
    tmpEnergySet.windSpeed = windSpeed / 3.6;

    tmpEnergySet.windSpeedIndex = 0;
    comboWindSpeed->setCurrentIndex(0);
    for (qint32 i = 1; i < windSpeeds.size(); ++i)
    {
        if (tmpEnergySet.windSpeed == windSpeeds[i].windSpeed)
        {
            tmpEnergySet.windSpeedIndex = i;
            comboWindSpeed->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterSpeedCycleEnergy::slotSetAirDensity(double airDensity)
{
    tmpEnergySet.airDensity = airDensity;
}

void CFilterSpeedCycleEnergy::slotSetComboWindPosition(qint32 index)
{
    tmpEnergySet.windPositionIndex = index;
    if (index > 0)
    {
        spinFrontalArea->setValue(windPositions[index].frontalArea);
        spinWindDragCoeff->setValue(windPositions[index].windDragCoeff);
        tmpEnergySet.frontalArea = windPositions[index].frontalArea;
        tmpEnergySet.windDragCoeff = windPositions[index].windDragCoeff;
    }
}

void CFilterSpeedCycleEnergy::slotSetWindPositionSpins(double)
{
    tmpEnergySet.frontalArea = spinFrontalArea->value();
    tmpEnergySet.windDragCoeff = spinWindDragCoeff->value();

    tmpEnergySet.windPositionIndex = 0;
    comboWindPosition->setCurrentIndex(0);
    for (qint32 i = 1; i < windPositions.size(); ++i)
    {
        if (tmpEnergySet.frontalArea == windPositions[i].frontalArea &&
                tmpEnergySet.windDragCoeff == windPositions[i].windDragCoeff)
        {
            tmpEnergySet.windPositionIndex = i;
            comboWindPosition->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterSpeedCycleEnergy::slotSetComboGround(qint32 index)
{
    tmpEnergySet.groundIndex = index;
    if (index > 0)
    {
        spinRollingCoeff->setValue(grounds[index].rollingCoeff);
        tmpEnergySet.rollingCoeff = grounds[index].rollingCoeff;
    }
}

void CFilterSpeedCycleEnergy::slotSetRollingCoeff(double rollingCoeff)
{
    tmpEnergySet.rollingCoeff = rollingCoeff;

    tmpEnergySet.groundIndex = 0;
    comboGround->setCurrentIndex(0);
    for (qint32 i = 1; i < grounds.size(); ++i)
    {
        if (rollingCoeff == grounds[i].rollingCoeff)
        {
            tmpEnergySet.groundIndex = i;
            comboGround->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterSpeedCycleEnergy::slotSetMuscleEff(double muscleEff)
{
    tmpEnergySet.muscleEff = muscleEff;
}

void CFilterSpeedCycleEnergy::slotSetPedalCadence(double pedalCadence)
{
    tmpEnergySet.pedalCadence = pedalCadence;
}

void CFilterSpeedCycleEnergy::slotSetPedalRange(double pedalRange)
{
    tmpEnergySet.pedalRange = pedalRange;
}

void CFilterSpeedCycleEnergy::slotSetCrankLength(double crankLength)
{
    tmpEnergySet.crankLength = crankLength;
}
