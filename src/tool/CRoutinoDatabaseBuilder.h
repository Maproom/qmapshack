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

#include "ui_IRoutinoDatabaseBuilder.h"
#include <QWidget>
#include <QProcess>
#include <QTemporaryFile>

class CRoutinoDatabaseBuilder : public QWidget, private Ui::IRoutinoDatabaseBuilder
{
    Q_OBJECT
public:
    CRoutinoDatabaseBuilder(QWidget * parent);
    virtual ~CRoutinoDatabaseBuilder();

private slots:
    void slotSelectSourceFiles();
    void slotSelectTargetPath();
    void slotStart();

    void slotStderr();
    void slotStdout();
    void slotFinished(int exitCode, QProcess::ExitStatus status);
    void enabelStartButton();

private:
    void stdOut(const QString& str, bool gui = false);
    void stdErr(const QString& str, bool gui = false);


    bool first;
    bool tainted;
    bool last;
    QString output;

    QProcess cmd;

    QStringList sourceFiles;
    QString targetPrefix;
    QString targetPath;

    QTemporaryFile xmlTagging;

};

#endif //CROUTINODATABASEBUILDER_H

