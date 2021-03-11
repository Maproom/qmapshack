/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CAbout.h"
#include "gis/proj_x.h"
#include "version.h"

#include <gdal.h>
#include <QtWidgets>


CAbout::CAbout(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    if(QString(VER_SUFFIX).isEmpty())
    {
        labelVersion->setText(VER_STR);
    }
    else
    {
        labelVersion->setText(VER_STR "." VER_SUFFIX);
    }

    labelQtVersion->setText(qVersion());
    labelGDALVersion->setText(GDALVersionInfo("--version"));
    labelProj4Version->setText(proj_info().release);
}


