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

#include "units/CTimeZoneSetup.h"
#include "units/IUnit.h"

CTimeZoneSetup::CTimeZoneSetup(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    QByteArray zone;
    IUnit::tz_mode_e mode;
    bool useShortFormat;

    IUnit::getTimeZoneSetup(mode, zone, useShortFormat);

    switch(mode)
    {
    case IUnit::eTZUtc:
        radioUtc->setChecked(true);
        break;

    case IUnit::eTZLocal:
        radioLocal->setChecked(true);
        break;

    case IUnit::eTZAuto:
        radioAutomatic->setChecked(true);
        break;

    case IUnit::eTZSelected:
        radioSelected->setChecked(true);
        break;
    }

    const char ** tz = IUnit::tblTimezone;
    while(*tz)
    {
        comboTimeZone->addItem(*tz);
        tz++;
    }

    if(useShortFormat)
    {
        radioShortFormat->setChecked(true);
    }
    else
    {
        radioLongFormat->setChecked(true);
    }

    comboTimeZone->setCurrentIndex(comboTimeZone->findText(QString(zone)));
}

CTimeZoneSetup::~CTimeZoneSetup()
{
}

void CTimeZoneSetup::accept()
{
    QByteArray zone = comboTimeZone->currentText().toLatin1();
    IUnit::tz_mode_e mode = IUnit::eTZUtc;
    bool useShortFormat = false;

    if(radioUtc->isChecked())
    {
        mode = IUnit::eTZUtc;
    }
    else if(radioLocal->isChecked())
    {
        mode = IUnit::eTZLocal;
    }
    else if(radioAutomatic->isChecked())
    {
        mode = IUnit::eTZAuto;
    }
    else if(radioSelected->isChecked())
    {
        mode = IUnit::eTZSelected;
    }

    if(radioShortFormat->isChecked())
    {
        useShortFormat = true;
    }

    IUnit::setTimeZoneSetup(mode, zone, useShortFormat);
    QDialog::accept();
}
