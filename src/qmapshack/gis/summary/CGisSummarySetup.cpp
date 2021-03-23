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

#include "gis/CGisDatabase.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/IDBFolder.h"
#include "gis/summary/CGisSummary.h"
#include "gis/summary/CGisSummarySetup.h"

#include <functional>
#include <QtWidgets>

CGisSummarySetup::CGisSummarySetup(CGisSummary &parent)
    : QDialog(&parent)
    , summary(parent)
{
    setupUi(this);

    setupSignals(CGisSummary::eDropZone1, lineName1, listWidget1, toolAdd1, toolDel1);
    setupSignals(CGisSummary::eDropZone2, lineName2, listWidget2, toolAdd2, toolDel2);
    setupSignals(CGisSummary::eDropZone3, lineName3, listWidget3, toolAdd3, toolDel3);
    setupSignals(CGisSummary::eDropZone4, lineName4, listWidget4, toolAdd4, toolDel4);

    labelHelp->setText(tr("<b>What is this about?</b><br/>"
                          "When using the database GIS items can be referenced by several projects. "
                          "A common use-case is to store tour data in it's own project folder. Additionally "
                          "the items are copied (referenced) into folders summarizing items of a certain "
                          "time span, e.g. monthly, yearly, total. You can do this manually by loading the "
                          "summary projects into the workspace, coping the items and saving the changes. Or "
                          "you use this dialog to define project folders in the database as summary folders "
                          "and copy the items by dragging them into the drag-n-drop area."
                          ));


    adjustSize();
}

void CGisSummarySetup::setupSignals(CGisSummary::dropzone_e number, QLineEdit * lineName, QListWidget * listWidget, QToolButton * toolAdd, QToolButton * toolDel)
{
    connect(toolAdd, &QToolButton::clicked, this, [this, listWidget](){slotAdd(listWidget);});
    connect(toolDel, &QToolButton::clicked, this, [this, listWidget](){slotDel(listWidget);});
    connect(listWidget, &QListWidget::itemSelectionChanged, this, [this, listWidget, toolDel](){slotItemSelectionChanged(listWidget, toolDel);});

    const CGisSummary::dropzone_t& dropzone = summary.getDropZone(number);
    for(const CGisSummary::folder_t& folder : dropzone.folders)
    {
        addFolder(folder.id, folder.db, listWidget);
    }

    lineName->setText(dropzone.name);
}

void CGisSummarySetup::accept()
{
    writeResults(CGisSummary::eDropZone1, lineName1, listWidget1);
    writeResults(CGisSummary::eDropZone2, lineName2, listWidget2);
    writeResults(CGisSummary::eDropZone3, lineName3, listWidget3);
    writeResults(CGisSummary::eDropZone4, lineName4, listWidget4);
    QDialog::accept();
}

void CGisSummarySetup::writeResults(CGisSummary::dropzone_e number, QLineEdit *lineName, QListWidget * listWidget) const
{
    CGisSummary::dropzone_t& dropzone = summary.getDropZone(number);

    dropzone.name = lineName->text();
    dropzone.folders.clear();
    const int N = listWidget->count();
    for(int n = 0; n < N; n++)
    {
        QListWidgetItem * item = listWidget->item(n);

        CGisSummary::folder_t folder;
        folder.name = item->text();
        folder.id   = item->data(eDataId).toULongLong();
        folder.db   = item->data(eDataDb).toString();

        dropzone.folders << folder;
    }
}

void CGisSummarySetup::slotAdd(QListWidget * listWidget)
{
    QList<quint64> ids;
    QString db;
    QString host;


    CSelectDBFolder dlg(ids, db, host, this);
    dlg.setWindowTitle(tr("Select summary project..."));
    dlg.setProjectsOnly(true);
    if(dlg.exec() == QDialog::Rejected)
    {
        return;
    }

    addFolder(ids[0], db, listWidget);
}

void CGisSummarySetup::slotDel(QListWidget * listWidget)
{
    QList<QListWidgetItem*> items = listWidget->selectedItems();
    qDeleteAll(items);
}

void CGisSummarySetup::slotItemSelectionChanged(QListWidget * listWidget, QToolButton * toolDel)
{
    bool items = !listWidget->selectedItems().isEmpty();
    toolDel->setEnabled(items);
}

void CGisSummarySetup::addFolder(quint64 id, const QString& db, QListWidget * listWidget)
{
    const QString& name = IDBFolder::getNameEx(db, id);

    QListWidgetItem * item = new QListWidgetItem(name, listWidget);
    item->setData(eDataId, id);
    item->setData(eDataDb, db);
}
