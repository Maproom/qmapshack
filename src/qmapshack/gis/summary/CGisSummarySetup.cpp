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

#include "gis/CGisDatabase.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/IDBFolder.h"
#include "gis/summary/CGisSummary.h"
#include "gis/summary/CGisSummarySetup.h"

#include <QtWidgets>

CGisSummarySetup::CGisSummarySetup(CGisSummary &parent)
    : QDialog(&parent)
    , summary(parent)
{
    setupUi(this);
    connect(toolAdd, &QToolButton::clicked, this, &CGisSummarySetup::slotAdd);
    connect(toolDel, &QToolButton::clicked, this, &CGisSummarySetup::slotDel);

    labelHelp->setText(tr("<b>What is this about?</b><br/>"
                          "When using the database GIS items can be referenced by several projects. "
                          "A common use-case is to store tour data in it's own project folder. Additionally "
                          "the items are copied (referenced) into folders collecting items of a certain "
                          "time span, e.g. monthly, yearly, total. You can do this manually by loading the "
                          "summary projects into the workspace, coping the items and saving the changes. Or "
                          "you use this dialog to define  projects folders in the database as summary folders "
                          "and copy the items by dragging them into the drag-n-drop area."
                          ));

    adjustSize();
}


void CGisSummarySetup::slotAdd()
{
    quint64 id;
    QString db;
    QString host;

    CSelectDBFolder dlg(id, db, host, this);
    dlg.setWindowTitle(tr("Select summary project..."));
    if(dlg.exec() == QDialog::Rejected)
    {
        return;
    }


    IDBFolder * folder = CGisDatabase::self().getFolderById(id, db);
    if(folder != nullptr)
    {
        qDebug() << folder->getDBName() << folder->getName() << folder->getId();
    }

    delete folder;
}

void CGisSummarySetup::slotDel()
{
}
