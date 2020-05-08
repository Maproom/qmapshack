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

#include "CMainWindow.h"
#include "CSingleInstanceProxy.h"
#include "setup/IAppSetup.h"
#include "version.h"

#include <QApplication>
#include <QtWidgets>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("QMapTool");
    QCoreApplication::setOrganizationName("QLandkarte");
    QCoreApplication::setOrganizationDomain("qlandkarte.org");

    IAppSetup& env = IAppSetup::createInstance(qApp);
    env.processArguments();
    env.initLogHandler();
    env.initQMapTool();

    // make sure this is the one and only instance on the system
    CSingleInstanceProxy s(qlOpts->arguments);

    QSplashScreen *splash = nullptr;
    if (!qlOpts->nosplash)
    {
        QPixmap pic(":/pic/splash.png");
        QPainter p(&pic);
        QFont f = p.font();
        f.setBold(true);

        p.setPen(Qt::black);
        p.setFont(f);
        p.drawText(260,195,"QMapTool, V " VER_STR);

        splash = new QSplashScreen(pic);
#ifdef Q_OS_MAC
        // remove the splash screen flag on OS-X as workaround for the reported bug
        // https://bugreports.qt.io/browse/QTBUG-49576
        splash->setWindowFlags(splash->windowFlags() & (~Qt::SplashScreen));
#endif
        splash->show();
    }

    CMainWindow w;
    w.show();

    if(nullptr != splash)
    {
        splash->finish(&w);
        delete splash;
    }

    return app.exec();
}


