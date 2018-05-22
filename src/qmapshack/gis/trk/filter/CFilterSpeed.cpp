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

CFilterSpeed::CFilterSpeed(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent), trk(trk), noOfFixTypes(4), noOfCustomTypes(3)
{
    setupUi(this);

    labelWarning->setText("");
    spinConstantSpeed->setSuffix(IUnit::self().speedunit);
    spinPlainSpeed->setSuffix(IUnit::self().speedunit);
    spinMinSpeed->setSuffix(IUnit::self().speedunit);
    spinMaxSpeed->setSuffix(IUnit::self().speedunit);

    QString val, slopeUnit;
    IUnit::self().slope2string(0, val, slopeUnit);
    spinSlopeAtMaxSpeed->setSuffix(slopeUnit);
    spinSlopeAtMinSpeed->setSuffix(slopeUnit);

    // 4 fix and 3 custom cycling types has be defined as default!
    const QList<cycling_type_t> cyclingTypeDefaults =
    {
        {
            tr("City")          // name           Fix
            , 15                // spinPlainSpeed
            , 5                 // spinMinSpeed
            , 5                 // spinSlopeAtMinSpeed
            , 35                // spinMaxSpeed
            , -5                // spinSlopeAtMaxSpeed
        },
        {
            tr("Trekking"), 20, 5, 8, 40, -5   // Fix
        },
        {
            tr("Sportive"), 27, 7, 10, 60, -8  // Fix
        },
        {
            tr("Mountain"), 15, 4, 15, 40, -15 // Fix
        },
        {
            tr("Custom 0"), 20, 5, 8, 40, -5 // Custom
        },
        {
            tr("Custom 1"), 20, 5, 8, 40, -5 // Custom
        },
        {
            tr("Custom 2"), 20, 5, 8, 40, -5 // Custom
        }
    };

    cycling_type_t cyclingType;
    for (int i = 0; i < noOfFixTypes; ++i)
    {
        const cycling_type_t &cyclingTypeDefault = cyclingTypeDefaults[i];
        cyclingType.name = cyclingTypeDefault.name;
        cyclingType.plainSpeed = cyclingTypeDefault.plainSpeed;
        cyclingType.minSpeed = cyclingTypeDefault.minSpeed;
        cyclingType.slopeAtMinSpeed = cyclingTypeDefault.slopeAtMinSpeed;
        cyclingType.maxSpeed = cyclingTypeDefault.maxSpeed;
        cyclingType.slopeAtMaxSpeed = cyclingTypeDefault.slopeAtMaxSpeed;

        comboCyclingType->addItem(cyclingType.name);
        cyclingTypes << cyclingType;
    }

    SETTINGS;
    cfg.beginReadArray("TrackDetails/Filter/Speed/CustomCyclingTypes");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const cycling_type_t &cyclingTypeDefault = cyclingTypeDefaults[noOfFixTypes + i];
        cfg.setArrayIndex(i);
        cyclingType.name = cyclingTypeDefault.name;
        cyclingType.plainSpeed = cfg.value("spinPlainSpeed", cyclingTypeDefault.plainSpeed).toDouble();
        cyclingType.minSpeed = cfg.value("spinMinSpeed", cyclingTypeDefault.minSpeed).toDouble();
        cyclingType.slopeAtMinSpeed = cfg.value("spinSlopeAtMinSpeed", cyclingTypeDefault.slopeAtMinSpeed).toDouble();
        cyclingType.maxSpeed = cfg.value("spinMaxSpeed", cyclingTypeDefault.maxSpeed).toDouble();
        cyclingType.slopeAtMaxSpeed = cfg.value("spinSlopeAtMaxSpeed", cyclingTypeDefault.slopeAtMaxSpeed).toDouble();

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

    spinConstantSpeed->setValue(cfg.value("TrackDetails/Filter/Speed/speed", 18.0).toDouble());

    connect(spinPlainSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetPlainSpeed(double)));
    connect(spinMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMinSpeed(double)));
    connect(spinSlopeAtMinSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMinSpeed(double)));
    connect(spinMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetMaxSpeed(double)));
    connect(spinSlopeAtMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(slotSetSlopeAtMaxSpeed(double)));

    connect(toolApply, &QToolButton::clicked, this, &CFilterSpeed::slotApply);

    connect(pushSetMinMaxSlope, SIGNAL(clicked(bool)), this, SLOT(slotSetMinMaxSlopes(bool)));
}

CFilterSpeed::~CFilterSpeed()
{
    SETTINGS;
    cfg.beginWriteArray("TrackDetails/Filter/Speed/CustomCyclingTypes");
    for (int i = 0; i < noOfCustomTypes; ++i)
    {
        const cycling_type_t &cyclingType = cyclingTypes[noOfFixTypes + i];
        cfg.setArrayIndex(i);
        cfg.setValue("name", cyclingType.name);
        cfg.setValue("spinPlainSpeed", cyclingType.plainSpeed);
        cfg.setValue("spinMinSpeed", cyclingType.minSpeed);
        cfg.setValue("spinSlopeAtMinSpeed", cyclingType.slopeAtMinSpeed);
        cfg.setValue("spinMaxSpeed", cyclingType.maxSpeed);
        cfg.setValue("spinSlopeAtMaxSpeed", cyclingType.slopeAtMaxSpeed);
    }
    cfg.endArray();

    cfg.setValue("TrackDetails/Filter/Speed/activityType", comboActivityType->currentIndex());
    cfg.setValue("TrackDetails/Filter/Speed/cyclingType", comboCyclingType->currentIndex());
    cfg.setValue("TrackDetails/Filter/Speed/speed", spinConstantSpeed->value());
}

void CFilterSpeed::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterSpeed");

    switch (comboActivityType->currentIndex())
    {
    case 0:
        trk.filterSpeed(spinConstantSpeed->value()/IUnit::self().speedfactor);
        break;

    case 1:
    {
        trk.filterSpeed(cyclingTypes[comboCyclingType->currentIndex()]);
        break;
    }

    case 2:
    {
        trk.filterSpeed(spinHikingPlainSpeed->value()/IUnit::self().speedfactor,
                        spinHikingAscending->value(), spinHikingDescending->value());
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
    updateUi();
}

void CFilterSpeed::updateUi()
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

    spinPlainSpeed->setValue(cyclingType.plainSpeed);
    spinMinSpeed->setValue(cyclingType.minSpeed);
    spinMaxSpeed->setValue(cyclingType.maxSpeed);

    if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        qreal val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingType.slopeAtMinSpeed);
        spinSlopeAtMinSpeed->setValue(val);
        val = IUnit::slopeConvert(IUnit::eSlopePercent, cyclingType.slopeAtMaxSpeed);
        spinSlopeAtMaxSpeed->setValue(val);
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopePercent)
    {
        spinSlopeAtMinSpeed->setValue(cyclingType.slopeAtMinSpeed);
        spinSlopeAtMaxSpeed->setValue(cyclingType.slopeAtMaxSpeed);
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
}

void CFilterSpeed::slotSetMinSpeed(double speed)
{
    cyclingTypes[comboCyclingType->currentIndex()].minSpeed = speed;
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
        spinSlopeAtMinSpeed->setValue(val);
        val = IUnit::slopeConvert(IUnit::eSlopeDegrees, minSlope);
        spinSlopeAtMaxSpeed->setValue(val);
    }
    else if(IUnit::getSlopeMode() == IUnit::eSlopeDegrees)
    {
        spinSlopeAtMinSpeed->setValue(maxSlope);
        spinSlopeAtMaxSpeed->setValue(minSlope);
    }
}
