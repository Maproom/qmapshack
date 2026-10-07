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

#include "shoot/CShotDocState.h"

#include <QLocalSocket>
#include <QProcess>
#include <QTimer>
#include <utility>

namespace {
/** How long the process gets to end on terminate(), then on kill() [ms]. */
constexpr qint32 kStopTimeoutMs = 5000;
}  // namespace

CShotDocState::CShotDocState(const QString& scenario, const QString& followUp, QObject* parent)
    : QObject(parent), scenarioName(scenario), followUp(followUp), process(new QProcess(this)) {
  process->setProcessChannelMode(QProcess::ForwardedChannels);
  // Every outcome is delivered queued and dropped once stopped: a failed start reports from inside start(), and a
  // receiver may replace this state from its slot.
  connect(process, &QProcess::finished, this, [this](int code, QProcess::ExitStatus how) {
    const end_e end = (QProcess::NormalExit == how && 0 == code) ? eClosed : eDied;
    QTimer::singleShot(0, this, [this, end, code]() {
      if (!stopped) {
        stopped = true;
        emit ended(end, QString("exit code %1").arg(code));
      }
    });
  });
  connect(process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError problem) {
    if (QProcess::FailedToStart != problem) {
      return;
    }
    const QString& why = process->errorString();
    QTimer::singleShot(0, this, [this, why]() {
      if (!stopped) {
        stopped = true;
        emit ended(eFailedToStart, why);
      }
    });
  });
}

CShotDocState::~CShotDocState() { stop(); }

void CShotDocState::start(const QString& program, const QStringList& args) { process->start(program, args); }

void CShotDocState::stop() {
  stopped = true;
  if (QProcess::NotRunning != process->state()) {
    process->terminate();
    if (!process->waitForFinished(kStopTimeoutMs)) {
      process->kill();
      process->waitForFinished(kStopTimeoutMs);
    }
  }
  if (!socket.isNull()) {
    socket->disconnect(this);
    socket->deleteLater();
  }
}

void CShotDocState::attach(QLocalSocket* incoming) {
  if (!socket.isNull()) {
    socket->deleteLater();
  }
  socket = incoming;
  incoming->setParent(this);
  connect(incoming, &QLocalSocket::disconnected, incoming, &QObject::deleteLater);
  connect(incoming, &QLocalSocket::readyRead, this, [this, incoming]() {
    while (incoming == socket && incoming->canReadLine()) {
      const QString& line = QString::fromUtf8(incoming->readLine()).trimmed();
      QTimer::singleShot(0, this, [this, line]() {
        if (!stopped) {
          handle(line);
        }
      });
    }
  });
}

bool CShotDocState::send(const QString& line) {
  if (stopped || socket.isNull() || QLocalSocket::ConnectedState != socket->state()) {
    return false;
  }
  socket->write(line.toUtf8() + '\n');
  socket->flush();
  return true;
}

void CShotDocState::handle(const QString& line) {
  const qsizetype split = line.indexOf(' ');
  const QString& what = (split < 0) ? line : line.left(split);
  const QString& rest = (split < 0) ? QString() : line.mid(split + 1);

  if ("ready" == what) {
    ready = true;
    const QString& next = std::exchange(followUp, QString());
    emit becameReady(rest);
    if (!next.isEmpty()) {
      send(next);
    }
    return;
  }
  if ("recording" == what) {
    recording = true;
  } else if (what.startsWith("recorded")) {
    recording = false;
  }
  emit reported(what, rest);
}
