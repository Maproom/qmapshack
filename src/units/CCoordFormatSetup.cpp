/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "units/CCoordFormatSetup.h"
#include "units/IUnit.h"

CCoordFormatSetup::CCoordFormatSetup(QWidget * parent)
    : QDialog(parent)
{
    setupUi(this);

    IUnit::coord_format_e coordFormat;
    IUnit::getCoordFormat(coordFormat);
    switch(coordFormat)
    {
    case IUnit::eCoordFormat1:
        radioFormat1->setChecked(true);
        break;

    case IUnit::eCoordFormat2:
        radioFormat2->setChecked(true);
        break;

    case IUnit::eCoordFormat3:
        radioFormat3->setChecked(true);
        break;
    }
}

CCoordFormatSetup::~CCoordFormatSetup()
{
}

void CCoordFormatSetup::accept()
{
    IUnit::coord_format_e coordFormat;

    if(radioFormat1->isChecked())
    {
        coordFormat = IUnit::eCoordFormat1;
    }
    else if(radioFormat2->isChecked())
    {
        coordFormat = IUnit::eCoordFormat2;
    }
    else if(radioFormat3->isChecked())
    {
        coordFormat = IUnit::eCoordFormat3;
    }

    IUnit::setCoordFormat(coordFormat);
    QDialog::accept();
}
