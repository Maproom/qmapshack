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

#include <QObject>
#include <QString>
#include <QStringList>

class QProcess;

/**
   @brief One run of an external program whose outcome is a single `finished()`.

   finished() is emitted exactly once and always queued, also when the program cannot be started, so a caller never
   sees it before the constructor has returned. The job deletes itself afterwards; hold it in a QPointer.
 */
class CShotsJob : public QObject {
  Q_OBJECT
 public:
  /** @param program  empty fails with @p missing */
  CShotsJob(const QString& program, const QStringList& args, const QString& missing, QObject* parent);
  /** @brief Kills a program still running; emits nothing. */
  ~CShotsJob() override;

 signals:
  /** @param error  empty when the program exited with 0 */
  void finished(bool ok, const QString& error);

 private:
  void finish(bool ok, const QString& error);

  QProcess* process = nullptr;
  bool done = false;
};

#endif  // CSHOTSJOB_H
