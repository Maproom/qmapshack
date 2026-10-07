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

#include "shoot/CShotRunner.h"

#include <QApplication>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <QWidget>

#include "shoot/CShotContext.h"
#include "shoot/CShotDocSelfTest.h"
#include "shoot/CShotFixture.h"
#include "shoot/CShotPage.h"
#include "shoot/CShotSelfTest.h"
#include "shoot/CShotWriter.h"

namespace {
/** CMainWindow maximizes a window with no stored geometry after 500 ms. */
constexpr qint32 kStartupMs = 1000;
}  // namespace

CShotRunner::CShotRunner(const CShotOptions::opts_t& opts, QWidget* window)
    : QObject(window),
      window(window),
      outDir(opts.shootDir),
      target(opts.shootTarget),
      only(opts.shootOnly),
      scenario(opts.shootScenario),
      selfTest(opts.shootSelfTest) {}

void CShotRunner::start() { QTimer::singleShot(0, this, &CShotRunner::slotRun); }

void CShotRunner::slotRun() {
  QEventLoop loop;
  QTimer::singleShot(kStartupMs, &loop, &QEventLoop::quit);
  loop.exec(QEventLoop::ExcludeUserInputEvents);

  if (target.isEmpty()) {
    qWarning() << "shoot: --shoot needs --shoot-target, the page's shot file";
    failures = 1;
  } else {
    const CShotWriter writer(outDir, "en");
    CShotContext ctx(writer);
    failures = CShotFixture::load(ctx);
    if (!selfTest) {
      failures += CShotPage::run(target, ctx, only, scenario);
    } else if (0 == failures) {
      failures += CShotSelfTest::run(ctx);
    } else {
      qWarning() << "shoot: the self test does not run: its fixture did not load";
    }
    // Needs no fixture.
    if (selfTest) {
      failures += CShotDocSelfTest::run();
    }
  }

  // Destroying the main window while shown crashes in its docks' visibilityChanged.
  window->close();
  qApp->quit();
}
