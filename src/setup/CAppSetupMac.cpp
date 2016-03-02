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

static QString relLogDir         = "Library/Logs"; // home


void CAppSetupMac::initQMapShack()
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
    IAppSetup::path(defaultCachePath(), 0, true, "CACHE");
    IAppSetup::path(userDataPath("WaypointIcons"), 0, true, "USER DATA");
    IAppSetup::path(logDir(), 0, false, "LOG");

    migrateDirContent(defaultCachePath());
    migrateDirContent(userDataPath());
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
    QString dataDir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
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

static void copyRecursively(const QString &srcFilePath,
                            const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir())
    {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.exists())
        {
            targetDir.mkdir(QFileInfo(tgtFilePath).fileName());
        }
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames)
        {
            const QString newSrcFilePath = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath = tgtFilePath + QLatin1Char('/') + fileName;
            copyRecursively(newSrcFilePath, newTgtFilePath);
        }
    }
    else
    {
        qDebug() << "migrate file from "<< srcFilePath << "to" << tgtFilePath;
        if (!QFile::rename(srcFilePath, tgtFilePath))
        {
            qDebug() << "error migrating file" << srcFilePath;
        }
    }
}


void CAppSetupMac::migrateDirContent(QString dir)
{
    QDir dirDest = QDir(dir);
    QDir dirSource = QDir(dir);
    dirSource.cdUp();
    dirSource.cdUp();
    dirSource.cd("QMapShack");
    if (dirSource.exists())
    {
        qDebug() << "migrate data from "<<dirSource.absolutePath() << "to" << dirDest.absolutePath();
        copyRecursively(dirSource.absolutePath(), dirDest.absolutePath());
        dirSource.removeRecursively();
    }
}