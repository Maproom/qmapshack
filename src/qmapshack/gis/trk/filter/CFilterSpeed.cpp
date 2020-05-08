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

#include "canvas/CCanvas.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/filter/CFilterSpeed.h"
#include "gis/trk/filter/CFilterSpeedConst.h"
#include "gis/trk/filter/CFilterSpeedCycle.h"
#include "gis/trk/filter/CFilterSpeedHike.h"
#include "helpers/CSettings.h"

CFilterSpeed::CFilterSpeed(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent), trk(trk)
{
    setupUi(this);

    filterConst = new CFilterSpeedConst(this);
    filterCycle = new CFilterSpeedCycle(this, trk);
    filterHike  = new CFilterSpeedHike(this);

    stackedWidget->addWidget(filterConst);
    stackedWidget->addWidget(filterCycle);
    stackedWidget->addWidget(filterHike);

    labelWarning->setText("");

    SETTINGS;
    cfg.beginGroup("TrackDetails/Filter/Speed/");
    comboActivityType->setCurrentIndex(cfg.value("activityType", 0).toInt());
    slotSetActivityType(comboActivityType->currentIndex());

    connect(comboActivityType, SIGNAL(activated(int)), this, SLOT(slotSetActivityType(int)));
    connect(toolApply, &QToolButton::clicked, this, &CFilterSpeed::slotApply);

    cfg.beginGroup("Const");
    filterConst->loadSettings(cfg);
    cfg.endGroup(); // Const

    cfg.beginGroup("Cycle");
    filterCycle->loadSettings(cfg);
    cfg.endGroup(); // Cycle

    cfg.beginGroup("Hike");
    filterHike->loadSettings(cfg);
    cfg.endGroup(); // Hike

    cfg.endGroup(); //TrackDetails/Filter/Speed
}

CFilterSpeed::~CFilterSpeed()
{
    SETTINGS;
    cfg.remove("TrackDetails/Filter/Speed/");
    cfg.beginGroup("TrackDetails/Filter/Speed/");
    cfg.setValue("activityType", comboActivityType->currentIndex());

    cfg.beginGroup("Const");
    filterConst->saveSettings(cfg);
    cfg.endGroup(); // Const

    cfg.beginGroup("Cycle");
    filterCycle->saveSettings(cfg);
    cfg.endGroup(); // Cycle

    cfg.beginGroup("Hike");
    filterHike->saveSettings(cfg);
    cfg.endGroup(); // Hike

    cfg.endGroup(); //TrackDetails/Filter/Speed
}

void CFilterSpeed::slotApply()
{
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);

    switch (comboActivityType->currentIndex())
    {
    case 0:
        filterConst->apply(trk);
        break;

    case 1:
    {
        filterCycle->apply(trk);
        break;
    }

    case 2:
    {
        filterHike->apply(trk);
        break;
    }

    default:
        break;
    }
}

void CFilterSpeed::slotSetActivityType(int type)
{
    stackedWidget->setCurrentIndex(type);
    updateUi();
}

void CFilterSpeed::updateUi()
{
    if(trk.isTrkElevationInvalid() && comboActivityType->currentIndex() > 0)
    {
        QString str = QString("<b style='color: red;'>" +
                              tr("Track has no or invalid elevation data. Please correct or set constant speed!") +
                              "</b><br/>");
        labelWarning->setText(str);
        filterCycle->setEnabled(false);
        filterHike->setEnabled(false);
        toolApply->setEnabled(false);
        return;
    }
    else
    {
        labelWarning->setText("");
        filterCycle->setEnabled(true);
        filterHike->setEnabled(true);
        toolApply->setEnabled(true);
    }
}
