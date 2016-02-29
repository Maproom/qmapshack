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

#include "config.h"
#include "helpers/CAppOpts.h"
#include "helpers/CAppSetup.h"

#include <gdal.h>
#include <iostream>


#ifndef _MKSTR_1
#define _MKSTR_1(x)    #x
#define _MKSTR(x)      _MKSTR_1(x)
#endif


class CLogHandler
{
public:
    CLogHandler(QString logDirectory, bool writeToFile, bool debugOutput) :
        writeToFile(writeToFile), debugOutput(debugOutput), logFile(QDir(logDirectory).absoluteFilePath(logfileName())),
        fileStream(&logFile)
    {
        if (writeToFile)
        {
            logFile.open(QIODevice::WriteOnly | QIODevice::Append);
        }
        qSetMessagePattern("%{time yyyy-MM-dd h:mm:ss.zzz} [%{type}] %{message}");
    }

    void log(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
#if QT_VERSION >= 0x050400
        QString txt = qFormatLogMessage(type, context, msg);
#else
        QString txt = msg;
#endif
        printToConsole(type, txt);
        appendToFile(type, txt);
    }

    void printLoggerInfo()
    {
        qDebug() << "Log configuration:" << "log file=" << logFile.fileName() << "write to file=" << writeToFile <<
            "debug output=" << debugOutput;
    }

private:
    QString logfileName()
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


    void appendToFile(QtMsgType type, QString formatedMsg)
    {
        Q_UNUSED(type);
        if (writeToFile)
        {
            fileStream << formatedMsg << endl;
        }
    }


    void printToConsole(QtMsgType type, QString formatedMsg)
    {
        switch (type)
        {
        case QtDebugMsg:
            if (debugOutput)
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
            std::cerr << formatedMsg.toUtf8().constData() << std::endl;
            break;

        case QtFatalMsg:
            std::cerr << formatedMsg.toUtf8().constData() << std::endl;
            abort();
            break;
        }
    }

    bool writeToFile;
    bool debugOutput;
    QFile logFile;
    QTextStream fileStream;
};


class CAppSetupWin : public CAppSetup
{
public:
    void initQMapShack() override
    {
        // setup environment variables for GDAL/Proj4
        QString apppath = QCoreApplication::applicationDirPath();
        apppath = apppath.replace("/", "\\");
        QString gdalDir = QString("%1\\data").arg(apppath);
        QString projDir = QString("%1\\share").arg(apppath);

        qunsetenv("GDAL_DRIVER_PATH");
        CAppSetup::prepareGdal(gdalDir, projDir);

        QString appResourceDir = QString("%1\\translations").arg(apppath).toUtf8();
        prepareTranslator(appResourceDir, "qtbase_");
        prepareTranslator(appResourceDir, "qmapshack_");

        //reset PATH to avoid that wrong .dll's are loaded
        qputenv("PATH", "");

        // create direcotries
        CAppSetup::path(defaultCachePath(), 0, true, "CACHE");
        CAppSetup::path(userDataPath("WaypointIcons"), 0, true, "USER DATA");
        CAppSetup::path(logDir(), 0, true, "LOG");
    }


    QString routinoPath(QString xmlFile) override
    {
        QString apppath = QCoreApplication::applicationDirPath();
        apppath = apppath.replace("/", "\\");
        QDir dirXml(QString("%1\\routino-xml").arg(apppath).toUtf8());
        return CAppSetup::path(dirXml.absolutePath(), xmlFile, false, "ROUTINO");
    }

    QString defaultCachePath() override
    {
        return CAppSetup::path(QDir::home().absolutePath(), ".QMapShack/", false, 0);
    }

    QString userDataPath(QString subdir = 0) override
    {
        QString path = QDir::home().absoluteFilePath(CONFIGDIR);
        return CAppSetup::path(path, subdir, false, 0);
    }

    QString logDir() override
    {
        return QDir::temp().absolutePath();
    }
};


class CAppSetupLinux : public CAppSetup
{
public:
    void initQMapShack() override
    {
        prepareGdal("", "");

        // setup translators
        QString resourceDir     = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        QString translationPath = QCoreApplication::applicationDirPath();
        translationPath.replace(QRegExp("bin$"), "share/qmapshack/translations");
        prepareTranslator(resourceDir, "qt_");
        prepareTranslator(translationPath, "qmapshack_");

        // create direcotries
        CAppSetup::path(defaultCachePath(), 0, true, "CACHE");
        CAppSetup::path(userDataPath("WaypointIcons"), 0, true, "USER DATA");
        CAppSetup::path(logDir(), 0, true, "LOG");
    }

    QString routinoPath(QString xmlFile) override
    {
        QDir dirXml(_MKSTR(ROUTINO_XML_PATH));
        return CAppSetup::path(dirXml.absolutePath(), xmlFile, false, "ROUTINO");
    }

    QString defaultCachePath() override
    {
        return CAppSetup::path(QDir::home().absolutePath(), ".QMapShack/", false, 0);
    }

    QString userDataPath(QString subdir = 0) override
    {
        QString path = QDir::home().absoluteFilePath(CONFIGDIR);
        return CAppSetup::path(path, subdir, false, 0);
    }

    QString logDir() override
    {
        return QDir::temp().absolutePath();
    }
};


static QString relTranslationDir = "Resources/translations"; // app
static QString relRoutinoDir     = "Resources/routino"; // app
static QString relGdalDir        = "Resources/gdal"; // app
static QString relProjDir        = "Resources/proj"; // app

static QString relLogDir         = "Library/Logs"; // home

class CAppSetupMac : public CAppSetup
{
public:
    void initQMapShack() override
    {
        // setup gdal
        QString gdalDir = getApplicationDir(relGdalDir).absolutePath();
        QString projDir = getApplicationDir(relProjDir).absolutePath();
        prepareGdal(gdalDir, projDir);

        // setup translators
        QString translationPath = getApplicationDir(relTranslationDir).absolutePath();
        prepareTranslator(translationPath, "qt_");
        prepareTranslator(translationPath, "qmapshack_");

        // create direcotries
        CAppSetup::path(defaultCachePath(), 0, true, "CACHE");
        CAppSetup::path(userDataPath("WaypointIcons"), 0, true, "USER DATA");
        CAppSetup::path(logDir(), 0, false, "LOG");
    }

    QString routinoPath(QString xmlFile) override
    {
        QDir dirXml = getApplicationDir(relRoutinoDir);
        return CAppSetup::path(dirXml.absolutePath(), xmlFile, false, "ROUTINO");
    }

    QString defaultCachePath() override
    {
        QString cachePath =  QStandardPaths::standardLocations(QStandardPaths::CacheLocation).at(0);
        return CAppSetup::path(cachePath, 0, false, 0);
    }

    QString userDataPath(QString subdir = 0) override
    {
        QString dataDir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);
        return CAppSetup::path(dataDir, subdir, false, 0);
    }

    QString logDir() override
    {
        // home location returns / (root) instead of user home...
        QString home = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0);
        QDir dir = QDir(home);
        dir.cdUp();
        return CAppSetup::path(dir.absolutePath(), relLogDir, false, 0);
    }

private:
    QDir getApplicationDir(QString subdir)
    {
        QDir appDir(QCoreApplication::applicationDirPath());
        appDir.cdUp();
        appDir.cd(subdir);
        return appDir;
    }
};


CAppSetup* instance = nullptr;

CAppSetup* CAppSetup::getPlattformInstance()
{
    if(nullptr == instance)
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


void CAppSetup::prepareGdal(QString gdalDir, QString projDir)
{
    qputenv("GDAL_DATA", gdalDir.toUtf8());
    qputenv("PROJ_LIB", projDir.toUtf8());

    qDebug() << "GDAL_DATA directory set to " + gdalDir;
    qDebug() << "PROJ_LIB directory set to " + projDir;
    GDALAllRegister();
}


QString CAppSetup::path(QString path, QString subdir, bool mkdir, QString debugName)
{
    QDir pathDir(path);

    if(subdir != 0)
    {
        pathDir = QDir(pathDir.absoluteFilePath(subdir));
    }
    if(mkdir && !pathDir.exists())
    {
        pathDir.mkpath(pathDir.absolutePath());
        qDebug() << debugName << "path created" << pathDir.absolutePath();
    }
    else if (debugName != 0)
    {
        qDebug() << debugName << "path" << pathDir.absolutePath();
    }
    return pathDir.absolutePath();
}


void CAppSetup::prepareTranslator(QString translationPath, QString translationPrefix)
{
    QString locale = QLocale::system().name();
    QDir dir(translationPath);
    if(!QFile::exists(dir.absoluteFilePath(translationPrefix + locale)))
    {
        locale = locale.left(2);
    }
    qDebug() << "locale" << locale;

    QApplication* app =  (QApplication*) QCoreApplication::instance();
    QTranslator *qtTranslator = new QTranslator(app);
    if (qtTranslator->load(translationPrefix + locale, translationPath))
    {
        app->installTranslator(qtTranslator);
        qDebug() << "using file '"+ translationPath + "/" + translationPrefix + locale + ".qm' for translations.";
    }
    else
    {
        qWarning() << "no file found for translations '"+ translationPath + "/" + translationPrefix + locale + "' (using default).";
    }
}


CLogHandler* logHandler = nullptr;

static void logCallback(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    logHandler->log(type, context, msg);
}

void CAppSetup::initLogHandler()
{
    logHandler = new CLogHandler(logDir(), qlOpts->logfile, qlOpts->debug);
    qInstallMessageHandler(logCallback);
    logHandler->printLoggerInfo();
}
