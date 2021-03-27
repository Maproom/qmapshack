/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#if defined (Q_OS_WIN32)
#define USE_KILL_FOR_SHUTDOWN
#endif

class CRouterBRouterToolShell : public IToolShell
{
    Q_OBJECT
public:
    CRouterBRouterToolShell(QTextBrowser *textBrowser, QWidget * parent);
    virtual ~CRouterBRouterToolShell();

    void start(const QString &dir, const QString &command, const QStringList &args);
    void stop();

signals:
    void sigProcessStateChanged(const QProcess::ProcessState newState);
    void sigProcessError(const QProcess::ProcessError error, const QString &errorString);

private slots:
    void slotStateChanged(const QProcess::ProcessState newState);
    void slotError(const QProcess::ProcessError error);
    void slotStartupTimer();

private:
    void finished(int exitCode, QProcess::ExitStatus status) override;

    bool isBeingKilled { false };
    bool isStarting { false };
    QTimer * startupTimer { nullptr };
};

#endif //CROUTERBROUTERTOOLSHELL_H

