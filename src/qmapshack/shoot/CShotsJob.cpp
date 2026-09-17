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

#include "shoot/CShotsJob.h"

#include <QProcess>
#include <QTimer>
#include <cstdio>

namespace {
/** How long a program asked to end gets to clean up [ms]. */
constexpr qint32 kTerminateTimeoutMs = 5000;
/** How long a killed program gets to go [ms]. */
constexpr qint32 kKillTimeoutMs = 5000;
}  // namespace

CShotsJob::CShotsJob(const QString& program, const QStringList& args, const QString& missing, QObject* parent)
    : QObject(parent) {
  if (program.isEmpty()) {
    finish(false, missing);
    return;
  }

  process = new QProcess(this);
  // Standard error is read, not forwarded, so a failure can say why; the console still gets every byte of it.
  process->setProcessChannelMode(QProcess::ForwardedOutputChannel);
  connect(process, &QProcess::readyReadStandardError, this, &CShotsJob::takeError);
  connect(process, &QProcess::finished, this, [this](int code, QProcess::ExitStatus how) {
    takeError();
    const bool ok = QProcess::NormalExit == how && 0 == code;
    // A failure's reason is what the program wrote to standard error; shots.py puts its summary there.
    const QString& said = QString::fromUtf8(err).replace("\r\n", "\n").trimmed();
    finish(ok, ok ? QString()
                  : (said.isEmpty() ? QString("%1 ended with exit code %2.").arg(process->program()).arg(code) : said));
  });
  connect(process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError problem) {
    // Every other error ends in finished().
    if (QProcess::FailedToStart == problem) {
      finish(false, QString("%1 cannot be started: %2").arg(process->program(), process->errorString()));
    }
  });
  process->start(program, args);
}

CShotsJob::~CShotsJob() {
  if (nullptr != process && QProcess::NotRunning != process->state()) {
    process->disconnect(this);
    // Asked first: shots.py kills the application it started and removes its scratch directory on SIGTERM. Windows
    // has no such request for a console program.
#if !defined(Q_OS_WIN)
    process->terminate();
    if (process->waitForFinished(kTerminateTimeoutMs)) {
      return;
    }
#endif
    process->kill();
    process->waitForFinished(kKillTimeoutMs);
  }
}

void CShotsJob::takeError() {
  const QByteArray& bytes = process->readAllStandardError();
  err += bytes;
  std::fwrite(bytes.constData(), 1, bytes.size(), stderr);
  std::fflush(stderr);
}

void CShotsJob::finish(bool ok, const QString& error) {
  if (done) {
    return;
  }
  done = true;
  // Queued: a failed start reports from inside start(), inside the constructor.
  QTimer::singleShot(0, this, [this, ok, error]() {
    emit finished(ok, error);
    deleteLater();
  });
}
