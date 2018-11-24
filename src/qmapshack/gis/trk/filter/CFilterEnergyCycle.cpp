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

#include "canvas/CCanvas.h"
#include "CFilterEnergyCycle.h"
#include "CFilterEnergyCycleDlg.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CSettings.h"

CFilterEnergyCycle::CFilterEnergyCycle(CGisItemTrk &trk, QWidget *parent) :
    QWidget(parent)
    , trk(trk)
    , energyDefaultSet{"", 75, 15, 1.2, 5, 0, 2, 0.65, 1.0, 3, 0.005, 75}
{
    setupUi(this);

    loadSettings();

    connect(comboBox, SIGNAL(activated(int)), this, SLOT(slotSetSetting(int)));
    connect(pushButtonRemove, SIGNAL(clicked(bool)), this, SLOT(slotRemove(bool)));
    connect(pushButtonEdit, SIGNAL(clicked(bool)), this, SLOT(slotEditSetting(bool)));
    connect(toolApply, &QToolButton::clicked, this, &CFilterEnergyCycle::slotApply);

    isValid();
}

void CFilterEnergyCycle::loadSettings()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/EnergyCycle/");

    energy_set_t energySet;

    cfg.beginReadArray("Set");
    for (int i = 0; i < noOfSets; ++i)
    {
        cfg.setArrayIndex(i);

        energySet.nameOfSet = cfg.value("nameOfSet", QString(tr("Set %1")).arg(i)).toString();
        energySet.driverWeight = cfg.value("personalWeight", energyDefaultSet.driverWeight).toDouble();
        energySet.bikeWeight = cfg.value("bikeWeight", energyDefaultSet.bikeWeight).toDouble();
        energySet.airDensity = cfg.value("airDensity", energyDefaultSet.airDensity).toDouble();
        energySet.windSpeedIndex = cfg.value("windSpeedIndex", energyDefaultSet.windSpeedIndex).toInt();
        energySet.windSpeed = cfg.value("windSpeed", energyDefaultSet.windSpeed).toDouble();
        energySet.windPositionIndex = cfg.value("windPositionIndex", energyDefaultSet.windPositionIndex).toInt();
        energySet.frontalArea = cfg.value("frontalArea", energyDefaultSet.frontalArea).toDouble();
        energySet.windDragCoeff = cfg.value("windDragCoeff", energyDefaultSet.windDragCoeff).toDouble();
        energySet.groundIndex = cfg.value("groundIndex", energyDefaultSet.groundIndex).toInt();
        energySet.rollingCoeff = cfg.value("rollingCoeff", energyDefaultSet.rollingCoeff).toDouble();
        energySet.pedalCadence = cfg.value("pedalCadence", energyDefaultSet.pedalCadence).toDouble();

        energySets << energySet;
        comboBox->addItem(energySet.nameOfSet);
    }
    cfg.endArray();
    currentSet = cfg.value("currentSet", 0).toInt();
    comboBox->setCurrentIndex(currentSet);
    cfg.endGroup();
}

void CFilterEnergyCycle::saveSettings()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/EnergyCycle/");

    cfg.setValue("currentSet", currentSet);

    cfg.beginWriteArray("Set");
    for (int i = 0; i < noOfSets; ++i)
    {
        const energy_set_t &energySet = energySets[i];
        cfg.setArrayIndex(i);
        cfg.setValue("nameOfSet", energySet.nameOfSet);
        cfg.setValue("personalWeight", energySet.driverWeight);
        cfg.setValue("bikeWeight", energySet.bikeWeight);
        cfg.setValue("airDensity", energySet.airDensity);
        cfg.setValue("windSpeedIndex", energySet.windSpeedIndex);
        cfg.setValue("windSpeed", energySet.windSpeed);
        cfg.setValue("windPositionIndex", energySet.windPositionIndex);
        cfg.setValue("frontalArea", energySet.frontalArea);
        cfg.setValue("windDragCoeff", energySet.windDragCoeff);
        cfg.setValue("groundIndex", energySet.groundIndex);
        cfg.setValue("rollingCoeff", energySet.rollingCoeff);
        cfg.setValue("pedalCadence", energySet.pedalCadence);
    }
    cfg.endArray();
    cfg.endGroup();
}

void CFilterEnergyCycle::slotApply()
{
    updateUi(eUpdateFromApply);
}

void CFilterEnergyCycle::slotSetSetting(int set)
{
    currentSet = set;
    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/EnergyCycle/");
    cfg.setValue("currentSet", currentSet);
    cfg.endGroup();
    toolApply->setEnabled(true);
}

void CFilterEnergyCycle::slotRemove(bool)
{
    trk.setEnergyUse(NOFLOAT);
    toolApply->setEnabled(true);
    pushButtonRemove->setEnabled(false);
    trk.updateHistory(CGisItemTrk::eVisualDetails);
}

void CFilterEnergyCycle::slotEditSetting(bool)
{
    energy_set_t &energySet = energySets[currentSet];

    CFilterEnergyCycleDlg energyDlg(this, trk, energySet, energyDefaultSet);
    if(energyDlg.exec() == QDialog::Accepted)
    {
        updateUi(eUpdateFromApply);

        if(comboBox->currentText() != energySet.nameOfSet)
        {
            comboBox->setItemText(currentSet, energySet.nameOfSet);
            saveSettings();
        }
    }
}

void CFilterEnergyCycle::updateUi(update_source_e updateSource)
{
    if(!isValid())
    {
        return;
    }

    switch(updateSource)
    {
    case eUpdateFromHistory:
    {
        if(trk.getEnergyUse() != NOFLOAT)
        {
            trk.updateVisuals(CGisItemTrk::eVisualDetails, "CFilterEnergyCycle::updateUi()");
        }
        break;
    }
    case eUpdateFromFilter:
    {
        if(trk.getEnergyUse() != NOFLOAT)
        {
            compute();
        }
        break;
    }
    case eUpdateFromApply:
    {
        compute();
        break;
    }
    }
}

bool CFilterEnergyCycle::isValid()
{
    pushButtonRemove->setEnabled(trk.getEnergyUse() != NOFLOAT ? true : false);

    if(!trk.isTrkTimeValid() || trk.isTrkElevationInvalid())
    {
        if(trk.getEnergyUse() != NOFLOAT)
        {
            trk.setEnergyUse(NOFLOAT);
            pushButtonRemove->setEnabled(false);
        }

        comboBox->setEnabled(false);
        pushButtonEdit->setEnabled(false);
        toolApply->setEnabled(false);
        return false;
    }

    comboBox->setEnabled(true);
    pushButtonEdit->setEnabled(true);
    toolApply->setEnabled(trk.getEnergyUse() == NOFLOAT ? true : false);

    return true;
}

void CFilterEnergyCycle::compute()
{
    energy_set_t &energySet = energySets[currentSet];
    trk.filterEnergyCycle(energySet);
    if(!qFuzzyCompare(trk.getEnergyUse() + 1, energySet.energyKcal + 1))
    {
        trk.setEnergyUse(energySet.energyKcal);
        toolApply->setEnabled(false);
        trk.updateHistory(CGisItemTrk::eVisualDetails);
    }
}
