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

#ifndef CMAPVRTBUILDER_H
#define CMAPVRTBUILDER_H

#include <QWidget>
#include <QProcess>
#include "ui_IMapVrtBuilder.h"

class CMapVrtBuilder : public QWidget, private Ui::IMapVrtBuilder
{
    Q_OBJECT;
    public:
        CMapVrtBuilder(QWidget * parent);
        virtual ~CMapVrtBuilder();

    private slots:
        void slotSelectSourceFiles();
        void slotSelectTargetFile();
        void slotStart();

        void slotStderr();
        void slotStdout();
        void slotFinished(int exitCode, QProcess::ExitStatus status);


    private:
        void enabelStartButton();
        void stdOut(const QString& str, bool gui = false);
        void stdErr(const QString& str, bool gui = false);


        bool tainted;
        QString output;

        QProcess cmd;
};

#endif //CMAPVRTBUILDER_H

