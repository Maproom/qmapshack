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

CTrkToRteDialog::CTrkToRteDialog(IGisProject *project, QString& routeName)
    : QDialog(CMainWindow::getBestWidgetForParent())
    , routeName(routeName)
    , project(project)
{
    setupUi(this);
    SETTINGS;
    cfg.beginGroup("TrkToRt");
    saveSubPoints = cfg.value("saveSubPoints", false).toBool();
    cfg.endGroup();

    checkBoxSubPoints->setChecked(saveSubPoints);
    key = IGisProject::getUserFocus();

    lineEditRouteName->setText(routeName);
    labelProjectName->setText(project->getName());
    buttonBoxEnabled();

    connect(pushButtonProject, &QPushButton::clicked, this, &CTrkToRteDialog::slotProject);
    connect(lineEditRouteName, &QLineEdit::textChanged, this, &CTrkToRteDialog::slotRouteChanged);
    connect(lineEditRouteName, &QLineEdit::textEdited, this, &CTrkToRteDialog::slotRouteChanged);

    adjustSize();

    CCanvas::setOverrideCursor(Qt::ArrowCursor, "CTrkToRteDialog");
}

CTrkToRteDialog::~CTrkToRteDialog()
{
    CCanvas::restoreOverrideCursor("~CTrkToRteDialog");
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

void CTrkToRteDialog::slotProject()
{
    IGisProject* pr = CGisWorkspace::self().selectProject(true);
    if(nullptr == pr)
    {
        return;
    }
    project = pr;
    labelProjectName->setText(project->getName());
}

void CTrkToRteDialog::buttonBoxEnabled()
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!routeName.isEmpty());
}

void CTrkToRteDialog::slotRouteChanged(const QString& text)
{
    routeName = text;
    buttonBoxEnabled();
}

