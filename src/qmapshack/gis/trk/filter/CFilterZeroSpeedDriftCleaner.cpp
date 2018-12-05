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
                             tr("Remove knots of trackpoints recorded on a break.\n\n"
                                "While having a break (no movement) most devices keep on recording. The result is a knot (cloud) "
                                "of points due to statistic and system based errors in the GPS position. As these points "
                                "influence the track statistics you might want to remove them.\n\n"
                                "This filter has 2 parameters:\n"
                                "-Distance defines the threshold at which knots are detected: knot starts when distance between 2 consecutive points "
                                "is shorter than this\n"
                                "-Ratio is used when track has straight parts at low speed: these are not knots and must not be removed. "
                                "Knots are twisty, slow speed parts are not.\n"
                                "The filter will detect a knot if the length of the knot is greater than the ratio multiplied by the distance on a straight "
                                "line between the beginning of the knot and its end. The default value of the ratio parameter is 2.")  );
}

