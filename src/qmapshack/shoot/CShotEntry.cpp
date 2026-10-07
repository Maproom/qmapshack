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

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "map/CMapDraw.h"
#include "setup/CAppOpts.h"
#include "shoot/CShotApplication.h"
#include "shoot/CShotDocLauncher.h"
#include "shoot/CShotDocMode.h"
#include "shoot/CShotRunner.h"
#include "theme/CQmsStyle.h"
#include "theme/CUiTheme.h"

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

  CShotRunner* runner = new CShotRunner(opts.doc, &window);
  runner->start();
  QApplication::exec();
  return qMin(runner->getFailures(), 255);
}

bool CShotEntry::isDocArgv(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    const QByteArray arg(argv[i]);
    if (arg.startsWith("--shoot") || arg.startsWith("--doc")) {
      return true;
    }
  }
  return false;
}

std::unique_ptr<QApplication> CShotEntry::createApplication(int& argc, char** argv) {
  if (isDocArgv(argc, argv)) {
    return std::make_unique<CShotApplication>(argc, argv);
  }
  return std::make_unique<QApplication>(argc, argv);
}

void CShotEntry::pinEnvironment(int argc, char** argv) {
#ifdef Q_OS_WIN
  // `generic` crashes the QApplication constructor on Windows.
  Q_UNUSED(argc)
  Q_UNUSED(argv)
#else
  if (isDocArgv(argc, argv)) {
    qputenv("QT_QPA_PLATFORMTHEME", "generic");
  }
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

  // Without --config ~CMainWindow overwrites the user's settings, and a stored cachePath there overrides
  // setCacheRoot().
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
  pinAppearance(opts.doc.colorScheme);
  registerFonts();

  if (!opts.doc.shootDir.isEmpty()) {
    return redirectUserData(QDir(opts.doc.shootDir).absoluteFilePath("_cache"),
                            QFileInfo(opts.doc.shootTarget).completeBaseName());
  }
  // The launcher and its state process run at once; one SQLite workspace for both is locked for seconds.
  const QString& name = opts.doc.docScenario.isEmpty() ? opts.doc.docPage + "-launcher" : opts.doc.docPage;
  return redirectUserData(QDir(opts.doc.docDir).absoluteFilePath("doc/shots/_cache"), name);
}

QString CShotEntry::scratchWorkspace(const QString& dir, const QString& name) {
  if (!QDir(dir).mkpath(".")) {
    return QString();
  }
  return QDir(dir).absoluteFilePath((name.isEmpty() ? QString("shoot") : name) + "-workspace.db");
}

bool CShotEntry::redirectUserData(const QString& cache, const QString& name) {
  const QString& workspace = scratchWorkspace(cache, name);
  if (workspace.isEmpty()) {
    qCritical() << "cannot create the scratch directory" << cache;
    return false;
  }

  CMapDraw::setCacheRoot(cache);
  CGisListWks::setDatabasePath(workspace);
  qDebug() << "documentation run: tile cache" << cache;
  qDebug() << "documentation run: workspace database" << workspace;
  return true;
}

void CShotEntry::registerFonts() {
  const QStringList fonts = {":/fonts/DejaVuSans.ttf", ":/fonts/DejaVuSans-Bold.ttf"};
  for (const QString& font : fonts) {
    if (QFontDatabase::addApplicationFont(font) < 0) {
      qWarning() << "could not register" << font;
    } else {
      qDebug() << "documentation run: font" << font;
    }
  }
}

void CShotEntry::pinAppearance(const QString& colorScheme) {
  CQmsStyle::pinThemeIndependentHints();

  if (colorScheme.isEmpty()) {
    return;
  }
  if ("light" == colorScheme || "dark" == colorScheme) {
    CUiTheme::pinColorScheme("dark" == colorScheme);
    qDebug() << "documentation run: colour scheme pinned to" << colorScheme;
  } else {
    qWarning() << "unknown --color-scheme" << colorScheme << "- expected light or dark";
  }
}
