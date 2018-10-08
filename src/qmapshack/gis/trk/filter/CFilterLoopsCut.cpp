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
    spinBox->setValue(cfg.value("TrackDetails/Filter/LoopsCut/distance",5).toInt());

    connect(toolApply, &QToolButton::clicked, this, &CFilterLoopsCut::slotApply);
}

CFilterLoopsCut::~CFilterLoopsCut()
{
    SETTINGS;
    cfg.setValue("TrackDetails/Filter/DouglasPeuker/distance", spinBox->value());
}

void CFilterLoopsCut::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor, "CFilterLoopsCut");
    trk.filterLoopsCut(spinBox->value()/IUnit::self().basefactor);
    CCanvas::restoreOverrideCursor("CFilterLoopsCut");
}
