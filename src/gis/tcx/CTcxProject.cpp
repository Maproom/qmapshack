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
#include "gis/CGisListWks.h"
#include "gis/tcx/CTcxProject.h"

#include <QtWidgets>

CTcxProject::CTcxProject(const QString &filename, CGisListWks * parent)
    : IGisProject(eTypeTcx, filename, parent)
{
    setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/TcxProject.png"));
    blockUpdateItems(true);
    loadTcx(filename);
    blockUpdateItems(false);
    setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}


void CTcxProject::loadTcx(const QString& filename)
{
    try
    {
        loadTcx(filename, this);
    }
    catch(QString &errormsg)
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                              tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
        valid = false;
    }
}


void CTcxProject::loadTcx(const QString &filename, CTcxProject *project)
{
    // place code to read TCX here. Use void CGpxProject::loadGpx(const QString &filename, CGpxProject *project)
    // as an example.
    qDebug() << "tcx";

    project->sortItems();
    project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
}
