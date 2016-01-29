/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/prj/IGisProject.h"
#include "gis/trk/CCombineTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "plot/CPlotTrack.h"

#include <QtWidgets>

CCombineTrk::CCombineTrk(CGisItemTrk& trk, const QList<IGisItem::key_t> &keysPreSel, IGisProject& project, QWidget * parent)
    : QDialog(parent)
    , trk(trk)
    , project(project)
{
    setupUi(this);

    const int N = project.childCount();
    for(int i = 0; i < N; i++)
    {
        CGisItemTrk * trk1 = dynamic_cast<CGisItemTrk*>(project.child(i));
        if(nullptr == trk1)
        {
            continue;
        }

        if(keysPreSel.contains(trk1->getKey()))
        {
            continue;
        }

        const IGisItem::key_t& key = trk1->getKey();
        QListWidgetItem * item = new QListWidgetItem(listAvailable);
        item->setText(trk1->getName());
        item->setIcon(trk1->getIcon());
        item->setData(Qt::UserRole + 1, key.item);
        item->setData(Qt::UserRole + 2, key.project);
        item->setData(Qt::UserRole + 3, key.device);
    }

    foreach(const IGisItem::key_t& key, keysPreSel)
    {
        IGisItem * gisItem = dynamic_cast<IGisItem*>(project.getItemByKey(key));
        if(nullptr == gisItem)
        {
            continue;
        }

        QListWidgetItem * item = new QListWidgetItem(listSelected);
        item->setText(gisItem->getName());
        item->setIcon(gisItem->getIcon());
        item->setData(Qt::UserRole + 1, key.item);
        item->setData(Qt::UserRole + 2, key.project);
        item->setData(Qt::UserRole + 3, key.device);
    }

    connect(listAvailable, &QListWidget::itemSelectionChanged, this, &CCombineTrk::slotSelectionChanged);
    connect(listSelected,  &QListWidget::itemSelectionChanged, this, &CCombineTrk::slotSelectionChanged);
    connect(toolSelect,    &QToolButton::clicked,              this, &CCombineTrk::slotSelect);
    connect(toolRemove,    &QToolButton::clicked,              this, &CCombineTrk::slotRemove);
    connect(toolUp,        &QToolButton::clicked,              this, &CCombineTrk::slotUp);
    connect(toolDown,      &QToolButton::clicked,              this, &CCombineTrk::slotDown);

    listAvailable->setCurrentItem(0);
    listSelected->setCurrentItem(0);

    slotSelectionChanged();
    updatePreview();
}

CCombineTrk::~CCombineTrk()
{
}

void CCombineTrk::accept()
{
    for(int i = 0; i < listSelected->count(); i++)
    {
        IGisItem::key_t key;
        key.item    = listSelected->item(i)->data(Qt::UserRole + 1).toString();
        key.project = listSelected->item(i)->data(Qt::UserRole + 2).toString();
        key.device  = listSelected->item(i)->data(Qt::UserRole + 3).toString();
        CGisItemTrk * trk1 = dynamic_cast<CGisItemTrk*>(project.getItemByKey(key));
        if(nullptr == trk1)
        {
            continue;
        }
        keys << key;
    }

    QDialog::accept();
}

void CCombineTrk::slotSelectionChanged()
{
    QListWidgetItem * item;
    item = listAvailable->currentItem();
    toolSelect->setEnabled(item != 0);

    item = listSelected->currentItem();
    toolRemove->setEnabled(item != 0);
    toolUp->setEnabled(item != 0);
    toolDown->setEnabled(item != 0);

    if(item)
    {
        if(listSelected->row(item) == 0)
        {
            toolUp->setEnabled(false);
        }
        if(listSelected->row(item) == (listSelected->count() - 1))
        {
            toolDown->setEnabled(false);
        }
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(listSelected->count() > 1);
}

void CCombineTrk::slotSelect()
{
    QListWidgetItem *item = listAvailable->currentItem();

    if(nullptr == item)
    {
        return;
    }

    listAvailable->takeItem(listAvailable->row(item));
    listSelected->addItem(item);

    slotSelectionChanged();
    updatePreview();
}

void CCombineTrk::slotRemove()
{
    QListWidgetItem *item = listSelected->currentItem();

    if(nullptr == item)
    {
        return;
    }

    IGisItem::key_t key;
    key.item    = item->data(Qt::UserRole + 1).toString();
    key.project = item->data(Qt::UserRole + 2).toString();
    key.device  = item->data(Qt::UserRole + 3).toString();

    if(key == trk.getKey())
    {
        return;
    }


    listSelected->takeItem(listSelected->row(item));
    listAvailable->addItem(item);

    slotSelectionChanged();
    updatePreview();
}

void CCombineTrk::slotUp()
{
    QListWidgetItem * item = listSelected->currentItem();
    if(item)
    {
        int row = listSelected->row(item);
        if(row == 0)
        {
            return;
        }
        listSelected->takeItem(row);
        row = row - 1;
        listSelected->insertItem(row,item);
        listSelected->setCurrentItem(item);
    }
    updatePreview();
}

void CCombineTrk::slotDown()
{
    QListWidgetItem * item = listSelected->currentItem();
    if(item)
    {
        int row = listSelected->row(item);
        if(row == (listSelected->count() - 1))
        {
            return;
        }
        listSelected->takeItem(row);
        row = row + 1;
        listSelected->insertItem(row,item);
        listSelected->setCurrentItem(item);
    }

    updatePreview();
}

void CCombineTrk::updatePreview()
{
    QPolygonF line;
    for(int i = 0; i < listSelected->count(); i++)
    {
        IGisItem::key_t key;
        key.item    = listSelected->item(i)->data(Qt::UserRole + 1).toString();
        key.project = listSelected->item(i)->data(Qt::UserRole + 2).toString();
        key.device  = listSelected->item(i)->data(Qt::UserRole + 3).toString();


        CGisItemTrk *trk1 = dynamic_cast<CGisItemTrk*>(project.getItemByKey(key));
        if(nullptr == trk1)
        {
            continue;
        }
        QPolygonF line1;
        trk1->getPolylineFromData(line1);

        line += line1;
    }

    plotTrack->setTrack(line);
    plotTrack->update();
}
