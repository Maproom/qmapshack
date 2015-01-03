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

#include "gis/trk/filter/CFilterSpeed.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"


CFilterSpeed::CFilterSpeed(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    doubleSpinBox->setSuffix(IUnit::self().speedunit);

    SETTINGS;
    doubleSpinBox->setValue(cfg.value("TrackDetails/Filter/Speed/speed",1).toDouble());

    connect(toolApply, SIGNAL(clicked()), this, SLOT(slotApply()));
}

CFilterSpeed::~CFilterSpeed()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/Speed/speed", doubleSpinBox->value());
}

void CFilterSpeed::slotApply()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    trk.filterSpeed(doubleSpinBox->value()/IUnit::self().speedfactor);
    QApplication::restoreOverrideCursor();
}
