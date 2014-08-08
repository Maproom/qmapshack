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
#include "helpers/CAppOpts.h"
#include "version.h"

#include <QtCore>
#include <QtWidgets>
#include <CGetOpt.h>
#include <iostream>
#include <gdal.h>

CAppOpts *qlOpts;

static void usage(std::ostream &s)
{
    s << "usage: qlandkartegt [-d | --debug]\n"
         "                    [-h | --help]\n"
         "                    [-n | --no-splash]\n"
         "                    [-c | --config=file]\n"
         "                    [files...]\n"
         "\n";
}


static void processOptions()
{
    CGetOpt opts;                // uses qApp->argc() and qApp->argv()
    bool doDebugOut;
    opts.addSwitch('d', "debug", &doDebugOut);
    bool doHelp;
    opts.addSwitch('h', "help", &doHelp);
    bool noSplash;
    opts.addSwitch('n', "no-splash", &noSplash);
    QStringList args;
    opts.addOptionalArguments("files", &args);
    QString config;
    opts.addOptionalOption('c', "config", &config, "");

    if (!opts.parse())
    {
        usage(std::cerr);
        exit(1);
    }

    if (doHelp)
    {
        usage(std::cout);
        exit(0);
    }

    qlOpts = new CAppOpts(doDebugOut, noSplash, config, args);

}


static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type)
    {
        case QtDebugMsg:
            if (qlOpts->debug)
            {

                std::cout << msg.toUtf8().constData() << std::endl;
            }
            break;

        case QtWarningMsg:
            std::cerr << "Warning: " << msg.toUtf8().constData() << std::endl;
            break;

        case QtCriticalMsg:
            std::cerr << "Critical: " <<  msg.toUtf8().constData() << std::endl;
            break;

        case QtFatalMsg:
            std::cerr << "Fatal: " << msg.toUtf8().constData() << std::endl;
            abort();
            break;
    }
}



int main(int argc, char ** argv)
{

    QApplication a(argc, argv);
    processOptions();

#ifndef Q_OS_WIN32
    qInstallMessageHandler(myMessageOutput);
#endif

    // find Qt's transaltions first
    QString locale = QLocale::system().name();
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QTranslator *qtTranslator = new QTranslator(&a);
    if (qtTranslator->load(QLatin1String("qt_") + locale,resourceDir))
    {
        a.installTranslator(qtTranslator);
    }
    else if (qtTranslator->load(QLatin1String("qt_") + locale,QCoreApplication::applicationDirPath()))
    {
        a.installTranslator(qtTranslator);
    }

    // find MapShack's translations
    QStringList dirList;
    dirList << QCoreApplication::applicationDirPath().replace(QRegExp("bin$"), "share/qmapshack/translations");
//    dirList << "./src";
    foreach(QString dir, dirList)
    {
        QString transName = QLatin1String("qmapshack_") + locale;
        if (qtTranslator->load( transName, dir))
        {
            a.installTranslator(qtTranslator);
            qDebug() << "using file '"+ QDir(dir).canonicalPath() + "/" + transName + ".qm' for translations.";
            break;
        }
    }



    GDALAllRegister();

    QCoreApplication::setApplicationName("QMapShack");
    QCoreApplication::setOrganizationName("QLandkarte");
    QCoreApplication::setOrganizationDomain("qlandkarte.org");

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

    return a.exec();
}
