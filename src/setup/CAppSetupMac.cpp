/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg

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

#include "setup/CAppSetupMac.h"

static QString relTranslationDir = "Resources/translations"; // app
static QString relRoutinoDir     = "Resources/routino"; // app
static QString relGdalDir        = "Resources/gdal"; // app
static QString relProjDir        = "Resources/proj"; // app
static QString relBinDir         = "Tools"; // app

static QString relLogDir         = "Library/Logs"; // home


void CAppSetupMac::extendPath()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList envlist = env.toStringList();
    QString value = "";
    for(int i=0; i < envlist.size(); i++)
    {
        QString entry = envlist[i];
        if(entry.startsWith("PATH="))
        {
            int index = entry.indexOf("=");

            if(index != -1)
            {
                value = entry.right(entry.length() - (index+1)) + ":";
            }
            break;
        }
    }
    QString binDir = getApplicationDir(relBinDir).absolutePath();
    qDebug() << "BIN" << binDir;
    value += binDir;
    qputenv("PATH", value.toLatin1().constData());
}


void CAppSetupMac::initQMapShack()
{
    extendPath();
    // setup gdal
    QString gdalDir = getApplicationDir(relGdalDir).absolutePath();
    QString projDir = getApplicationDir(relProjDir).absolutePath();
    prepareGdal(gdalDir, projDir);

    // setup translators
    QString translationPath = getApplicationDir(relTranslationDir).absolutePath();
    prepareTranslator(translationPath, "qt_");
    prepareTranslator(translationPath, "qmapshack_");

    migrateDirContent(defaultCachePath());
    migrateDirContent(userDataPath());

    // create directories
    IAppSetup::path(defaultCachePath(), 0, true, "CACHE");
    IAppSetup::path(userDataPath("WaypointIcons"), 0, true, "USER DATA");
    IAppSetup::path(logDir(), 0, false, "LOG");
}


QString CAppSetupMac::routinoPath(QString xmlFile)
{
    QDir dirXml = getApplicationDir(relRoutinoDir);
    return IAppSetup::path(dirXml.absolutePath(), xmlFile, false, "ROUTINO");
}


QString CAppSetupMac::defaultCachePath()
{
    QString cachePath =  QStandardPaths::standardLocations(QStandardPaths::CacheLocation).first();
    return IAppSetup::path(cachePath, 0, false, 0);
}


QString CAppSetupMac::userDataPath(QString subdir)
{
#if QT_VERSION >= 0x050400
    QString dataDir = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).first();
#else
    QString dataDir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
#endif
    return IAppSetup::path(dataDir, subdir, false, 0);
}


QString CAppSetupMac::logDir()
{
    // home location returns / (root) instead of user home...
    QString home = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first();
    QDir dir = QDir(home);
    dir.cdUp();
    return IAppSetup::path(dir.absolutePath(), relLogDir, false, 0);
}


QDir CAppSetupMac::getApplicationDir(QString subdir)
{
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    appDir.cd(subdir);
    return appDir;
}


void CAppSetupMac::migrateDirContent(QString dest)
{
    QString src = dest;
    src.replace("/QLandkarte/", "/");
    QDir dirDest = QDir(dest);
    QDir dirSource = QDir(src);

    if (!dirDest.exists() && dirSource.exists())
    {
        qDebug() << "src directory for migration" << src;
        qDebug() << "dst directory for migration" << dest;

        QDir wdir;
        QString newdir = dest;
        newdir.remove("/QMapShack");
        wdir.mkdir(newdir);
        qDebug() << "directory created" << newdir;

        qDebug() << "migrate data from "<<dirSource.absolutePath() << "to" << dirDest.absolutePath();
        QDir mvDir;
        if(!mvDir.rename(dirSource.absolutePath(), dirDest.absolutePath()))
        {
            qDebug() << "error migrating directory" << dirSource;
        }
    }
}
