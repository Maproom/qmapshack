/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsessr norbert.truchsess@t-online.de

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

#ifndef CROUTERBROUTERTOOLSHELL_H
#define CROUTERBROUTERTOOLSHELL_H

#include "tool/IToolShell.h"
#include <QWidget>

class CRouterBRouterToolShell : public IToolShell
{
    Q_OBJECT
public:
    CRouterBRouterToolShell(QTextBrowser *&textBrowser, QWidget * parent);
    virtual ~CRouterBRouterToolShell();

    void start(const QString dir, const QString command, const QStringList args);
    void stop();
    void out(const QString out);
    void execute(const QString dir, const QString command, const QStringList args);
    QProcess::ExitStatus getExitStatus();
    const int getExitCode();

signals:
    void processStateChanged(const QProcess::ProcessState newState);

private slots:
    void slotStateChanged(const QProcess::ProcessState newState);

private:
    void finished(int exitCode, QProcess::ExitStatus status) override;
    QProcess::ExitStatus exitStatus;
    int exitCode;
};

#endif //CROUTERBROUTERTOOLSHELL_H

