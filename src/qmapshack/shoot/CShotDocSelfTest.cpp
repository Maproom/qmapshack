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

#include "shoot/CShotDocSelfTest.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QPointer>
#include <QTemporaryDir>
#include <QTimer>
#include <functional>

#include "shoot/CShotDocPanel.h"
#include "shoot/CShotDocState.h"
#include "shoot/CShotFiles.h"
#include "shoot/CShotsJob.h"

namespace {
qint32 cases = 0;
qint32 failures = 0;

/** A process outcome arrives well within this [ms]. */
constexpr qint32 kWaitMs = 20000;

void verdict(const QString& name, bool ok, const QString& detail = QString()) {
  cases++;
  failures += ok ? 0 : 1;
  qWarning().noquote() << (ok ? "shoot: PASS" : "shoot: FAIL") << name << (detail.isEmpty() ? "" : "| " + detail);
}

/** @brief Run the event loop until @p done or @p ms; processEvents() does not wait. */
void waitFor(const std::function<bool()>& done, qint32 ms = kWaitMs) {
  QEventLoop loop;
  QTimer deadline;
  deadline.setSingleShot(true);
  QObject::connect(&deadline, &QTimer::timeout, &loop, &QEventLoop::quit);
  QTimer poll;
  QObject::connect(&poll, &QTimer::timeout, &loop, [&]() {
    if (done()) {
      loop.quit();
    }
  });
  deadline.start(ms);
  poll.start(10);
  if (!done()) {
    loop.exec();
  }
}

void settle(qint32 ms) {
  waitFor([]() { return false; }, ms);
}

void write(const QString& path, const QByteArray& content) {
  QDir().mkpath(QFileInfo(path).absolutePath());
  QFile out(path);
  if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate) || out.write(content) != content.size()) {
    qWarning() << "shoot: the self test cannot write" << path;
  }
}

QByteArray read(const QString& path) {
  QFile in(path);
  return in.open(QIODevice::ReadOnly) ? in.readAll() : QByteArray();
}

/**
   A scratch checkout. Page `p`: shots `p/a` (widget, scenario `s`, picture and work picture), `p/b` (bare, scenario
   `s`), `p/c` (widget, base, picture), `p/d` (bare, base, no picture), `p/e` (used only by page `q`), `p/f` (used by
   no page, picture). The page `p` also references `p/g` (picture, no shot) and `p/h` (nothing). Scenario `s` has a
   configuration, scenario `t` none.
 */
struct checkout_t {
  QTemporaryDir dir;
  QDir repo;
  CShotFiles files;

  checkout_t() : repo(dir.path()), files(repo, "p") {
    const QJsonObject content{
        {"scenarios", QJsonObject{{"s", QJsonArray{QJsonObject{{"do", "view"}}}}, {"t", QJsonArray()}}},
        {"shots", QJsonArray{QJsonObject{{"id", "p/a"}, {"scenario", "s"}, {"widget", ""}},
                             QJsonObject{{"id", "p/b"}, {"scenario", "s"}},
                             QJsonObject{{"id", "p/c"}, {"widget", "menuProject"}}, QJsonObject{{"id", "p/d"}},
                             QJsonObject{{"id", "p/e"}}, QJsonObject{{"id", "p/f"}}}}};
    write(files.shotFile(), QJsonDocument(content).toJson(QJsonDocument::Indented));
    write(files.pageFile(),
          "![](../images/p/a.png)\n![](../images/p/b.png)\n![](../images/p/c.png)\n"
          "![](../images/p/d.png)\n![](../images/p/g.png)\n![](../images/p/h.png)\n");
    write(repo.absoluteFilePath("doc/pages/q.md"), "![](../images/p/e.png)\n");
    write(files.scenarioConfig("s"), "[General]\n");
    for (const char* id : {"p/a", "p/c", "p/f", "p/g"}) {
      write(files.publishedImage(id), "png");
    }
    write(files.workImage("p/a"), "png");
  }

  QJsonObject content() const { return QJsonDocument::fromJson(read(files.shotFile())).object(); }
};

void testNames() {
  const QStringList& good = {"zoomed", "units dialog 2", "a.b"};
  const QStringList& bad = {"", "-", "(base)", "-zoomed", "a/b", "a\\b", "a:b", "a*b", "a?b", " a", "a ", "a."};
  QStringList wrong;
  for (const QString& name : good) {
    if (!CShotFiles::nameProblem(name).isEmpty()) {
      wrong << "refused " + name;
    }
  }
  for (const QString& name : bad) {
    if (CShotFiles::nameProblem(name).isEmpty()) {
      wrong << "accepted '" + name + "'";
    }
  }
  verdict("scenario names: file names only, no option, not reserved", wrong.isEmpty(), wrong.join(", "));
}

void testRename() {
  {
    checkout_t c;
    const QString& error = c.files.renameScenario("s", "zoomed");
    const QJsonObject& content = c.content();
    const bool ok = error.isEmpty() && content["scenarios"].toObject().contains("zoomed") &&
                    !content["scenarios"].toObject().contains("s") && c.files.shotsIn("zoomed").size() == 2 &&
                    QFileInfo::exists(c.files.scenarioConfig("zoomed")) &&
                    !QFileInfo::exists(c.files.scenarioConfig("s"));
    verdict("rename moves the scenario, its shots' references and its configuration", ok, error);
  }
  {
    checkout_t c;
    const QString& error = c.files.renameScenario("s", "S");
    const bool ok = error.isEmpty() && c.files.scenarioNames().contains("S") &&
                    QFile(c.files.scenarioConfig("S")).exists() && c.files.shotsIn("S").size() == 2;
    verdict("rename that changes only the case", ok, error);
  }
  for (const QString& to : {QString("t"), QString("a/b"), QString("(base)")}) {
    checkout_t c;
    const QByteArray& before = read(c.files.shotFile());
    const QString& error = c.files.renameScenario("s", to);
    const bool ok =
        !error.isEmpty() && read(c.files.shotFile()) == before && QFileInfo::exists(c.files.scenarioConfig("s"));
    verdict(QString("rename to '%1' is refused and changes nothing").arg(to), ok, error);
  }
  {
    checkout_t c;
    // The shot file cannot be written, the configuration can be moved.
    const QString& shots = c.repo.absoluteFilePath("doc/shots");
    QFile::setPermissions(shots, QFileDevice::ReadOwner | QFileDevice::ExeOwner);
    const bool locked = !QFile(QDir(shots).absoluteFilePath("probe")).open(QIODevice::WriteOnly);
    const QByteArray& before = read(c.files.shotFile());
    const QString& error = c.files.renameScenario("s", "zoomed");
    const bool ok = !error.isEmpty() && read(c.files.shotFile()) == before &&
                    QFileInfo::exists(c.files.scenarioConfig("s")) &&
                    !QFileInfo::exists(c.files.scenarioConfig("zoomed"));
    QFile::setPermissions(shots, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
    if (locked) {
      verdict("a rename whose shot file cannot be written moves the configuration back", ok, error);
    } else {
      qWarning() << "shoot: SKIP a rename whose shot file cannot be written: the directory could not be locked";
    }
  }
  {
    checkout_t c;
    write(c.files.shotFile(), "not json");
    const QString& error = c.files.renameScenario("s", "zoomed");
    const bool ok =
        !error.isEmpty() && read(c.files.shotFile()) == "not json" && QFileInfo::exists(c.files.scenarioConfig("s"));
    verdict("a broken shot file is refused, never overwritten", ok, error);
  }
}

void testDelete() {
  checkout_t c;
  const QString& error = c.files.deleteScenario("s");
  const QJsonObject& content = c.content();
  const bool ok = error.isEmpty() && !content["scenarios"].toObject().contains("s") &&
                  c.files.shot("p/a") == QJsonObject{{"id", "p/a"}} &&
                  c.files.shot("p/b") == QJsonObject{{"id", "p/b"}} &&
                  !QFileInfo::exists(c.files.publishedImage("p/a")) && !QFileInfo::exists(c.files.workImage("p/a")) &&
                  !QFileInfo::exists(c.files.scenarioConfig("s")) && QFileInfo::exists(c.files.publishedImage("p/c"));
  verdict("delete reduces the scenario's shots to their id and takes their pictures and configuration", ok, error);
}

void testRebind() {
  {
    checkout_t c;
    QJsonObject content = c.content();
    QJsonArray shots = content["shots"].toArray();
    shots.append(QJsonObject{{"id", "p/n"}, {"scenario", "s"}, {"note", "only a note"}});
    content["shots"] = shots;
    write(c.files.shotFile(), QJsonDocument(content).toJson());
    const bool ok = c.files.rebindLoses("p/a") && c.files.rebindLoses("p/c") && c.files.rebindLoses("p/g") &&
                    c.files.rebindLoses("p/n") && !c.files.rebindLoses("p/b") && !c.files.rebindLoses("p/h");
    verdict("rebind loses something: any key but id and scenario, a picture, or a picture without a shot", ok);
  }
  {
    checkout_t c;
    const QString& error = c.files.rebindShot("p/c", "t");
    const bool ok = error.isEmpty() && c.files.shot("p/c") == QJsonObject{{"id", "p/c"}, {"scenario", "t"}} &&
                    !QFileInfo::exists(c.files.publishedImage("p/c"));
    verdict("rebind reduces the shot to id and scenario and deletes its picture", ok, error);
  }
  {
    checkout_t c;
    const QByteArray& before = read(c.files.shotFile());
    const QString& error = c.files.rebindShot("p/c", "nowhere");
    verdict("rebind to a scenario the page has not got is refused",
            !error.isEmpty() && read(c.files.shotFile()) == before, error);
  }
}

void testUnused() {
  checkout_t c;
  const QStringList& unused = c.files.unusedShots();
  verdict("unused: a shot another page references is used", unused == QStringList{"p/f"}, unused.join(", "));

  const QString& error = c.files.removeShots(unused);
  const bool ok = error.isEmpty() && c.files.shot("p/f").isEmpty() && !c.files.shot("p/e").isEmpty() &&
                  !QFileInfo::exists(c.files.publishedImage("p/f")) && c.files.scenarioNames().size() == 2;
  verdict("remove takes the shots and their pictures, the scenarios stay", ok, error);
}

void testUnicodeNames() {
  checkout_t c;
  write(c.repo.absoluteFilePath("doc/pages/q.md"),
        QString("![](../images/p/e.png)\n![](../images/p/f-größe.png)\n").toUtf8());
  QJsonObject content = c.content();
  QJsonArray shots = content["shots"].toArray();
  shots.append(QJsonObject{{"id", QString("p/f-größe")}});
  content["shots"] = shots;
  write(c.files.shotFile(), QJsonDocument(content).toJson());
  const QStringList& unused = c.files.unusedShots();
  verdict("a picture name beyond ASCII is a reference, as in shots.py", unused == QStringList{"p/f"},
          unused.join(", "));
}

void testForeignPicture() {
  // Page q references p/e, whose shot is in p.json.
  checkout_t c;
  CShotFiles q(c.repo, "q");
  QStringList listed;
  const QList<CShotFiles::row_t>& rows = q.rows();
  for (const CShotFiles::row_t& row : rows) {
    listed << row.id;
  }
  write(c.files.publishedImage("p/e"), "png");
  const QString& error = q.rebindShot("p/e", QString());
  const bool ok = listed.isEmpty() && !error.isEmpty() && QFileInfo::exists(c.files.publishedImage("p/e")) &&
                  !QFileInfo::exists(q.shotFile());
  verdict("another page's picture is not listed and cannot be rebound here", ok,
          QString("listed '%1', %2").arg(listed.join(", "), error));
}

void testBusyBox() {
  QTemporaryDir dir;
  CShotDocPanel panel("p", QDir(dir.path()).absoluteFilePath("panel.ini"), nullptr);
  panel.show();
  panel.setBusy(true, "wait");
  QMessageBox* box = panel.findChild<QMessageBox*>();
  const bool noButtons = nullptr != box && box->buttons().isEmpty();
  if (nullptr != box) {
    QKeyEvent escape(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QCoreApplication::sendEvent(box, &escape);
    box->close();
    settle(100);
  }
  const bool stays = nullptr != box && box->isVisible();
  panel.setBusy(false, QString());
  panel.hide();
  // Reaching this line is the other half: Escape on the old order crashed.
  verdict("the busy box has no button and ignores Escape and a close request", noButtons && stays);
}

void testRows() {
  checkout_t c;
  QStringList got;
  const QList<CShotFiles::row_t>& rows = c.files.rows();
  for (const CShotFiles::row_t& row : rows) {
    got << QString("%1:%2%3").arg(row.id).arg(row.state).arg(row.changed ? "*" : "");
  }
  // eTaken 0, eNoImage 1, eNotUsed 2, eMissing 3, eUnregistered 4
  const QStringList& want = {"p/a:0*", "p/b:1", "p/c:0", "p/d:1", "p/e:1", "p/f:2", "p/g:4", "p/h:3"};
  verdict("rows: shots in order, then the page's references without a shot", got == want, got.join(" "));

  checkout_t d;
  const QString& error = d.files.revertShot("p/a");
  verdict("revert deletes only the work picture",
          error.isEmpty() && !QFileInfo::exists(d.files.workImage("p/a")) &&
              QFileInfo::exists(d.files.publishedImage("p/a")),
          error);
}

void testJob() {
  const QString& app = QCoreApplication::applicationFilePath();
  const auto outcome = [](const QString& program, const QStringList& args) {
    qint32 count = 0;
    bool ok = false;
    QPointer<CShotsJob> job = new CShotsJob(program, args, "missing", nullptr);
    // Connected after the constructor returned: an outcome emitted inside it would be lost here.
    QObject::connect(job, &CShotsJob::finished, [&](bool result, const QString&) {
      count++;
      ok = result;
    });
    waitFor([&]() { return count > 0; });
    settle(200);
    return QString("%1 %2 %3").arg(count).arg(ok ? "ok" : "failed").arg(job.isNull() ? "deleted" : "alive");
  };
  QString got = outcome(QString(), {});
  verdict("job without a program: one failure, after the constructor, then deleted", got == "1 failed deleted", got);
  got = outcome("/nonexistent/program", {});
  verdict("job that cannot start: one failure, after the constructor, then deleted", got == "1 failed deleted", got);
  got = outcome(app, {"--help"});
  verdict("job that exits with 0: one success, then deleted", got == "1 ok deleted", got);
  got = outcome(app, {"--no-such-switch"});
  verdict("job that exits with another code: one failure, then deleted", got == "1 failed deleted", got);

  QString said;
  bool done = false;
  QPointer<CShotsJob> job = new CShotsJob(app, {"--no-such-switch"}, "missing", nullptr);
  QObject::connect(job, &CShotsJob::finished, [&](bool, const QString& error) {
    said = error;
    done = true;
  });
  waitFor([&]() { return done; });
  verdict("a failed job reports what the program wrote to standard error", said.contains("no-such-switch"), said);
}

void testStateProcess() {
  const QString& app = QCoreApplication::applicationFilePath();
  const auto end = [](const QString& program, const QStringList& args) {
    CShotDocState state(QString(), "retake p/a", nullptr);
    qint32 count = 0;
    CShotDocState::end_e how = CShotDocState::eClosed;
    QObject::connect(&state, &CShotDocState::ended, [&](CShotDocState::end_e end, const QString&) {
      count++;
      how = end;
    });
    state.start(program, args);
    waitFor([&]() { return count > 0; });
    settle(200);
    return QString("%1x%2").arg(count).arg(how);
  };
  // eClosed 0, eDied 1, eFailedToStart 2
  QString got = end("/nonexistent/program", {});
  verdict("state that cannot start ends once as failed to start", got == "1x2", got);
  got = end(app, {"--help"});
  verdict("state that exits with 0 ends once as closed", got == "1x0", got);
  got = end(app, {"--no-such-switch"});
  verdict("state that exits with another code ends once as died", got == "1x1", got);

  // The channel, with this process as the state process.
  QLocalServer server;
  const QString& name = QString("qms-doc-selftest-%1").arg(QCoreApplication::applicationPid());
  QLocalServer::removeServer(name);
  if (!server.listen(name)) {
    verdict("the self test's channel listens", false, server.errorString());
    return;
  }
  const auto connectPair = [&](CShotDocState& state, QLocalSocket& client) {
    client.connectToServer(name);
    waitFor([&]() { return server.hasPendingConnections(); });
    QLocalSocket* incoming = server.nextPendingConnection();
    if (nullptr != incoming) {
      state.attach(incoming);
    }
    return nullptr != incoming;
  };

  {
    CShotDocState state("s", "retake p/a", nullptr);
    QLocalSocket client;
    qint32 readies = 0;
    QObject::connect(&state, &CShotDocState::becameReady, [&](const QString&) { readies++; });
    const bool paired = connectPair(state, client);
    client.write("ready one\n");
    client.flush();
    waitFor([&]() { return readies > 0; });
    waitFor([&]() { return client.canReadLine(); }, 2000);
    const QByteArray& first = client.readLine();
    client.write("ready two\n");
    client.flush();
    waitFor([&]() { return readies > 1; }, 2000);
    settle(200);
    const QByteArray& second = client.readAll();
    verdict("the follow-up is sent once, after the first ready",
            paired && 2 == readies && "retake p/a\n" == first && second.isEmpty(),
            QString("%1 readies, first '%2', then '%3'")
                .arg(readies)
                .arg(QString::fromUtf8(first).trimmed(), QString::fromUtf8(second).trimmed()));
  }
  {
    CShotDocState state("s", QString(), nullptr);
    QLocalSocket client;
    bool wasRecording = false;
    QObject::connect(&state, &CShotDocState::reported, [&](const QString& what, const QString&) {
      wasRecording = wasRecording || ("recording" == what && state.isRecording());
    });
    connectPair(state, client);
    client.write("recording\n");
    client.flush();
    waitFor([&]() { return wasRecording; }, 2000);
    client.write("recorded-none\n");
    client.flush();
    waitFor([&]() { return !state.isRecording(); }, 2000);
    verdict("recording follows the state's recording and recorded reports", wasRecording && !state.isRecording());
  }
  {
    CShotDocState state("s", QString(), nullptr);
    QLocalSocket client;
    qint32 reports = 0;
    QObject::connect(&state, &CShotDocState::reported, [&](const QString&, const QString&) { reports++; });
    connectPair(state, client);
    client.write("status one\n");
    client.flush();
    waitFor([&]() { return reports > 0; }, 2000);
    client.write("status two\nstatus three\n");
    client.flush();
    state.stop();
    settle(300);
    const bool refused = !state.send("select");
    verdict("after stop nothing is reported and nothing is sent", 1 == reports && refused,
            QString("%1 reports").arg(reports));
  }
  {
    // The launcher's pattern: a stopped state is deleted later, and its channel with it.
    QPointer<CShotDocState> state = new CShotDocState("s", QString(), nullptr);
    QLocalSocket client;
    connectPair(*state, client);
    state->stop();
    state->deleteLater();
    settle(300);
    const bool deferred = !state.isNull();
    // A nested event loop does not deliver a DeferredDelete posted outside it.
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    waitFor([&]() { return QLocalSocket::ConnectedState != client.state(); }, 2000);
    verdict("a stopped state deleted later leaves no channel",
            state.isNull() && QLocalSocket::ConnectedState != client.state(),
            QString("deleted inside the nested loop: %1, client %2")
                .arg(deferred ? "no" : "yes")
                .arg(QLocalSocket::ConnectedState != client.state() ? "disconnected" : "connected"));
  }
}
}  // namespace

qint32 CShotDocSelfTest::run() {
  cases = 0;
  failures = 0;
  testNames();
  testRename();
  testDelete();
  testRebind();
  testUnused();
  testUnicodeNames();
  testForeignPicture();
  testBusyBox();
  testRows();
  testJob();
  testStateProcess();
  qWarning().noquote() << "shoot: self test (documentation mode)" << (cases - failures) << "of" << cases
                       << "cases pass";
  return failures;
}
