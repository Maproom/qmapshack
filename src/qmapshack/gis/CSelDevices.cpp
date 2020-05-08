/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "device/IDevice.h"
#include "gis/CGisListWks.h"
#include "gis/CSelDevices.h"
#include "gis/prj/IGisProject.h"

#include <QtWidgets>

CSelDevices::CSelDevices(IGisProject * project, QTreeWidget *wks)
    : QDialog(wks)

{
    setupUi(this);

    const int N = wks->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        IDevice *device = dynamic_cast<IDevice*>(wks->topLevelItem(n));
        if(nullptr == device)
        {
            continue;
        }

        QListWidgetItem * item = new QListWidgetItem(listWidget);
        item->setText(device->getName());
        item->setData(Qt::UserRole, device->getKey());
        item->setIcon(device->icon(CGisListWks::eColumnIcon));

        IGisProject *proj = device->getProjectByKey(project->getKey());
        item->setCheckState(nullptr == proj ? Qt::Unchecked : Qt::Checked);
    }

    CCanvas::setOverrideCursor(Qt::ArrowCursor, "CSelDevices");
}

CSelDevices::~CSelDevices()
{
    CCanvas::restoreOverrideCursor("~CSelDevices");
}


void CSelDevices::getSlectedDevices(QSet<QString>& keys)
{
    keys.clear();

    const int N = listWidget->count();
    for(int n = 0; n < N; n++)
    {
        QListWidgetItem * item = listWidget->item(n);
        if(item->checkState() == Qt::Checked)
        {
            keys << item->data(Qt::UserRole).toString();
        }
    }
}
