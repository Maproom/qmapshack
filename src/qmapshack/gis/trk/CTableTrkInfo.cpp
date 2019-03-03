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

#include "gis/trk/CTableTrkInfo.h"
#include "helpers/CDraw.h"

#include <QtWidgets>

CTableTrkInfo::CTableTrkInfo(QWidget *parent)
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
}

CTableTrkInfo::~CTableTrkInfo()
{
    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }
}

void CTableTrkInfo::setTrack(CGisItemTrk * track)
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
    const CTrackData& t = trk->getTrackData();
    for(const CTrackData::trkptinfo_t& info : t.infos)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setIcon(eColNum, CDraw::number(cnt++, 21, Qt::black));
        item->setText(eColDesc, info.desc);
        item->setFlags(item->flags()|Qt::ItemIsEditable);

        items << item;
    }

    clear();
    addTopLevelItems(items);
    header()->resizeSections(QHeaderView::ResizeToContents);
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

    QTreeWidgetItem * item = currentItem();
    editItem(item, eColDesc);
}

void CTableTrkInfo::slotDelete()
{
    if(trk == nullptr)
    {
        return;
    }

    QList<int> indices;
    for(QTreeWidgetItem* item : selectedItems())
    {
        indices << indexOfTopLevelItem(item);
    }

    trk->removeTrackPointInfosByIndex(indices);
}


void CTableTrkInfo::slotItemChanged(QTreeWidgetItem * item, int column)
{
    if((trk == nullptr) || (column != eColDesc))
    {
        return;
    }

    trk->editTrackPointInfoByIndex(indexOfTopLevelItem(item), item->text(eColDesc));
}
