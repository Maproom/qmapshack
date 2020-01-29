/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2020 Henri Hornburg hrnbg@t-online.de


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

#include "config.h"
#include "gis/CGisWorkspace.h"
#include "gis/db/CSetupWorkspace.h"
#include "helpers/CSettings.h"
#include <QtWidgets>

CSetupWorkspace::CSetupWorkspace(CGisWorkspace * workspace, QWidget *parent)
    : QDialog(parent), workspace(workspace)
{
    setupUi(this);

    SETTINGS;
    cfg.beginGroup("Database");
    checkSaveOnExit->setChecked(cfg.value("saveOnExit", true).toBool());
    spinSaveEvery->setValue(cfg.value("saveEvery", 5).toInt());
    checkDbUpdate->setChecked(cfg.value("listenUpdate", false).toBool());
    linePort->setText(cfg.value("port", "34123").toString());
    checkDeviceSupport->setChecked(cfg.value("device support", true).toBool());
    cfg.endGroup();

    checkShowTags->setChecked(!workspace->areTagsHidden());

    connect(checkSaveOnExit, &QCheckBox::toggled, spinSaveEvery, &QSpinBox::setEnabled);
}

CSetupWorkspace::~CSetupWorkspace()
{
}

void CSetupWorkspace::accept()
{
    SETTINGS;
    cfg.beginGroup("Database");
    cfg.setValue("saveOnExit", checkSaveOnExit->isChecked());
    cfg.setValue("saveEvery", spinSaveEvery->value());
    cfg.setValue("listenUpdate", checkDbUpdate->isChecked());
    cfg.setValue("port", linePort->text());
    cfg.setValue("device support", checkDeviceSupport->isChecked());
    cfg.endGroup();

    workspace->setTagsHidden(!checkShowTags->isChecked());

    QMessageBox::information(this, tr("Setup database..."), tr("Changes to database settings will become active after an application's restart."), QMessageBox::Ok);

    QDialog::accept();
}

