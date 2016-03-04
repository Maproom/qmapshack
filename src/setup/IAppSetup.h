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

#ifndef CAPPSETUP_H
#define CAPPSETUP_H

#include <QApplication>
#include <QtCore>
#include "CAppOpts.h"


class IAppSetup
{
public:
    static IAppSetup *getPlatformInstance();
    virtual void initQMapShack() = 0;
    void initLogHandler();
    void processArguments();

    virtual QString routinoPath(QString xmlFile) = 0;
    virtual QString defaultCachePath() = 0;
    virtual QString userDataPath(QString subdir = 0) = 0;
    virtual QString logDir() = 0;

protected:
    void prepareGdal(QString gdalDir, QString projDir);
    void prepareTranslator(QString translationPath, QString translationPrefix);

    QString path(QString path, QString subdir, bool mkdir, QString debugName);
};

#endif // CAPPSETUP_H
