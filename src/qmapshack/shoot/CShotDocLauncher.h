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

#ifndef CSHOTDOCLAUNCHER_H
#define CSHOTDOCLAUNCHER_H

#include <QDir>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <functional>

#include "shoot/CShotDocState.h"
#include "shoot/CShotFiles.h"

class CShotDocPanel;
class CShotsJob;
class QFileSystemWatcher;
class QJsonArray;
class QJsonObject;
class QLocalServer;
class QTemporaryDir;

/**
   @brief The writer's session: the panel, the state process, and every file operation but two.

   It never renders. What runs is held in self-clearing pointers - the state process (CShotDocState) and at most one
   `shots.py` job (CShotsJob) - and everything the panel shows about it, busy or recording, is derived from those. Page
   files change only through CShotFiles.
 */
class CShotDocLauncher : public QObject {
  Q_OBJECT
 public:
  CShotDocLauncher(const QDir& repo, const QString& page, QObject* parent);
  ~CShotDocLauncher() override;

  /** @brief Show the panel, listen on the channel and start the state process in `(base)`. */
  void start();

 protected:
  /** @brief Closing the hidden main window, which SIGTERM does, ends the session. */
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  /** @brief Replace the state process with one in @p scenario; @p followUp is sent once it is ready. */
  void enterScenario(const QString& scenario, const QString& followUp = QString());
  /** @brief Replace the state process with one in the base that replays the parked recording and stores it as @p name.
   */
  void tryRecording(const QString& name);
  /** @brief Compose the configuration and start a state process in @p scenario; @p trial names a parked recording. */
  void startState(const QString& scenario, const QString& followUp, const QString& trial);
  void stopState();
  void onStateReady(const QString& text);
  void onStateReport(const QString& what, const QString& rest);
  void onStateEnded(CShotDocState::end_e how, const QString& detail);

  /**
     @brief End the session: the state process, a running job, the windows, then this process.

     The windows first: a modal box runs an event loop of its own that exit() would end instead of the session.
   */
  void endSession();

  /** @brief Send @p line to the running state; says so on the panel when there is none. */
  void command(const QString& line);
  /** @brief Send @p line to the running state if there is one, quietly: housekeeping, not an action. */
  void notify(const QString& line);
  /** @return @p id's marks: its entry's, or the pending ones while it has no shot */
  QJsonArray marksOf(const QString& id) const;
  /** @brief Store @p marks as @p id's, refresh the panel with @p done and tell the state. */
  void storeMarks(const QString& id, const QJsonArray& marks, const QString& done);
  /** @brief Append @p mark, placed by pointing, to @p id's marks. */
  void addMark(const QString& id, const QJsonObject& mark);
  /** @brief Take the selected image's mark at @p index off. */
  void removeMark(qint32 index);
  /** @brief Tell the state the selected image and its marks; quiet when there is none. */
  void sendSelection();

  /** @brief Set the panel's busy box and recording mode from what runs. */
  void updateBusy();
  /** @brief Re-read the page into the panel. */
  void refreshPanel(const QString& status = QString());
  /** @brief Say it on the panel and in a box; a Windows build has no console. */
  void reportFailure(const QString& text);

  /**
     @brief Compose a run's configuration through `shots.py compose`.

     @param source  compose from this file instead of @p scenario's own or the base; a trial's parked settings
     @return the composed configuration, empty with @p error set when composing failed
   */
  QString composeConfig(const QString& scenario, QString& error, const QString& source = QString());
  /** @return this process' arguments with the launcher's switches replaced for a state in @p scenario */
  QStringList childArguments(const QString& config, const QString& scenario, const QString& trial) const;
  QString channelName() const;

  void showShot(const QString& id);
  /** @return the name a new recording is stored under, empty when the writer cancelled */
  QString askRecordingName();
  void storeConfig();
  /**
     @brief Let the writer pick a base to copy over the page's.

     @param creating  the page has none yet: no question before replacing
     @return true when the page's base was written
   */
  bool chooseBase(bool creating);
  void renameScenario();
  void deleteScenario();
  void rebindShot(const QString& id, const QString& scenario);
  void reapUnused();
  void revertShot(const QString& id);
  /** @brief Send @p verb for @p id to a state in the shot's own scenario, starting one when needed. */
  void actOnShot(const QString& verb, const QString& id);

  /** @brief Take @p id again headless, into `doc/<lang>/images/_work/`, the way a build renders it. */
  void retakeShot(const QString& id);
  /** @brief retakeShot() once the writer knows what the replay would miss. */
  void runRetakeShot(const QString& id);
  /** @brief Ask the state which settings on screen its replay would not show, then @p go, after asking the writer */
  void askDrift(const std::function<void()>& go);
  /** @brief The state's answer: @p keys the settings that differ from those @p name was set up with */
  void onDrift(const QString& name, const QString& keys);
  /** @brief `shots.py replay` of this page into `_work`: every image taken again, to compare and publish. */
  void retakePage();
  /** @brief retakePage() once the writer knows what the replay would miss. */
  void runRetakePage();
  /**
     @brief `shots.py publish`; with @p thenEnd the session ends once it succeeded.

     @param only  the ids to publish as a glob, empty for every image taken again
   */
  void publishImages(bool thenEnd, const QString& only = QString());
  /** @brief Watch the page and its shot file, so an edit made outside shows in the panel */
  void watchPage();
  /**
     @brief Start `shots.py` with @p args into @p slot.

     @return false when a job runs already; otherwise @p done gets the outcome exactly once
   */
  bool runShots(QPointer<CShotsJob>& slot, const QStringList& args,
                const std::function<void(bool, const QString&)>& done);

  /** @brief Ask before closing over unpublished images: Publish, No, Cancel; refuse only while publishing. */
  bool mayClose();

  QDir repo;
  QString page;
  CShotFiles files;

  /** The writer's selection; a new image is taken in `selectedScenario`. */
  QString selectedScenario;
  QString selectedShot;
  /** What the running state said on entering its scenario, a failed replay included; empty without a state. */
  QString stateText;
  bool ending = false;

  CShotDocPanel* panel = nullptr;
  /** While the state process chooses a mark's target, Return and Escape go to it. */
  bool marking = false;
  /** What waits for the state's `drift` answer. */
  std::function<void()> afterDrift;
  /** The base is being saved: the next `drift` answer comes after the save. */
  bool savingBase = false;
  QLocalServer* server = nullptr;
  QFileSystemWatcher* watcher = nullptr;
  QTemporaryDir* scratch = nullptr;
  QPointer<CShotDocState> state;
  QPointer<CShotsJob> replayJob;
  QPointer<CShotsJob> publishJob;
};

#endif  // CSHOTDOCLAUNCHER_H
