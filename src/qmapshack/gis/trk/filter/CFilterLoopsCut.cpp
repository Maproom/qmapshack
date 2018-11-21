/**********************************************************************************************
    Copyright (C) 2018 Michel Durand zero@cms123.fr

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
#include "gis/trk/filter/CFilterLoopsCut.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

#include <QtWidgets>

CFilterLoopsCut::CFilterLoopsCut(CGisItemTrk &trk, QWidget * parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    spinBox->setSuffix(IUnit::self().baseunit);

    SETTINGS;
    spinBox->setValue(cfg.value("TrackDetails/Filter/LoopsCut/minLoopLength",10).toInt());

    connect(toolApply, &QToolButton::clicked, this, &CFilterLoopsCut::slotApply);
    connect(help, &QToolButton::clicked, this, &CFilterLoopsCut::showHelp);
}

CFilterLoopsCut::~CFilterLoopsCut()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/LoopsCut/minLoopLength", spinBox->value());
}

void CFilterLoopsCut::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterLoopsCut");
    trk.filterLoopsCut(spinBox->value()/IUnit::self().basefactor);
    CCanvas::restoreOverrideCursor("CFilterLoopsCut");
}

void CFilterLoopsCut::showHelp()
{
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help")
                                   , tr("Sometimes tracks have loops.\n\n"
                                        "It is often the case for mountain bikes tours where the same fire road is used several "
                                        "times to go back uphill to an high point. "
                                        "Several downhill paths start from this high point and the same fire road is used again "
                                        "and again to go back up. Then the recorded track is made of several loops, and following "
                                        "a track made of loops on a Garmin device is difficult, as it is impossible to know in what "
                                        "order loops have to be ridden.\n\n"
                                        "Then the purpose of this filter is to split the input track into several separate tracks. "
                                        "The obtained tracks have no loops and are easy to follow on a Garmin device: start with part 1, "
                                        "then when end of part 1 is reached, switch to part 2, etc..\n\n"
                                        "This filter detects loops by looking for intersections. A new track is created as soon as an "
                                        "intersection is detected.\n\n"
                                        "The only input parameter is minimum loop length: this is to prevent cutting tracks in "
                                        "tight switchbacks, where recorded tracks can be made of tiny loops. These loops will be ignored "
                                        "if their lengths are smaller that the given value.")  );

}
