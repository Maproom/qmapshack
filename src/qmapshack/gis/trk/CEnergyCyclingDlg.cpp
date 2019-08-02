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

#include "CMainWindow.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CEnergyCyclingDlg.h"

CEnergyCyclingDlg::CEnergyCyclingDlg(QWidget *parent, CEnergyCycling &energyCycling) :
    QDialog(parent)
  , energyCycling(energyCycling)
{
    setupUi(this);

    for(const wind_speed_t &windSpeed : windSpeeds)
    {
        comboWindSpeed->addItem(windSpeed.name);
    }
    for(const wind_position_t &windPosition : windPositions)
    {
        comboWindPosition->addItem(windPosition.name);
    }
    for(const ground_condition_t &ground : grounds)
    {
        comboGround->addItem(ground.name);
    }
    setWindowTitle(tr("Energy Use Cycling Parameter Set"));

// How to put this tooltips in the GUI?
    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setToolTip(tr("Load the previous saved parameter set."));
    buttonBox->button(QDialogButtonBox::Reset)->setToolTip(tr("Remove the Energy Use Cycling value from the track."));

    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)), this, SLOT(slotApply(bool)));
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked(bool)), this, SLOT(slotLoadFromSettings(bool)));
    connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked(bool)), this, SLOT(slotRemove(bool)));
    connect(spinDriverWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));
    connect(spinBikeWeight, SIGNAL(valueChanged(double)), this, SLOT(slotSetWeight(double)));
    connect(comboWindSpeed, SIGNAL(activated(int)), this, SLOT(slotSetComboWindSpeed(int)));
    connect(spinWindSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindSpeed(double)));
    connect(spinAirDensity, SIGNAL(valueChanged(double)), this, SLOT(slotSetAirDensity(double)));
    connect(comboWindPosition, SIGNAL(activated(int)), this, SLOT(slotSetComboWindPosition(int)));
    connect(spinFrontalArea, SIGNAL(valueChanged(double)), this, SLOT(slotSetFrontalAreaSpin(double)));
    connect(spinWindDragCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotSetWindDragCoeffSpin(double)));
    connect(comboGround, SIGNAL(activated(int)), this, SLOT(slotSetComboGround(int)));
    connect(spinRollingCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotSetRollingCoeff(double)));
    connect(spinPedalCadence, SIGNAL(valueChanged(double)), this, SLOT(slotSetPedalCadence(double)));
    connect(pushHelp, SIGNAL(clicked(bool)), this, SLOT(slotShowHelp()));

    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setText(tr("Load previous Set"));
    buttonBox->button(QDialogButtonBox::Reset)->setText(tr("Remove"));

    energyTmpSet = energyCycling.getEnergyTrkSet();
    updateUi();

    if (energyCycling.getEnergyUseCycling() == NOFLOAT)
    {
        buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
    }

    slotApply(true);
}

CEnergyCyclingDlg::~CEnergyCyclingDlg()
{
}

void CEnergyCyclingDlg::updateUi()
{
    spinDriverWeight->setValue(energyTmpSet.driverWeight);
    spinBikeWeight->setValue(energyTmpSet.bikeWeight);
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(energyTmpSet.driverWeight + energyTmpSet.bikeWeight, 0, 'f', 1));

    comboWindSpeed->setCurrentIndex(energyTmpSet.windSpeedIndex);
    if(energyTmpSet.windSpeedIndex > 0)
    {
        spinWindSpeed->setValue(windSpeeds[energyTmpSet.windSpeedIndex].windSpeed * 3.6); // m/s ==> km/h
    }
    else
    {
        spinWindSpeed->setValue(energyTmpSet.windSpeed * 3.6);
    }

    spinAirDensity->setValue(energyTmpSet.airDensity);

    comboWindPosition->setCurrentIndex(energyTmpSet.windPositionIndex);
    if(energyTmpSet.windPositionIndex > 0)
    {
        spinFrontalArea->setValue(windPositions[energyTmpSet.windPositionIndex].frontalArea);
        spinWindDragCoeff->setValue(windPositions[energyTmpSet.windPositionIndex].windDragCoeff);
    }
    else
    {
        spinFrontalArea->setValue(energyTmpSet.frontalArea);
        spinWindDragCoeff->setValue(energyTmpSet.windDragCoeff);
    }

    comboGround->setCurrentIndex(energyTmpSet.groundIndex);
    if(energyTmpSet.groundIndex > 0)
    {
        spinRollingCoeff->setValue(grounds[energyTmpSet.groundIndex].rollingCoeff);
    }
    else
    {
        spinRollingCoeff->setValue(energyTmpSet.rollingCoeff);
    }

    spinPedalCadence->setValue(energyTmpSet.pedalCadence);
}

void CEnergyCyclingDlg::slotOk(bool)
{
    energyCycling.setEnergyTrkSet(energyTmpSet, true);
    energyCycling.compute();
    energyCycling.saveSettings();
    accept();
}

void CEnergyCyclingDlg::slotApply(bool)
{
    energyCycling.compute(energyTmpSet);

    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(energyTmpSet.driverWeight + energyTmpSet.bikeWeight, 0, 'f', 1));

    labelAirResistForce->setText(QString("<b>%1N</b>").arg(energyTmpSet.airResistForce, 0, 'f', 1));
    labelRollResistForce->setText(QString("<b>%1N</b>").arg(energyTmpSet.rollResistForce, 0, 'f', 1));
    labelGravitySlopeForce->setText(QString("<b>%1N</b>").arg(energyTmpSet.gravitySlopeForce, 0, 'f', 1));
    labelSumForce->setText(QString("<b>%1N</b>").arg(energyTmpSet.sumForce, 0, 'f', 1));

    QString val, unit;
    IUnit::self().seconds2time(energyTmpSet.powerMovingTime, val, unit);
    labelPowerMovingTime->setText(QString("<b>%1%2</b>").arg(val).arg(unit));
    labelPowerMovingTimeRatio->setText(QString("<b>%1%</b>").arg(energyTmpSet.powerMovingTimeRatio * 100, 0, 'f', 1));

    labelPower->setText(QString("<b>%1W</b>").arg(energyTmpSet.power, 0, 'f', 1));
    labelPositivePower->setText(QString("<b>%1W</b>").arg(energyTmpSet.positivePower, 0, 'f', 1));

    labelEnergyKJoule->setText(QString("<b>%1kJ</b>").arg(energyTmpSet.energyKJoule, 0, 'f', 0));
    labelEnergyKcal->setText(QString("<b><u>%1kcal</u></b>").arg(energyTmpSet.energyKcal, 0, 'f', 0));

    labelPositivePedalForce->setText(QString("<b>%1N</b>").arg(energyTmpSet.positivePedalForce, 0, 'f', 1));
}

void CEnergyCyclingDlg::slotLoadFromSettings(bool)
{
    energyCycling.loadSettings(energyTmpSet);
    updateUi();
    slotApply(true);
}

void CEnergyCyclingDlg::slotRemove(bool)
{
    energyCycling.remove();
    reject();
}

void CEnergyCyclingDlg::slotSetWeight(qreal)
{
    energyTmpSet.driverWeight = spinDriverWeight->value();
    energyTmpSet.bikeWeight = spinBikeWeight->value();
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(energyTmpSet.driverWeight + energyTmpSet.bikeWeight, 0, 'f', 1));
}

void CEnergyCyclingDlg::slotSetComboWindSpeed(qint32 windSpeedIndex)
{
    energyTmpSet.windSpeedIndex = windSpeedIndex;
    if(windSpeedIndex > 0)
    {
        spinWindSpeed->setValue(windSpeeds[windSpeedIndex].windSpeed * 3.6); // m/s ==> km/h
        energyTmpSet.windSpeed = windSpeeds[windSpeedIndex].windSpeed;
    }
}

void CEnergyCyclingDlg::slotSetWindSpeed(qreal windSpeed)
{
    if(qFuzzyIsNull(windSpeed)) // to avoid numerical noise
    {
        energyTmpSet.windSpeed = 0;
        spinWindSpeed->setValue(0);
    }
    else
    {
        energyTmpSet.windSpeed = windSpeed / 3.6;
    }

    energyTmpSet.windSpeedIndex = 0;
    comboWindSpeed->setCurrentIndex(0);
    for(qint32 i = 1; i < windSpeeds.size(); ++i)
    {
        if(qFuzzyCompare(1 + energyTmpSet.windSpeed, 1 + windSpeeds[i].windSpeed))
        {
            energyTmpSet.windSpeedIndex = i;
            comboWindSpeed->setCurrentIndex(i);
            break;
        }
    }
}

void CEnergyCyclingDlg::slotSetAirDensity(qreal airDensity)
{
    energyTmpSet.airDensity = airDensity;
}

void CEnergyCyclingDlg::slotSetComboWindPosition(qint32 windPositionIndex)
{
    energyTmpSet.windPositionIndex = windPositionIndex;
    if(windPositionIndex > 0)
    {
        spinFrontalArea->setValue(windPositions[windPositionIndex].frontalArea);
        spinWindDragCoeff->setValue(windPositions[windPositionIndex].windDragCoeff);
        energyTmpSet.frontalArea = windPositions[windPositionIndex].frontalArea;
        energyTmpSet.windDragCoeff = windPositions[windPositionIndex].windDragCoeff;
    }
}

void CEnergyCyclingDlg::slotSetFrontalAreaSpin(qreal)
{
    energyTmpSet.frontalArea = spinFrontalArea->value();
    checkWindPositionSpins();
}

void CEnergyCyclingDlg::slotSetWindDragCoeffSpin(qreal)
{
    energyTmpSet.windDragCoeff = spinWindDragCoeff->value();
    checkWindPositionSpins();
}

void CEnergyCyclingDlg::checkWindPositionSpins()
{
    energyTmpSet.windPositionIndex = 0;
    comboWindPosition->setCurrentIndex(0);
    for(qint32 i = 1; i < windPositions.size(); ++i)
    {
        if(qFuzzyCompare(1 + energyTmpSet.frontalArea, 1 + windPositions[i].frontalArea)
                && qFuzzyCompare(1 + energyTmpSet.windDragCoeff, 1 + windPositions[i].windDragCoeff))
        {
            energyTmpSet.windPositionIndex = i;
            comboWindPosition->setCurrentIndex(i);
            break;
        }
    }
}

void CEnergyCyclingDlg::slotSetComboGround(qint32 groundIndex)
{
    energyTmpSet.groundIndex = groundIndex;
    if(groundIndex > 0)
    {
        spinRollingCoeff->setValue(grounds[groundIndex].rollingCoeff);
        energyTmpSet.rollingCoeff = grounds[groundIndex].rollingCoeff;
    }
}

void CEnergyCyclingDlg::slotSetRollingCoeff(qreal rollingCoeff)
{
    energyTmpSet.rollingCoeff = rollingCoeff;

    energyTmpSet.groundIndex = 0;
    comboGround->setCurrentIndex(0);
    for(qint32 i = 1; i < grounds.size(); ++i)
    {
        if(qFuzzyCompare(1 + rollingCoeff, 1 + grounds[i].rollingCoeff))
        {
            energyTmpSet.groundIndex = i;
            comboGround->setCurrentIndex(i);
            break;
        }
    }
}

void CEnergyCyclingDlg::slotSetPedalCadence(qreal pedalCadence)
{
    energyTmpSet.pedalCadence = pedalCadence;
}

void CEnergyCyclingDlg::slotShowHelp()
{
    QString msg = tr("<p><b>Set Energy Use for Cycling</b></p>"
                     "<p>With this filter your personal energy use (consumption) for a cycling tour can be computed.</p>"
                     "<p>The computed value of \"Energy Use Cycling\" can be see as an indicator for the exertion of a cycling tour.</p>"
                     "<p>The tour length, speed and slope values will be taken into account.</p>"
                     "<p>To individualize your personal energy use the following input data are more needed:"
                     "<ul>"
                     "<li>Driver and bicyle weight</li>"
                     "<li>Air density, wind speed and position to the wind to consider the wind drag resistance</li>"
                     "<li>Ground situation (tyre and ground) to consider the rolling resistance</li>"
                     "<li>Average pedal cadence for the computation of pedal force</li>"
                     "</ul></p>"
                     "<p>The individualize data will be defined in this dialog and more computed values will be shown here.</p>"
                     "<p>The energy use in unit \"kcal\" will be stored in the track and can be remove later on when no longer needed.</p>"
                     "<p>For more information see tooltips on input and output values.</p>");

    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"), msg);
}
