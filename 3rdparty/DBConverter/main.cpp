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

#include <QtWidgets>
#include "CMainWindow.h"
#include "helpers/CAppOpts.h"

CAppOpts *qlOpts;


int main(int argc, char ** argv)
{
    qlOpts = new CAppOpts(true, false, "", QStringList());

    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("qlgt2qms");
    QCoreApplication::setOrganizationName("QLandkarte");
    QCoreApplication::setOrganizationDomain("qlandkarte.org");

    CMainWindow w;
    w.show();

    return a.exec();
}
