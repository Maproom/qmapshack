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

#include "CMainWindow.h"
#include "gis/CGisListDB.h"
#include "gis/db/CSetupDatabase.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CSetupDatabase::CSetupDatabase(CGisListDB &parent)
    : QDialog(&parent)
    , list(parent)
{
    setupUi(this);

    connect(toolNewDB, SIGNAL(clicked()), this, SLOT(slotNewDB()));
    connect(toolAddDB, SIGNAL(clicked()), this, SLOT(slotOpenDB()));
    connect(lineName, SIGNAL(textChanged(QString)), this, SLOT(slotUpdateButtonBox()));
    connect(lineServer, SIGNAL(textChanged(QString)), this, SLOT(slotUpdateButtonBox()));
    connect(lineUser, SIGNAL(textChanged(QString)), this, SLOT(slotUpdateButtonBox()));
    connect(radioSqlite, SIGNAL(clicked(bool)), this, SLOT(slotUpdateButtonBox()));
    connect(radioMysql, SIGNAL(clicked(bool)), this, SLOT(slotUpdateButtonBox()));

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

    if(radioSqlite->isChecked())
    {
        if(labelFilename->text() == "-")
        {
            enable = false;
        }
        frameSqlite->setEnabled(true);
        frameMysql->setEnabled(false);
    }
    else if(radioMysql->isChecked())
    {
        if(lineServer->text().isEmpty())
        {
            enable = false;
        }
        if(lineUser->text().isEmpty())
        {
            enable = false;
        }
        frameSqlite->setEnabled(false);
        frameMysql->setEnabled(true);
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}


void CSetupDatabase::accept()
{
    QString name = lineName->text();
    if(list.hasDatabase(name))
    {
        QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("There is already a database with name '%1'").arg(name), QMessageBox::Abort);
        return;
    }

    QDialog::accept();
}


void CSetupDatabase::slotNewDB()
{
    SETTINGS;
    QString path = cfg.value("Database/lastDatabasePath", QDir::homePath()).toString();

    QString filename = QFileDialog::getSaveFileName(this, tr("New database..."), path, "QMapShack Database (*.db)");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    if(fi.suffix().toLower() != "db")
    {
        filename += ".db";
    }


    cfg.setValue("Database/lastDatabasePath", fi.absolutePath());

    labelFilename->setText(filename);

    slotUpdateButtonBox();
}

void CSetupDatabase::slotOpenDB()
{
    SETTINGS;
    QString path = cfg.value("Database/lastDatabasePath", QDir::homePath()).toString();

    QString filename = QFileDialog::getOpenFileName(this, tr("Open database..."), path, "QMapShack Database (*.db)");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    if(fi.suffix().toLower() != "db")
    {
        filename += ".db";
    }


    cfg.setValue("Database/lastDatabasePath", fi.absolutePath());

    labelFilename->setText(filename);

    slotUpdateButtonBox();
}
