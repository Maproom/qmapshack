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
#include "helpers/CAppSetup.h"
#include "helpers/CCommandProcessor.h"
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
    qlOpts = cmdParse.processOptions(app.arguments());

    env->prepareConfig();
    env->prepareTranslators(&app);
    env->prepareGdal();

    QSplashScreen *splash = nullptr;
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

    uint seed = QDateTime::currentDateTime().toTime_t();
    qsrand(seed);

    CMainWindow w;
    w.show();

    if(nullptr != splash)
    {
        splash->finish(&w);
        delete splash;
    }

    return app.exec();
}
