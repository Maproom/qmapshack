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
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
                             tr( "Hide knots in the track:\n\n"
                                 "During a break many GPS devices continue to record. As the GPS accuracy "
                                 "has some variance the result is a cloud of points forming kond of a knot "
                                 "in the track. This leads to accumulated errors in the statistics. That's "
                                 "why you might want to remove them.\n\n"
                                 "Typically a knots fulfills two conditions:\n"
                                 "- A series of points very close to each other.\n"
                                 "- The linear distance between the first point and the last is very small.\n\n"
                                 "This filter is designed to detect and hide them considering these two "
                                 "conditions based on two customizable variables:\n"
                                 "- Distance: When a series of points are found separated by a distance less "
                                 "than the indicated one, it is considered that it may be a knot and a second "
                                 "verification is done based on the ratio value.\n"
                                 "- Ratio: The minimum distance in a straight line between the first and the "
                                 "last point of the series is checked. When the distance drawn by the track "
                                 "is greater than that minimum distance multiplied by the value entered as "
                                 "ratio, it is considered to be a knot and the trackpoints are hidden\n\n."
                                 "You can modify both parameters to customize the behavior from the filter. "
                                 "For the distance it is advisable to indicate a value lower than the GPS accuracy. "
                                 "Keep in mind that if a high distance and a low ratio are indicated, it is possible "
                                 "for the filter to detect a knot by mistake (for example, areas with sharp curves "
                                 "(serpentines) or sections of very slow speed)."));
}

