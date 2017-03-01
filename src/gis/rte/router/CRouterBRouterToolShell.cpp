/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#include "CMainWindow.h"
#include <QWidget>
#include <QTextBrowser>
#include "CRouterBRouterToolShell.h"


CRouterBRouterToolShell::CRouterBRouterToolShell(QTextBrowser *&textBrowser, QWidget * parent)
    : IToolShell(textBrowser,parent)
{
    connect(&cmd, &QProcess::stateChanged, this, &CRouterBRouterToolShell::slotStateChanged);
}

CRouterBRouterToolShell::~CRouterBRouterToolShell()
{
}

void CRouterBRouterToolShell::out(const QString out)
{
    stdOut(out);
}

void CRouterBRouterToolShell::execute(const QString dir, const QString command, const QStringList args)
{
    start(dir,command,args);
    cmd.waitForFinished();
}

void CRouterBRouterToolShell::start(const QString dir, const QString command, const QStringList args)
{
    stdOut("cd " + dir);
    stdOut(command+" " + args.join(" ") + "\n");
    cmd.setWorkingDirectory(dir);
    cmd.start(command,args);
    cmd.waitForStarted();
}

void CRouterBRouterToolShell::stop()
{
    if (cmd.state() != QProcess::NotRunning)
    {
        cmd.terminate();
        cmd.waitForFinished();
    }
}

void CRouterBRouterToolShell::slotStateChanged(const QProcess::ProcessState newState)
{
    emit processStateChanged(newState);
}

void CRouterBRouterToolShell::finished(const int exitCode, const QProcess::ExitStatus status)
{
    this->exitCode = exitCode;
    this->exitStatus = status;
    if (status == QProcess::ExitStatus::NormalExit)
    {
        text->setTextColor(Qt::darkGreen);
        text->append(tr("!!! done !!!\n"));
    }
    else
    {
        text->setTextColor(Qt::darkRed);
        text->append(tr("!!! failed !!!\n"));
    }
}
