/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#include <QAbstractListModel>
#include "CSelectDoubleListWidget.h"

CSelectDoubleListWidget::CSelectDoubleListWidget(QWidget * parent) : QWidget(parent)
{
    setupUi(this);

    connect(listSelected,  &QListView::clicked,   this, &CSelectDoubleListWidget::slotSelectedClicked);
    connect(listAvailable, &QListView::clicked,   this, &CSelectDoubleListWidget::slotAvailableClicked);
    connect(toolAdd,       &QToolButton::clicked, this, &CSelectDoubleListWidget::slotAdd);
    connect(toolRemove,    &QToolButton::clicked, this, &CSelectDoubleListWidget::slotRemove);
    connect(toolUp,        &QToolButton::clicked, this, &CSelectDoubleListWidget::slotUp);
    connect(toolDown,      &QToolButton::clicked, this, &CSelectDoubleListWidget::slotDown);

    listAvailable->setModel(&modAvailable);
    listSelected->setModel(&modSelected);
}

CSelectDoubleListWidget::~CSelectDoubleListWidget()
{

}

void CSelectDoubleListWidget::setAvailable(const QList<sItem> & available)
{
    this->available.clear();
    this->available << available;
    filterModAvailable();
}

void CSelectDoubleListWidget::setSelected(const QList<sItem> & selected)
{
    modSelected.items.clear();
    modSelected.items << selected;
    filterModAvailable();
}

const QList<CSelectDoubleListWidget::sItem> CSelectDoubleListWidget::selected()
{
    return modSelected.items;
}

void CSelectDoubleListWidget::filterModAvailable()
{
    modAvailable.items.clear();
    for (const sItem & item : available)
    {
        if (!modSelected.items.contains(item))
        {
            modAvailable.items << item;
        }
    }
}

void CSelectDoubleListWidget::slotSelectedClicked(const QModelIndex & index)
{
    listAvailable->clearSelection();
}

void CSelectDoubleListWidget::slotAvailableClicked(const QModelIndex & index)
{
    listSelected->clearSelection();
}

void CSelectDoubleListWidget::slotAdd()
{
    QList<sItem> items;
    for (const QModelIndex & index : listAvailable->selectionModel()->selectedIndexes())
    {
        items << modAvailable.items.at(index.row());
    }
    for (const sItem & item : items)
    {
        int index = modAvailable.items.indexOf(item);
        modAvailable.beginRemoveRows(QModelIndex(),index,index);
        sItem removed = modAvailable.items.takeAt(index);
        modAvailable.endRemoveRows();
        modSelected.beginInsertRows(QModelIndex(),modSelected.items.size(),modSelected.items.size());
        modSelected.items.append(removed);
        modSelected.endInsertRows();
    }
}

void CSelectDoubleListWidget::slotRemove()
{
    QList<sItem> items;
    for (const QModelIndex & index : listSelected->selectionModel()->selectedIndexes())
    {
        items << modSelected.items.at(index.row());
    }
    for (const sItem & item : items)
    {
        int index = modSelected.items.indexOf(item);
        modSelected.beginRemoveRows(QModelIndex(),index,index);
        sItem removed = modSelected.items.takeAt(index);
        modSelected.endRemoveRows();

        index = -1;
        for (int i = available.indexOf(removed)-1; i >= 0 ; i--)
        {
            index = modAvailable.items.indexOf(available.at(i));
            if (index >= 0)
            {
                break;
            }
        }
        index++;
        modAvailable.beginInsertRows(QModelIndex(),index,index);
        modAvailable.items.insert(index,removed);
        modAvailable.endInsertRows();
    }
}

void CSelectDoubleListWidget::slotUp()
{
    QList<int> indices;
    QList<sItem> items;
    for (const QModelIndex & index : listSelected->selectionModel()->selectedIndexes())
    {
        indices << index.row();
    }
    std::sort(indices.begin(),indices.end());
    for (const int & index : indices)
    {
        items << modSelected.items.at(index);
    }
    int i=0;
    for (const sItem & item : items)
    {
        int index = modSelected.items.indexOf(item);
        if (index > i)
        {
            modSelected.beginRemoveRows(QModelIndex(),index,index);
            sItem removed = modSelected.items.takeAt(index);
            modSelected.endRemoveRows();
            modSelected.beginInsertRows(QModelIndex(),index-1,index-1);
            modSelected.items.insert(index-1,removed);
            modSelected.endInsertRows();
            listSelected->selectionModel()->select(modSelected.modelIndex(index-1),QItemSelectionModel::Select);
        }
        i++;
    }
}

void CSelectDoubleListWidget::slotDown()
{
    struct {
      bool operator() (int i,int j) { return (i>j);}
    } descending;

    QList<int> indices;
    QList<sItem> items;
    for (const QModelIndex & index : listSelected->selectionModel()->selectedIndexes())
    {
        indices << index.row();
    }
    std::sort(indices.begin(),indices.end(),descending);
    for (const int & index : indices)
    {
        items << modSelected.items.at(index);
    }
    int i=modSelected.items.size()-1;
    for (const sItem & item : items)
    {
        int index = modSelected.items.indexOf(item);
        if (index < i)
        {
            modSelected.beginRemoveRows(QModelIndex(),index,index);
            sItem removed = modSelected.items.takeAt(index);
            modSelected.endRemoveRows();
            modSelected.beginInsertRows(QModelIndex(),index+1,index+1);
            modSelected.items.insert(index+1,removed);
            modSelected.endInsertRows();
            listSelected->selectionModel()->select(modSelected.modelIndex(index+1),QItemSelectionModel::Select);
        }
        i--;
    }
}

int CSelectDoubleListWidget::CItemListModel::rowCount(const QModelIndex &parent) const
{
    return items.size();
}

QVariant CSelectDoubleListWidget::CItemListModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
    {
        return items.at(index.row()).text;
        break;
    }
    case Qt::DecorationRole:
    {
        return items.at(index.row()).icon;
        break;
    }
    case Qt::UserRole:
    {
        return items.at(index.row()).data;
    }
    default:
        return QVariant();
    }
}

const QModelIndex CSelectDoubleListWidget::CItemListModel::modelIndex(int row) const
{
    return createIndex(row,0);
}
