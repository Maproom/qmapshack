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

    connect(comboBoxSetting, SIGNAL(activated(int)), this, SLOT(slotSetSetting(int)));
    connect(pushButtonRemove, SIGNAL(clicked(bool)), this, SLOT(slotRemove(bool)));
    connect(pushButtonEdit, SIGNAL(clicked(bool)), this, SLOT(slotEditSetting(bool)));
    connect(toolApply, &QToolButton::clicked, this, &CFilterEnergyCycle::slotApply);
    connect(pushButtonHelp, &QToolButton::clicked, this, &CFilterEnergyCycle::slotShowHelp);

/*  GER:
    Benutzerspezifische Auswahl eines Parameterset für die Berechnung des Energieverbrauchs für einen Fahrrad Track.
*/
    comboBoxSetting->setToolTip(tr("<p>Select a user defined parameter set to compute energy use (consumption) for a cycling track.</p>"));

/*  GER:
    Öffnet den Dialog für die Änderung der Parameter für die Berechnung des Energieverbrauchs für einen Fahrrad Track.
    Zeigt weitere Werte für Kraft, Leistung und Energie.
*/
    pushButtonEdit->setToolTip(tr("<p>Open dialog window to modify parameter set and compute energy use (consumption) for a cycling track.</p>"
                                  "<p>Shows more computed values for Force, Power and Energy.</p>"));

/*  GER:
    Löscht den Wert für den Energieverbrauch aus dem Track.
*/
    pushButtonRemove->setToolTip(tr("<p>Remove the Energy Use Cycling value from the track.</p>"));

/*  GER:
    Berechnet den Energieverbrauch für einen Fahrrad Track auf Basis des gewählten Parametersets.
*/
    toolApply->setToolTip(tr("<p>Computes energy use (consumption) for a cycling track based on the selected parameter set.</p>"));

    isValid();
}

void CFilterEnergyCycle::loadSettings()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/EnergyCycle/");

    energySets.clear();
    comboBoxSetting->clear();

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
        comboBoxSetting->addItem(energySet.nameOfSet);
    }
    cfg.endArray();
    currentSet = cfg.value("currentSet", 0).toInt();
    comboBoxSetting->setCurrentIndex(currentSet);
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
    loadSettings(); // In case another track changed the settings in parallel

    energy_set_t &energySet = energySets[currentSet];

    CFilterEnergyCycleDlg energyDlg(this, trk, energySet, currentSet, energyDefaultSet);
    if(energyDlg.exec() == QDialog::Accepted)
    {
        updateUi(eUpdateFromApply);

        if(comboBoxSetting->currentText() != energySet.nameOfSet)
        {
            comboBoxSetting->setItemText(currentSet, energySet.nameOfSet);
        }
        saveSettings();
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

        comboBoxSetting->setEnabled(false);
        pushButtonEdit->setEnabled(false);
        toolApply->setEnabled(false);
        return false;
    }

    comboBoxSetting->setEnabled(true);
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

void CFilterEnergyCycle::slotShowHelp()
{
/*  GER:
    Energieverbrauch beim Fahrradfahren
    Mit diesem Filter kann der Energieverbrauch beim Fahrradfahren berechnet werden.
    Der Wert des \"Energieverbrauchs\" kann als Indikator für die Anstregung einer Fahrradtour angesehen werden.
    Die Tourlänge, die Geschwindigkeit und die Steigungen werden berücksichtigt.
    Um den persönlichen Energieverbrauch einer Tour zu spezifizieren werden weitere Daten benötigt:
    Das Gewicht des Fahrers und Fahrrads
    Die Luftdichte, Windgeschwindigkeit und die Position im Wind für die Berücksichtigung des Luftwiderstands
    Der Untergrund und die Bereifung für die Berücksichtigung des Rollwiderstands
    Die Trittfrequenz, um die Pedalkraft zu berechnen
    Die Werte werden in einem separaten Dialog eingegeben und weitere Werte werden dort berechnet und angezeigt.
    Fünf individualiserte Parametersets können für die spätere Wiederverwendung definiert werden.
    Der berechnete Energieverbrauch in der Einheit kcal wird im Track gespeichert und kann auch wieder gelöscht werden, sollte er nicht mehr benötigt werden.
    Weitere Information werden als Tooltipps für die Eingabe- und Ausgabewerte im Dialog \"Parameterset\" ausgegeben.
*/

    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help")
    , tr("<h3>Set Energy Use for Cycling</h3>"
         "<p>With this filter your personal energy use (consumption) for a cycling tour can be computed.</p>"
         "<p>The computed vaule of \"Energy Use\" can be see as an indicator for the exertion of a cycling tour.</p>"
         "<p>The tour length, speed and slope values will be taken into account.</p>"
         "<p>To individualize your personal energy use the following input data are more needed:"
         "<ul><li>Drivers and bicyle weight</li>"
         "<li>Air density, wind speed and position to the wind to consider the wind drag resistance</li>"
         "<li>Ground situation (tyre and ground) to consider the rolling resistance</li>"
         "<li>Average pedal cadence for the computation of pedal force</li></ul></p>"
         "<p>The individualize data will be defined a separate \"Edit Setting\" dialog and more computed values will be shown in this dialog.</p>"
         "<p>Five individualize parameter sets can be stored to reuse later on.</p>"
         "<p>The energy use in unit \"kcal\" will be stored in the track and can be remove later on when not longer needed.</p>"
         "<p>For more information see tooltips on input and output values in the \"Edit Setting\" dialog.</p>"));
}
