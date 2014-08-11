/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/wpt/CProjWpt.h"
#include "gis/wpt/CGisItemWpt.h"
#include "units/IUnit.h"

#include <QtWidgets>

CProjWpt::CProjWpt(CGisItemWpt& wpt, QWidget *parent)
    : QDialog(parent)
    , wpt(wpt)
{
    setupUi(this);

    QString val, unit;
    IUnit::self().meter2distance(0,val,unit);
    labelDistUnit->setText(unit);
}

CProjWpt::~CProjWpt()
{

}

void CProjWpt::accept()
{
    qreal dist = lineDist->text().toDouble();
    qreal head = lineHead->text().toDouble();

    if((dist <= 0) || (head > 360) || (head < -360))
    {
        return;
    }

    QDialog::accept();
}
