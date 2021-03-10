/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/CGisWorkspace.h"
#include "gis/IGisLine.h"
#include "gis/prj/IGisProject.h"
#include "gis/proj_x.h"
#include "gis/rte/CCreateRouteFromWpt.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/wpt/CGisItemWpt.h"

#include <QtWidgets>

CCreateRouteFromWpt::CCreateRouteFromWpt(const QList<IGisItem::key_t> &keys, QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    for(const IGisItem::key_t& key : keys)
    {
        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
        if(nullptr == wpt)
        {
            continue;
        }

        QListWidgetItem * item = new QListWidgetItem(listWidget);
        item->setText(wpt->getName());
        item->setIcon(wpt->getIcon());
        item->setToolTip(wpt->getInfo(IGisItem::eFeatureShowName));
        item->setData(Qt::UserRole + 0, QPointF(wpt->getPosition() * DEG_TO_RAD));
    }

    connect(listWidget, &QListWidget::itemSelectionChanged, this, &CCreateRouteFromWpt::slotSelectionChanged);
    connect(toolUp,     &QToolButton::clicked,              this, &CCreateRouteFromWpt::slotUp);
    connect(toolDown,   &QToolButton::clicked,              this, &CCreateRouteFromWpt::slotDown);
}

CCreateRouteFromWpt::~CCreateRouteFromWpt()
{
}

void CCreateRouteFromWpt::accept()
{
    QDialog::accept();

    QString name;
    IGisProject *project = nullptr;

    if(!IGisItem::getNameAndProject(name, project, tr("route")))
    {
        return;
    }

    SGisLine points;
    for(int i = 0; i < listWidget->count(); i++)
    {
        QListWidgetItem * item = listWidget->item(i);
        points << IGisLine::point_t(item->data(Qt::UserRole + 0).toPointF(), item->text());
    }

    CGisItemRte* rte = new CGisItemRte(points, name, project, NOIDX);
    rte->calc();
}

void CCreateRouteFromWpt::slotSelectionChanged()
{
    QListWidgetItem * item = listWidget->currentItem();
    if(item != nullptr)
    {
        int row = listWidget->row(item);
        toolUp->setEnabled(row != 0);
        toolDown->setEnabled(row != (listWidget->count() - 1));
    }
    else
    {
        toolUp->setEnabled(false);
        toolDown->setEnabled(false);
    }
}

void CCreateRouteFromWpt::slotUp()
{
    QListWidgetItem * item = listWidget->currentItem();
    if(item)
    {
        int row = listWidget->row(item);
        if(row == 0)
        {
            return;
        }
        listWidget->takeItem(row);
        row = row - 1;
        listWidget->insertItem(row, item);
        listWidget->setCurrentItem(item);
    }
}

void CCreateRouteFromWpt::slotDown()
{
    QListWidgetItem * item = listWidget->currentItem();
    if(item)
    {
        int row = listWidget->row(item);
        if(row == (listWidget->count() - 1))
        {
            return;
        }
        listWidget->takeItem(row);
        row = row + 1;
        listWidget->insertItem(row, item);
        listWidget->setCurrentItem(item);
    }
}
