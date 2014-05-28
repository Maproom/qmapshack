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
#include "CSettings.h"

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

}

CMainWindow::~CMainWindow()
{
    SETTINGS;
    cfg.setValue("MainWindow/state", saveState());
    cfg.setValue("MainWindow/geometry", saveGeometry());

}

