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

#include "helpers/CSettings.h"
#include "qlgt/CImportDatabase.h"
#include "qlgt/CQlgtDb.h"

#include <QtWidgets>

CImportDatabase::CImportDatabase(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    setObjectName(tr("Import QLandkarte Database"));

    SETTINGS;

    labelSource->setText(cfg.value("ConvertDB/source","-").toString());
    labelTarget->setText(cfg.value("ConvertDB/target","-").toString());

    textBrowser->setFont(QFont("Courier",10));

    connect(toolSelectSource, SIGNAL(clicked()), this, SLOT(slotSelectSource()));
    connect(toolSelectTarget, SIGNAL(clicked()), this, SLOT(slotSelectTarget()));
    connect(pushStart, SIGNAL(clicked()), this, SLOT(slotStart()));

    if(QFile::exists(labelSource->text()))
    {
        dbQlgt = new CQlgtDb(labelSource->text(), this);
    }
}

CImportDatabase::~CImportDatabase()
{
    SETTINGS;
    cfg.setValue("ConvertDB/source", labelSource->text());
    cfg.setValue("ConvertDB/target", labelTarget->text());
}

void CImportDatabase::stdOut(const QString& str)
{
    textBrowser->setTextColor(Qt::black);
    textBrowser->append(str);
}


void CImportDatabase::stdErr(const QString& str)
{
    textBrowser->setTextColor(Qt::red);
    textBrowser->append(str);
}


void CImportDatabase::slotSelectSource()
{
    SETTINGS;
    QString path = cfg.value("Path/source",QDir::homePath()).toString();
    QString filename = QFileDialog::getOpenFileName(this, tr("Select source database..."), path, "*.db");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    cfg.setValue("Path/source", fi.absolutePath());

    labelSource->setText(filename);

    delete dbQlgt;
    textBrowser->clear();
    dbQlgt = new CQlgtDb(filename, this);
}

void CImportDatabase::slotSelectTarget()
{
    SETTINGS;
    QString path = cfg.value("Path/target",QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName(this, tr("Select target database..."), path, "*.db");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    cfg.setValue("Path/target", fi.absolutePath());
    if(fi.suffix().toLower() != "db")
    {
        filename += ".db";
    }

    labelTarget->setText(filename);
}

void CImportDatabase::slotStart()
{
    pushStart->setEnabled(false);
    dbQlgt->start(labelTarget->text());
    pushStart->setEnabled(true);
}

