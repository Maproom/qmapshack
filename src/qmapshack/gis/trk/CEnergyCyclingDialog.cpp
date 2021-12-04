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
#include "gis/trk/CEnergyCycling.h"
#include "gis/trk/CEnergyCyclingDialog.h"
#include "gis/trk/CGisItemTrk.h"


/** @brief Constructor - Initiate the dialog GUI

   @param energyCycling Reference to the track's CEnergyCycling object
   @param parent Pointer to the parent widget
 */
CEnergyCyclingDialog::CEnergyCyclingDialog(CEnergyCycling& energyCycling, QWidget* parent) :
    QDialog(parent)
    , energyCycling(energyCycling)
{
    setupUi(this);

    for(const wind_speed_t& windSpeed : windSpeeds)
    {
        comboWindSpeed->addItem(windSpeed.name);
    }
    for(const wind_position_t& windPosition : windPositions)
    {
        comboWindPosition->addItem(windPosition.name);
    }
    for(const ground_condition_t& ground : grounds)
    {
        comboGround->addItem(ground.name);
    }

    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setText(tr("Load previous Set"));
    buttonBox->button(QDialogButtonBox::Reset)->setText(tr("Remove"));

    buttonBox->button(QDialogButtonBox::Ok)->setToolTip(tr("Compute the \"Energy Use Cycling\" value, store the parameter set and close this dialog."));
    buttonBox->button(QDialogButtonBox::Apply)->setToolTip(tr("Compute the \"Energy Use Cycling\" value in this dialog."));
    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setToolTip(tr("Load the previous saved parameter set."));
    buttonBox->button(QDialogButtonBox::Reset)->setToolTip(tr("Remove the \"Energy Use Cycling\" value from the track."));

    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &CEnergyCyclingDialog::slotOk);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &CEnergyCyclingDialog::slotApply);
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &CEnergyCyclingDialog::slotLoadFromSettings);
    connect(buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &CEnergyCyclingDialog::slotRemove);
    connect(spinDriverWeight, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetDriverWeight);
    connect(spinBikeWeight, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetBikeWeight);
    connect(comboWindSpeed, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CEnergyCyclingDialog::slotSetComboWindSpeed);
    connect(spinWindSpeed, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetWindSpeed);
    connect(spinAirDensity, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetAirDensity);
    connect(comboWindPosition, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CEnergyCyclingDialog::slotSetComboWindPosition);
    connect(spinFrontalArea, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetFrontalAreaSpin);
    connect(spinWindDragCoeff, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetWindDragCoeffSpin);
    connect(comboGround, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CEnergyCyclingDialog::slotSetComboGround);
    connect(spinRollingCoeff, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetRollingCoeff);
    connect(spinPedalCadence, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &CEnergyCyclingDialog::slotSetPedalCadence);
    connect(pushHelp, &QPushButton::clicked, this, &CEnergyCyclingDialog::slotShowHelp);

    energyTmpSet = energyCycling.getEnergyTrkSet(); // put the track parameter set in a temporarily one, used by the dialog
    updateUi();

    if (energyCycling.getEnergyUseCycling() == NOFLOAT) // No need to remove the "Energy Use Cycling" value from the status panel
    {
        buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
    }

    slotApply(true); // Compute "Energy Use Cycling" and put all results in the dialog output widgets
}

CEnergyCyclingDialog::~CEnergyCyclingDialog()
{
}

/** @brief Update all widgets when a input value has changed in dialog
 */
void CEnergyCyclingDialog::updateUi()
{
    spinDriverWeight->setValue(energyTmpSet.driverWeight);
    spinBikeWeight->setValue(energyTmpSet.bikeWeight);
    labelTotalWeight->setText(QString("<b>%L1kg</b>").arg(energyTmpSet.driverWeight + energyTmpSet.bikeWeight, 0, 'f', 1));

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

/** @brief When "Ok" button is clicked:
     Set the temporarily parameter set back to parameter set of the track
     Compute the "Energy Use Cycling" value in track parameter set
     Update history
     Update status panel
     Save parameter set to SETTINGS
 */
void CEnergyCyclingDialog::slotOk(bool)
{
    energyCycling.setEnergyTrkSet(energyTmpSet, true);
    energyCycling.compute();
    energyCycling.saveSettings();
    accept();
}

/** @brief When "Apply" button is clicked:
     Compute the "Energy Use Cycling" value in the temporarily parameter set
     Update all computed values in the dialog output widgets
 */
void CEnergyCyclingDialog::slotApply(bool)
{
    energyCycling.compute(energyTmpSet);

    labelTotalWeight->setText(QString("<b>%L1kg</b>").arg(energyTmpSet.driverWeight + energyTmpSet.bikeWeight, 0, 'f', 1));

    labelAirResistForce->setText(QString("<b>%L1N</b>").arg(energyTmpSet.airResistForce, 0, 'f', 1));
    labelRollResistForce->setText(QString("<b>%L1N</b>").arg(energyTmpSet.rollResistForce, 0, 'f', 1));
    labelGravitySlopeForce->setText(QString("<b>%L1N</b>").arg(energyTmpSet.gravitySlopeForce, 0, 'f', 1));
    labelSumForce->setText(QString("<b>%L1N</b>").arg(energyTmpSet.sumForce, 0, 'f', 1));

    QString val, unit;
    IUnit::self().seconds2time(energyTmpSet.powerMovingTime, val, unit);
    labelPowerMovingTime->setText(QString("<b>%L1%2</b>").arg(val, unit));
    labelPowerMovingTimeRatio->setText(QString("<b>%L1%</b>").arg(energyTmpSet.powerMovingTimeRatio * 100, 0, 'f', 1));

    labelPower->setText(QString("<b>%L1W</b>").arg(energyTmpSet.power, 0, 'f', 1));
    labelPositivePower->setText(QString("<b>%L1W</b>").arg(energyTmpSet.positivePower, 0, 'f', 1));

    labelEnergyKJoule->setText(QString("<b>%L1kJ</b>").arg(energyTmpSet.energyKJoule, 0, 'f', 0));
    labelEnergyKcal->setText(QString("<b><u>%L1kcal</u></b>").arg(energyTmpSet.energyKcal, 0, 'f', 0));

    labelPositivePedalForce->setText(QString("<b>%L1N</b>").arg(energyTmpSet.positivePedalForce, 0, 'f', 1));
}

/** @brief Loads parameters from SETTINGS into the temporarily parameter for modifying in the dialog
     Update all dialog GUI widgets
 */
void CEnergyCyclingDialog::slotLoadFromSettings(bool)
{
    energyCycling.loadSettings(energyTmpSet);
    updateUi();
    slotApply(true);
}

/** @brief Removes the "Energy Use Cycling" value from the status panel
   @note QDialog::reject() will be called to be catched by CTrackDetailsTrk for the update of the status panel
 */
void CEnergyCyclingDialog::slotRemove(bool)
{
    energyCycling.remove();
    reject();
}

void CEnergyCyclingDialog::slotSetDriverWeight(qreal /*weight*/)
{
    energyTmpSet.driverWeight = spinDriverWeight->value();
    labelTotalWeight->setText(QString("<b>%L1kg</b>").arg(energyTmpSet.driverWeight + energyTmpSet.bikeWeight, 0, 'f', 1));
}

void CEnergyCyclingDialog::slotSetBikeWeight(qreal /*weight*/)
{
    energyTmpSet.bikeWeight = spinBikeWeight->value();
    labelTotalWeight->setText(QString("<b>%L1kg</b>").arg(energyTmpSet.driverWeight + energyTmpSet.bikeWeight, 0, 'f', 1));
}

void CEnergyCyclingDialog::slotSetComboWindSpeed(qint32 windSpeedIndex)
{
    energyTmpSet.windSpeedIndex = windSpeedIndex;
    if(windSpeedIndex > 0)
    {
        spinWindSpeed->setValue(windSpeeds[windSpeedIndex].windSpeed * 3.6); // m/s ==> km/h
        energyTmpSet.windSpeed = windSpeeds[windSpeedIndex].windSpeed;
    }
}

void CEnergyCyclingDialog::slotSetWindSpeed(qreal windSpeed)
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

void CEnergyCyclingDialog::slotSetAirDensity(qreal airDensity)
{
    energyTmpSet.airDensity = airDensity;
}

void CEnergyCyclingDialog::slotSetComboWindPosition(qint32 windPositionIndex)
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

void CEnergyCyclingDialog::slotSetFrontalAreaSpin(qreal frontalArea)
{
    energyTmpSet.frontalArea = frontalArea;
    checkWindPositionSpins();
}

void CEnergyCyclingDialog::slotSetWindDragCoeffSpin(qreal windDragCoeff)
{
    energyTmpSet.windDragCoeff = windDragCoeff;
    checkWindPositionSpins();
}

void CEnergyCyclingDialog::checkWindPositionSpins()
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

void CEnergyCyclingDialog::slotSetComboGround(qint32 groundIndex)
{
    energyTmpSet.groundIndex = groundIndex;
    if(groundIndex > 0)
    {
        spinRollingCoeff->setValue(grounds[groundIndex].rollingCoeff);
        energyTmpSet.rollingCoeff = grounds[groundIndex].rollingCoeff;
    }
}

void CEnergyCyclingDialog::slotSetRollingCoeff(qreal rollingCoeff)
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

void CEnergyCyclingDialog::slotSetPedalCadence(qreal pedalCadence)
{
    energyTmpSet.pedalCadence = pedalCadence;
}

void CEnergyCyclingDialog::slotShowHelp()
{
    QString msg = tr("<p><b>Set Energy Use for Cycling</b></p>"
                     "<p>Within this dialog your personal energy use (consumption) for a cycling tour can be computed.</p>"
                     "<p>The computed value of \"Energy Use Cycling\" can be see as an indicator for the exertion of a cycling tour.</p>"
                     "<p>The tour length, speed and slope values will be taken into account.</p>"
                     "<p>To individualize your personal energy use the following input data are more needed:"
                     "<ul>"
                     "<li>Driver and bicycle weight</li>"
                     "<li>Air density, wind speed and position to the wind to consider the wind drag resistance</li>"
                     "<li>Ground situation (tyre and ground) to consider the rolling resistance</li>"
                     "<li>Average pedal cadence for the computation of pedal force</li>"
                     "</ul></p>"
                     "<p>The individualize data will be defined in this dialog and more computed values will be shown here.</p>"
                     "<p>When loading older tracks or switching in history to tracks with a different parameter set compared to the previous saved parameter set"
                     ", the shown parameter set in this dialog can be replaced by the previous saved parameter set."
                     "<p>The energy use in unit \"kcal\" will be stored in the track (qms format only) and can be remove later on when no longer needed.</p>"
                     "<p>For more information see tooltips on input and output values.</p>");

    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"), msg);
}
