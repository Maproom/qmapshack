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

#include "gis/CGisListDB.h"
#include "gis/db/CSetupDatabase.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CSetupDatabase::CSetupDatabase(QString& name, QString& filename, CGisListDB &parent)
    : QDialog(&parent)
    , list(parent)
    , name(name)
    , filename(filename)
{
    setupUi(this);

    lineName->setText(name);
    labelFilename->setText(filename);

    connect(toolNewDB, SIGNAL(clicked()), this, SLOT(slotNewDB()));
    connect(toolAddDB, SIGNAL(clicked()), this, SLOT(slotOpenDB()));
    connect(lineName, SIGNAL(textChanged(QString)), this, SLOT(slotUpdateButtonBox()));

    slotUpdateButtonBox();
}

CSetupDatabase::~CSetupDatabase()
{
}

void CSetupDatabase::slotUpdateButtonBox()
{
    bool enable = true;

    if(lineName->text().isEmpty())
    {
        enable = false;
    }
    if(labelFilename->text() == "-")
    {
        enable = false;
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}


void CSetupDatabase::accept()
{
    name = lineName->text();
    if(list.hasDatabase(name))
    {
        QMessageBox::warning(0, tr("Error..."), tr("There is already a database with name '%1'").arg(name), QMessageBox::Abort);
        return;
    }
    filename = labelFilename->text();

    QDialog::accept();
}


void CSetupDatabase::slotNewDB()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastDatabasePath", QDir::homePath()).toString();

    QString filename = QFileDialog::getSaveFileName(this, tr("New database..."), path, "Database (*.db)");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    if(fi.suffix().toLower() != "db")
    {
        filename += ".db";
    }


    cfg.setValue("Paths/lastDatabasePath", fi.absolutePath());

    labelFilename->setText(filename);

    slotUpdateButtonBox();
}

void CSetupDatabase::slotOpenDB()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastDatabasePath", QDir::homePath()).toString();

    QString filename = QFileDialog::getOpenFileName(this, tr("Open database..."), path, "Database (*.db)");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    if(fi.suffix().toLower() != "db")
    {
        filename += ".db";
    }


    cfg.setValue("Paths/lastDatabasePath", fi.absolutePath());

    labelFilename->setText(filename);

    slotUpdateButtonBox();
}
