/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/trk/filter/CFilterInterpolateElevation.h"

CFilterInterpolateElevation::CFilterInterpolateElevation(CGisItemTrk &trk, QWidget *parent)
    : QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    bool enabled = trk.isInterpolationEnabled();
    checkPreview->setChecked(enabled);
    toolApply->setEnabled(enabled);

    connect(toolApply, &QToolButton::clicked, this, &CFilterInterpolateElevation::slotApply);
    connect(checkPreview, &QCheckBox::toggled, this, &CFilterInterpolateElevation::slotPreview);
}


void CFilterInterpolateElevation::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor,"CFilterInterpolateElevation");
    trk.filterInterpolateElevation();
    CCanvas::restoreOverrideCursor("CFilterInterpolateElevation");
}

void CFilterInterpolateElevation::slotPreview(bool yes)
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterInterpolateElevation::slotPreview()");
    trk.setupInterpolation(yes);
    toolApply->setEnabled(trk.isInterpolationEnabled());
    CCanvas::restoreOverrideCursor("CFilterInterpolateElevation::slotPreview()");
}
