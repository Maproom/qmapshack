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
    minSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/minSpeed",1).toDouble());
    maxSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/maxSpeed",1).toDouble());
    slopeAtMinSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/slopeAtMinSpeed",1).toDouble());
    slopeAtMaxSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/slopeAtMaxSpeed",1).toDouble());
    isSlopeEnabled->setChecked(cfg.value("TrackDetails/Filter/Speed/isSlopeEnabled",1).toBool());

    connect(toolApply, &QToolButton::clicked, this, &CFilterSpeed::slotApply);
    connect(isSlopeEnabled, &QCheckBox::toggled, this, &CFilterSpeed::slotSlopeEnabled);

    slotSlopeEnabled(isSlopeEnabled->isChecked());
}

CFilterSpeed::~CFilterSpeed()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/Speed/speed", linearSpeed->value());
    cfg.setValue("TrackDetails/Filter/Speed/minSpeed", minSpeed->value());
    cfg.setValue("TrackDetails/Filter/Speed/maxSpeed", maxSpeed->value());
    cfg.setValue("TrackDetails/Filter/Speed/slopeAtMinSpeed", slopeAtMinSpeed->value());
    cfg.setValue("TrackDetails/Filter/Speed/slopeAtMaxSpeed", slopeAtMaxSpeed->value());
    cfg.setValue("TrackDetails/Filter/Speed/isSlopeEnabled", isSlopeEnabled->isChecked());
}

void CFilterSpeed::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterSpeed");

    if(isSlopeEnabled->isChecked())
    {
//        QMessageBox msgBox;
//        msgBox.setText("The document has been modified.");
//        msgBox.exec();
        trk.filterSpeed(linearSpeed->value() / IUnit::self().speedfactor,
                        minSpeed->value() / IUnit::self().speedfactor, slopeAtMinSpeed->value(),
                        maxSpeed->value() / IUnit::self().speedfactor, slopeAtMaxSpeed->value());
    }
    else
    {
        trk.filterSpeed(linearSpeed->value()/IUnit::self().speedfactor);
    }

    CCanvas::restoreOverrideCursor("CFilterSpeed");
}

void CFilterSpeed::slotSlopeEnabled(bool checked)
{
    if (checked)
    {
        minSpeed->setEnabled(true);
        maxSpeed->setEnabled(true);
        slopeAtMinSpeed->setEnabled(true);
        slopeAtMaxSpeed->setEnabled(true);
    }
    else
    {
        minSpeed->setEnabled(false);
        maxSpeed->setEnabled(false);
        slopeAtMinSpeed->setEnabled(false);
        slopeAtMaxSpeed->setEnabled(false);
    }
}
