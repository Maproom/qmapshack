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

#include "CAbout.h"
#include "version.h"

#include <QtWidgets>
#include <gdal.h>
#include <proj_api.h>
#include <routino.h>

CAbout::CAbout(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    labelVersion->setText(VER_STR);
    labelQtVersion->setText(qVersion());
    labelGDALVersion->setText(GDALVersionInfo("--version"));
    labelProj4Version->setText(QString::number(PJ_VERSION));
    if(Routino_CheckAPIVersion() != ROUTINO_ERROR_NONE)
    {
        //labelRoutinoVersion->setText(tr("%1 (API V%2, expected V%3)").arg(Routino_Version).arg(ROUTINO_API_VERSION).arg(Routino_APIVersion));
    }
    else
    {
        //labelRoutinoVersion->setText(tr("%1 (API V%2)").arg(Routino_Version).arg(Routino_APIVersion));
    }
}

CAbout::~CAbout()
{
}

