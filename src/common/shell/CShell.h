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

#include <QList>
#include <QProcess>
#include <QTextBrowser>

#include "shell/CShellCmd.h"

/**
   @brief A QTextBrowser-based widget that runs a queue of external commands (CShellCmd)
          and echoes their stdout/stderr into itself.

   Compiled into both the qmapshack and qmaptool targets (src/common/), but only
   qmaptool's IMainWindow.ui promotes a singleton instance (isSingleton=true, the
   default) - that's what makes self() safe there. qmapshack has no such instance;
   CVrtAdvisoryDialog is the only place qmapshack constructs a CShell, and it does
   so with isSingleton=false specifically so it does NOT become self(). Do not call
   CShell::self() from qmapshack code - pSelf is never set there and self() dereferences
   it unconditionally.
 */
class CShell : public QTextBrowser {
  Q_OBJECT
 public:
  /// @brief Construct a shell widget.
  /// @param parent      parent widget
  /// @param isSingleton if true (default), this instance becomes CShell::self() - matches the
  ///                     promoted-widget instance Qt Designer's generated IMainWindow UI
  ///                     constructs. Pass false for any other instance (e.g. one a dialog owns)
  ///                     that must not hijack the shared singleton.
  CShell(QWidget* parent, bool isSingleton = true);
  ~CShell() override;

  /// @brief The app-wide singleton instance. Only ever set in qmaptool (see this class's
  ///        doc comment) - calling this from qmapshack code dereferences a null pSelf.
  static CShell& self() { return *pSelf; }

  int execute(QList<CShellCmd> cmds);

  /// @brief True if the last completed job finished without any command failing.
  bool lastJobSucceeded() const { return jobSucceeded_; }

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

 public:
  /// write text to stdout color channel of the text browser
  void stdOut(const QString& str);
  /// write text to stderr color channel of the text browser
  void stdErr(const QString& str);

 protected:
  void nextCommand();

  QProcess cmd;

  QList<CShellCmd> commands;
  qint32 idxCommand = 0;
  qint32 jobId = 0;

 private:
  static CShell* pSelf;
  bool jobSucceeded_ = false;
};

#endif  // CSHELL_H
