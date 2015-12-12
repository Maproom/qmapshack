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

#include "config.h"
#include "gis/db/CSetupWorkspace.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CSetupWorkspace::CSetupWorkspace(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    SETTINGS;
    cfg.beginGroup("Database");
    checkSaveOnExit->setChecked(cfg.value("saveOnExit", true).toBool());
    spinSaveEvery->setValue(cfg.value("saveEvery",5).toInt());
    cfg.endGroup();

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
    cfg.endGroup();

    QMessageBox::information(this, tr("Setup database..."), tr("Changes will become active after an application's restart."), QMessageBox::Ok);

    QDialog::accept();
}

