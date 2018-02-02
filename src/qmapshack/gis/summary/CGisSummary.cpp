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

#include "gis/summary/CGisSummary.h"
#include "gis/summary/CGisSummaryDropZone.h"
#include "gis/summary/CGisSummarySetup.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CGisSummary::CGisSummary(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(toolSetup, &QToolButton::clicked, this, &CGisSummary::slotSetup);

    SETTINGS;
    cfg.beginGroup("Database");
    cfg.beginGroup("Summary");
    int cnt = 1;
    for(dropzone_t& dropZone : dropZones)
    {
        const QString& name = QString("DropZone%1").arg(cnt++);
        cfg.beginGroup(name);
        dropZone.name = cfg.value("name", name).toString();
        const int N = cfg.value("numberOfFolders",0).toInt();
        for(int n = 0; n < N; n++)
        {
            cfg.beginGroup(QString("Folder%1").arg(n));
            folder_t folder;
            folder.name = cfg.value("name", "").toString();
            folder.id   = cfg.value("id", 0).toULongLong();
            folder.db   = cfg.value("db", "").toString();
            dropZone.folders << folder;
            cfg.endGroup(); // Folder%1
        }
        cfg.endGroup(); // "Dropzone%1"
    }
    cfg.endGroup(); // Summary
    cfg.endGroup(); // Database

    setupDropZones();
}

CGisSummary::~CGisSummary()
{
    SETTINGS;
    cfg.beginGroup("Database");
    cfg.beginGroup("Summary");
    cfg.remove("");

    int cnt = 1;
    for(dropzone_t& dropZone : dropZones)
    {
        const QString& name = QString("DropZone%1").arg(cnt++);
        cfg.beginGroup(name);
        cfg.setValue("name", dropZone.name);
        const int N = dropZone.folders.size();
        cfg.setValue("numberOfFolders",N);
        for(int n = 0; n < N; n++)
        {
            cfg.beginGroup(QString("Folder%1").arg(n));
            const folder_t& folder = dropZone.folders[n];
            cfg.setValue("name", folder.name);
            cfg.setValue("id", folder.id);
            cfg.setValue("db", folder.db);
            cfg.endGroup(); // Folder%1
        }
        cfg.endGroup(); // "Dropzone%1"
    }
    cfg.endGroup(); // Summary
    cfg.endGroup(); // Database
}

void CGisSummary::slotSetup()
{
    CGisSummarySetup dlg(*this);
    dlg.exec();

    setupDropZones();
}

void CGisSummary::setupDropZones()
{
    QLayout * theLayout = layout();

    bool isEmpty = true;
    for(dropzone_t& dropZone : dropZones)
    {
        delete dropZone.zone;
        dropZone.zone = nullptr;

        if(!dropZone.folders.isEmpty())
        {
            dropZone.zone = new CGisSummaryDropZone(dropZone,this);
            theLayout->addWidget(dropZone.zone);
            isEmpty = false;
        }
    }

    labelDrop->setVisible(isEmpty);
}
