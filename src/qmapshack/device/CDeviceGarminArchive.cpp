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

#include "canvas/CCanvas.h"
#include "device/CDeviceGarmin.h"
#include "device/CDeviceGarminArchive.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/gpx/CGpxProject.h"

#include <QtWidgets>

CDeviceGarminArchive::CDeviceGarminArchive(const QString &path, CDeviceGarmin *parent)
    : IDevice(path, eTypeGarmin, parent->getKey(), parent)
{
    setText(CGisListWks::eColumnName, tr("Archive - expand to load"));
    setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    connect(treeWidget(), &QTreeWidget::itemExpanded, this, &CDeviceGarminArchive::slotExpanded);
    connect(treeWidget(), &QTreeWidget::itemCollapsed, this, &CDeviceGarminArchive::slotCollapsed);
}


void CDeviceGarminArchive::slotExpanded(QTreeWidgetItem * item)
{
    if((item != this) || (childCount() != 0))
    {
        return;
    }

    setText(CGisListWks::eColumnName, tr("Archive - loaded"));

    QMutexLocker lock(&IGisItem::mutexItems);
    CDeviceMountLock mountLock(*this);
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
    qDebug() << "reading files from device: " << dir.path();
    QStringList entries = dir.entryList(QStringList("*.gpx"));
    for(const QString &entry : entries)
    {
        const QString filename = dir.absoluteFilePath(entry);
        IGisProject * project = new CGpxProject(filename, this);
        if(!project->isValid())
        {
            delete project;
        }
    }
}

void CDeviceGarminArchive::slotCollapsed(QTreeWidgetItem * item)
{
    if((item != this) || (childCount() == 0))
    {
        return;
    }

    QMutexLocker lock(&IGisItem::mutexItems);
    CDeviceMountLock mountLock(*this);
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);

    qDeleteAll(takeChildren());

    setText(CGisListWks::eColumnName, tr("Archive - expand to load"));
    emit CGisWorkspace::self().sigChanged();
}
