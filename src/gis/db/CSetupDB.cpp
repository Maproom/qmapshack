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

#include "gis/db/CSetupDB.h"
#include "helpers/CSettings.h"
#include "config.h"

#include <QtWidgets>

CSetupDB::CSetupDB(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    SETTINGS;
    cfg.beginGroup("Database");
    checkSaveOnExit->setChecked(cfg.value("saveOnExit", true).toBool());
    spinSaveEvery->setValue(cfg.value("saveEvery",5).toInt());
    labelDatabasePath->setText(cfg.value("path",QDir::home().filePath(CONFIGDIR).append("database.db")).toString());
    cfg.endGroup();

    connect(toolDatabasePath, SIGNAL(clicked()), this, SLOT(slotSetupPath()));
    connect(checkSaveOnExit, SIGNAL(toggled(bool)), spinSaveEvery, SLOT(setEnabled(bool)));
}

CSetupDB::~CSetupDB()
{

}

void CSetupDB::accept()
{
    SETTINGS;
    cfg.beginGroup("Database");
    cfg.setValue("saveOnExit", checkSaveOnExit->isChecked());
    cfg.setValue("saveEvery", spinSaveEvery->value());
    cfg.setValue("path", labelDatabasePath->text());
    cfg.endGroup();

    QMessageBox::information(this, tr("Setup database..."), tr("Changes will become active after an application's restart."), QMessageBox::Ok);

    QDialog::accept();
}

void CSetupDB::slotSetupPath()
{
    QFileInfo fi(labelDatabasePath->text());
    QString filename = QFileDialog::getSaveFileName(this, tr("Select database path..."),fi.absoluteFilePath(), "Database (*.db)");
    if(filename.isEmpty())
    {
        return;
    }
    labelDatabasePath->setText(filename);
}
