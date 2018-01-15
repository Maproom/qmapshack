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

    lblWarning->setText("");
    constantSpeed->setSuffix(IUnit::self().speedunit);
    plainSpeed->setSuffix(IUnit::self().speedunit);
    minSpeed->setSuffix(IUnit::self().speedunit);
    maxSpeed->setSuffix(IUnit::self().speedunit);

    QString val, slopeUnit;
    IUnit::self().slope2string(0, val, slopeUnit);
    slopeAtMaxSpeed->setSuffix(slopeUnit);
    slopeAtMinSpeed->setSuffix(slopeUnit);

    cycling_type_t cyclingType;
    cyclingType.name = "City";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 15;
    cyclingType.minSpeed = 5;
    cyclingType.slopeAtMinSpeed = 5;
    cyclingType.maxSpeed = 35;
    cyclingType.slopeAtMaxSpeed = -5;
    cyclingTypes << cyclingType;
    cyclingType.name = "Trekking";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 20;
    cyclingType.minSpeed = 5;
    cyclingType.slopeAtMinSpeed = 8;
    cyclingType.maxSpeed = 40;
    cyclingType.slopeAtMaxSpeed = -5;
    cyclingTypes << cyclingType;
    cyclingType.name = "Sportive";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 27;
    cyclingType.minSpeed = 7;
    cyclingType.slopeAtMinSpeed = 10;
    cyclingType.maxSpeed = 60;
    cyclingType.slopeAtMaxSpeed = -8;
    cyclingTypes << cyclingType;
    cyclingType.name = "Mountain";
    cbxCyclingType->addItem(cyclingType.name);
    cyclingType.plainSpeed = 15;
    cyclingType.minSpeed = 4;
    cyclingType.slopeAtMinSpeed = 15;
    cyclingType.maxSpeed = 40;
    cyclingType.slopeAtMaxSpeed = -15;
    cyclingTypes << cyclingType;

    for (int i = 0; i < 3; ++i)
    {
        cyclingType.name = QString("Custom %1").arg(i);
        cbxCyclingType->addItem(cyclingType.name);
        cyclingType.plainSpeed = 20;
        cyclingType.minSpeed = 5;
        cyclingType.slopeAtMinSpeed = 8;
        cyclingType.maxSpeed = 40;
        cyclingType.slopeAtMaxSpeed = -5;
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
    slotSetActivityType(cbxActivityType->currentIndex());
    connect(cbxActivityType, SIGNAL(activated(int)), this, SLOT(slotSetActivityType(int)));

    cbxCyclingType->setCurrentIndex(cfg.value("TrackDetails/Filter/Speed/cyclingType", 0).toInt());
    slotSetCyclingType(cbxCyclingType->currentIndex());
    connect(cbxCyclingType, SIGNAL(activated(int)), this, SLOT(slotSetCyclingType(int)));

    constantSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/speed", 18.0).toDouble());

    connect(plainSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetPlainSpeed(double)));
    connect(minSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMinSpeed(double)));
    connect(slopeAtMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMinSpeed(double)));
    connect(maxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMaxSpeed(double)));
    connect(slopeAtMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMaxSpeed(double)));

    connect(toolApply, &QToolButton::clicked, this, &CFilterSpeed::slotApply);

    connect(pbSetMinMaxSlope, SIGNAL(clicked(bool)), this, SLOT(slotSetMinMaxSlopes(bool)));
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
        qreal slopeAtMinSpeedUnit = 0, slopeAtMaxSpeedUnit = 0;

        if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
        {
            slopeAtMinSpeedUnit = IUnit::slopeConvert(IUnit::eSlopeDegrees, cyclingTypes[i].slopeAtMinSpeed);
            slopeAtMaxSpeedUnit = IUnit::slopeConvert(IUnit::eSlopeDegrees, cyclingTypes[i].slopeAtMaxSpeed);
        }
        else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
        {
            slopeAtMinSpeedUnit = cyclingTypes[i].slopeAtMinSpeed;
            slopeAtMaxSpeedUnit = cyclingTypes[i].slopeAtMaxSpeed;
        }
        trk.filterSpeed(cyclingTypes[i].plainSpeed / IUnit::self().speedfactor,
                        cyclingTypes[i].minSpeed / IUnit::self().speedfactor, slopeAtMinSpeedUnit,
                        cyclingTypes[i].maxSpeed / IUnit::self().speedfactor, slopeAtMaxSpeedUnit);
        break;
    }
    default:
        break;
    }

    CCanvas::restoreOverrideCursor("CFilterSpeed");
}

void CFilterSpeed::slotSetActivityType(int type)
{
    stackedWidget->setCurrentIndex(type);

    if(type == 1 && !trk.filterCheckForValidSlopes())
    {
        QString str = QString("<b style='color: red;'>" +
                              tr("Track has no or invalid elevation data. Please correct or set constant speed!") +
                              "</b><br/>");
        lblWarning->setText(str);
        pgeCycling->setEnabled(false);
        toolApply->setEnabled(false);
        return;
    }

    lblWarning->setText("");
    pgeCycling->setEnabled(true);
    toolApply->setEnabled(true);
}

void CFilterSpeed::slotSetCyclingType(int type)
{
    plainSpeed->setValue(cyclingTypes[type].plainSpeed);
    maxSpeed->setMinimum(cyclingTypes[type].plainSpeed);
    minSpeed->setMaximum(cyclingTypes[type].plainSpeed);

    minSpeed->setValue(cyclingTypes[type].minSpeed);
    plainSpeed->setMinimum(cyclingTypes[type].minSpeed);

    maxSpeed->setValue(cyclingTypes[type].maxSpeed);
    plainSpeed->setMaximum(cyclingTypes[type].maxSpeed);

    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingTypes[type].slopeAtMinSpeed);
        slopeAtMinSpeed->setValue(val);
        val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingTypes[type].slopeAtMaxSpeed);
        slopeAtMaxSpeed->setValue(val);
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        slopeAtMinSpeed->setValue(cyclingTypes[type].slopeAtMinSpeed);
        slopeAtMaxSpeed->setValue(cyclingTypes[type].slopeAtMaxSpeed);
    }

    if (type < 4)
    {
        frmCyclingParameter->hide();
    }
    else
    {
        frmCyclingParameter->show();
    }
}

void CFilterSpeed::slotSetPlainSpeed(double speed)
{
    cyclingTypes[cbxCyclingType->currentIndex()].plainSpeed = speed;
    maxSpeed->setMinimum(speed);
    minSpeed->setMaximum(speed);
}

void CFilterSpeed::slotSetMinSpeed(double speed)
{
    cyclingTypes[cbxCyclingType->currentIndex()].minSpeed = speed;
    plainSpeed->setMinimum(speed);
}

void CFilterSpeed::slotSetSlopeAtMinSpeed(double slope)
{
    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, slope);
        cyclingTypes[cbxCyclingType->currentIndex()].slopeAtMinSpeed = val;
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        cyclingTypes[cbxCyclingType->currentIndex()].slopeAtMinSpeed = slope;
    }
}

void CFilterSpeed::slotSetMaxSpeed(double speed)
{
    cyclingTypes[cbxCyclingType->currentIndex()].maxSpeed = speed;
    plainSpeed->setMaximum(speed);
}

void CFilterSpeed::slotSetSlopeAtMaxSpeed(double slope)
{
    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, slope);
        cyclingTypes[cbxCyclingType->currentIndex()].slopeAtMaxSpeed = val;
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        cyclingTypes[cbxCyclingType->currentIndex()].slopeAtMaxSpeed = slope;
    }
}

void CFilterSpeed::slotSetMinMaxSlopes(bool)
{
    qreal minSlope, maxSlope;
    trk.filterGetMinMaxSlopes(minSlope, maxSlope);

    if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, maxSlope);
        slopeAtMinSpeed->setValue(val);
        val = IUnit::slopeConvert(IUnit::eSlopeDegrees, minSlope);
        slopeAtMaxSpeed->setValue(val);
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        slopeAtMinSpeed->setValue(maxSlope);
        slopeAtMaxSpeed->setValue(minSlope);
    }
}
