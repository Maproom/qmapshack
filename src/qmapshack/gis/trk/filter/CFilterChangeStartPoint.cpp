/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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
#include "CFilterChangeStartPoint.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"

CFilterChangeStartPoint::CFilterChangeStartPoint(CGisItemTrk &trk, QWidget *parent) :
    QWidget(parent)
    , trk(trk)
{
    setupUi(this);

    IGisProject *project = trk.getParentProject();
    if(nullptr == project)
    {
        return;
    }

    CTrackData trkData = trk.getTrackData();
    qint32 noOfItems = 0;

    for(CTrackData::trkpt_t& pt : trkData)
    {
        if(pt.isHidden())
        {
            continue;
        }
        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(project->getItemByKey(pt.keyWpt));
        if(nullptr == wpt)
        {
            continue;
        }
        if (pt.idxTotal) // exclude original start point
        {
            comboBox->insertItem(noOfItems, wpt->getName(), pt.idxVisible);
            ++noOfItems;
        }
    }

    if (noOfItems == 0)
    {
        toolApply->setEnabled(false);
    }
    else
    {
        toolApply->setEnabled(true);
    }
    connect(toolApply, &QToolButton::clicked, this, &CFilterChangeStartPoint::slotApply);
}

void CFilterChangeStartPoint::slotApply()
{
    CCanvas::setOverrideCursor(Qt::WaitCursor,"filterChangeStartPoint");
    trk.filterChangeStartPoint(comboBox->currentData().toInt());
    CCanvas::restoreOverrideCursor("filterChangeStartPoint");
}
