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

    labelWarning->setText("");
    constantSpeed->setSuffix(IUnit::self().speedunit);
    plainSpeed->setSuffix(IUnit::self().speedunit);
    minSpeed->setSuffix(IUnit::self().speedunit);
    maxSpeed->setSuffix(IUnit::self().speedunit);

    QString val, slopeUnit;
    IUnit::self().slope2string(0, val, slopeUnit);
    slopeAtMaxSpeed->setSuffix(slopeUnit);
    slopeAtMinSpeed->setSuffix(slopeUnit);

    const QList<cycling_type_t> cyclingTypeDefaults =
    {
        {
           tr("City")           // name
           , 15                 // plainSpeed
           , 5                  // minSpeed
           , 5                  // slopeAtMinSpeed
           , 35                 // maxSpeed
           , -5                 // slopeAtMaxSpeed
        },
        {
            tr("Trekking"), 20, 5, 8, 40, -5
        },
        {
            tr("Sportive"), 27, 7, 10, 60, -8
        },
        {
            tr("Mountain"), 15, 4, 15, 40, -15
        },
        {
            tr("Custom 0"), 20, 5, 8, 40,-5
        },
        {
            tr("Custom 1"), 20, 5, 8, 40,-5
        },
        {
            tr("Custom 2"), 20, 5, 8, 40,-5
        }
    };

    SETTINGS;
    cycling_type_t cyclingType;
    cfg.beginReadArray("TrackDetails/Filter/Speed/CustomCyclingTypes");
    for (int i = 0; i < cyclingTypeDefaults.size(); ++i)
    {
        cfg.setArrayIndex(i);
        cyclingType.name = cfg.value("name", cyclingTypeDefaults[i].name).toString();
        cyclingType.plainSpeed = cfg.value("plainSpeed", cyclingTypeDefaults[i].plainSpeed).toDouble();
        cyclingType.minSpeed = cfg.value("minSpeed", cyclingTypeDefaults[i].minSpeed).toDouble();
        cyclingType.slopeAtMinSpeed = cfg.value("slopeAtMinSpeed", cyclingTypeDefaults[i].slopeAtMinSpeed).toDouble();
        cyclingType.maxSpeed = cfg.value("maxSpeed", cyclingTypeDefaults[i].maxSpeed).toDouble();
        cyclingType.slopeAtMaxSpeed = cfg.value("slopeAtMaxSpeed", cyclingTypeDefaults[i].slopeAtMaxSpeed).toDouble();

        comboCyclingType->addItem(cyclingType.name);
        cyclingTypes << cyclingType;
    }
    cfg.endArray();

    comboActivityType->setCurrentIndex(cfg.value("TrackDetails/Filter/Speed/activityType", 0).toInt());
    slotSetActivityType(comboActivityType->currentIndex());
    connect(comboActivityType, SIGNAL(activated(int)), this, SLOT(slotSetActivityType(int)));

    comboCyclingType->setCurrentIndex(cfg.value("TrackDetails/Filter/Speed/cyclingType", 0).toInt());
    slotSetCyclingType(comboCyclingType->currentIndex());
    connect(comboCyclingType, SIGNAL(activated(int)), this, SLOT(slotSetCyclingType(int)));

    constantSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/speed", 18.0).toDouble());

    connect(plainSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetPlainSpeed(double)));
    connect(minSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMinSpeed(double)));
    connect(slopeAtMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMinSpeed(double)));
    connect(maxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMaxSpeed(double)));
    connect(slopeAtMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMaxSpeed(double)));

    connect(toolApply, &QToolButton::clicked, this, &CFilterSpeed::slotApply);

    connect(pushSetMinMaxSlope, SIGNAL(clicked(bool)), this, SLOT(slotSetMinMaxSlopes(bool)));
}

CFilterSpeed::~CFilterSpeed()
{
    SETTINGS;
    cfg.beginWriteArray("TrackDetails/Filter/Speed/CustomCyclingTypes");
    int i = 0;
    for(const cycling_type_t& cyclingType : cyclingTypes)
    {
        cfg.setArrayIndex(i++);
        cfg.setValue("name", cyclingType.name);
        cfg.setValue("plainSpeed", cyclingType.plainSpeed);
        cfg.setValue("minSpeed", cyclingType.minSpeed);
        cfg.setValue("slopeAtMinSpeed", cyclingType.slopeAtMinSpeed);
        cfg.setValue("maxSpeed", cyclingType.maxSpeed);
        cfg.setValue("slopeAtMaxSpeed", cyclingType.slopeAtMaxSpeed);
    }
    cfg.endArray();

    cfg.setValue("TrackDetails/Filter/Speed/activityType", comboActivityType->currentIndex());
    cfg.setValue("TrackDetails/Filter/Speed/cyclingType", comboCyclingType->currentIndex());
    cfg.setValue("TrackDetails/Filter/Speed/speed", constantSpeed->value());
}

void CFilterSpeed::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterSpeed");

    switch (comboActivityType->currentIndex())
    {
    case 0:
        trk.filterSpeed(constantSpeed->value()/IUnit::self().speedfactor);
        break;
    case 1:
    {
        qint32 i = comboCyclingType->currentIndex();
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

void CFilterSpeed::slotSetActivityType(int type)
{
    stackedWidget->setCurrentIndex(type);
    setElevationValid();
}

void CFilterSpeed::setElevationValid()
{
    if(trk.isTrkElevationInvalid() && comboActivityType->currentIndex() == 1)
    {
        QString str = QString("<b style='color: red;'>" +
                              tr("Track has no or invalid elevation data. Please correct or set constant speed!") +
                              "</b><br/>");
        labelWarning->setText(str);
        pageCycling->setEnabled(false);
        toolApply->setEnabled(false);
        return;
    }
    else
    {
        labelWarning->setText("");
        pageCycling->setEnabled(true);
        toolApply->setEnabled(true);
    }

}

void CFilterSpeed::slotSetCyclingType(int type)
{
    const cycling_type_t &cyclingType = cyclingTypes[type];

    plainSpeed->setValue(cyclingType.plainSpeed);
    maxSpeed->setMinimum(cyclingType.plainSpeed);
    minSpeed->setMaximum(cyclingType.plainSpeed);

    minSpeed->setValue(cyclingType.minSpeed);
    plainSpeed->setMinimum(cyclingType.minSpeed);

    maxSpeed->setValue(cyclingType.maxSpeed);
    plainSpeed->setMaximum(cyclingType.maxSpeed);

    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingType.slopeAtMinSpeed);
        slopeAtMinSpeed->setValue(val);
        val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingType.slopeAtMaxSpeed);
        slopeAtMaxSpeed->setValue(val);
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        slopeAtMinSpeed->setValue(cyclingType.slopeAtMinSpeed);
        slopeAtMaxSpeed->setValue(cyclingType.slopeAtMaxSpeed);
    }

    if (type < 4)
    {
        frameCycling->setEnabled(false);
    }
    else
    {
        frameCycling->setEnabled(true);
    }
}

void CFilterSpeed::slotSetPlainSpeed(double speed)
{
    cyclingTypes[comboCyclingType->currentIndex()].plainSpeed = speed;
    maxSpeed->setMinimum(speed);
    minSpeed->setMaximum(speed);
}

void CFilterSpeed::slotSetMinSpeed(double speed)
{
    cyclingTypes[comboCyclingType->currentIndex()].minSpeed = speed;
    plainSpeed->setMinimum(speed);
}

void CFilterSpeed::slotSetSlopeAtMinSpeed(double slope)
{
    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, slope);
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMinSpeed = val;
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMinSpeed = slope;
    }
}

void CFilterSpeed::slotSetMaxSpeed(double speed)
{
    cyclingTypes[comboCyclingType->currentIndex()].maxSpeed = speed;
    plainSpeed->setMaximum(speed);
}

void CFilterSpeed::slotSetSlopeAtMaxSpeed(double slope)
{
    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopeDegrees, slope);
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMaxSpeed = val;
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        cyclingTypes[comboCyclingType->currentIndex()].slopeAtMaxSpeed = slope;
    }
}

void CFilterSpeed::slotSetMinMaxSlopes(bool)
{
    qreal minSlope, maxSlope;
    trk.filterGetSlopeLimits(minSlope, maxSlope);

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
