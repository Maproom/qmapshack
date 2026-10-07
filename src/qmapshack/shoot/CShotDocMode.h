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

#ifndef CSHOTDOCMODE_H
#define CSHOTDOCMODE_H

#include <QByteArray>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <memory>

#include "shoot/CShotFiles.h"

class CMainWindow;
class CShotContext;
class CShotRecorder;
class CShotWriter;
class QLocalSocket;
class QMenu;
class QTimer;
class QVariant;
class QWidget;

/**
   @brief The state process: one scenario, put up once, held, and thrown away with the process.

   It loads the fixture, replays its scenario - or a parked recording on trial - and reports `ready` to the launcher.
   Ctrl+Shift+F9 photographs what the writer points at; the launcher's commands take a region, take a picture again,
   record, and store the settings. It ends when its window is closed or the channel drops.
 */
class CShotDocMode : public QObject {
  Q_OBJECT
 public:
  /**
     @param scenario  CShotPage::kBaseScenario for `(base)`
     @param trial     the name a parked recording is stored under once it replays, empty for none
   */
  CShotDocMode(const QDir& repo, const QString& page, const QString& scenario, const QString& trial, QObject* parent);
  virtual ~CShotDocMode();

  /** @brief Connect the channel and set the scenario up once the main window has settled. */
  void start();

  /**
     @brief QWidget::saveGeometry() without the screen and its width.

     restoreGeometry() drops the whole record when the screen width differs by more than a quarter; a width of 0 takes
     the branch that only rejects a window wider than one and a half screens.

     @param known  out: false when the record is not magic 0x1D9D0CB version 3 and came back untouched
   */
  static QByteArray portableGeometry(const QByteArray& geometry, bool& known);

  /** @return true when @p value is an absolute path, or a list holding one; those exist on this machine only */
  static bool namesAPlace(const QVariant& value);

  /**
     @brief What the writer sees @p w called: a dock's caption, its tab's text, a group box's title, a window's title.

     @return empty when nothing on screen names it; the caller falls back to the class name
   */
  static QString onScreenName(const QWidget* w);

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private slots:
  void slotSetUp();

 private:
  /** @brief End the process once: close the window, exit, and exit hard if the event loop does not return. */
  void leave(const QString& why);
  /** @brief One command from the launcher. */
  void obey(const QString& line);
  /** @brief One line to the launcher; logged when no channel is connected. */
  void send(const QString& line);
  /** @brief The launcher's status line. */
  void report(const QString& status);

  /** @brief Replay the parked recording and store it under `trial` if it replays; reports how it went. */
  void runTrial();
  /** @brief Move the window to where the writer left the last one, keeping its size. */
  void placeWindow();
  /** @brief Write the window's position for the next state process. */
  void storePlacement();

  /** @brief Ctrl+Shift+F9: photograph what the writer points at. */
  void tag();
  /** @brief Drag a rectangle over the window and store it as @p id's `rect`. */
  void takeRegion(const QString& id);
  /** @brief Save config: the arrangement, view and settings on screen as the base; a scenario keeps its own. */
  void updateScenario(const QString& target);
  /** @brief `record`: snapshot the settings the steps are recorded against, then run the recorder. */
  void startRecording();
  /** @brief `stop`: keep the steps until the launcher names them, or report that nothing was recorded. */
  void stopRecording();
  /** @brief `name`: park the stopped recording; the launcher starts its trial. */
  void parkRecording(const QString& name);

  /** @return the window the writer means: the popup, the modal dialog, the active window, the focus' window */
  static QWidget* activeTarget();
  /**
     @brief Every part of the main window the writer could mean, from what they point at outwards, then the window.

     @param labels  out: one label per part
   */
  QList<QWidget*> livePartsAt(CMainWindow* main, QStringList& labels) const;
  /** @return the part picked from @p parts, nullptr when the writer cancelled */
  QWidget* chooseLivePart(CMainWindow* main, const QList<QWidget*>& parts, const QStringList& labels) const;
  /** @return the picture the writer picks among what the page references, empty when cancelled */
  QString askForId() const;
  /** @return @p id's scenario: the shot's, or this process' own for a picture without a shot */
  QString scenarioFor(const QString& id) const;
  /** @return true when @p id may be taken here; otherwise the writer is told where */
  bool takenHere(const QString& id);
  /** @return true to keep @p id's work picture; false restores @p previous, or deletes it when that is empty */
  bool confirmResult(const QString& id, const QByteArray& previous) const;
  /** @return the work picture of @p id before it is taken again, empty when there is none */
  QByteArray previousPicture(const QString& id) const;
  /** @brief Tell the writer the line a developer has to add for an unexposed class. */
  void reportUnexposed(const QWidget* target) const;

  /**
     @brief Write the running settings to @p path, leaving out what names a place on this machine.

     @param dropped  out: how many keys were left out
     @return the number of keys written
   */
  qint32 storeSettings(const QString& path, qint32& dropped) const;
  /** @brief Remember the settings the state stands in; what the writer changes after this is drift. */
  void snapshotSetup();
  /** @return how many settings on screen differ from the ones snapshotSetup() kept, 0 without a snapshot */
  qint32 settingsDrift() const;
  /** @return the sentence appended to a status when the settings drifted, else empty */
  QString driftWarning(const QString& scenario) const;

  QDir repo;
  QString page;
  CShotFiles files;
  /** Empty is `(base)`; a trial becomes its scenario once it replays. */
  QString scenario;
  QString trial;
  /** The picture the writer has selected in the panel. */
  QString wantedShot;

  std::unique_ptr<CShotWriter> writer;
  std::unique_ptr<CShotContext> ctx;
  CShotRecorder* recorder = nullptr;
  /** What a stopped recording returned, until the launcher names it. */
  QJsonArray pendingSteps;
  /** The context menu shown last and the step that opens it; a picture of it keeps the step. */
  QPointer<QMenu> contextMenu;
  QJsonObject contextMenuOpen;
  /** Holds the settings the state was set up with; drift is measured against them. */
  QTemporaryDir setUpWith;
  QLocalSocket* channel = nullptr;
  /** Writes the window's position once a drag has stopped moving it. */
  QTimer* placementTimer = nullptr;
  bool leaving = false;
  /** F9 and the commands wait for the scenario to be set up. */
  bool ready = false;
  /** Against re-entering from the dialogs a picture opens. */
  bool busy = false;
};

#endif  // CSHOTDOCMODE_H
