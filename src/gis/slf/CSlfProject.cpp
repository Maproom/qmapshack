/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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
#include "gis/slf/CSlfProject.h"
#include "gis/slf/CSlfReader.h"

#include <QtWidgets>

CSlfProject::CSlfProject(const QString &filename, bool readFile)
    : IGisProject(eTypeSlf, filename, (CGisListWks*)nullptr)
{
    setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/SlfProject.png"));
    blockUpdateItems(true);

    valid = true;
    if(readFile)
    {
        try
        {
            CSlfReader::readFile(filename, this);
        }
        catch(QString &errormsg)
        {
            QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                                  tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
            valid = false;
        }
    }
    else
    {
        IGisProject::filename.clear();
    }
    blockUpdateItems(false);
    setupName(QFileInfo(filename).baseName().replace("_", " "));
}

CSlfProject::~CSlfProject()
{
}

