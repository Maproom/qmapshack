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

#ifndef CSHELL_H
#define CSHELL_H

#include "shell/CShellCmd.h"

#include <QList>
#include <QProcess>
#include <QTextBrowser>

class CShell : public QTextBrowser
{
    Q_OBJECT
public:
    static CShell& self()
    {
        return *pSelf;
    }

    virtual ~CShell() = default;

    int execute(QList<CShellCmd> cmds);
signals:
    void sigFinishedJob(qint32 jobId);

public slots:
    void slotCancel();

protected slots:
    /// read the stderr from the process and paste it into the text browser
    void slotStderr();
    /// read the stdout from the process and paste it into the text browser
    void slotStdout();
    void slotError(QProcess::ProcessError error);
    virtual void slotFinished(int exitCode, QProcess::ExitStatus status);

protected:
    void nextCommand();

    /// write text to stdout color channel of the text browser
    void stdOut(const QString& str);
    /// write text to stderr color channel of the text browser
    void stdErr(const QString& str);

    QProcess cmd;

    QList<CShellCmd> commands;
    qint32 idxCommand = 0;
    qint32 jobId = 0;

private:
    friend class Ui_IMainWindow;
    CShell(QWidget * parent);
    static CShell * pSelf;
};

#endif //CSHELL_H

