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

#ifndef ITOOLSHELL_H
#define ITOOLSHELL_H

#include <QProcess>
#include <QWidget>

class QTextBrowser;

class IToolShell : public QWidget
{
    Q_OBJECT
public:
    IToolShell(QTextBrowser *textBrowser, QWidget *parent);
    virtual ~IToolShell();

protected slots:
    void slotStderr();
    void slotStdout();
    void slotError(QProcess::ProcessError error);
    virtual void slotFinished(int exitCode, QProcess::ExitStatus status);

protected:
    virtual void finished(int exitCode, QProcess::ExitStatus status) = 0;

    void setOutputBrowser(QTextBrowser * textBrowser);
    void stdOut(const QString& str, bool gui = false);
    void stdErr(const QString& str, bool gui = false);

    QProcess cmd;

    QTextBrowser * text;
};

#endif //ITOOLSHELL_H

