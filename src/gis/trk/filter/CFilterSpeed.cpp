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

#include "canvas/CCanvas.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterSpeed.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"


CFilterSpeed::CFilterSpeed(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    linearSpeed->setSuffix(IUnit::self().speedunit);
    minSpeed->setSuffix(IUnit::self().speedunit);
    maxSpeed->setSuffix(IUnit::self().speedunit);

    SETTINGS;
    linearSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/speed",1).toDouble());

    connect(toolApply, &QToolButton::clicked, this, &CFilterSpeed::slotApply);
    connect(isSlopeCalcEnabled, &QCheckBox::toggled, this, &CFilterSpeed::slotSlopeCalcEnabled);
}

CFilterSpeed::~CFilterSpeed()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/Speed/speed", linearSpeed->value());
}

void CFilterSpeed::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterSpeed");

    if(isSlopeCalcEnabled)
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.exec();
        trk.filterSpeed(linearSpeed->value()/IUnit::self().speedfactor);
    }
    else
    {
        trk.filterSpeed(linearSpeed->value()/IUnit::self().speedfactor);

    }


    CCanvas::restoreOverrideCursor("CFilterSpeed");
}

void CFilterSpeed::slotSlopeCalcEnabled(bool checked)
{
    if (checked)
    {
//        speedSlopeWidget->setVisible(true);
//        minSpeed->setEnabled(true);
//        maxSpeed->setEnabled(true);
//        minSlope->setEnabled(true);
//        maxSlope->setEnabled(true);
    }
    else
    {
//        speedSlopeWidget->setVisible(false);
//        minSpeed->setEnabled(false);
//        maxSpeed->setEnabled(false);
//        minSlope->setEnabled(false);
//        maxSlope->setEnabled(false);
    }

}
