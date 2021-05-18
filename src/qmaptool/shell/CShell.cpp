/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "shell/CShell.h"

#include <QtWidgets>

CShell* CShell::pSelf = nullptr;

CShell::CShell(QWidget* parent)
    : QTextBrowser(parent)
{
    pSelf = this;

    connect(&cmd, &QProcess::readyReadStandardError, this, &CShell::slotStderr);
    connect(&cmd, &QProcess::readyReadStandardOutput, this, &CShell::slotStdout);

    connect(&cmd, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &CShell::slotFinished);
    connect(&cmd, static_cast<void (QProcess::*)(QProcess::ProcessError)   >(&QProcess::error), this, &CShell::slotError);
}

void CShell::slotError(QProcess::ProcessError error)
{
    setTextColor(Qt::red);
    insertPlainText(QString(tr("Execution of external program `%1` failed: ")).arg(cmd.program()));
    switch(error)
    {
    case QProcess::FailedToStart:
        insertPlainText(QString(tr("Process cannot be started.\n")));
        insertPlainText(QString(tr("Make sure the required packages are installed, `%1` exists and is executable.\n")).arg(cmd.program()));
        break;

    case QProcess::Crashed:
        insertPlainText(QString(tr("External process crashed.\n")));
        break;

    default:
        insertPlainText(QString(tr("An unknown error occurred.\n")));
        break;
    }
}

void CShell::slotStderr()
{
    QString str;
    setTextColor(Qt::red);
    str = cmd.readAllStandardError();

    if(str[0] == '\r')
    {
#ifdef Q_OS_WIN64
        if(str.contains("\n"))
        {
            insertPlainText("\n");
        }
        else
#endif // Q_OS_WIN64
        {
            moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
            moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
            moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
            textCursor().removeSelectedText();
        }


#ifdef Q_OS_WIN64
        str = str.split("\r").last().remove("\r").remove("\n");
#else
        str = str.split("\r").last();
#endif
    }

    insertPlainText(str);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void CShell::slotStdout()
{
    QString str;
    setTextColor(Qt::blue);
    str = cmd.readAllStandardOutput();

    if(str[0] == '\r')
    {
#ifdef Q_OS_WIN64
        if(str.contains("\n"))
        {
            insertPlainText("\n");
        }
        else
#endif // Q_OS_WIN64
        {
            moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
            moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
            moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
            textCursor().removeSelectedText();
        }

#ifdef Q_OS_WIN64
        str = str.split("\r").last().remove("\r").remove("\n");
#else
        str = str.split("\r").last();
#endif
    }

    insertPlainText(str);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void CShell::stdOut(const QString& str)
{
    setTextColor(Qt::black);
    append(str);
}


void CShell::stdErr(const QString& str)
{
    setTextColor(Qt::red);
    append(str);
}


void CShell::slotFinished(int exitCode, QProcess::ExitStatus status)
{
    if(exitCode || status)
    {
        emit sigFinishedJob(jobId);
        setTextColor(Qt::red);
        append(tr("!!! failed !!!\n"));
        return;
    }

    ++idxCommand;
    nextCommand();
}

void CShell::slotCancel()
{
    if(cmd.state() == QProcess::NotRunning)
    {
        return;
    }

    stdOut(tr("\nCanceled by user's request.\n"));
    cmd.kill();
    cmd.waitForFinished(10000);
}

int CShell::execute(QList<CShellCmd> cmds)
{
    CMainWindow::self().makeShellVisible();

    if(cmd.state() != QProcess::NotRunning)
    {
        return -1;
    }

    clear();

    idxCommand = 0;
    commands = cmds;

    nextCommand();
    return ++jobId;
}


void CShell::nextCommand()
{
    if(idxCommand >= commands.size())
    {
        emit sigFinishedJob(jobId);
        setTextColor(Qt::darkGreen);
        append(tr("!!! done !!!\n"));
        return;
    }

    const CShellCmd& command = commands[idxCommand];
    stdOut(command.getCmd() + " " + command.getArgs().join(" ") + "\n");
    cmd.start(command.getCmd(), command.getArgs());
}
