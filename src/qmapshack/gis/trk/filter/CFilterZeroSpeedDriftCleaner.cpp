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
#include "gis/trk/filter/CFilterZeroSpeedDriftCleaner.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

#include <QtWidgets>

CFilterZeroSpeedDriftCleaner::CFilterZeroSpeedDriftCleaner(CGisItemTrk &trk, QWidget * parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    distance->setSuffix(IUnit::self().baseunit);

    SETTINGS;
    distance->setValue(cfg.value("TrackDetails/Filter/ZeroSpeedDriftCleaner/distance",0.75 * IUnit::self().basefactor).toDouble() * IUnit::self().basefactor);
    ratio->setValue(cfg.value("TrackDetails/Filter/ZeroSpeedDriftCleaner/ratio",2).toDouble());

    connect(toolApply, &QToolButton::clicked, this, &CFilterZeroSpeedDriftCleaner::slotApply);
    connect(help, &QToolButton::clicked, this, &CFilterZeroSpeedDriftCleaner::showHelp);
}

CFilterZeroSpeedDriftCleaner::~CFilterZeroSpeedDriftCleaner()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/ZeroSpeedDriftCleaner/distance", distance->value() / IUnit::self().basefactor);
    cfg.setValue("TrackDetails/Filter/ZeroSpeedDriftCleaner/ratio", ratio->value());
}

void CFilterZeroSpeedDriftCleaner::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "FilterZeroSpeedDriftCleaner");
    trk.filterZeroSpeedDriftCleaner(distance->value()/IUnit::self().basefactor, ratio->value());
    CCanvas::restoreOverrideCursor("CFilterZeroSpeedDriftCleaner");
}

void CFilterZeroSpeedDriftCleaner::showHelp()
{
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help")
                             , tr("This filter is designed to remove unwanted points when stopped and when recording in device "
                                  "is set to 1pt/s without autopause or auto record method.\n\n"

                                  "Sometimes setting recording method to auto with highest precision is not enough to capture all of "
                                  "the track details. In this case recording method has to be set to 1pt/s.\n"
                                  "Unfortunately, when stopped, receivers are much more subject to multipath signals, and the result "
                                  "is a track looking like a big knot: position is slowly drifting in random directions at low speed.\n\n"

                                  "There at least 3 drawbacks:\n"
                                  "-total track length is greater than reality\n"
                                  "-stopped time is smaller\n"
                                  "-number of points is unnecessary high\n\n"

                                  "This filter has 2 parameters:\n"
                                  "-Distance defines the threshold at which knots are detected: knot starts when distance between 2 consecutive points "
                                  "is shorter than this\n"
                                  "-Ratio is used when track has straight parts at low speed: these are not knots and must not be removed. "
                                  "Knots are twisty, slow speed parts are not.\n"
                                  "Default value of 2 means that filter will detect a knot if the length of the knot is greater than 2 "
                                  "times the distance on a straight line between the beginning of the knot and its end")  );
}

