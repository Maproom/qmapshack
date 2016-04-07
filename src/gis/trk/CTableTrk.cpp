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

#include "gis/trk/CTableTrk.h"
#include "helpers/CSettings.h"


#include <QtWidgets>
#include <proj_api.h>

CTableTrk::CTableTrk(QWidget *parent)
    : QTreeWidget(parent)
    , INotifyTrk(CGisItemTrk::eVisualTrkTable)
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    header()->restoreState(cfg.value("trackPointListState").toByteArray());
    cfg.endGroup();

    connect(this, &CTableTrk::itemSelectionChanged, this, &CTableTrk::slotItemSelectionChanged);
}

CTableTrk::~CTableTrk()
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    cfg.setValue("trackPointListState", header()->saveState());
    cfg.endGroup();

    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }
}

void CTableTrk::setTrack(CGisItemTrk * track)
{
    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }

    clear();
    trk = track;

    if(trk != nullptr)
    {
        trk->registerVisual(this);
        updateData();
    }
}

void CTableTrk::updateData()
{
    if(trk == nullptr)
    {
        return;
    }

    // use all valid flags as invalid mask. By that only
    // invalid flags for properties with valid points count
    quint32 invalidMask = (trk->getAllValidFlags() & CGisItemTrk::trkpt_t::eValidMask) << 16;

    QList<QTreeWidgetItem*> items;
    const CGisItemTrk::trk_t& t = trk->getTrackData();
    for(const CGisItemTrk::trkseg_t& seg : t.segs)
    {
        for(const CGisItemTrk::trkpt_t& trkpt : seg.pts)
        {
            QString val, unit;

            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setTextAlignment(eColNum,     Qt::AlignLeft);
            item->setTextAlignment(eColEle,     Qt::AlignRight);
            item->setTextAlignment(eColDelta,   Qt::AlignRight);
            item->setTextAlignment(eColDist,    Qt::AlignRight);
            item->setTextAlignment(eColAscend,  Qt::AlignRight);
            item->setTextAlignment(eColDescend, Qt::AlignRight);
            item->setTextAlignment(eColSpeed,   Qt::AlignRight);

            QBrush bg = item->background(0);
            if(trkpt.isInvalid(CGisItemTrk::trkpt_t::invalid_e(invalidMask)))
            {
                bg = QColor(255, 100, 100);
            }

            QBrush fg( trkpt.flags & CGisItemTrk::trkpt_t::eHidden ? Qt::gray : Qt::black );
            for(int i = 0; i < eColMax; i++)
            {
                item->setBackground(i, bg);
                item->setForeground(i, fg);
            }

            item->setText(eColNum,QString::number(trkpt.idxTotal));

            item->setText(eColTime, trkpt.time.isValid()
                          ? IUnit::self().datetime2string(trkpt.time, true, QPointF(trkpt.lon, trkpt.lat)*DEG_TO_RAD)
                          : "-"
                          );

            if(trkpt.ele != NOINT)
            {
                IUnit::self().meter2elevation(trkpt.ele, val, unit);
                item->setText(eColEle, tr("%1 %2").arg(val).arg(unit));
            }
            else
            {
                item->setText(eColEle, "-");
            }

            IUnit::self().meter2distance(trkpt.deltaDistance, val, unit);
            item->setText(eColDelta, tr("%1 %2").arg(val).arg(unit));

            IUnit::self().meter2distance(trkpt.distance, val, unit);
            item->setText(eColDist, tr("%1 %2").arg(val).arg(unit));

            if(trkpt.speed != NOFLOAT)
            {
                IUnit::self().meter2speed(trkpt.speed, val, unit);
                item->setText(eColSpeed, tr("%1 %2").arg(val).arg(unit));
            }
            else
            {
                item->setText(eColSpeed, "-");
            }

            item->setText(eColSlope,
                          (trkpt.slope1 != NOFLOAT)
                          ? QString("%1°(%2%)").arg(trkpt.slope1, 2, 'f', 0).arg(trkpt.slope2, 2, 'f', 0)
                          : "-"
                          );

            IUnit::self().meter2elevation(trkpt.ascend, val, unit);
            item->setText(eColAscend, tr("%1 %2").arg(val).arg(unit));
            IUnit::self().meter2elevation(trkpt.descend, val, unit);
            item->setText(eColDescend, tr("%1 %2").arg(val).arg(unit));

            // position
            QString str;
            IUnit::degToStr(trkpt.lon, trkpt.lat, str);
            item->setText(eColPosition,str);

            items << item;
        }
    }

    clear();
    addTopLevelItems(items);
    header()->resizeSections(QHeaderView::ResizeToContents);

}


void CTableTrk::slotItemSelectionChanged()
{
    QTreeWidgetItem * item = currentItem();
    if(nullptr != item)
    {
        quint32 idx = item->text(eColNum).toUInt();
        trk->setMouseFocusByTotalIndex(idx, CGisItemTrk::eFocusMouseMove, "CTableTrk");
    }
}
