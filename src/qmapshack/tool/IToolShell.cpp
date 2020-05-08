/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "IToolShell.h"

#include <QtWidgets>

IToolShell::IToolShell(QWidget * parent)
    : QWidget(parent)
{
    connect(&cmd, &QProcess::readyReadStandardError,  this, &IToolShell::slotStderr);
    connect(&cmd, &QProcess::readyReadStandardOutput, this, &IToolShell::slotStdout);

    connect(&cmd, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &IToolShell::slotFinished);
    connect(&cmd, static_cast<void (QProcess::*)(QProcess::ProcessError)   >(&QProcess::error),    this, &IToolShell::slotError);
}

IToolShell::~IToolShell()
{
    text = nullptr;
}


void IToolShell::slotError(QProcess::ProcessError error)
{
    if (text.isNull())
    {
        return;
    }
    text->setTextColor(Qt::red);
    text->insertPlainText(QString(tr("Execution of external program `%1` failed: ")).arg(cmd.program()));
    switch(error)
    {
    case QProcess::FailedToStart:
        text->insertPlainText(QString(tr("Process cannot be started.\n")));
        text->insertPlainText(QString(tr("Make sure the required packages are installed, `%1` exists and is executable.\n")).arg(cmd.program()));
        break;

    case QProcess::Crashed:
        text->insertPlainText(QString(tr("External process crashed.\n")));
        break;

    default:
        text->insertPlainText(QString(tr("An unknown error occurred.\n")));
        break;
    }
}

void IToolShell::slotStderr()
{
    if (text.isNull())
    {
        return;
    }

    QString str;
    text->setTextColor(Qt::red);
    str = cmd.readAllStandardError();

    if(str[0] == '\r')
    {
#ifdef WIN32
        if(str.contains("\n"))
        {
            text->insertPlainText("\n");
        }
        else
#endif // WIN32
        {
            text->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
            text->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
            text->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
            text->textCursor().removeSelectedText();
        }


#ifdef WIN32
        str = str.split("\r").last().remove("\r").remove("\n");
#else
        str = str.split("\r").last();
#endif
    }

    text->insertPlainText(str);
    text->verticalScrollBar()->setValue(text->verticalScrollBar()->maximum());
}

void IToolShell::slotStdout()
{
    if (text.isNull())
    {
        return;
    }

    QString str;
    text->setTextColor(Qt::blue);
    str = cmd.readAllStandardOutput();

    if(str[0] == '\r')
    {
#ifdef WIN32
        if(str.contains("\n"))
        {
            text->insertPlainText("\n");
        }
        else
#endif // WIN32
        {
            text->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
            text->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
            text->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
            text->textCursor().removeSelectedText();
        }

#ifdef WIN32
        str = str.split("\r").last().remove("\r").remove("\n");
#else
        str = str.split("\r").last();
#endif
    }

    text->insertPlainText(str);
    text->verticalScrollBar()->setValue(text->verticalScrollBar()->maximum());
}

void IToolShell::stdOut(const QString& str)
{
    if (text.isNull())
    {
        return;
    }

    text->setTextColor(Qt::black);
    text->append(str);
}


void IToolShell::stdErr(const QString& str)
{
    if (text.isNull())
    {
        return;
    }

    text->setTextColor(Qt::red);
    text->append(str);
}


void IToolShell::slotFinished(int exitCode, QProcess::ExitStatus status)
{
    if(exitCode || status)
    {
        if (!text.isNull())
        {
            text->setTextColor(Qt::red);
            text->append(tr("!!! failed !!!\n"));
        }
        return;
    }

    finished(exitCode, status);
}
