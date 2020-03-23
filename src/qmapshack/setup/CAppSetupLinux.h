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

#ifndef CAPPSETUPLINUX_H
#define CAPPSETUPLINUX_H

#include "setup/IAppSetup.h"

#include <QtCore>

class CAppSetupLinux : public IAppSetup
{
public:
    void initQMapShack() override;
    QString routinoPath(QString xmlFile) override;
    QString defaultCachePath() override;
    QString userDataPath(QString subdir = 0) override;
    QString logDir() override;
    QString findExecutable(const QString &name) override { return QStandardPaths::findExecutable(name); }
    QString helpFile() override;
};


#endif // CAPPSETUPLINUX_H
