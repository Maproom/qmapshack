/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/proj_x.h"
#include "gis/trk/CTableTrk.h"
#include "helpers/CElevationDialog.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

#include <QtWidgets>

CTableTrk::CTableTrk(QWidget *parent)
    : QTreeWidget(parent)
    , INotifyTrk(CGisItemTrk::eVisualTrkTable)
{
    SETTINGS;
    cfg.beginGroup("TrackDetails");
    header()->restoreState(cfg.value("trackPointListState").toByteArray());
    cfg.endGroup();

    connect(this, &CTableTrk::itemSelectionChanged, this, &CTableTrk::slotItemSelectionChanged);
    connect(this, &CTableTrk::itemDoubleClicked, this, &CTableTrk::slotItemDoubleClicked);
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


void CTableTrk::showTopItem()
{
    scrollTo(indexFromItem(topLevelItem(0), QAbstractItemView::PositionAtCenter));
}

void CTableTrk::showNextInvalid()
{
    qint32 index = 0;
    QTreeWidgetItem * item = currentItem();
    if(item != nullptr)
    {
        index = indexOfTopLevelItem(item) + 1;
    }

    const qint32 N = topLevelItemCount();
    for(; index < N; index++)
    {
        QTreeWidgetItem * item = topLevelItem(index);
        if(item->data(eColNum, Qt::UserRole).toUInt() != 0)
        {
            scrollTo(indexFromItem(item, QAbstractItemView::PositionAtCenter));
            break;
        }
    }
}

void CTableTrk::showPrevInvalid()
{
    qint32 index = 0;
    QTreeWidgetItem * item = currentItem();
    if(item != nullptr)
    {
        index = indexOfTopLevelItem(item) - 1;
    }

    for(; index >= 0; index--)
    {
        QTreeWidgetItem * item = topLevelItem(index);
        if(item->data(eColNum, Qt::UserRole).toUInt() != 0)
        {
            scrollTo(indexFromItem(item, QAbstractItemView::PositionAtCenter));
            break;
        }
    }
}


void CTableTrk::setTrack(CGisItemTrk * track)
{
    setColumnCount(eColMax);

    QStringList labels;
    labels << "#";
    labels << tr("Time");
    labels << tr("Ele.");
    labels << tr("Delta");
    labels << tr("Dist.");
    labels << tr("Speed");
    labels << tr("Slope");
    labels << tr("Ascent");
    labels << tr("Descent");
    labels << tr("Position");
    setHeaderLabels(labels);

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

    adjustSize();
}

void CTableTrk::updateData()
{
    if(trk == nullptr)
    {
        return;
    }

    // use all valid flags as invalid mask. By that only
    // invalid flags for properties with valid points count
    quint32 invalidMask = (trk->getAllValidFlags() & CTrackData::trkpt_t::eValidMask) << 16;

    QList<QTreeWidgetItem*> items;
    const CTrackData& t = trk->getTrackData();
    for(const CTrackData::trkpt_t& trkpt : t)
    {
        QString val, unit;

        QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setTextAlignment(eColNum,     Qt::AlignLeft);
        item->setTextAlignment(eColEle,     Qt::AlignRight);
        item->setTextAlignment(eColDelta,   Qt::AlignRight);
        item->setTextAlignment(eColDist,    Qt::AlignRight);
        item->setTextAlignment(eColAscent,  Qt::AlignRight);
        item->setTextAlignment(eColDescent, Qt::AlignRight);
        item->setTextAlignment(eColSpeed,   Qt::AlignRight);

        if(!trk->isReadOnly())
        {
            item->setToolTip(eColEle, tr("Double click to edit elevation value"));
        }

        QBrush bg = item->background(0);
        if(trkpt.isInvalid(CTrackData::trkpt_t::invalid_e(invalidMask)) && !trkpt.isHidden())
        {
            bg = QColor(255, 100, 100);
            item->setData(eColNum, Qt::UserRole, quint32(invalidMask));
        }

        QBrush fg( trkpt.isHidden() ? Qt::gray : Qt::black );
        for(int i = 0; i < eColMax; i++)
        {
            item->setBackground(i, bg);
            item->setForeground(i, fg);
        }

        item->setText(eColNum, QString::number(trkpt.idxTotal));

        item->setText(eColTime, trkpt.time.isValid()
                      ? IUnit::self().datetime2string(trkpt.time, true, QPointF(trkpt.lon, trkpt.lat) * DEG_TO_RAD)
                      : "-"
                      );

        if(trkpt.ele != NOINT)
        {
            IUnit::self().meter2elevation(trkpt.ele, val, unit);
            item->setText(eColEle, tr("%1%2").arg(val, unit));
        }
        else
        {
            item->setText(eColEle, "-");
        }

        IUnit::self().meter2distance(trkpt.deltaDistance, val, unit);
        item->setText(eColDelta, tr("%1%2").arg(val, unit));

        IUnit::self().meter2distance(trkpt.distance, val, unit);
        item->setText(eColDist, tr("%1%2").arg(val, unit));

        if(trkpt.speed != NOFLOAT)
        {
            IUnit::self().meter2speed(trkpt.speed, val, unit);
            item->setText(eColSpeed, tr("%1%2").arg(val, unit));
        }
        else
        {
            item->setText(eColSpeed, "-");
        }

        IUnit::self().slope2string(trkpt.slope1, val, unit);
        item->setText(eColSlope, (trkpt.slope1 != NOFLOAT)
                      ? QString("%1%2").arg(val, unit)
                      : "-"
                      );

        IUnit::self().meter2elevation(trkpt.ascent, val, unit);
        item->setText(eColAscent, tr("%1%2").arg(val, unit));
        IUnit::self().meter2elevation(trkpt.descent, val, unit);
        item->setText(eColDescent, tr("%1%2").arg(val, unit));

        // position
        QString str;
        IUnit::degToStr(trkpt.lon, trkpt.lat, str);
        item->setText(eColPosition, str);

        items << item;
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

void CTableTrk::slotItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    if(trk->isReadOnly())
    {
        return;
    }

    qint32 idx = item->text(eColNum).toInt();
    qint32 ele = trk->getElevation(idx);
    qreal lon, lat;
    IUnit::strToDeg(item->text(eColPosition), lon, lat);

    if((column == eColEle) && (lon != NOFLOAT) && (lat != NOFLOAT))
    {
        QVariant var(ele);
        CElevationDialog dlg(this, var, ele, {lon, lat});

        if(dlg.exec() == QDialog::Accepted)
        {
            trk->setElevation(idx, var.toInt());
        }
    }
}
