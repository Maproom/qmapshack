/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "helpers/CSettings.h"
#include "mouse/CScrOptRuler.h"
#include "mouse/IMouse.h"

#include <functional>
#include <QtWidgets>

CScrOptRuler::CScrOptRuler(IMouse *mouse, CCanvas *canvas)
    : IScrOpt(mouse)
{
    setupUi(this);
    comboStepDist->addItem("-", 0);
    comboStepDist->addItem("10m", 10);
    comboStepDist->addItem("100m", 100);
    comboStepDist->addItem("1000m", 1000);

    comboStepAngle->addItem("-", 0);
    comboStepAngle->addItem("5°", 5);
    comboStepAngle->addItem("10°", 10);
    comboStepAngle->addItem("15°", 15);
    comboStepAngle->addItem("30°", 30);
    comboStepAngle->addItem("45°", 45);
    comboStepAngle->addItem("60°", 60);
    comboStepAngle->addItem("90°", 90);

    connect(pushClose, &QPushButton::clicked, this, &CScrOptRuler::slotClose);

    auto func = [canvas](){canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawMouse);};
    connect(toolShowTable, &QToolButton::toggled, this, func);
    connect(toolShowAscent, &QToolButton::toggled, this, func);
    connect(toolShowCourse, &QToolButton::toggled, this, func);
    connect(toolShowAngle, &QToolButton::toggled, this, func);
    connect(toolShowPrecision, &QToolButton::toggled, this, func);

    SETTINGS;
    cfg.beginGroup("Ruler");
    toolShowTable->setChecked(cfg.value("showTable", true).toBool());
    toolShowCourse->setChecked(cfg.value("showCourse", true).toBool());
    toolShowAscent->setChecked(cfg.value("showAscent", true).toBool());
    toolShowAngle->setChecked(cfg.value("showAngle", false).toBool());
    toolShowPrecision->setChecked(cfg.value("showPrecision", false).toBool());
    comboStepDist->setCurrentIndex(cfg.value("comboStepDist", 0).toInt());
    comboStepAngle->setCurrentIndex(cfg.value("comboStepAngle", 0).toInt());
    cfg.endGroup(); // Ruler

    move(0, 0);
    adjustSize();
    show();
}

CScrOptRuler::~CScrOptRuler()
{
    SETTINGS;
    cfg.remove("Ruler");
    cfg.beginGroup("Ruler");
    cfg.setValue("showTable", toolShowTable->isChecked());
    cfg.setValue("showCourse", toolShowCourse->isChecked());
    cfg.setValue("showAscent", toolShowAscent->isChecked());
    cfg.setValue("showAngle", toolShowAngle->isChecked());
    cfg.setValue("showPrecision", toolShowPrecision->isChecked());
    cfg.setValue("comboStepDist", comboStepDist->currentIndex());
    cfg.setValue("comboStepAngle", comboStepAngle->currentIndex());
    cfg.endGroup(); // Ruler
}

void CScrOptRuler::slotClose()
{
    mouse->IMouse::rightButtonDown(QPoint(0, 0));
}
