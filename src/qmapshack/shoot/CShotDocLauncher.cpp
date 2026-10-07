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

#include "shoot/CShotDocLauncher.h"

#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QProcess>
#include <QProxyStyle>
#include <QPushButton>
#include <QScreen>
#include <QTemporaryDir>
#include <QTimer>
#include <QVBoxLayout>
#include <cstdlib>
#include <utility>

#include "CMainWindow.h"
#include "setup/CAppOpts.h"
#include "setup/IAppSetup.h"
#include "shoot/CShotDocPanel.h"
#include "shoot/CShotPage.h"
#include "shoot/CShotsJob.h"

namespace {
/** How long `compose` gets [ms]; it takes some 30 ms, a first Python start on a cold machine far longer. */
constexpr qint32 kComposeTimeoutMs = 30000;
/** How long endSession() waits for the event loop to return before exiting [ms]. */
constexpr qint32 kEndTimeoutMs = 2000;

const QString kNoPython =
    "No Python interpreter was found. Start the session with shots.py take, which passes its own as --doc-python.";

/** @return --doc-python, else python3 or python from PATH, else empty */
QString python() {
  // On Windows `python3` on PATH is usually the store's app execution alias.
  if (!qlOpts->doc.docPython.isEmpty() && QFileInfo::exists(qlOpts->doc.docPython)) {
    return qlOpts->doc.docPython;
  }
  for (const QString& name : {QStringLiteral("python3"), QStringLiteral("python")}) {
    const QString& path = IAppSetup::getPlatformInstance()->findExecutable(name);
    if (!path.isEmpty()) {
      return path;
    }
  }
  return QString();
}

QString label(const QString& scenario) { return scenario.isEmpty() ? CShotFiles::kBaseLabel : scenario; }
}  // namespace

CShotDocLauncher::CShotDocLauncher(const QDir& repo, const QString& page, QObject* parent)
    : QObject(parent), repo(repo), page(page), files(repo, page), scratch(new QTemporaryDir) {}

CShotDocLauncher::~CShotDocLauncher() {
  // The state and a running job are children and go with this object.
  stopState();
  delete scratch;
}

void CShotDocLauncher::start() {
  // Without it composed configurations and publish reports would land in the working directory.
  if (!scratch->isValid()) {
    QMessageBox::critical(nullptr, "Documentation mode",
                          QString("No temporary directory for the session: %1").arg(scratch->errorString()));
    QTimer::singleShot(0, this, [this]() { endSession(); });
    return;
  }
  panel = new CShotDocPanel(page, files.placementFile(), nullptr);
  panel->setPickedHandler([this](const QString& id) { showShot(id); });
  panel->setScenarioPickedHandler([this](const QString& name) {
    selectedScenario = name;
    // A shot taken in another scenario is not what this one is for; one without a shot is taken anywhere.
    if (const QJsonObject& shot = files.shot(selectedShot); !shot.isEmpty() && shot["scenario"].toString() != name) {
      selectedShot.clear();
    }
    enterScenario(name);
  });
  panel->setRecordHandler([this]() {
    if (!state.isNull() && state->isRecording()) {
      command("stop");
      return;
    }
    recordingName = askRecordingName();
    if (recordingName.isEmpty()) {
      return;
    }
    // A recording always starts from the base, so it stores a whole state.
    refreshPanel("A recording starts from the base, so the application starts again there.");
    enterScenario(QString(), "record");
  });
  panel->setRenameHandler([this]() { renameScenario(); });
  panel->setDeleteScenarioHandler([this]() { deleteScenario(); });
  panel->setStoreConfigHandler([this]() { storeConfig(); });
  panel->setCopyBaseHandler([this]() {
    if (chooseBase(false)) {
      selectedScenario.clear();
      enterScenario(QString());
      refreshPanel(QString("%1 has a new base.").arg(page));
    }
  });
  panel->setRebindHandler([this](const QString& id, const QString& scenario) { rebindShot(id, scenario); });
  panel->setRetakeShotHandler([this](const QString& id) { retakeShot(id); });
  panel->setTakeRegionHandler([this](const QString& id) { actOnShot("region", id); });
  panel->setResetShotHandler([this](const QString& id) { revertShot(id); });
  panel->setRetakePageHandler([this]() { retakePage(); });
  panel->setReapHandler([this]() { reapUnused(); });
  panel->setPublishShotHandler([this](const QString& id) { publishPictures(false, id); });
  panel->setPublishHandler([this]() { publishPictures(false, page + "/*"); });
  panel->setCloseRequestHandler([this]() { return mayClose(); });
  panel->setClosedHandler([this]() {
    qDebug() << "doc: the panel was closed, the session ends";
    endSession();
  });
  panel->show();

  connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() { stopState(); });
  // SIGTERM closes the main window, which is never shown here.
  if (!CMainWindow::isNull()) {
    CMainWindow::self().installEventFilter(this);
  }

  // One server for the session; each state process connects to the same name.
  server = new QLocalServer(this);
  QLocalServer::removeServer(channelName());
  if (!server->listen(channelName())) {
    reportFailure(QString("No channel to the state process: %1").arg(server->errorString()));
  }
  connect(server, &QLocalServer::newConnection, this, [this]() {
    while (QLocalSocket* incoming = server->nextPendingConnection()) {
      // A replaced state is stopped and waited for before the next starts, so a connection is the current state's.
      if (state.isNull()) {
        incoming->deleteLater();
      } else {
        state->attach(incoming);
      }
    }
  });

  // Queued: start() runs before the event loop, and endSession()'s exit() ends only a running one.
  QTimer::singleShot(0, this, [this]() {
    // A page owns its base from the start; which one it begins with is the writer's choice.
    if (!QFileInfo::exists(files.baseFile()) && !chooseBase(true)) {
      endSession();
      return;
    }
    // Where the writer puts what the page's fixture differs in.
    if (const QString& error = files.makeFixtureDir(); !error.isEmpty()) {
      reportFailure(error);
    }
    watchPage();
    refreshPanel();
    enterScenario(QString());
  });
}

bool CShotDocLauncher::chooseBase(bool creating) {
  const QList<QPair<QString, QString>>& bases = files.bases();
  QStringList labels;
  for (const QPair<QString, QString>& base : bases) {
    labels << base.first;
  }
  bool ok = false;
  const QString& text = creating ? QString("%1 has no base yet. Which one does it start from?").arg(page)
                                 : QString("Replace the base of %1 with:").arg(page);
  const QString& picked = QInputDialog::getItem(panel, "Base of the page", text, labels, 0, false, &ok);
  if (!ok) {
    return false;
  }
  if (!creating &&
      QMessageBox::Yes != QMessageBox::question(panel, "Base of the page",
                                                QString("Every picture of %1 taken in (base) changes. Replace its base "
                                                        "with %2?")
                                                    .arg(page, picked),
                                                QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
    return false;
  }
  if (const QString& error = files.copyBase(bases.value(labels.indexOf(picked)).second); !error.isEmpty()) {
    reportFailure(error);
    return false;
  }
  return true;
}

bool CShotDocLauncher::eventFilter(QObject* watched, QEvent* event) {
  if (QEvent::Close == event->type() && !CMainWindow::isNull() && watched == &CMainWindow::self()) {
    qDebug() << "doc: the launcher's main window was closed, the session ends";
    endSession();
  }
  return QObject::eventFilter(watched, event);
}

QString CShotDocLauncher::channelName() const { return QString("qms-doc-%1").arg(QCoreApplication::applicationPid()); }

// --- the state process ---------------------------------------------------------------------------

void CShotDocLauncher::enterScenario(const QString& scenario, const QString& followUp) {
  stopState();
  // Without the channel a state never reports ready and the busy box would never close.
  if (!server->isListening()) {
    reportFailure(QString("There is no channel to the application: %1").arg(server->errorString()));
    updateBusy();
    return;
  }

  QString error;
  const QString& config = composeConfig(scenario, error);
  if (config.isEmpty()) {
    reportFailure(QString("The configuration of %1 could not be composed.\n\n%2").arg(label(scenario), error));
    updateBusy();
    return;
  }
  startState(scenario, config, followUp, QString());
}

void CShotDocLauncher::tryRecording(const QString& name) {
  stopState();
  if (!server->isListening()) {
    reportFailure(QString("There is no channel to the application: %1").arg(server->errorString()));
    updateBusy();
    return;
  }
  // The trial replays against the settings the recording started from, which become the scenario's own.
  QString error;
  const QString& parked = files.trialConfig();
  const QString& config = composeConfig(QString(), error, parked);
  if (config.isEmpty()) {
    reportFailure(QString("The configuration of %1 could not be composed.\n\n%2").arg(name, error));
    updateBusy();
    return;
  }
  startState(name, config, QString(), name);
}

void CShotDocLauncher::startState(const QString& scenario, const QString& config, const QString& followUp,
                                  const QString& trial) {
  CShotDocState* next = new CShotDocState(scenario, followUp, this);
  state = next;
  connect(next, &CShotDocState::becameReady, this, &CShotDocLauncher::onStateReady);
  connect(next, &CShotDocState::reported, this, &CShotDocLauncher::onStateReport);
  connect(next, &CShotDocState::ended, this, &CShotDocLauncher::onStateEnded);
  updateBusy();
  next->start(QCoreApplication::applicationFilePath(),
              childArguments(config, trial.isEmpty() ? scenario : QString(), trial));
}

void CShotDocLauncher::stopState() {
  if (state.isNull()) {
    return;
  }
  CShotDocState* going = state;
  state = nullptr;
  going->stop();
  going->deleteLater();
}

void CShotDocLauncher::onStateReady(const QString& text) {
  updateBusy();
  refreshPanel(text);
  // Not on connect: the fixture is not loaded then.
  sendSelection();
}

void CShotDocLauncher::onStateReport(const QString& what, const QString& rest) {
  if ("status" == what) {
    panel->setStatus(rest);
  } else if ("tagged" == what) {
    selectedShot = rest;
    refreshPanel(QString("%1 taken.").arg(rest));
    sendSelection();
  } else if ("recording" == what) {
    updateBusy();
  } else if ("recorded-pending" == what) {
    updateBusy();
    // Named when Record was pressed.
    if (recordingName.isEmpty()) {
      command("discard");
      refreshPanel("The recording has no name and was thrown away; record again.");
    } else {
      command("name " + std::exchange(recordingName, QString()));
    }
  } else if ("recorded-none" == what) {
    recordingName.clear();
    updateBusy();
    refreshPanel("Nothing was recorded.");
  } else if ("recorded" == what) {
    selectedScenario = rest;
    // Parked, not stored: a state replays it and stores it only if it replays, then holds it.
    tryRecording(rest);
  } else if ("trial-failed" == what) {
    selectedScenario.clear();
    reportFailure(rest);
    enterScenario(QString());
  } else {
    qWarning() << "doc: unknown report from the state process:" << what << rest;
  }
}

void CShotDocLauncher::onStateEnded(CShotDocState::end_e how, const QString& detail) {
  // Only the current state emits: a replaced one was stopped.
  if (!state.isNull()) {
    state->deleteLater();
    state = nullptr;
  }
  updateBusy();

  switch (how) {
    case CShotDocState::eClosed:
      qDebug() << "doc: the application window was closed, the session ends";
      endSession();
      break;
    case CShotDocState::eDied:
      reportFailure(QString("The application ended unexpectedly, %1. Pick a scenario to start it again.").arg(detail));
      break;
    case CShotDocState::eFailedToStart:
      reportFailure(QString("The application could not be started.\n\n%1").arg(detail));
      break;
  }
}

void CShotDocLauncher::endSession() {
  if (ending) {
    return;
  }
  ending = true;
  stopState();
  // Later: this may run inside a job's own finished().
  for (CShotsJob* job : {replayJob.data(), publishJob.data()}) {
    if (nullptr != job) {
      job->deleteLater();
    }
  }

  QApplication::closeAllWindows();
  qApp->exit(0);

  // The writer is owed their shell back more than this process a tidy end.
  QTimer::singleShot(kEndTimeoutMs, qApp, []() {
    qWarning() << "doc: the session did not end by itself";
    // No static destructors under a live QApplication.
    std::_Exit(0);
  });
}

void CShotDocLauncher::command(const QString& line) {
  if (state.isNull() || !state->send(line)) {
    refreshPanel("The application is not running. Pick a scenario first.");
  }
}

void CShotDocLauncher::notify(const QString& line) {
  if (!state.isNull()) {
    state->send(line);
  }
}

void CShotDocLauncher::sendSelection() {
  notify(selectedShot.isEmpty() ? QString("select") : "select " + selectedShot);
}

void CShotDocLauncher::updateBusy() {
  if (nullptr == panel || ending) {
    return;
  }
  if (!publishJob.isNull()) {
    panel->setBusy(true, "Please wait, publishing");
  } else if (!replayJob.isNull()) {
    panel->setBusy(true, "Please wait, replaying");
  } else if (!state.isNull() && !state->isReady()) {
    panel->setBusy(true, QString("Please wait, starting the application in %1").arg(label(state->scenario())));
  } else {
    panel->setBusy(false, QString());
  }
  panel->setRecording(!state.isNull() && state->isRecording());
}

QString CShotDocLauncher::composeConfig(const QString& scenario, QString& error, const QString& source) {
  const QString& interpreter = python();
  if (interpreter.isEmpty()) {
    error = kNoPython;
    return QString();
  }

  const QString& out = QDir(scratch->path()).absoluteFilePath("state.ini");
  QFile::remove(out);
  QStringList args{repo.absoluteFilePath("doc/tools/shots.py"), "compose", page, "--out", out};
  if (!scenario.isEmpty()) {
    args << "--scenario" << scenario;
  }
  if (!source.isEmpty()) {
    args << "--from" << source;
  }

  QProcess compose;
  compose.start(interpreter, args);
  if (!compose.waitForFinished(kComposeTimeoutMs)) {
    error = QString("%1 did not answer: %2").arg(interpreter, compose.errorString());
    compose.kill();
    return QString();
  }
  if (QProcess::NormalExit != compose.exitStatus() || 0 != compose.exitCode()) {
    error = QString("%1 %2 failed:\n%3")
                .arg(interpreter, args.join(' '), QString::fromUtf8(compose.readAllStandardError()).trimmed());
    return QString();
  }
  if (!QFileInfo::exists(out)) {
    error = QString("%1 wrote no %2").arg(interpreter, out);
    return QString();
  }
  return out;
}

QStringList CShotDocLauncher::childArguments(const QString& config, const QString& scenario,
                                             const QString& trial) const {
  QStringList args = QCoreApplication::arguments();
  args.removeFirst();

  QStringList out;
  for (qsizetype i = 0; i < args.size(); i++) {
    const QString& arg = args.at(i);
    if ("-c" == arg || arg.startsWith("--config") || arg.startsWith("--doc-scenario") ||
        arg.startsWith("--doc-channel") || arg.startsWith("--doc-trial")) {
      // `--config x` carries its value in the next argument, `--config=x` does not.
      if (!arg.contains('=')) {
        i++;
      }
      continue;
    }
    out << arg;
  }

  // Qt removes -style from arguments(); CQmsStyle is a proxy with no name of its own.
  const QStyle* style = QApplication::style();
  if (const QProxyStyle* proxy = qobject_cast<const QProxyStyle*>(style); nullptr != proxy) {
    style = proxy->baseStyle();
  }
  if (nullptr != style && !style->name().isEmpty()) {
    out << "-style" << style->name();
  }
  out << "--config" << config;
  out << "--doc-scenario" << (scenario.isEmpty() ? CShotPage::kBaseScenario : scenario);
  out << "--doc-channel" << channelName();
  if (!trial.isEmpty()) {
    out << "--doc-trial" << trial;
  }
  return out;
}

// --- what the panel asks for ---------------------------------------------------------------------

void CShotDocLauncher::showShot(const QString& id) {
  selectedShot = id;
  const QJsonObject& shot = files.shot(id);
  if (shot.isEmpty()) {
    refreshPanel(QString("%1 has no shot yet. Take it in the running scenario with Ctrl+Shift+F9.").arg(id));
    sendSelection();
    return;
  }

  selectedScenario = shot["scenario"].toString();
  if (!state.isNull() && state->scenario() == selectedScenario) {
    refreshPanel(QString("The application is in %1. Point at what to photograph and press Ctrl+Shift+F9.")
                     .arg(label(selectedScenario)));
    sendSelection();
    return;
  }
  enterScenario(selectedScenario);
}

void CShotDocLauncher::actOnShot(const QString& verb, const QString& id) {
  // A picture without a shot is taken in the base.
  const QString& scenario = files.shot(id)["scenario"].toString();
  selectedShot = id;
  if (!state.isNull() && state->isReady() && state->scenario() == scenario) {
    command(verb + " " + id);
    return;
  }
  selectedScenario = scenario;
  enterScenario(scenario, verb + " " + id);
}

QString CShotDocLauncher::askRecordingName() {
  QDialog dialog(panel);
  dialog.setWindowTitle("Record a scenario");
  QVBoxLayout* layout = new QVBoxLayout(&dialog);
  layout->addWidget(new QLabel("What is this scenario called?", &dialog));
  QLineEdit* edit = new QLineEdit(selectedScenario, &dialog);
  layout->addWidget(edit);
  QLabel* problem = new QLabel(&dialog);
  problem->setWordWrap(true);
  layout->addWidget(problem);
  QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
  layout->addWidget(buttons);
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  const auto check = [this, edit, problem, buttons]() {
    const QString& text = edit->text();
    QString why = CShotFiles::nameProblem(text);
    if (const QString& twin = files.caseTwinOf(text); why.isEmpty() && !twin.isEmpty()) {
      why = QString("%1 differs from the scenario %2 only in case.").arg(text, twin);
    }
    // An empty field is not an error yet, only not a name.
    problem->setText(text.isEmpty() ? QString() : why);
    buttons->button(QDialogButtonBox::Ok)->setEnabled(why.isEmpty());
  };
  connect(edit, &QLineEdit::textChanged, &dialog, check);
  check();

  while (QDialog::Accepted == dialog.exec()) {
    const QString& name = edit->text();
    if (!files.scenarioNames().contains(name) ||
        QMessageBox::Yes == QMessageBox::question(panel, "Record a scenario",
                                                  QString("%1 exists. Record it again and replace it?").arg(name),
                                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
      return name;
    }
  }
  return QString();
}

void CShotDocLauncher::storeConfig() {
  if (state.isNull()) {
    refreshPanel("The application is not running. Pick a scenario first.");
    return;
  }
  // A configuration stored after a scenario ran holds what its steps did; a replay would do them again on top.
  if (!state->scenario().isEmpty()) {
    refreshPanel(
        QString("%1 keeps the settings it was recorded with. Record it again to change them.").arg(state->scenario()));
    return;
  }
  if (QMessageBox::Yes ==
      QMessageBox::question(panel, "Save as base",
                            "This page's base is what it opens on and what a new recording starts from. Scenarios "
                            "keep the settings they were recorded with.\n\nSave the arrangement, size and settings "
                            "on screen as this page's base? Paths on this machine are left out.",
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
    command("update " + CShotPage::kBaseScenario);
  }
}

void CShotDocLauncher::renameScenario() {
  if (selectedScenario.isEmpty()) {
    refreshPanel("Select a scenario first.");
    return;
  }
  bool ok = false;
  const QString& to =
      QInputDialog::getText(panel, "Rename scenario", "New name:", QLineEdit::Normal, selectedScenario, &ok);
  if (!ok || to == selectedScenario) {
    return;
  }
  if (const QString& error = files.renameScenario(selectedScenario, to); !error.isEmpty()) {
    refreshPanel(error);
    return;
  }
  // A running state in the old name is the same state under the new one.
  const bool live = !state.isNull() && state->scenario() == selectedScenario;
  selectedScenario = to;
  if (live) {
    enterScenario(to);
  } else {
    notify("sync");
  }
  refreshPanel(QString("Renamed to %1. No picture changed.").arg(to));
}

void CShotDocLauncher::deleteScenario() {
  if (selectedScenario.isEmpty()) {
    refreshPanel("Select a scenario first.");
    return;
  }
  const QString name = selectedScenario;
  const QStringList& affected = files.shotsIn(name);
  const QString& question =
      affected.isEmpty() ? QString("Delete the scenario %1?").arg(name)
                         : QString(
                               "Delete the scenario %1?\n\nThese pictures are taken in it. They lose their picture "
                               "and how they were taken, and have to be taken again:\n\n%2")
                               .arg(name, affected.join("\n"));
  if (QMessageBox::Yes !=
      QMessageBox::question(panel, "Delete scenario", question, QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
    return;
  }
  if (const QString& error = files.deleteScenario(name); !error.isEmpty()) {
    reportFailure(error);
    return;
  }
  selectedScenario.clear();
  refreshPanel(QString("%1 deleted, %2 picture(s) have to be taken again.").arg(name).arg(affected.size()));
  // Nothing describes the state on screen any more.
  if (!state.isNull() && state->scenario() == name) {
    enterScenario(QString());
  } else {
    notify("sync");
  }
}

void CShotDocLauncher::rebindShot(const QString& id, const QString& scenario) {
  const QString& was = files.shot(id)["scenario"].toString();
  if (was == scenario) {
    return;
  }
  if (files.rebindLoses(id) &&
      QMessageBox::Yes != QMessageBox::question(panel, "Take it in another scenario",
                                                QString("%1 is taken in %2. Its picture and how it was taken are "
                                                        "thrown away and it has to be taken again.\n\nGo on?")
                                                    .arg(id, label(was)),
                                                QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
    // Puts the row's combo box back.
    refreshPanel(QString("%1 is still taken in %2.").arg(id, label(was)));
    return;
  }
  if (const QString& error = files.rebindShot(id, scenario); !error.isEmpty()) {
    reportFailure(error);
    return;
  }
  notify("sync");
  // The shot is now what the scenario is selected for, and the application goes there.
  selectedShot = id;
  selectedScenario = scenario;
  enterScenario(scenario);
  refreshPanel(QString("%1 is taken in %2 now.").arg(id, label(scenario)));
}

void CShotDocLauncher::reapUnused() {
  const QStringList& unused = files.unusedShots();
  if (unused.isEmpty()) {
    refreshPanel("Every picture is used by a page.");
    return;
  }
  if (QMessageBox::Yes !=
      QMessageBox::question(
          panel, "Remove unused",
          QString("No page references these. Delete their shots and pictures?\n\n%1").arg(unused.join("\n")),
          QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
    return;
  }
  if (const QString& error = files.removeShots(unused); !error.isEmpty()) {
    reportFailure(error);
    return;
  }
  notify("sync");
  refreshPanel(QString("Removed %1 unused.").arg(unused.size()));
}

void CShotDocLauncher::revertShot(const QString& id) {
  if (const QString& error = files.revertShot(id); !error.isEmpty()) {
    refreshPanel(error);
    return;
  }
  refreshPanel(QFileInfo::exists(files.publishedImage(id))
                   ? QString("%1 is back to the published picture.").arg(id)
                   : QString("%1 has no picture again; none is published.").arg(id));
}

// --- shots.py ------------------------------------------------------------------------------------

bool CShotDocLauncher::runShots(QPointer<CShotsJob>& slot, const QStringList& args,
                                const std::function<void(bool, const QString&)>& done) {
  if (!replayJob.isNull() || !publishJob.isNull()) {
    return false;
  }
  const QString& interpreter = python();
  // `--binary` is shots.py's global option and comes before the command.
  const QStringList& all =
      QStringList{repo.absoluteFilePath("doc/tools/shots.py"), "--binary", QCoreApplication::applicationFilePath()} +
      args;
  CShotsJob* job = new CShotsJob(interpreter, all, kNoPython, this);
  slot = job;
  QPointer<CShotsJob>* running = &slot;
  connect(job, &CShotsJob::finished, this, [this, running, done](bool ok, const QString& error) {
    // Cleared here, not by the job's own deleteLater(), so what is derived from it is current in done().
    *running = nullptr;
    updateBusy();
    done(ok, error);
  });
  updateBusy();
  return true;
}

void CShotDocLauncher::retakeShot(const QString& id) {
  // Headless, the way a build renders it; a picture that does not come out keeps the work picture.
  const bool started =
      runShots(replayJob, {"-o", repo.absoluteFilePath("doc/images/_work"), "replay", "--only", id},
               [this, id](bool ok, const QString& error) {
                 refreshPanel(ok ? QString("%1 was taken again; Publish puts it into the "
                                           "documentation.")
                                       .arg(id)
                                 : QString("%1 was not taken again; the console says why. %2").arg(id, error));
               });
  if (!started) {
    refreshPanel("Something is running already; wait for it.");
  }
}

void CShotDocLauncher::retakePage() {
  // Headless into _work, like Take again for every row: each is judged side by side and published or reverted.
  runShots(replayJob, {"-o", repo.absoluteFilePath("doc/images/_work"), "replay", "--only", page + "/*"},
           [this](bool ok, const QString& error) {
             refreshPanel(ok ? QString("Every picture of this page was taken again; compare them, revert what is not "
                                       "better, then Publish all.")
                             : QString("Some pictures do not replay any more; the console says which step fails. "
                                       "The others were taken again. %1")
                                   .arg(error));
           });
}

void CShotDocLauncher::watchPage() {
  if (nullptr == watcher) {
    watcher = new QFileSystemWatcher(this);
    QTimer* settle = new QTimer(this);
    settle->setSingleShot(true);
    // An editor saves in several steps; one refresh after the last.
    settle->setInterval(300);
    connect(settle, &QTimer::timeout, this, [this]() {
      watchPage();
      refreshPanel(QString());
    });
    connect(watcher, &QFileSystemWatcher::fileChanged, settle, qOverload<>(&QTimer::start));
    connect(watcher, &QFileSystemWatcher::directoryChanged, settle, qOverload<>(&QTimer::start));
  }
  // A part filled or emptied changes where the page's pictures take it from.
  for (const QString& folder : files.fixtureFolders()) {
    if (QFileInfo::exists(folder) && !watcher->directories().contains(folder)) {
      watcher->addPath(folder);
    }
  }
  // A file saved by rename is a new file the watcher lost; its directory sees it appear.
  for (const QString& file : {files.pageFile(), files.shotFile()}) {
    for (const QString& path : {file, QFileInfo(file).absolutePath()}) {
      if (QFileInfo::exists(path) && !watcher->files().contains(path) && !watcher->directories().contains(path)) {
        watcher->addPath(path);
      }
    }
  }
}

void CShotDocLauncher::publishPictures(bool thenEnd, const QString& only) {
  const QString& report = QDir(scratch->path()).absoluteFilePath("publish.json");
  QFile::remove(report);
  QStringList args{"publish", "--report", report};
  if (!only.isEmpty()) {
    args << "--only" << only;
  }
  runShots(publishJob, args, [this, report, thenEnd](bool ok, const QString& error) {
    if (!ok) {
      // A failed publish does not end the session: the next close asks again.
      reportFailure(QString("The pictures could not be published.\n\n%1").arg(error));
      return;
    }
    QFile in(report);
    const qsizetype published =
        in.open(QIODevice::ReadOnly) ? QJsonDocument::fromJson(in.readAll()).object()["published"].toArray().size() : 0;
    refreshPanel(0 == published ? QString("Nothing was taken again, nothing to publish.")
                                : QString("%1 picture(s) published and ready to commit.").arg(published));
    if (thenEnd) {
      endSession();
    }
  });
}

bool CShotDocLauncher::mayClose() {
  // The session is ending already; work pictures stay in _work for the next one.
  if (ending) {
    return true;
  }
  if (!publishJob.isNull()) {
    return false;
  }
  if (!files.hasUnpublishedImages()) {
    return true;
  }
  const QMessageBox::StandardButton answer = QMessageBox::question(
      panel, "Publish before closing?", "There are pictures taken again and not published.\n\nPublish them now?",
      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
  if (QMessageBox::No == answer) {
    return true;
  }
  if (QMessageBox::Yes == answer) {
    publishPictures(true);
  }
  return false;
}

// --- the panel -----------------------------------------------------------------------------------

void CShotDocLauncher::reportFailure(const QString& text) {
  qWarning().noquote() << "doc:" << text;
  refreshPanel(text);
  if (nullptr == panel) {
    return;
  }
  // Queued: reached from start() too, before the application's event loop runs.
  QTimer::singleShot(0, panel, [this, text]() { QMessageBox::warning(panel, "Documentation mode", text); });
}

void CShotDocLauncher::refreshPanel(const QString& status) {
  if (nullptr == panel) {
    return;
  }
  const bool hasPage = QFileInfo::exists(files.pageFile());
  const QString& pageName = repo.relativeFilePath(files.pageFile());
  panel->setPage(pageName, hasPage);
  panel->setFixture(files.ownFixtureParts(), repo.relativeFilePath(files.fixtureDir()));
  panel->setScenarios(files.scenarioNames(), selectedScenario);
  const QList<CShotFiles::row_t>& rows = files.rows();
  panel->setShots(rows);
  panel->setCurrentShot(selectedShot);

  // Shown before anything else: every list above reads as empty then.
  if (const QString& problem = files.shotFileProblem(); !problem.isEmpty()) {
    panel->setStatus(problem);
  } else if (!status.isEmpty()) {
    panel->setStatus(status);
  } else if (!hasPage) {
    panel->setStatus(QString("Write %1 first; its image lines name the pictures.").arg(pageName));
  } else if (rows.isEmpty()) {
    panel->setStatus(QString("%1 references no picture yet.").arg(pageName));
  }
}
