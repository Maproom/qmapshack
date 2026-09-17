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

#include "shoot/CShotDocMode.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QEventLoop>
#include <QJsonArray>
#include <QLocalSocket>
#include <QTimer>
#include <cstdlib>

#include "CMainWindow.h"
#include "setup/CAppOpts.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotFiles.h"
#include "shoot/CShotFixture.h"
#include "shoot/CShotPage.h"
#include "shoot/CShotReplay.h"
#include "shoot/CShotWriter.h"

namespace {
/** CMainWindow maximizes a window with no stored geometry after 500 ms. */
constexpr qint32 kStartupMs = 1000;
/** How long leave() waits for the event loop to return before exiting [ms]. */
constexpr qint32 kLeaveTimeoutMs = 2000;
}  // namespace

CShotDocMode::CShotDocMode(const QDir& repo, const QString& page, const QString& scenario, QObject* parent)
    : QObject(parent),
      repo(repo),
      page(page),
      scenario(CShotPage::kBaseScenario == scenario ? QString() : scenario),
      // Never doc/images: only publishing writes there.
      writer(std::make_unique<CShotWriter>(repo.absoluteFilePath("doc/images/_work"))),
      ctx(std::make_unique<CShotContext>(*writer)) {}

CShotDocMode::~CShotDocMode() = default;

void CShotDocMode::start() {
  if (!qlOpts->doc.docChannel.isEmpty()) {
    channel = new QLocalSocket(this);
    connect(channel, &QLocalSocket::readyRead, this, [this]() {
      while (nullptr != channel && channel->canReadLine()) {
        obey(QString::fromUtf8(channel->readLine()).trimmed());
      }
    });
    // A state without its launcher is an orphan.
    connect(channel, &QLocalSocket::disconnected, this, [this]() { leave("the launcher is gone"); });
    // A connect that fails emits errorOccurred and never disconnected (measured: ServerNotFoundError).
    connect(channel, &QLocalSocket::errorOccurred, this, [this]() {
      if (QLocalSocket::ConnectedState != channel->state()) {
        leave("there is no launcher");
      }
    });
    channel->connectToServer(qlOpts->doc.docChannel);
  }
  if (!CMainWindow::isNull()) {
    CMainWindow::self().installEventFilter(this);
  }
  QTimer::singleShot(0, this, &CShotDocMode::slotSetUp);
}

bool CShotDocMode::eventFilter(QObject* watched, QEvent* event) {
  // quitOnLastWindowClosed does not end the process.
  if (QEvent::Close == event->type() && !CMainWindow::isNull() && watched == &CMainWindow::self()) {
    leave("the application window was closed");
  }
  return QObject::eventFilter(watched, event);
}

void CShotDocMode::leave(const QString& why) {
  if (leaving) {
    return;
  }
  leaving = true;
  qDebug().noquote() << "doc:" << why << "- the state process ends";
  // Destroying the main window while shown crashes in its docks' visibilityChanged.
  if (!CMainWindow::isNull()) {
    CMainWindow::self().close();
  }
  qApp->exit(0);
  // exit() ends only a running event loop: called before exec() or from a nested loop the process stays (measured).
  QTimer::singleShot(kLeaveTimeoutMs, qApp, []() {
    qWarning() << "doc: the state process did not end by itself";
    std::_Exit(0);
  });
}

void CShotDocMode::slotSetUp() {
  QEventLoop loop;
  QTimer::singleShot(kStartupMs, &loop, &QEventLoop::quit);
  loop.exec(QEventLoop::ExcludeUserInputEvents);
  if (leaving) {
    return;
  }

  const QString& label = scenario.isEmpty() ? CShotFiles::kBaseLabel : scenario;
  if (0 != CShotFixture::load(repo.absoluteFilePath("doc/shots/fixture"), *ctx)) {
    send(QString("ready The fixture did not load in %1. The console says why.").arg(label));
    return;
  }

  if (!scenario.isEmpty()) {
    const QJsonObject& recorded = CShotFiles(repo, page).scenarios();
    if (!recorded.contains(scenario)) {
      send(QString("ready The page has no scenario %1; the application is in %2.")
               .arg(scenario, CShotFiles::kBaseLabel));
      return;
    }
    if (0 != CShotReplay::replay(recorded.value(scenario).toArray(), *ctx, {})) {
      send(QString("ready %1 does not replay. The console names the step.").arg(scenario));
      return;
    }
  }
  qDebug() << "doc: state" << label << "of page" << page;
  send(QString("ready The application is in %1.").arg(label));
}

void CShotDocMode::obey(const QString& line) {
  const QString& verb = line.section(' ', 0, 0);
  if ("select" == verb || "sync" == verb) {
    return;
  }
  qWarning() << "doc: the state process cannot do" << line << "yet";
  send(QString("status This build cannot do '%1' yet.").arg(verb));
}

void CShotDocMode::send(const QString& line) {
  if (nullptr == channel || QLocalSocket::ConnectedState != channel->state()) {
    qDebug().noquote() << "doc:" << line;
    return;
  }
  channel->write(line.toUtf8() + '\n');
  channel->flush();
}
