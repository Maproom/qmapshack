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

class CRouterBRouterToolShell : public IToolShell
{
    Q_OBJECT
public:
    CRouterBRouterToolShell(QTextBrowser *textBrowser, QWidget * parent);
    virtual ~CRouterBRouterToolShell();

    void start(const QString &dir, const QString &command, const QStringList &args);
    void stop();
    void out(const QString &out) { stdOut(out); }
    void error(const QString &error) { stdErr(error); }

    void execute(const QString &dir, const QString &command, const QStringList &args);

signals:
    void sigProcessStateChanged(const QProcess::ProcessState newState) const;
    void sigProcessError(const QProcess::ProcessError error, const QString &errorString) const;

private slots:
    void slotStateChanged(const QProcess::ProcessState newState) const;
    void slotError(const QProcess::ProcessError error) const;

private:
    void finished(int exitCode, QProcess::ExitStatus status) override;
};

#endif //CROUTERBROUTERTOOLSHELL_H

