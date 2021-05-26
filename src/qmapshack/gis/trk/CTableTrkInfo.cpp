/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/trk/CTableTrkInfo.h"
#include "helpers/CDraw.h"

#include <QtWidgets>

CTableTrkInfo::CTableTrkInfo(QWidget* parent)
    : QTreeWidget(parent)
    , INotifyTrk(CGisItemTrk::eVisualTrkTable)
{
    actionEdit = new QAction(QIcon("://icons/32x32/EditText.png"), tr("Edit..."), this);
    actionDelete = new QAction(QIcon("://icons/32x32/DeleteMultiple.png"), tr("Delete"), this);

    menu = new QMenu(this);
    menu->addAction(actionEdit);
    menu->addAction(actionDelete);

    connect(actionEdit, &QAction::triggered, this, &CTableTrkInfo::slotEdit);
    connect(actionDelete, &QAction::triggered, this, &CTableTrkInfo::slotDelete);

    connect(this, &CTableTrkInfo::customContextMenuRequested, this, &CTableTrkInfo::slotContextMenu);
    connect(this, &CTableTrkInfo::itemChanged, this, &CTableTrkInfo::slotItemChanged);
    connect(this, &CTableTrkInfo::itemSelectionChanged, this, &CTableTrkInfo::slotItemSelectionChanged);
}

CTableTrkInfo::~CTableTrkInfo()
{
    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }
}

void CTableTrkInfo::setTrack(CGisItemTrk* track)
{
    setColumnCount(eColMax);

    QStringList labels;
    labels << "#";
    labels << tr("Description");
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

void CTableTrkInfo::updateData()
{
    if(trk == nullptr)
    {
        return;
    }

    qint32 cnt = 1;
    QList<QTreeWidgetItem*> items;
    for(const CTrackData::trkpt_t& trkpt : trk->getTrackData())
    {
        if(trkpt.desc.isEmpty())
        {
            continue;
        }

        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setIcon(eColNum, CDraw::number(cnt++, Qt::black));
        item->setText(eColDesc, trkpt.desc);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(eColDesc, Qt::UserRole, trkpt.idxTotal);
        items << item;
    }

    clear();
    addTopLevelItems(items);
    header()->resizeSections(QHeaderView::ResizeToContents);

    emit sigHasTrkPtInfo(cnt > 1);
}

void CTableTrkInfo::slotContextMenu(const QPoint& point)
{
    if(trk == nullptr)
    {
        return;
    }

    const QList<QTreeWidgetItem*>& items = selectedItems();
    actionEdit->setEnabled(items.count() == 1);

    const QPoint& p = mapToGlobal(point);
    menu->exec(p);
}

void CTableTrkInfo::slotEdit()
{
    if(trk == nullptr)
    {
        return;
    }

    QTreeWidgetItem* item = currentItem();
    editItem(item, eColDesc);
}

void CTableTrkInfo::slotDelete()
{
    if(trk == nullptr)
    {
        return;
    }

    QList<int> indices;
    const QList<QTreeWidgetItem*>& items = selectedItems();
    for(QTreeWidgetItem* item : items)
    {
        indices << item->data(eColDesc, Qt::UserRole).toInt();
    }

    trk->delTrkPtDesc(indices);
}


void CTableTrkInfo::slotItemChanged(QTreeWidgetItem* item, int column)
{
    if((trk == nullptr) || (column != eColDesc))
    {
        return;
    }

    trk->setTrkPtDesc(item->data(eColDesc, Qt::UserRole).toInt(), item->text(eColDesc));
}

void CTableTrkInfo::slotItemSelectionChanged()
{
    if(trk == nullptr)
    {
        return;
    }

    QTreeWidgetItem* item = currentItem();
    if(nullptr != item)
    {
        trk->setMouseFocusByTotalIndex(item->data(eColDesc, Qt::UserRole).toInt(), CGisItemTrk::eFocusMouseMove, "CTableTrk");
    }
}

