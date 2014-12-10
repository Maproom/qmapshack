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
#include "helpers/CSettings.h"

#include <QtWidgets>

CMainWindow::CMainWindow()
{
    setupUi(this);

    SETTINGS;

    // start ---- restore window geometry -----
    if ( cfg.contains("MainWindow/geometry"))
    {
        restoreGeometry(cfg.value("MainWindow/geometry").toByteArray());
    }
    else
    {
        setGeometry(0,0,800,600);
    }

    if ( cfg.contains("MainWindow/state"))
    {
        restoreState(cfg.value("MainWindow/state").toByteArray());
    }
    // end ---- restore window geometry -----

    labelSource->setText(cfg.value("File/source","-").toString());
    labelTarget->setText(cfg.value("File/target","-").toString());


    connect(toolSource, SIGNAL(clicked()), this, SLOT(slotSelectSource()));
    connect(toolTarget, SIGNAL(clicked()), this, SLOT(slotSelectTarget()));


}

CMainWindow::~CMainWindow()
{
    SETTINGS;
    cfg.setValue("MainWindow/state", saveState());
    cfg.setValue("MainWindow/geometry", saveGeometry());

    cfg.setValue("File/source", labelSource->text());
    cfg.setValue("File/target", labelTarget->text());
}

void CMainWindow::slotSelectSource()
{
    SETTINGS;
    QString path = cfg.value("Path/source",QDir::homePath()).toString();
    QString filename = QFileDialog::getOpenFileName(this, tr("Select source databse..."), path, "*.db");
    if(filename.isEmpty())
    {
        return;
    }

    QFileInfo fi(filename);
    cfg.setValue("Path/source", fi.absolutePath());

    labelSource->setText(filename);

}

void CMainWindow::slotSelectTarget()
{
    SETTINGS;
    QString path = cfg.value("Path/target",QDir::homePath()).toString();
    QString filename = QFileDialog::getSaveFileName(this, tr("Select target databse..."), path, "*.db");
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

