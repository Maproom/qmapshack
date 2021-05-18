/**********************************************************************************************

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
#include "gis/trk/filter/CFilterChangeStartPoint.h"
#include "gis/wpt/CGisItemWpt.h"

CFilterChangeStartPoint::CFilterChangeStartPoint(CGisItemTrk& trk, QWidget* parent) :
    QWidget(parent)
    , trk(trk)
{
    setupUi(this);
    updateUi();

    connect(toolApply, &QToolButton::clicked, this, &CFilterChangeStartPoint::slotApply);
}

void CFilterChangeStartPoint::slotApply()
{
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);

    trk.filterChangeStartPoint(comboBox->currentData().toInt(), comboBox->currentText());
    updateUi();
}

void CFilterChangeStartPoint::updateUi()
{
    IGisProject* project = trk.getParentProject();
    if(nullptr == project)
    {
        return;
    }

    CTrackData trkData = trk.getTrackData();
    qint32 noOfItems = 0;

    comboBox->clear();

    for(CTrackData::trkpt_t& pt : trkData)
    {
        if(pt.isHidden())
        {
            continue;
        }
        CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(project->getItemByKey(pt.keyWpt));
        if(nullptr == wpt)
        {
            continue;
        }

//        qDebug() << "wptName=" << wpt->getName() << "pntIdx=" << pt.idxVisible << "maxIdx=" << trk.getNumberOfVisiblePoints();
        if (pt.idxVisible > 0 && pt.idxVisible < trk.getNumberOfVisiblePoints() - 1) // to exclude original start and end point
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
}
