/**********************************************************************************************
    Copyright (C) 2021 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "CMainWindow.h"
#include "gis/CGisDatabase.h"
#include "gis/CGisListWks.h"
#include "gis/db/CDBProject.h"
#include "gis/db/CSelectDBFolder.h"
#include "gis/db/CSetupFolder.h"
#include "gis/gpx/CGpxProject.h"
#include "gis/qms/CQmsProject.h"
#include "helpers/CSettings.h"
#include "gis/trk/CTrkToRteDialog.h"

#include <QtWidgets>

CTrkToRteDialog::CTrkToRteDialog(QString& projectName, QString& routeName,
                                                     IGisProject::type_e& type)
    : QDialog(CMainWindow::getBestWidgetForParent())
    , routeName(routeName)
    , projectName(projectName)
    , type(type)
{
    setupUi(this);
    SETTINGS;
    cfg.beginGroup("TrkToRt");
    saveSubPoints = cfg.value("saveSubPoints", false).toBool();
    cfg.endGroup();

    checkBoxSubPoints->setChecked(saveSubPoints);
    key = IGisProject::getUserFocus();
    QListWidgetItem* lastSelectedItem = nullptr;
    const QTreeWidget *parent = CGisWorkspace::self().getWks();
    for(int i = 0; i < parent->topLevelItemCount(); i++)
    {
        IGisProject* project = dynamic_cast<IGisProject*>(parent->topLevelItem(i));
        if(nullptr == project)
        {
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(project->icon(CGisListWks::eColumnIcon),
                                project->text(CGisListWks::eColumnName), listWidgetProjects);
        item->setData(Qt::UserRole + 0, project->getKey());
        item->setData(Qt::UserRole + 1, project->getType());
        item->setData(Qt::UserRole + 2, project->getName());

        if(project->getName() == projectName)
        {
            lastSelectedItem = item;
            type = project->getType();
            key = project->getKey();
        }
    }

    frameType->setEnabled(listWidgetProjects->count() == 0);

    if(nullptr == lastSelectedItem)
    {
        SETTINGS;
        QString filter = cfg.value("Paths/lastGisFilter", "GPS Exchange Format (*.qms)").toString();
        if(filter.contains("gpx"))
        {
            type = IGisProject::eTypeGpx;
        }
        else
        {
            type = IGisProject::eTypeQms;
        }
    }
    else
    {
        slotItemClicked(lastSelectedItem);
    }

    lineEditRouteName->setText(routeName);
    lineEditNewProject->setText(projectName);
    buttonBoxEnabled();
    setType(type);

    connect(listWidgetProjects, &QListWidget::itemClicked, this, &CTrkToRteDialog::slotItemClicked);
    connect(listWidgetProjects, &QListWidget::itemDoubleClicked, this, &CTrkToRteDialog::slotItemDoubleClicked);
    connect(lineEditNewProject, &QLineEdit::textChanged, this, &CTrkToRteDialog::slotProjectChanged);
    connect(lineEditNewProject, &QLineEdit::textEdited, this, &CTrkToRteDialog::slotProjectEdited);
    connect(radioQms, &QRadioButton::clicked, this, &CTrkToRteDialog::slotTypeChanged);
    connect(radioGpx, &QRadioButton::clicked, this, &CTrkToRteDialog::slotTypeChanged);
    connect(radioDatabase, &QRadioButton::toggled, this, &CTrkToRteDialog::slotTypeChanged);
    connect(lineEditRouteName, &QLineEdit::textChanged, this, &CTrkToRteDialog::slotRouteChanged);
    connect(lineEditRouteName, &QLineEdit::textEdited, this, &CTrkToRteDialog::slotRouteChanged);

    lineEditNewProject->setFocus();
    adjustSize();

    CCanvas::setOverrideCursor(Qt::ArrowCursor, "CTrkToRteDialog");
}

CTrkToRteDialog::~CTrkToRteDialog()
{
    CCanvas::restoreOverrideCursor("~CTrkToRteDialog");
}

void CTrkToRteDialog::reject()
{
    key.clear();
    routeName.clear();
    projectName.clear();

    QDialog::reject();
}

void CTrkToRteDialog::accept()
{
    saveSubPoints = checkBoxSubPoints->isChecked();
    SETTINGS;
    cfg.beginGroup("TrkToRt");
    cfg.setValue("saveSubPoints", saveSubPoints);
    cfg.endGroup();
    QDialog::accept();
    CCanvas::restoreOverrideCursor("~CTrkToRteDialog");
}

void CTrkToRteDialog::slotItemClicked(QListWidgetItem* item)
{
    lineEditNewProject->setText(item->data(Qt::UserRole + 2).toString());
    key = item->data(Qt::UserRole).toString();
    type = IGisProject::type_e(item->data(Qt::UserRole + 1).toInt());
    setType(type);

    frameType->setEnabled(false);
}

void CTrkToRteDialog::slotItemDoubleClicked(QListWidgetItem* item)
{
    lineEditNewProject->setText(item->data(Qt::UserRole + 2).toString());
    key = item->data(Qt::UserRole).toString();
    type = IGisProject::type_e(item->data(Qt::UserRole + 1).toInt());

    QDialog::accept();
}

void CTrkToRteDialog::buttonBoxEnabled()
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
            !(projectName.isEmpty() || routeName.isEmpty()));
}

void CTrkToRteDialog::slotProjectChanged(const QString& text)
{
    projectName = text;
    buttonBoxEnabled();
}

void CTrkToRteDialog::slotProjectEdited(const QString& text)
{
    key.clear();
    projectName = text;
    buttonBoxEnabled();
    frameType->setEnabled(true);
}

void CTrkToRteDialog::slotRouteChanged(const QString& text)
{
    routeName = text;
    buttonBoxEnabled();
}


void CTrkToRteDialog::slotTypeChanged()
{
    if(radioQms->isChecked())
    {
        type = IGisProject::eTypeQms;
    }
    else if(radioGpx->isChecked())
    {
        type = IGisProject::eTypeGpx;
    }
    else if(radioDatabase->isChecked())
    {
        type = IGisProject::eTypeDb;
    }
}

void CTrkToRteDialog::setType(IGisProject::type_e& t)
{
    switch(type)
    {
    case IGisProject::eTypeGpx:
        radioGpx->setChecked(true);
        break;

    case IGisProject::eTypeDb:
        radioDatabase->setChecked(true);
        break;

    default:
        radioQms->setChecked(true);
        t = IGisProject::eTypeQms;
        break;
    }
}

IGisProject* CTrkToRteDialog::getProject()
{
    IGisProject *project = nullptr;
    CGisListWks* treeWks = CGisWorkspace::self().getWks();
    if(!key.isEmpty())
    {
        QMutexLocker lock(&IGisItem::mutexItems);
        for (int i = 0; i < treeWks->topLevelItemCount(); i++)
        {
            project = dynamic_cast<IGisProject*>(treeWks->topLevelItem(i));
            if(nullptr == project)
            {
                continue;
            }
            if(key == project->getKey())
            {
                return project;
            }
        }
    }
    else if(type == IGisProject::eTypeDb)
    {
        QList<quint64> ids;
        QString db;
        QString host;
        IDBFolder::type_e type;

        CSelectDBFolder dlg1(ids, db, host, this);
        if((dlg1.exec() == QDialog::Rejected) || ids.isEmpty())
        {
            return nullptr;
        }

        CSetupFolder dlg2(type, projectName, false, this);
        if(dlg2.exec() == QDialog::Rejected)
        {
            return nullptr;
        }

        QMutexLocker lock(&IGisItem::mutexItems);
        CEvtW2DCreate evt(projectName, type, ids[0], db, host);
        CGisDatabase::self().sendEventForDb(&evt);

        if(evt.idChild)
        {
            CDBProject *p = nullptr;
            while (nullptr == p)
            {
                QApplication::processEvents(
                        QEventLoop::WaitForMoreEvents
                                | QEventLoop::ExcludeUserInputEvents, 100);
                p = dynamic_cast<CDBProject*>(treeWks->getProjectById(
                        evt.idChild, db));
            }
            /*
             Creating a project usually does initiate an info request. However as the project isn't in the workspace
             the moment we create it, the request will fail. That is why we send the info now.
             */
            p->postStatus(false);
            project = p;
        }
    }
    else if(!projectName.isEmpty())
    {
        QMutexLocker lock(&IGisItem::mutexItems);
        if(type == IGisProject::eTypeGpx)
        {
            project = new CGpxProject(projectName, treeWks);
        }
        else if(type == IGisProject::eTypeQms)
        {
            project = new CQmsProject(projectName, treeWks);
        }
    }

    return project;
}
