/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CROUTINODATABASEBUILDER_H
#define CROUTINODATABASEBUILDER_H

#include "tool/IToolShell.h"
#include "ui_IRoutinoDatabaseBuilder.h"
#include <QTemporaryFile>


class CRoutinoDatabaseBuilder : public IToolShell, private Ui::IRoutinoDatabaseBuilder
{
    Q_OBJECT
public:
    CRoutinoDatabaseBuilder(QWidget * parent);
    virtual ~CRoutinoDatabaseBuilder();

private slots:
    void slotSelectSourceFiles();
    void slotSelectTargetPath();
    void slotStart();

    void enabelStartButton();

private:
    void finished(int exitCode, QProcess::ExitStatus status);

    bool first = false;
    bool tainted = false;
    bool last = false;

    QStringList sourceFiles;
    QString targetPrefix;
    QString targetPath;
};

#endif //CROUTINODATABASEBUILDER_H

