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
#include "gis/trk/filter/CFilterEnergyCycleDlg.h"

CFilterEnergyCycleDlg::CFilterEnergyCycleDlg(QWidget *parent, CGisItemTrk &trk,
                                             CFilterEnergyCycle::energy_set_t &energySet, qint32 currentSet, const CFilterEnergyCycle::energy_set_t &defaultSet) :
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
        {tr("Asphalt, small tyres"),    0.004},
        {tr("Asphalt, normal tyres"),   0.005},
        {tr("Mix Asphalt+Forest"),      0.007},
        {tr("Mountain Bike Trail"),     0.015},
        {tr("Hard Forest Path"),        0.010},
        {tr("Soft Forest Path"),        0.050},
        {tr("Sand"),                    0.300}
    }
  , trk(trk)
  , energySet(energySet)
  , defaultSet(defaultSet)
{
    setupUi(this);

    tmpEnergySet = energySet; // Changes will be made first in tmpEnergySet before saving back on OK button

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
    labelNameOfSet->setText(tr("Name of Set") +  QString(" %1").arg(currentSet));

// Tooltips
/*  GER:
    Parameterset
    Individueller Name (max. 32 Zeichen) für das Parameterset.
*/
    lineEditNameOfSet->setToolTip("<p>" + tr("Individual name (max. 32 characters) for the parameter set.") + "</p>");

/*  GER:
    Fahrergewicht
    Das Fahrergewicht in kg einschließlich Kleidung.
*/
    spinDriverWeight->setToolTip("<p>" + tr("The driver's weight in kg including clothing.") + "</p>");

/*  GER:
    Fahrradgewicht
    Das Fahrradgewicht in kg einschließlich Gepäck.
*/
    spinBikeWeight->setToolTip("<p>" + tr("The weight of the bicycle in kg including luggage.") + "</p>");

/*  GER:
    Luftdichte
    Luftdichte in kg/m3. Die Luftdichte ist abhängig von barometrischen Luftdruck, Höhe über Normalnull und Temperatur.
    Beispiele:
    Meereshöhe: 1,226kg/m3
    1500 Meter: 1,056kg/m3
    3000 Meter: 0,905kg/m3
*/
    spinAirDensity->setToolTip("<p>" + tr("Air density in kg/m3. The air density depends on barometric air pressure, altitude and temperature.") + "</p>" +
                               "<p>" + tr("Examples:") +
                               "<ul><li>" + tr("Sea level: 1.226kg/m3") + "</li>" +
                               "<li>" + tr("1500 meter: 1.056kg/m3") + "</li>"
                               "<li>" + tr("3000 meter: 0.905kg/m3") + "</li></ul></p>");

/*  GER:
    Windgeschw. in Bft
    Werte für die Windgeschwindigkeit in Beaufortskala für die Vorauswahl der Windgeschwindigkeit in km/h.
    Positive Werte für Gegenwind und negative Werte für Rückenwind.
*/
    comboWindSpeed->setToolTip("<p>" + tr("Values for the wind speed in Beaufort scale for specifying the wind speed in km/h.") + "</p>" +
                               "<p>" + tr("Positive values for headwind and negative values for tailwind.") + "</p>");

/*  GER:
    Windgeschw. in km/h
    Die Windgeschwindigkeit in km/h wird bei der Berechnung der benötigten Kraft für die Überwindung des Luftwiderstands berücksichtigt.
    Positive Werte werden für Gegenwind und negative Werte für Rückenwind berücksichtigt.
    Die Werte können unabhängig von den Werten der Beaufortskala verändert werden.
*/
    spinWindSpeed->setToolTip("<p>" + tr("The wind speed in km/h is taken into account when calculating the force required to overcome the air drag.") + "</p>" +
                              "<p>" + tr("Positive values for headwind and negative values for tailwind are taken into account.") + "</p>" +
                              "<p>" + tr("The value can be changed independently of the Beaufort scale values.") + "</p>");

/*  GER:
    Trittfrequenz
    Die durchschnittliche Trittfrequenz in U/min wird für die Berechnung der Pedalkraft benötigt.
    Realistische Werte liegen zwischen 50 und 100 U/min.
*/
    spinPedalCadence->setToolTip("<p>" + tr("The average pedal cadence in rpm is needed to calculate the pedal force.") + "</p>" +
                                 "<p>" + tr("Realistic values are between 50 and 100 rpm.") + "</p>");

/*  GER:
    Position im Wind
    Werte für die Art der Sitzposition als Vorauswahl für die Windangriffsfläche in m2 und der Luftwiderstandszahl (cw-Wert).
*/
    comboWindPosition->setToolTip("<p>" + tr("Values for the type of seat position as pre-selection for the wind frontal area in m2 and the drag coefficient (cw value).") + "</p>");

/*  GER:
    Windangriffsfläche
    Die Fläche in m2 (Fahrer, Fahrrad, Gepäcktaschen, etc.), die dem Wind ausgesetzt ist.
*/
    spinFrontalArea->setToolTip("<p>" + tr("The frontal area in m2 (driver, bicycle, bags, etc.) exposed to the wind.") + "</p>");

/*  GER:
    cw-Wert (Luftwiderstandsbeiwert)
    Der cw-Wert (Luftwiderstandsbeiwert) gibt die Windschlüpfigkeit von Fahrer und Fahrrad gegenüber der Luft (Wind) an.
    Je größer dieser Wert ist, um so mehr Kraft wird für die Überwindung der Windkraft benötigt.
    Typische Werte für Fahrräder liegen zwischen 0,2 und 1.2.
*/
    spinWindDragCoeff->setToolTip("<p>" + tr("The cw value (drag coefficient) indicates the wind slip of the driver and bicycle in relation to the air (wind).") + "</p>" +
                                  "<p>" + tr("The greater this value, the more force is required to overcome the wind force.") + "</p>" +
                                  "<p>" + tr("Typical values for bicycles are between 0.2 and 1.2.") + "</p>");

/*  GER:
    Untergrund/Bereifung
    Werte für die Art des Untergrunds und der Bereifung für die Bestimmung des Rollwiderstandsbeiwerts.
*/
    comboGround->setToolTip("<p>" + tr("Values for the type of ground surface and the tyres.") + "</p>" +
                            "<p>" + tr("Used to determine the rolling resistance coefficient.") + "</p>");

/*  GER:
    cr-Wert (Rollwiderstandsbeiwert)
    Der Rollwiderstandsbeiwert bestimmt die Kraft, die für die Überwindung der Rollwiderstandskraft bestimmt.
    Maßgebliche Faktoren sind der Fahrbahnuntergrund und die Bereifung.
    Je größer dieser Wert ist, um so mehr Kraft wird für die Überwindung des Rollwiderstands benötigt.
*/
    spinRollingCoeff->setToolTip("<p>" + tr("The rolling resistance coefficient specifies the force used to overcome the rolling resistance force.") + "</p>" +
                                 "<p>" + tr("The relevant factors are the road surface and the tyres.") + "</p>" +
                                 "<p>" + tr("The higher this value is, the more force is required to overcome the rolling resistance.") + "</p>");

/*  GER:
    Gesamtgewicht
    Das Gesamtgewicht in kg von Fahrer und Fahrrad inklusive Kleidung und Gepäck.
*/
    labelTotalWeight->setToolTip("<p>"+ tr("The total weight in kg of driver and bike including clothes and luggage.") + "</p>");

/*  GER:
    Luftwiderstandskraft
    Die Luftwiderstandskraft ist die Kraft in Newton, die im Durchschnitt über alle Trackpunkte für die Überwindung des Luftwiderstands benötigt wird.
    Berücksichtigt wird der Fahrtwind und Gegen- und Rückenwind.
    Bei starkem Rückenwind kann dieser Wert auch negativ sein.
*/
    labelAirResistForce->setToolTip("<p>" + tr("The air resistance force is the force in Newton required on average over all track points to overcome the air resistance.") + "<p>" +
                                    "<p>" + tr("The airstream, headwind and tailwind are taken into account.") + "</p>" +
                                    "<p>" + tr("With a strong tailwind this value can be negative.") + "</p");

/*  GER:
    Rollwiderstandskraft
    Die Rollwiderstandskraft ist die Kraft in Newton, die im Durchschnitt über alle Trackpunkte für die Überwindung des Rollwiderstands benötigt wird.
*/
    labelRollResistForce->setToolTip("<p>" + tr("The rolling resistance force is the force in Newton required on average over all track points to overcome the rolling resistance.") + "</p>");

/*  GER:
    Neigungskraft
    Die Neigungskraft ist die Kraft in Newton, die im Durchschnitt über alle Trackpunkte für die Überwindung der Neigung bei der Bergauffahrt benötigt wird.
    Bei überwiegender Bergabfahrt kann die Neigungskraft negativ sein.
*/
    labelGravitySlopeForce->setToolTip("<p>" + tr("The gravity slope force is the force in Newton needed on average over all track points to overcome the slope on the uphill.") + "</p>" +
                                       "<p>" + tr("When mostly downhill driving, the gravity slope force can be negative.") + "</p>");

/*  GER:
    Summe Kräfte
    Summe der durchschnittlich benötigten Kraft für den Track für die Überwindung des Luftwiderstands, Rollwiderstands und Neigung.
    Dieser Wert kann negativ bei starkem Rückwind und/oder Bergabfahrt sein.
*/
    labelSumForce->setToolTip("<p>" + tr("Sum of the average force required for the track to overcome wind resistance, rolling resistance and slope.") + "</p>" +
                              "<p>" + tr("This value can be negative for strong tailwind and/or downhill.") + "</p>");

/*  GER:
    Pedalkraft
    Die Kraft in Newton, die im Durchschnitt über alle Trackpunkte auf das Pedal aufgebracht wird.
    Negative Kräfte aus starken Rückenwind oder Bergabfahrt sind nicht berücksichtigt.
    Diese Kraft muss durch den Fahrer auf das Pedal aufgebracht werden. Je größer die Trittfrequenz, desto niedriger ist die Pedalkraft.
*/
    labelPositivePedalForce->setToolTip("<p>" + tr("The force in Newton applied to the pedal on average over all track points.") + "</p>" +
                                        "<p>" + tr("Negative forces from strong tail wind or downhill driving are not considered.") + "</p>" +
                                        "<p>" + tr("This force must be applied to the pedal by the driver.") + "</p>" +
                                        "<p>" + tr("The higher the pedal cadence, the lower the pedal force.") + "</p>");

/*  GER:
    Leistung
    Die Leistung in Watt im Durchschnitt über alle Trackpunkte. Negative Leistung aus starken Rückenwind oder Bergabfahrt sind berücksichtigt.
*/
    labelPower->setToolTip("<p>" +tr("The power in Watt on average over all track points.") + "</p>" +
                           "<p>" +tr("Negative power from strong tailwind or downhill driving are taken into account.") + "</p>");

/*  GER:
    Positive Leistung
    Die positive Leistung in Watt, die im Durchschnitt über alle Trackpunkte aufgebracht werden muss.
    Negative Leistung aus starken Rückenwind oder Bergabfahrt sind nicht berücksichtigt.
    Diese Leistung muss durch den Fahrer im Durchschnitt für den Track aufgebracht werden.
*/
    labelPositivePower->setToolTip("<p>" + tr("The positive power in Watt that must be applied on average for all track points.") + "</p>" +
                                   "<p>" + tr("Negative power from strong tailwind or downhill driving are not taken into account.") + "</p>" +
                                   "<p>" + tr("This power must be applied by the driver on average for the track.") + "</p>");
/*  GER:
    Leistungsbewegungszeit
    Die Zeit in Bewegung und gleichzeitiger positiver Leistung durch den Fahrer.
    Zeiten mit negativer Leistung aus starkem Rückenwind oder Bergabfahrt werden nicht berücksichtigt.
*/
    labelPowerMovingTime->setToolTip("<p>" + tr("The time in moving and with positive power by the driver.") + "</p>" +
                                     "<p>" + tr("Times with negative power in strong tail wind or downhill driving are not taken into account.") + "</p>");

/*  GER:
    Verhältnis Leistungsbewegungszeit
    Das Verhältnis der Leistungsbewegungszeit bezogen auf die Track Bewegungszeit in Prozent. Je geringer das Verhältnis ist, desto größer ist der Einfluss von Rückenwind und/oder Bergabfahrt.
*/
    labelPowerMovingTimeRatio->setToolTip("<p>" + tr("The ratio of the power moving time related to the track moving time in percent.") + "</p>" +
                                          "<p>" + tr("The lower the ratio the greater the influence of tailwind and/or downhill driving.") + "</p>");

/*  GER:
    Energieverbrauch in kJoule
    Der Energieverbrauch in kJoule für die Umsetzung der positiven Leistung durch den Fahrer.
    Der Muskel-Wirkungsgrad ist mit 23% angenommen.
*/
    labelEnergyKJoule->setToolTip("<p>" + tr("The energy use (consumption) in kJoule by transforming the positive power by the driver.") + "</p>" +
                                  "<p>" + tr("The muscle efficiency ratio ist considered by 23%.") + "</p>");

/*  GER:
    Energieverbrauch in kcal
    Der Energieverbrauch in kcal für die Umsetzung der positiven Leistung durch den Fahrer.
    Der Muskel-Wirkungsgrad ist mit 23% angenommen.
*/
    labelEnergyKcal->setToolTip("<p>" + tr("The energy use (consumption) in kcal by transforming the positive power by the driver.") + "</p>" +
                                "<p>" + tr("The muscle efficiency ratio ist considered by 23%.") + "</p>");

/*  GER:
    Voreinstellung mit:
    Fahrergewicht: 75,0kg
    Fahrradgewicht: 15,0kg
    Luftdichte: 1,200kg/m3
    Windgeschwindigkeit Bft: 0 Kein Wind
    Windgeschwindigkeit km/h: 0,0km/h
    Trittfrequenz: 75U/min
    Position im Wind: Trekking
    Frontalfläche: 0,65m2
    cw-Wert (Luftwiderstandsbeiwert): 1,000
    Untergrund/Reifen: Asphalt, normale Bereifung
    cr-Wert (Rollwiderstandsbeiwert): 0,005
*/
    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setToolTip("<p><b>" + tr("Set default values to:") + "</b></p>" +
                                                                     "<p>" + tr("Driver Weight: 75.0kg") +
                                                                     "<br/>" + tr("Bike Weight: 15.0kg") +
                                                                     "<br/>" + tr("Air Density: 1.200kg/m3") +
                                                                     "<br/>" + tr("Wind Speed Bft: 0 No Wind") +
                                                                     "<br/>" + tr("Wind Speed km/h: 0.0km/h") +
                                                                     "<br/>" + tr("Pedal Cadence: 75rpm") +
                                                                     "<br/>" + tr("Position in Wind: Trekking") +
                                                                     "<br/>" + tr("Frontal Area: 0.65m2") +
                                                                     "<br/>" + tr("Wind drag Coeff: 1.000") +
                                                                     "<br/>" + tr("Ground/Tyres: Asphalt, normal tyres") +
                                                                     "<br/>" + tr("Rolling Coeff: 0.005") + "</p>");

    updateUi();

    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)), this, SLOT(slotApply(bool)));
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked(bool)), this, SLOT(slotRestoreDefaults(bool)));
    connect(lineEditNameOfSet, SIGNAL(editingFinished()), this, SLOT(slotSetNameOfSet()));
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

CFilterEnergyCycleDlg::~CFilterEnergyCycleDlg()
{
}

void CFilterEnergyCycleDlg::updateUi()
{
    lineEditNameOfSet->setText(tmpEnergySet.nameOfSet);
    slotSetNameOfSet(); // To fill windowTitle

    spinDriverWeight->setValue(tmpEnergySet.driverWeight);
    spinBikeWeight->setValue(tmpEnergySet.bikeWeight);
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.driverWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));

    comboWindSpeed->setCurrentIndex(tmpEnergySet.windSpeedIndex);
    if(tmpEnergySet.windSpeedIndex > 0)
    {
        spinWindSpeed->setValue(windSpeeds[tmpEnergySet.windSpeedIndex].windSpeed * 3.6); // m/s ==> km/h
    }
    else
    {
        spinWindSpeed->setValue(tmpEnergySet.windSpeed * 3.6);
    }

    spinAirDensity->setValue(tmpEnergySet.airDensity);

    comboWindPosition->setCurrentIndex(tmpEnergySet.windPositionIndex);
    if(tmpEnergySet.windPositionIndex > 0)
    {
        spinFrontalArea->setValue(windPositions[tmpEnergySet.windPositionIndex].frontalArea);
        spinWindDragCoeff->setValue(windPositions[tmpEnergySet.windPositionIndex].windDragCoeff);
    }
    else
    {
        spinFrontalArea->setValue(tmpEnergySet.frontalArea);
        spinWindDragCoeff->setValue(tmpEnergySet.windDragCoeff);
    }

    comboGround->setCurrentIndex(tmpEnergySet.groundIndex);
    if(tmpEnergySet.groundIndex > 0)
    {
        spinRollingCoeff->setValue(grounds[tmpEnergySet.groundIndex].rollingCoeff);
    }
    else
    {
        spinRollingCoeff->setValue(tmpEnergySet.rollingCoeff);
    }

    spinPedalCadence->setValue(tmpEnergySet.pedalCadence);
}

void CFilterEnergyCycleDlg::slotOk(bool)
{
    energySet = tmpEnergySet; // Saving back
    accept();
}

void CFilterEnergyCycleDlg::slotApply(bool)
{
    trk.filterEnergyCycle(tmpEnergySet); // Calculation for dialog only; no stat; no history

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

void CFilterEnergyCycleDlg::slotRestoreDefaults(bool)
{
    QString sikName = tmpEnergySet.nameOfSet;
    tmpEnergySet = defaultSet;
    tmpEnergySet.nameOfSet = sikName;
    updateUi();
    slotApply(true);
}

void CFilterEnergyCycleDlg::slotSetNameOfSet()
{
    tmpEnergySet.nameOfSet = lineEditNameOfSet->text();
    setWindowTitle(tr("Energy Cycling Set: '") + tmpEnergySet.nameOfSet + "'");
}

void CFilterEnergyCycleDlg::slotSetWeight(qreal)
{
    tmpEnergySet.driverWeight = spinDriverWeight->value();
    tmpEnergySet.bikeWeight = spinBikeWeight->value();
    labelTotalWeight->setText(QString("<b>%1kg</b>").arg(tmpEnergySet.driverWeight + tmpEnergySet.bikeWeight, 0, 'f', 1));
}

void CFilterEnergyCycleDlg::slotSetComboWindSpeed(qint32 windSpeedIndex)
{
    tmpEnergySet.windSpeedIndex = windSpeedIndex;
    if(windSpeedIndex > 0)
    {
        spinWindSpeed->setValue(windSpeeds[windSpeedIndex].windSpeed * 3.6); // m/s ==> km/h
        tmpEnergySet.windSpeed = windSpeeds[windSpeedIndex].windSpeed;
    }
}

void CFilterEnergyCycleDlg::slotSetWindSpeed(qreal windSpeed)
{
    if(qFuzzyIsNull(windSpeed)) // to avoid numerical noise
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
    for(qint32 i = 1; i < windSpeeds.size(); ++i)
    {
        if(qFuzzyCompare(1 + tmpEnergySet.windSpeed, 1 + windSpeeds[i].windSpeed))
        {
            tmpEnergySet.windSpeedIndex = i;
            comboWindSpeed->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterEnergyCycleDlg::slotSetAirDensity(qreal airDensity)
{
    tmpEnergySet.airDensity = airDensity;
}

void CFilterEnergyCycleDlg::slotSetComboWindPosition(qint32 windPositionIndex)
{
    tmpEnergySet.windPositionIndex = windPositionIndex;
    if(windPositionIndex > 0)
    {
        spinFrontalArea->setValue(windPositions[windPositionIndex].frontalArea);
        spinWindDragCoeff->setValue(windPositions[windPositionIndex].windDragCoeff);
        tmpEnergySet.frontalArea = windPositions[windPositionIndex].frontalArea;
        tmpEnergySet.windDragCoeff = windPositions[windPositionIndex].windDragCoeff;
    }
}

void CFilterEnergyCycleDlg::slotSetWindPositionSpins(qreal)
{
    tmpEnergySet.frontalArea = spinFrontalArea->value();
    tmpEnergySet.windDragCoeff = spinWindDragCoeff->value();

    tmpEnergySet.windPositionIndex = 0;
    comboWindPosition->setCurrentIndex(0);
    for(qint32 i = 1; i < windPositions.size(); ++i)
    {
        if(qFuzzyCompare(1 + tmpEnergySet.frontalArea, 1 + windPositions[i].frontalArea)
                && qFuzzyCompare(1 + tmpEnergySet.windDragCoeff, 1 + windPositions[i].windDragCoeff))
        {
            tmpEnergySet.windPositionIndex = i;
            comboWindPosition->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterEnergyCycleDlg::slotSetComboGround(qint32 groundIndex)
{
    tmpEnergySet.groundIndex = groundIndex;
    if(groundIndex > 0)
    {
        spinRollingCoeff->setValue(grounds[groundIndex].rollingCoeff);
        tmpEnergySet.rollingCoeff = grounds[groundIndex].rollingCoeff;
    }
}

void CFilterEnergyCycleDlg::slotSetRollingCoeff(qreal rollingCoeff)
{
    tmpEnergySet.rollingCoeff = rollingCoeff;

    tmpEnergySet.groundIndex = 0;
    comboGround->setCurrentIndex(0);
    for(qint32 i = 1; i < grounds.size(); ++i)
    {
        if(qFuzzyCompare(1 + rollingCoeff, 1 + grounds[i].rollingCoeff))
        {
            tmpEnergySet.groundIndex = i;
            comboGround->setCurrentIndex(i);
            break;
        }
    }
}

void CFilterEnergyCycleDlg::slotSetPedalCadence(qreal pedalCadence)
{
    tmpEnergySet.pedalCadence = pedalCadence;
}
