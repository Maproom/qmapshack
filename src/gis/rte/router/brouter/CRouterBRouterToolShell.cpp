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
#include "CRouterBRouterToolShell.h"

#include <QtWidgets>

CRouterBRouterToolShell::CRouterBRouterToolShell(QTextBrowser *textBrowser, QWidget * parent)
    : IToolShell(parent)
{
    setTextBrowser(textBrowser);
    connect(&cmd, &QProcess::stateChanged, this, &CRouterBRouterToolShell::slotStateChanged);
    connect(&cmd, static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error), this, &CRouterBRouterToolShell::slotError);
}

CRouterBRouterToolShell::~CRouterBRouterToolShell()
{
}

void CRouterBRouterToolShell::start(const QString &dir, const QString &command, const QStringList &args)
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
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(__FreeBSD_kernel__) || defined(__GNU__)
        cmd.terminate();
#elif defined (Q_OS_WIN32)
        cmd.kill();
#else
  #error OS not supported
#endif
    }
}

void CRouterBRouterToolShell::slotStateChanged(const QProcess::ProcessState newState) const
{
    emit sigProcessStateChanged(newState);
}

void CRouterBRouterToolShell::slotError(const QProcess::ProcessError error) const
{
    emit sigProcessError(error, cmd.errorString());
}

void CRouterBRouterToolShell::finished(const int exitCode, const QProcess::ExitStatus status)
{
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
