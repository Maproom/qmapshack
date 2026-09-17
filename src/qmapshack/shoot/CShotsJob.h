/**********************************************************************************************
   Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSHOTSJOB_H
#define CSHOTSJOB_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>

class QProcess;

/**
   @brief One run of an external program whose outcome is a single `finished()`.

   finished() is emitted exactly once and always queued, also when the program cannot be started, so a caller never
   sees it before the constructor has returned. The job deletes itself afterwards; hold it in a QPointer.

   The program's output goes on to this process' console. Its standard error is also kept, and a failure reports it,
   so the writer reads the reason in a dialog.
 */
class CShotsJob : public QObject {
  Q_OBJECT
 public:
  /** @param program  empty fails with @p missing */
  CShotsJob(const QString& program, const QStringList& args, const QString& missing, QObject* parent);
  /** @brief Kills a program still running; emits nothing. */
  ~CShotsJob() override;

 signals:
  /** @param error  empty when the program exited with 0, else what it wrote to standard error */
  void finished(bool ok, const QString& error);

 private:
  void finish(bool ok, const QString& error);
  /** @brief Keep what the program wrote to standard error so far and pass it on to the console. */
  void takeError();
  QProcess* process = nullptr;
  QByteArray err;
  bool done = false;
};

#endif  // CSHOTSJOB_H
