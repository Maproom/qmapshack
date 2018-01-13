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

    constantSpeed->setSuffix(IUnit::self().speedunit);
    plainSpeed->setSuffix(IUnit::self().speedunit);
    minSpeed->setSuffix(IUnit::self().speedunit);
    maxSpeed->setSuffix(IUnit::self().speedunit);

    cycling_type_t cyclingType;
    cyclingType.name = "City";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 15;
    cyclingType.minSpeed = 4;
    cyclingType.slopeAtMinSpeed = 5;
    cyclingType.maxSpeed = 35;
    cyclingType.slopeAtMaxSpeed = -10;
    cyclingTypes << cyclingType;
    cyclingType.name = "Trekking";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 20;
    cyclingType.minSpeed = 5;
    cyclingType.slopeAtMinSpeed = 8;
    cyclingType.maxSpeed = 45;
    cyclingType.slopeAtMaxSpeed = -10;
    cyclingTypes << cyclingType;
    cyclingType.name = "Sportive";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 25;
    cyclingType.minSpeed = 7;
    cyclingType.slopeAtMinSpeed = 12;
    cyclingType.maxSpeed = 60;
    cyclingType.slopeAtMaxSpeed = -14;
    cyclingTypes << cyclingType;
    cyclingType.name = "Mountain";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 15;
    cyclingType.minSpeed = 4;
    cyclingType.slopeAtMinSpeed = 15;
    cyclingType.maxSpeed = 40;
    cyclingType.slopeAtMaxSpeed = -20;
    cyclingTypes << cyclingType;

    for (int i = 0; i < 3; ++i)
    {
        cyclingType.name = QString("Custom %1").arg(i);
        cbxCyclingType->addItem(cyclingType.name);
        cyclingType.plainSpeed = 20;
        cyclingType.minSpeed = 5;
        cyclingType.slopeAtMinSpeed = 8;
        cyclingType.maxSpeed = 40;
        cyclingType.slopeAtMaxSpeed = -10;
        cyclingTypes << cyclingType;
    }

    SETTINGS;
    int size = cfg.beginReadArray("TrackDetails/Filter/Speed/CustomCyclingTypes");
    for (int i = 0; i < size; ++i)
    {
        cfg.setArrayIndex(i);
        QString defaultName = QString("Custom %1").arg(i);
        cyclingType.name = cfg.value("name", defaultName).toString();
        cyclingType.plainSpeed = cfg.value("plainSpeed", 20).toDouble();
        cyclingType.minSpeed = cfg.value("minSpeed", 5).toDouble();
        cyclingType.slopeAtMinSpeed = cfg.value("slopeAtMinSpeed", 8).toDouble();
        cyclingType.maxSpeed = cfg.value("maxSpeed", 40).toDouble();
        cyclingType.slopeAtMaxSpeed = cfg.value("slopeAtMaxSpeed", -10).toDouble();

        cyclingTypes.replace(i, cyclingType);
        cbxCyclingType->setItemText(i, cyclingType.name);
    }
    cfg.endArray();

    cbxActivityType->setCurrentIndex(cfg.value("TrackDetails/Filter/Speed/activityType", 0).toInt());
    stackedWidget->setCurrentIndex(cbxActivityType->currentIndex());

    cbxCyclingType->setCurrentIndex(cfg.value("TrackDetails/Filter/Speed/cyclingType", 0).toInt());
    setCyclingType(cbxCyclingType->currentIndex());

    constantSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/speed", 18.0).toDouble());

    connect(cbxActivityType, SIGNAL(activated(int)), stackedWidget, SLOT(setCurrentIndex(int)));
    connect(cbxCyclingType, SIGNAL(activated(int)), this, SLOT(setCyclingType(int)));

    connect(plainSpeed, SIGNAL(valueChanged(double)), this, SLOT(setPlainSpeed(double)));
    connect(minSpeed, SIGNAL(valueChanged(double)), this, SLOT(setMinSpeed(double)));
    connect(slopeAtMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(setSlopeAtMinSpeed(double)));
    connect(maxSpeed, SIGNAL(valueChanged(double)), this, SLOT(setMaxSpeed(double)));
    connect(slopeAtMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(setSlopeAtMaxSpeed(double)));

    connect(toolApply, &QToolButton::clicked, this, &CFilterSpeed::slotApply);
}

CFilterSpeed::~CFilterSpeed()
{
    SETTINGS;
    cfg.beginWriteArray("TrackDetails/Filter/Speed/CustomCyclingTypes");
    for (int i = 0; i < cyclingTypes.size(); ++i) {
        cfg.setArrayIndex(i);
        cfg.setValue("name", cyclingTypes.at(i).name);
        cfg.setValue("plainSpeed", cyclingTypes.at(i).plainSpeed);
        cfg.setValue("minSpeed", cyclingTypes.at(i).minSpeed);
        cfg.setValue("slopeAtMinSpeed", cyclingTypes.at(i).slopeAtMinSpeed);
        cfg.setValue("maxSpeed", cyclingTypes.at(i).maxSpeed);
        cfg.setValue("slopeAtMaxSpeed", cyclingTypes.at(i).slopeAtMaxSpeed);
    }
    cfg.endArray();

    cfg.setValue("TrackDetails/Filter/Speed/activityType", cbxActivityType->currentIndex());
    cfg.setValue("TrackDetails/Filter/Speed/cyclingType", cbxCyclingType->currentIndex());

    cfg.setValue("TrackDetails/Filter/Speed/speed", constantSpeed->value());
}

void CFilterSpeed::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterSpeed");

    switch (cbxActivityType->currentIndex())
    {
    case 0:
        trk.filterSpeed(constantSpeed->value()/IUnit::self().speedfactor);
        break;
    case 1:
    {
        qint32 i = cbxCyclingType->currentIndex();
        trk.filterSpeed(cyclingTypes[i].plainSpeed / IUnit::self().speedfactor,
                        cyclingTypes[i].minSpeed / IUnit::self().speedfactor, cyclingTypes[i].slopeAtMinSpeed,
                        cyclingTypes[i].maxSpeed / IUnit::self().speedfactor, cyclingTypes[i].slopeAtMaxSpeed);
        break;
    }
    default:
        break;
    }

    CCanvas::restoreOverrideCursor("CFilterSpeed");
}


void CFilterSpeed::setCyclingType(int type)
{
    plainSpeed->setValue(cyclingTypes[type].plainSpeed);
    minSpeed->setValue(cyclingTypes[type].minSpeed);
    slopeAtMinSpeed->setValue(cyclingTypes[type].slopeAtMinSpeed);
    maxSpeed->setValue(cyclingTypes[type].maxSpeed);
    slopeAtMaxSpeed->setValue(cyclingTypes[type].slopeAtMaxSpeed);

    if (type <= 3)
    {
        frmCyclingParameter->hide();
    }
    else
    {
        frmCyclingParameter->show();
    }
}

void CFilterSpeed::setPlainSpeed(double speed)
{
    cyclingTypes[cbxCyclingType->currentIndex()].plainSpeed = speed;
}

void CFilterSpeed::setMinSpeed(double speed)
{
    cyclingTypes[cbxCyclingType->currentIndex()].minSpeed = speed;
}

void CFilterSpeed::setSlopeAtMinSpeed(double slope)
{
    cyclingTypes[cbxCyclingType->currentIndex()].slopeAtMinSpeed = slope;
}

void CFilterSpeed::setMaxSpeed(double speed)
{
    cyclingTypes[cbxCyclingType->currentIndex()].maxSpeed = speed;
}

void CFilterSpeed::setSlopeAtMaxSpeed(double slope)
{
    cyclingTypes[cbxCyclingType->currentIndex()].slopeAtMaxSpeed = slope;
}

