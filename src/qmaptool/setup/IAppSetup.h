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

#ifndef IAPPSETUP_H
#define IAPPSETUP_H

#include "CAppOpts.h"
#include <QApplication>
#include <QtCore>


class IAppSetup : public QObject
{
    Q_OBJECT
public:
    static IAppSetup& self();
    static IAppSetup &createInstance(QObject * parent);

    virtual ~IAppSetup();

    virtual void initQMapTool() = 0;
    void initLogHandler();
    void processArguments();

    virtual QString defaultCachePath() = 0;
    virtual QString userDataPath(QString subdir = 0) = 0;
    virtual QString logDir() = 0;
    virtual QString findExecutable(const QString &name) = 0;

    QString getGdaladdo() const
    {
        return QFile::exists(pathGdaladdo) ? pathGdaladdo : QFile::exists(pathGdaladdoOverride) ? pathGdaladdoOverride : "";
    }

    QString getGdaltranslate() const
    {
        return QFile::exists(pathGdaltranslate) ? pathGdaltranslate : QFile::exists(pathGdaltranslateOverride) ? pathGdaltranslateOverride : "";
    }

    QString getGdalwarp() const
    {
        return QFile::exists(pathGdalwarp) ? pathGdalwarp : QFile::exists(pathGdalwarpOverride) ? pathGdalwarpOverride : "";
    }

    QString getGdalbuildvrt() const
    {
        return QFile::exists(pathGdalbuildvrt) ? pathGdalbuildvrt : QFile::exists(pathGdalbuildvrtOverride) ? pathGdalbuildvrtOverride : "";
    }

    QString getQmtrgb2pct() const
    {
        return QFile::exists(pathQmtrgb2pct) ? pathQmtrgb2pct : QFile::exists(pathQmtrgb2pctOverride) ? pathQmtrgb2pctOverride : "";
    }

    void setGdaladdoOverride(const QString& path)
    {
        pathGdaladdoOverride = path;
        emit sigSetupChanged();
    }

    void setGdaltranslateOverride(const QString& path)
    {
        pathGdaltranslateOverride = path;
        emit sigSetupChanged();
    }

    void setGdalwarpOverride(const QString& path)
    {
        pathGdalwarpOverride = path;
        emit sigSetupChanged();
    }

    void setGdalbuildvrtOverride(const QString& path)
    {
        pathGdalbuildvrtOverride = path;
        emit sigSetupChanged();
    }

    void setQmtrgb2pctOverride(const QString& path)
    {
        pathQmtrgb2pctOverride = path;
        emit sigSetupChanged();
    }

signals:
    void sigSetupChanged();

protected:
    static IAppSetup* pSelf;

    IAppSetup(QObject * parent)
        : QObject(parent)
    {
        pSelf = this;
    }

    void prepareGdal(QString gdalDir, QString projDir);
    void prepareTranslator(QString translationPath, QString translationPrefix);
    void prepareToolPaths();

    QString path(QString path, QString subdir, bool mkdir, QString debugName);

    QString pathGdaladdo;
    QString pathGdaltranslate;
    QString pathGdalwarp;
    QString pathGdalbuildvrt;
    QString pathQmtrgb2pct;

    QString pathGdaladdoOverride;
    QString pathGdaltranslateOverride;
    QString pathGdalwarpOverride;
    QString pathGdalbuildvrtOverride;
    QString pathQmtrgb2pctOverride;
};

#endif // IAPPSETUP_H
