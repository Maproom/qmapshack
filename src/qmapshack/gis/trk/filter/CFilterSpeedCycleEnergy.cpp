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

    spinDriverWeight->setValue(tmpEnergySet.driverWeight);
    spinBikeWeight->setValue(tmpEnergySet.bikeWeight);
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.driverWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));

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
        spinWindSpeed->setValue(tmpEnergySet.windSpeed * 3.6);
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

    spinPedalCadence->setValue(tmpEnergySet.pedalCadence);

    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)), this, SLOT(slotApply(bool)));
    connect(spinDriverWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));
    connect(spinBikeWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));
    connect(comboWindSpeed, SIGNAL(activated(int)), this, SLOT(slotSetComboWindSpeed(int)));
    connect(spinWindSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindSpeed(double)));
    connect(spinAirDensity, SIGNAL(valueChanged(double)), this, SLOT(slotSetAirDensity(double)));
    connect(comboWindPosition, SIGNAL(activated(int)), this, SLOT(slotSetComboWindPosition(int)));
    connect(spinFrontalArea, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindPositionSpins(double)));
    connect(spinWindDragCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindPositionSpins(double)));
    connect(comboGround, SIGNAL(activated(int)), this, SLOT(slotSetComboGround(int)));
    connect(spinRollingCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotSetRollingCoeff(double)));
    connect(spinPedalCadence, SIGNAL(valueChanged(double)), this, SLOT(slotSetPedalCadence(double)));

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

    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.driverWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));

    labelAirResistForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.airResistForce, 0, 'f', 1));
    labelRollResistForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.rollResistForce, 0, 'f', 1));
    labelGravitySlopeForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.gravitySlopeForce, 0, 'f', 1));
    labelSumForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.sumForce, 0, 'f', 1));

    QString val, unit;
    IUnit::self().seconds2time(tmpEnergySet.powerMovingTime, val, unit);
    labelPowerMovingTime->setText(QString("<b>%1%2</b>").arg(val).arg(unit));
    labelPowerMovingTimeRatio->setText(QString("<b>%1%</b>").arg(tmpEnergySet.powerMovingTimeRatio * 100, 0, 'f', 1));

    labelPower->setText(QString("<b>%1W</b>").arg(tmpEnergySet.power, 0, 'f', 1));
    labelPositivePower->setText(QString("<b>%1W</b>").arg(tmpEnergySet.positivePower, 0, 'f', 1));

    labelEnergyKJoule->setText(QString("<b>%1kJ</b>").arg(tmpEnergySet.energyKJoule, 0, 'f', 0));
    labelEnergyKcal->setText(QString("<b><u>%1kcal</u></b>").arg(tmpEnergySet.energyKcal, 0, 'f', 0));

    labelPositivePedalForce->setText(QString("<b>%1N</b>").arg(tmpEnergySet.positivePedalForce, 0, 'f', 1));
}

void CFilterSpeedCycleEnergy::slotSetWeight(qreal)
{
    tmpEnergySet.driverWeight = spinDriverWeight->value();
    tmpEnergySet.bikeWeight = spinBikeWeight->value();
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.driverWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));
}

void CFilterSpeedCycleEnergy::slotSetComboWindSpeed(qint32 windSpeedIndex)
{
    tmpEnergySet.windSpeedIndex = windSpeedIndex;
    if (windSpeedIndex > 0)
    {
        spinWindSpeed->setValue(windSpeeds[windSpeedIndex].windSpeed * 3.6); // m/s ==> km/h
        tmpEnergySet.windSpeed = windSpeeds[windSpeedIndex].windSpeed;
    }
}

void CFilterSpeedCycleEnergy::slotSetWindSpeed(qreal windSpeed)
{
    if (qFuzzyIsNull(windSpeed)) // to avoid numerical noise
    {
        tmpEnergySet.windSpeed = 0;
        spinWindSpeed->setValue(0);
    }
    else
    {
        tmpEnergySet.windSpeed = windSpeed / 3.6;
    }

    tmpEnergySet.windSpeedIndex = 0;
    comboWindSpeed->setCurrentIndex(0);
    for (qint32 i = 1; i < windSpeeds.size(); ++i)
    {
        if (qFuzzyCompare(1 + tmpEnergySet.windSpeed, 1 + windSpeeds[i].windSpeed))
        {
            tmpEnergySet.windSpeedIndex = i;
            comboWindSpeed->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterSpeedCycleEnergy::slotSetAirDensity(qreal airDensity)
{
    tmpEnergySet.airDensity = airDensity;
}

void CFilterSpeedCycleEnergy::slotSetComboWindPosition(qint32 windPositionIndex)
{
    tmpEnergySet.windPositionIndex = windPositionIndex;
    if (windPositionIndex > 0)
    {
        spinFrontalArea->setValue(windPositions[windPositionIndex].frontalArea);
        spinWindDragCoeff->setValue(windPositions[windPositionIndex].windDragCoeff);
        tmpEnergySet.frontalArea = windPositions[windPositionIndex].frontalArea;
        tmpEnergySet.windDragCoeff = windPositions[windPositionIndex].windDragCoeff;
    }
}

void CFilterSpeedCycleEnergy::slotSetWindPositionSpins(qreal)
{
    tmpEnergySet.frontalArea = spinFrontalArea->value();
    tmpEnergySet.windDragCoeff = spinWindDragCoeff->value();

    tmpEnergySet.windPositionIndex = 0;
    comboWindPosition->setCurrentIndex(0);
    for (qint32 i = 1; i < windPositions.size(); ++i)
    {
        if (qFuzzyCompare(1 + tmpEnergySet.frontalArea, 1 + windPositions[i].frontalArea)
                && qFuzzyCompare(1 + tmpEnergySet.windDragCoeff, 1 + windPositions[i].windDragCoeff))
        {
            tmpEnergySet.windPositionIndex = i;
            comboWindPosition->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterSpeedCycleEnergy::slotSetComboGround(qint32 groundIndex)
{
    tmpEnergySet.groundIndex = groundIndex;
    if (groundIndex > 0)
    {
        spinRollingCoeff->setValue(grounds[groundIndex].rollingCoeff);
        tmpEnergySet.rollingCoeff = grounds[groundIndex].rollingCoeff;
    }
}

void CFilterSpeedCycleEnergy::slotSetRollingCoeff(qreal rollingCoeff)
{
    tmpEnergySet.rollingCoeff = rollingCoeff;

    tmpEnergySet.groundIndex = 0;
    comboGround->setCurrentIndex(0);
    for (qint32 i = 1; i < grounds.size(); ++i)
    {
        if (qFuzzyCompare(1 + rollingCoeff, 1 + grounds[i].rollingCoeff))
        {
            tmpEnergySet.groundIndex = i;
            comboGround->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterSpeedCycleEnergy::slotSetPedalCadence(qreal pedalCadence)
{
    tmpEnergySet.pedalCadence = pedalCadence;
}
