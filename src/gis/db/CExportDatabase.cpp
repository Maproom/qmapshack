/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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
#include "gis/db/CExportDatabase.h"
#include "gis/db/CExportDatabaseThread.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CExportDatabase::CExportDatabase(quint64 id, QSqlDatabase &db, QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    SETTINGS;
    cfg.beginGroup("ExportDB");
    labelPath->setText(cfg.value("path","-").toString());
    checkGpx11->setChecked(cfg.value("asGpx11", false).toBool());
    checkDel->setChecked(cfg.value("delOld", false).toBool());
    cfg.endGroup();

    QDir dir(labelPath->text());
    pushStart->setEnabled(dir.exists());

    connect(toolPath, &QToolButton::clicked, this, &CExportDatabase::slotSetPath);
    connect(pushStart, &QPushButton::clicked, this, &CExportDatabase::slotStart);

    thread = new CExportDatabaseThread(id, db, this);
    connect(pushAbort, &QPushButton::clicked, thread, &CExportDatabaseThread::slotAbort);
    connect(thread, &CExportDatabaseThread::started, this, &CExportDatabase::slotStarted);
    connect(thread, &CExportDatabaseThread::finished, this, &CExportDatabase::slotFinished);
    connect(thread, &CExportDatabaseThread::sigOut, this, &CExportDatabase::slotStdout);
    connect(thread, &CExportDatabaseThread::sigErr, this, &CExportDatabase::slotStderr);
}

CExportDatabase::~CExportDatabase()
{
    SETTINGS;
    cfg.beginGroup("ExportDB");
    cfg.setValue("path", labelPath->text());
    cfg.setValue("asGpx11", checkGpx11->isChecked());
    cfg.setValue("delOld", checkDel->isChecked());
    cfg.endGroup();
}

void CExportDatabase::closeEvent(QCloseEvent * e)
{
    if(thread->isRunning())
    {
        e->ignore();
    }
    else
    {
        e->accept();
    }
}

void CExportDatabase::slotStdout(const QString& str)
{
    textBrowser->setTextColor(Qt::black);
    textBrowser->append(str);
}


void CExportDatabase::slotStderr(const QString& str)
{
    textBrowser->setTextColor(Qt::red);
    textBrowser->append(str);
}


void CExportDatabase::slotSetPath()
{
    QString path = labelPath->text();

    path = QFileDialog::getExistingDirectory(CMainWindow::self().getBestWidgetForParent(), tr("Select export path..."), path);
    if(!path.isEmpty())
    {
        labelPath->setText(path);
    }

    QDir dir(labelPath->text());
    pushStart->setEnabled(dir.exists());
}

void CExportDatabase::slotStart()
{
    textBrowser->clear();
    thread->start(labelPath->text(), checkGpx11->isChecked(), checkDel->isChecked());
}

void CExportDatabase::slotStarted()
{
    pushStart->setEnabled(false);
    pushAbort->setEnabled(true);
    pushClose->setEnabled(false);
}

void CExportDatabase::slotFinished()
{
    pushStart->setEnabled(true);
    pushAbort->setEnabled(false);
    pushClose->setEnabled(true);
}

