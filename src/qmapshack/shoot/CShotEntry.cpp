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

#include "shoot/CShotEntry.h"

#include <QtSystemDetection>
#if defined(Q_OS_WIN32)
// The only Win32 include: MSVC needs windows.h ahead of every other Win32 header.
#include <windows.h>

#include <cstdio>
#endif

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>
#include <QTimer>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "map/CMapDraw.h"
#include "setup/CAppOpts.h"
#include "shoot/CShotApplication.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotDocLauncher.h"
#include "shoot/CShotDocMode.h"
#include "shoot/CShotDocSelfTest.h"
#include "shoot/CShotFiles.h"
#include "shoot/CShotFixture.h"
#include "shoot/CShotPage.h"
#include "shoot/CShotReplay.h"
#include "shoot/CShotSelfTest.h"
#include "shoot/CShotWait.h"
#include "shoot/CShotWriter.h"
#include "theme/CQmsStyle.h"
#include "theme/CUiTheme.h"

namespace {
#if defined(Q_OS_WIN32)
/** @return true when the parent handed this stream a pipe or a file */
bool isHandedOver(DWORD stream) {
  const HANDLE handle = GetStdHandle(stream);
  if (nullptr == handle || INVALID_HANDLE_VALUE == handle) {
    return false;
  }
  const DWORD type = GetFileType(handle);
  return FILE_TYPE_PIPE == type || FILE_TYPE_DISK == type;
}

/** @brief Attach a GUI-subsystem build to the parent's console, keeping streams handed over as pipe or file. */
void attachParentConsole() {
  // Before attaching: a handed-over pipe must be kept.
  const bool stdoutHandedOver = isHandedOver(STD_OUTPUT_HANDLE);
  const bool stderrHandedOver = isHandedOver(STD_ERROR_HANDLE);
  if ((stdoutHandedOver && stderrHandedOver) || !AttachConsole(ATTACH_PARENT_PROCESS)) {
    return;
  }

  // NUL on failure: freopen() has closed the stream already.
  if (!stdoutHandedOver && nullptr == freopen("CONOUT$", "w", stdout)) {
    freopen("NUL", "w", stdout);
  }
  if (!stderrHandedOver && nullptr == freopen("CONOUT$", "w", stderr)) {
    freopen("NUL", "w", stderr);
  }
}
#endif
}  // namespace

namespace {
/** @return true when the raw @p argv asks for a shoot or documentation run */
bool isDocArgv(int argc, char** argv);
/** @return the workspace database path, empty when @p dir could not be created */
QString scratchWorkspace(const QString& dir, const QString& name);
/** @return false when the scratch directory could not be created; @p fresh removes a workspace an earlier run left */
bool redirectUserData(const QString& tiles, const QString& scratch, const QString& name, bool fresh);
/** @brief The offscreen platform on Windows has no fonts. */
void registerFonts();
void pinAppearance(const QString& colorScheme);
}  // namespace

namespace {
/** @return the failures of taking the shots, or running the self test, that @p opts ask for */
qint32 shoot(const CShotOptions::opts_t& opts) {
  // The main window finishes initialising on timers.
  CShotWait::wait(CShotWait::kStartupMs);
  if (opts.shootTarget.isEmpty()) {
    qWarning() << "shoot: --shoot needs --shoot-target, the page's shot file";
    return 1;
  }

  const CShotWriter writer(opts.shootDir);
  CShotContext ctx(writer);
  qint32 failures = CShotFixture::load(ctx);
  if (0 != failures) {
    // Every picture would fail for it and bury the reason.
    qWarning() << "shoot: nothing is taken: the example data did not load";
  } else if (opts.shootSelfTest) {
    failures += CShotSelfTest::run(ctx);
  } else {
    failures += CShotPage::run(opts.shootTarget, ctx, opts.shootOnly, opts.shootScenario);
  }
  // Needs no fixture.
  if (opts.shootSelfTest) {
    failures += CShotDocSelfTest::run();
  }
  return failures;
}
}  // namespace

bool CShotEntry::isDocRun(const CAppOpts& opts) { return !opts.doc.shootDir.isEmpty() || !opts.doc.docDir.isEmpty(); }

bool CShotEntry::showsMainWindow(const CAppOpts& opts) {
  return opts.doc.docDir.isEmpty() || !opts.doc.docScenario.isEmpty();
}

std::optional<qint32> CShotEntry::run(const CAppOpts& opts, CMainWindow& window) {
  if (!opts.doc.docDir.isEmpty()) {
    if (opts.doc.docScenario.isEmpty()) {
      // CMainWindow maximizes itself 500 ms after construction when the configuration stores no geometry.
      window.setAttribute(Qt::WA_DontShowOnScreen);
      (new CShotDocLauncher(QDir(opts.doc.docDir), opts.doc.docPage, &window))->start();
    } else {
      (new CShotDocMode(QDir(opts.doc.docDir), opts.doc.docPage, opts.doc.docScenario, opts.doc.docTrial, &window))
          ->start();
    }
    return std::nullopt;
  }
  if (opts.doc.shootDir.isEmpty()) {
    return std::nullopt;
  }

  qint32 failures = 0;
  QTimer::singleShot(0, &window, [&opts, &window, &failures]() {
    failures = shoot(opts.doc);
    // Destroying the main window while shown crashes in its docks' visibilityChanged.
    window.close();
    qApp->quit();
  });
  QApplication::exec();
  return qMin(failures, CShotReplay::kMaxFailureExitCode);
}

namespace {
bool isDocArgv(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    const QByteArray arg(argv[i]);
    if (arg.startsWith("--shoot") || arg.startsWith("--doc")) {
      return true;
    }
  }
  return false;
}
}  // namespace

std::unique_ptr<QApplication> CShotEntry::createApplication(int& argc, char** argv) {
  if (isDocArgv(argc, argv)) {
    return std::make_unique<CShotApplication>(argc, argv);
  }
  return std::make_unique<QApplication>(argc, argv);
}

void CShotEntry::pinEnvironment(int argc, char** argv) {
  if (!isDocArgv(argc, argv)) {
    return;
  }
  // CMainWindow::getUser() puts the login into history entries and the database dialog.
  qputenv("USER", "user");
  qputenv("USERNAME", "user");
#if defined(Q_OS_WIN32)
  // `generic` crashes the QApplication constructor on Windows.
  attachParentConsole();
#else
  qputenv("QT_QPA_PLATFORMTHEME", "generic");
#endif
}

bool CShotEntry::prepare(const CAppOpts& opts) {
  if (!opts.doc.stray.isEmpty()) {
    qCritical().noquote() << opts.doc.stray.join(' ') << "belong to a documentation run; give --shoot or --doc too";
    return false;
  }
  if (!isDocRun(opts)) {
    return true;
  }

  // Without --config ~CMainWindow overwrites the user's settings.
  if (opts.configfile.isEmpty()) {
    qCritical() << "a documentation run needs --config: without it it writes the user's settings";
    return false;
  }

  if (!opts.doc.shootDir.isEmpty() && !opts.doc.docDir.isEmpty()) {
    qCritical() << "--shoot and --doc are two different runs; give one";
    return false;
  }

  qDebug() << "documentation run: platform theme" << qgetenv("QT_QPA_PLATFORMTHEME");
  // -d traces every user input frame.
  CShotApplication::setTrace(opts.debug);
  pinAppearance(opts.colorscheme);
  registerFonts();

  if (!opts.doc.shootDir.isEmpty()) {
    // The shot file lies in doc/shots: tiles are kept across runs. The workspace sits beside the output, which
    // selftest keeps, so each run starts it empty.
    const QFileInfo target(opts.doc.shootTarget);
    QDir repo = target.absoluteDir();
    repo.cd("../..");
    // The self test loads the default fixture whatever page names the run.
    const QString& tiles = opts.doc.shootSelfTest ? CShotFiles::sharedTileCache(repo)
                                                  : CShotFiles(repo, target.completeBaseName()).tileCache();
    return redirectUserData(tiles, QDir(opts.doc.shootDir).absoluteFilePath("_cache"), target.completeBaseName(), true);
  }
  // The launcher and its state process run at once; one SQLite workspace for both is locked for seconds.
  const QString& name = opts.doc.docScenario.isEmpty() ? opts.doc.docPage + "-launcher" : opts.doc.docPage;
  const QDir repo(opts.doc.docDir);
  return redirectUserData(CShotFiles(repo, opts.doc.docPage).tileCache(), repo.absoluteFilePath("doc/shots/_cache"),
                          name, false);
}

namespace {
QString scratchWorkspace(const QString& dir, const QString& name) {
  if (!QDir(dir).mkpath(".")) {
    return QString();
  }
  return QDir(dir).absoluteFilePath((name.isEmpty() ? QString("shoot") : name) + "-workspace.db");
}
}  // namespace

namespace {
bool redirectUserData(const QString& tiles, const QString& scratch, const QString& name, bool fresh) {
  const QString& workspace = scratchWorkspace(scratch, name);
  if (workspace.isEmpty()) {
    qCritical() << "cannot create the scratch directory" << scratch;
    return false;
  }
  if (fresh && QFileInfo::exists(workspace) && !QFile::remove(workspace)) {
    qCritical() << "cannot remove the workspace an earlier run left" << workspace;
    return false;
  }

  CMapDraw::setCacheRoot(tiles);
  CGisListWks::setDatabasePath(workspace);
  qDebug() << "documentation run: tile cache" << tiles;
  qDebug() << "documentation run: workspace database" << workspace;
  return true;
}
}  // namespace

namespace {
void registerFonts() {
  const QStringList fonts = {":/fonts/DejaVuSans.ttf", ":/fonts/DejaVuSans-Bold.ttf"};
  for (const QString& font : fonts) {
    if (QFontDatabase::addApplicationFont(font) < 0) {
      qWarning() << "could not register" << font;
    } else {
      qDebug() << "documentation run: font" << font;
    }
  }
}
}  // namespace

namespace {
void pinAppearance(const QString& colorScheme) {
  CQmsStyle::pinThemeIndependentHints();

  // A picture must not depend on the platform's palette.
  const Qt::ColorScheme scheme = CUiTheme::colorSchemeFromName(colorScheme);
  if (Qt::ColorScheme::Unknown != scheme) {
    CUiTheme::pinColorScheme(scheme, true);
    qDebug() << "documentation run: color scheme pinned to Fusion's" << colorScheme << "palette";
  }
}
}  // namespace
