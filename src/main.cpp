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
#include "helpers/CCommandProcessor.h"
#include "helpers/CAppSetup.h"
#include "version.h"

#include <QtCore>
#include <QtWidgets>
#include <iostream>

CAppOpts *qlOpts;

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    
    QCoreApplication::setApplicationName("QMapShack");
    QCoreApplication::setOrganizationName("QLandkarte");
    QCoreApplication::setOrganizationDomain("qlandkarte.org");
    
    CAppSetup* env = CAppSetup::getPlattformInstance();
    env->installMessageHandler();
    
    CCommandProcessor cmdParse;
    qlOpts = cmdParse.processOptions();
    
    env->prepareConfig();
    env->prepareTranslators(&app);
    env->prepareGdal();
    
    QSplashScreen *splash = 0;
    if (!qlOpts->nosplash)
    {
        QPixmap pic(":/pics/splash.png");
        QPainter p(&pic);
        QFont f = p.font();
        f.setBold(true);

        p.setPen(Qt::black);
        p.setFont(f);
        p.drawText(400,395,"V " VER_STR);

        splash = new QSplashScreen(pic);
        splash->show();
    }

    CMainWindow w;
    w.show();

    if (splash != 0)
    {
        splash->finish(&w);
        delete splash;
    }

    return app.exec();
}
