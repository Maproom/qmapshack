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

#ifndef CSHOTDOCSTATE_H
#define CSHOTDOCSTATE_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>

class QLocalSocket;
class QProcess;

/**
   @brief The launcher's handle on one state process: the process, its channel and what it was started for.

   Everything that belongs to one start lives and dies here, so replacing a state leaves nothing behind to reset. After
   stop() the object emits nothing, also for reports already queued; the launcher then deletes it later.
 */
class CShotDocState : public QObject {
  Q_OBJECT
 public:
  enum end_e {
    eClosed,        ///< exited with 0: the writer closed the application window
    eDied,          ///< crashed or exited with another code
    eFailedToStart  ///< never ran
  };

  /**
     @param scenario  empty is `(base)`
     @param followUp  one command sent after the first `ready`, empty for none
   */
  CShotDocState(const QString& scenario, const QString& followUp, QObject* parent);
  /** @brief stop() */
  ~CShotDocState() override;

  void start(const QString& program, const QStringList& args);

  /** @brief End the process and wait for it; nothing is emitted afterwards. */
  void stop();

  /** @brief Take the channel the process connected on; a second one replaces the first. */
  void attach(QLocalSocket* socket);

  /** @return false when no channel is connected */
  bool send(const QString& line);

  const QString& scenario() const { return scenarioName; }
  bool isReady() const { return ready; }
  /** @return true between the state's `recording` and its `recorded`, `recorded-pending` or `recorded-none` */
  bool isRecording() const { return recording; }

 signals:
  /** @brief `ready`; the follow-up is sent right after. */
  void becameReady(const QString& text);
  /** @brief Every report but `ready`. */
  void reported(const QString& what, const QString& rest);
  void ended(CShotDocState::end_e how, const QString& detail);

 private:
  void handle(const QString& line);

  QString scenarioName;
  QString followUp;
  QProcess* process = nullptr;
  QPointer<QLocalSocket> socket;
  bool ready = false;
  bool recording = false;
  bool stopped = false;
};

#endif  // CSHOTDOCSTATE_H
