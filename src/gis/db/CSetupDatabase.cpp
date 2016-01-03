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

    lineUser->setText(CMainWindow::getUser());

    connect(toolNewDB,   &QToolButton::clicked,   this, &CSetupDatabase::slotNewDB);
    connect(toolAddDB,   &QToolButton::clicked,   this, &CSetupDatabase::slotOpenDB);
    connect(lineName,    &QLineEdit::textChanged, this, &CSetupDatabase::slotUpdateButtonBox);
    connect(lineServer,  &QLineEdit::textChanged, this, &CSetupDatabase::slotUpdateButtonBox);
    connect(lineUser,    &QLineEdit::textChanged, this, &CSetupDatabase::slotUpdateButtonBox);
    connect(radioSqlite, &QRadioButton::clicked,  this, &CSetupDatabase::slotUpdateButtonBox);
    connect(radioMysql,  &QRadioButton::clicked,  this, &CSetupDatabase::slotUpdateButtonBox);
    connect(checkMySQLNoPasswd, &QCheckBox::clicked, linePasswd, &QLineEdit::setDisabled);

    slotUpdateButtonBox();
}

CSetupDatabase::~CSetupDatabase()
{
}

void CSetupDatabase::slotUpdateButtonBox()
{
    bool enable = !lineName->text().isEmpty();

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


bool CSetupDatabase::noPasswd() const
{
    return radioMysql->isChecked() && checkMySQLNoPasswd->isChecked();
}
