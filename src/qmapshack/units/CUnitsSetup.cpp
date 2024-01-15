/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "units/CUnitsSetup.h"

#include "CMainWindow.h"
#include "units/IUnit.h"

#include "helpers/CSettings.h"

CUnitsSetup::CUnitsSetup(QWidget* parent) : QDialog(parent) {
  setupUi(this);

  switch (IUnit::self().type) {
    case IUnit::eTypeMetric:
      radioMetric->setChecked(true);
      spinRoundLimit->setSuffix(" Km");
      break;

    case IUnit::eTypeImperial:
      radioImperial->setChecked(true);
      spinRoundLimit->setSuffix(" mi");
      break;

    case IUnit::eTypeNautic:
      radioNautic->setChecked(true);
      spinRoundLimit->setSuffix(" nm");
      break;

    case IUnit::eTypeAviation:
      radioAviation->setChecked(true);
      spinRoundLimit->setSuffix(" nm");
      break;
  }

  switch (IUnit::getSlopeMode()) {
    case IUnit::eSlopeDegrees:
      radioDegrees->setChecked(true);
      break;

    case IUnit::eSlopePercent:
      radioPercent->setChecked(true);
      break;
  }
 
  connect(radioMetric, &QRadioButton::clicked, this, &CUnitsSetup::RoundUnitChange);
  connect(radioImperial, &QRadioButton::clicked, this, &CUnitsSetup::RoundUnitChange);
  connect(radioNautic, &QRadioButton::clicked, this, &CUnitsSetup::RoundUnitChange);
  connect(radioAviation, &QRadioButton::clicked, this, &CUnitsSetup::RoundUnitChange);

  SETTINGS;
  cfg.beginGroup("Units");
  spinRoundLimit->setValue(cfg.value("roundLimit", 20).toInt());
  cfg.endGroup();

  adjustSize();
}

void CUnitsSetup::RoundUnitChange() {
  if (radioMetric->isChecked()) {
    spinRoundLimit->setSuffix(" km");
  } else if (radioImperial->isChecked()) {
    spinRoundLimit->setSuffix(" mi ");
  } else if (radioNautic->isChecked()) {
    spinRoundLimit->setSuffix(" nm");
  } else if (radioAviation->isChecked()) {
    spinRoundLimit->setSuffix(" nm");
  }
}

void CUnitsSetup::accept() {
  if (radioMetric->isChecked()) {
    IUnit::setUnitType(IUnit::eTypeMetric, &CMainWindow::self());
  } else if (radioImperial->isChecked()) {
    IUnit::setUnitType(IUnit::eTypeImperial, &CMainWindow::self());
  } else if (radioNautic->isChecked()) {
    IUnit::setUnitType(IUnit::eTypeNautic, &CMainWindow::self());
  } else if (radioAviation->isChecked()) {
    IUnit::setUnitType(IUnit::eTypeAviation, &CMainWindow::self());
  }

  if (radioDegrees->isChecked()) {
    IUnit::setSlopeMode(IUnit::eSlopeDegrees);
  } else if (radioPercent->isChecked()) {
    IUnit::setSlopeMode(IUnit::eSlopePercent);
  }

  SETTINGS;
  cfg.beginGroup("Units");
  cfg.setValue("roundLimit", spinRoundLimit->value());
  cfg.endGroup();

  QDialog::accept();
}

