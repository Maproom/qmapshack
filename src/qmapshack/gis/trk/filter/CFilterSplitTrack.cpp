/**********************************************************************************************
    Copyright (C) 2018 Michel Durand <zero@cms123.fr>

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
#include "gis/trk/filter/CFilterSplitTrack.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

#include <QtWidgets>

CFilterSplitTrack::CFilterSplitTrack(CGisItemTrk& trk, QWidget* parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    SETTINGS;
    spinBox->setValue(cfg.value("TrackDetails/Filter/SplitTrack/nTracks").toInt());

    connect(toolApply, &QToolButton::clicked, this, &CFilterSplitTrack::slotApply);
    connect(help, &QToolButton::clicked, this, &CFilterSplitTrack::showHelp);
}

CFilterSplitTrack::~CFilterSplitTrack()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/SplitTrack/nTracks", spinBox->value());
}

void CFilterSplitTrack::slotApply()
{
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
    trk.filterSplitTrack(spinBox->value());
}

void CFilterSplitTrack::showHelp()
{
    QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help")
                             , tr("Sometimes tracks have too many points to be transfered to a gps device.\n\n"
                                  "This filter splits the track into multiple smaller tracks.")  );
}
