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

#include "gis/CGisListWks.h"
#include "gis/CGisProject.h"

#include <QtWidgets>

CGisListWks::CGisListWks(QWidget *parent)
    : QTreeWidget(parent)
{

    menuProject     = new QMenu(this);
    actionSave      = menuProject->addAction(QIcon("://icons/32x32/SaveGIS.png"),tr("Save"), this, SLOT(slotSaveProject()));
    actionSaveAs    = menuProject->addAction(QIcon("://icons/32x32/SaveGIS.png"),tr("Save As..."), this, SLOT(slotSaveAsProject()));
    actionClose     = menuProject->addAction(QIcon("://icons/32x32/Close.png"),tr("Close"), this, SLOT(slotCloseProject()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
}

CGisListWks::~CGisListWks()
{

}

bool CGisListWks::hasProject(const QString& key)
{
    for(int i = 0; i < topLevelItemCount(); i++)
    {
        CGisProject * item = dynamic_cast<CGisProject*>(topLevelItem(i));
        if(item && item->getKey() == key)
        {
            return true;
        }
    }
    return false;
}

void CGisListWks::slotContextMenu(const QPoint& point)
{
    CGisProject * item = dynamic_cast<CGisProject*>(currentItem());

    if(item != 0)
    {
        QPoint p = mapToGlobal(point);
        menuProject->exec(p);
    }
}

void CGisListWks::slotCloseProject()
{
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(item);
        if(project != 0)
        {
            delete project;
        }
    }
    emit sigChanged();
}

void CGisListWks::slotSaveProject()
{
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(item);
        if(project != 0)
        {
            project->save();
        }
    }
}

void CGisListWks::slotSaveAsProject()
{
    QList<QTreeWidgetItem*> items = selectedItems();
    foreach(QTreeWidgetItem * item, items)
    {
        CGisProject * project = dynamic_cast<CGisProject*>(item);
        if(project != 0)
        {
            project->saveAs();
        }
    }
}
