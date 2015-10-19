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

#include "helpers/CAppOpts.h"
#include "helpers/CAppSetup.h"

#include <QtCore>
#include <gdal.h>
#include <iostream>

#include <qdebug.h>

CAppSetup* instance;

CAppSetup* CAppSetup::getPlattformInstance()
{
    if(instance == 0)
    {
#ifdef Q_OS_MAC
        instance = new CAppSetupMac();
#endif
#ifdef Q_OS_LINUX
        instance = new CAppSetupLinux();
#endif
#ifdef Q_OS_WIN32
        instance = new CAppSetupWin();
#endif
    }
    return instance;
}

QString CAppSetup::logName()
{
    QStringList domainSplit = QCoreApplication::organizationDomain().split(".");
    QString fileName;
    foreach(QString part, domainSplit)
    {
        fileName = fileName.insert(0, part + ".");
    }
    fileName.append(QCoreApplication::applicationName() + ".log");
    return fileName;
}



QString CAppSetup::logFilename()
{
    QDir dir = QDir::temp();
    return dir.absoluteFilePath(logName());
}

void CAppSetup::appendToFile(QtMsgType type, QString formatedMsg)
{
    Q_UNUSED(type);
    if(qlOpts->logfile)
    {
        QFile outFile(logFilename());
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << formatedMsg << endl;
    }
}


void CAppSetup::printToConsole(QtMsgType type, QString formatedMsg)
{
    switch (type)
    {
    case QtDebugMsg:
        if (qlOpts->debug)
        {
            std::cout << formatedMsg.toUtf8().constData() << std::endl;
        }
        break;

#if QT_VERSION >= 0x050500
    case QtInfoMsg:
        std::cout << formatedMsg.toUtf8().constData() << std::endl;
        break;

#endif
    case QtWarningMsg:
        std::cerr << formatedMsg.toUtf8().constData() << std::endl;
        break;

    case QtCriticalMsg:
        std::cerr <<  formatedMsg.toUtf8().constData() << std::endl;
        break;

    case QtFatalMsg:
        std::cerr << formatedMsg.toUtf8().constData() << std::endl;
        abort();
        break;
    }
}



void CAppSetup::consoleMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
#if QT_VERSION >= 0x050400
    QString txt = qFormatLogMessage(type, context, msg);
#else
    QString txt = msg;
#endif
    CAppSetup::getPlattformInstance()->printToConsole(type, txt);
    CAppSetup::getPlattformInstance()->appendToFile(type, txt);
}


void CAppSetup::prepareGdal()
{
    QString gdal = qgetenv("GDAL_DATA");
    QString proj = qgetenv("PROJ_LIB");
    qDebug() << "GDAL_DATA direcotry set to " + gdal;
    qDebug() << "PROJ_LIB direcotry set to " + proj;
    GDALAllRegister();
}


void CAppSetup::prepareTranslator(QApplication* app, QTranslator *qtTranslator, QString translationPath, QString translationPrefix)
{
    QString locale = QLocale::system().name();

    QDir dir(translationPath);
    if(!QFile::exists(dir.absoluteFilePath(translationPrefix + locale)))
    {
        locale = locale.left(2);
    }

    if (qtTranslator->load(translationPrefix + locale, translationPath))
    {
        app->installTranslator(qtTranslator);
        qDebug() << "using file '"+ translationPath + "/" + translationPrefix + locale + ".qm' for translations.";
    }
}


void CAppSetup::prepareConfig()
{
    QDir dir = QDir::home();
    if(!dir.exists(".config/QLandkarte"))
    {
        dir.mkpath(".config/QLandkarte");
    }
}


void CAppSetup::installMessageHandler()
{
    qSetMessagePattern("%{time yyyy-MM-dd h:mm:ss.zzz} [%{type}] %{message}");
    qInstallMessageHandler(consoleMessageHandler);
}


CAppSetupMac::CAppSetupMac()
{
}


void CAppSetupMac::prepareGdal()
{
    QString gdalDir = getResourceDir("gdal");
    QString projDir = getResourceDir("proj");

    qputenv("GDAL_DATA", gdalDir.toUtf8());
    qputenv("PROJ_LIB", projDir.toUtf8());

    CAppSetup::prepareGdal();
}


QString CAppSetupMac::getResourceDir(QString subdir)
{
    QString appResourceDir = QCoreApplication::applicationDirPath();

    QDir resourcesDir(appResourceDir);
    resourcesDir.cdUp();
    resourcesDir.cd("Resources");
    resourcesDir.cd(subdir);
    appResourceDir = resourcesDir.absolutePath();

    return appResourceDir;
}


void CAppSetupMac::prepareTranslators(QApplication* app)
{
    QString translationPath = getResourceDir("translations");

    QTranslator *qtTranslator = new QTranslator(app);
    prepareTranslator(app, qtTranslator, translationPath, "qt_");

    QTranslator *qlandkartegtTranslator = new QTranslator(app);
    prepareTranslator(app, qlandkartegtTranslator, translationPath, "qmapshack_");
}


void CAppSetupMac::prepareConfig()
{
    // nothing to do
}


QString CAppSetupMac::logFilename()
{
    QDir dir = QDir::home();
    dir.cd("Library");
    dir.cd("Logs");

    return dir.absoluteFilePath(logName());
}


CAppSetupLinux::CAppSetupLinux()
{
}


void CAppSetupLinux::prepareTranslators(QApplication* app)
{
    QString resourceDir     = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QString translationPath = QCoreApplication::applicationDirPath();
    translationPath.replace(QRegExp("bin$"), "share/qmapshack/translations");

    QTranslator *qtTranslator = new QTranslator(app);
    prepareTranslator(app, qtTranslator, resourceDir, "qt_");

    QTranslator *qlandkartegtTranslator = new QTranslator(app);
    prepareTranslator(app, qlandkartegtTranslator, translationPath, "qmapshack_");
}


CAppSetupWin::CAppSetupWin()
{
}


void CAppSetupWin::prepareGdal()
{
    // setup environment variables for GDAL/Proj4
    QString apppath = QCoreApplication::applicationDirPath();
    apppath = apppath.replace("/", "\\");
    QString gdalDir = QString("%1\\data").arg(apppath);
    QString projDir = QString("%1\\share").arg(apppath);

    qputenv("GDAL_DATA", gdalDir.toUtf8());
    qputenv("PROJ_LIB", projDir.toUtf8());

    CAppSetup::prepareGdal();
}


void CAppSetupWin::prepareTranslators(QApplication* app)
{
    QString apppath = QCoreApplication::applicationDirPath();
    apppath = apppath.replace("/", "\\");
    QString appResourceDir = QString("%1\\translations").arg(apppath).toUtf8();

    QTranslator *qtTranslator = new QTranslator(app);
    prepareTranslator(app, qtTranslator, appResourceDir, "qtbase_");

    QTranslator *qlandkartegtTranslator = new QTranslator(app);
    prepareTranslator(app, qlandkartegtTranslator, appResourceDir, "qmapshack_");
}

void CAppSetupWin::prepareConfig()
{
    CAppSetup::prepareConfig();
    //reset PATH to avoid that wrong .dll's are loaded
    qputenv("PATH", "");
}
