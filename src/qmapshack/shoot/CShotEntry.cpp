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
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>
#include <QPalette>
#include <QStyleHints>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "map/CMapDraw.h"
#include "setup/CAppOpts.h"
#include "shoot/CShotApplication.h"
#include "shoot/CShotDocLauncher.h"
#include "shoot/CShotDocMode.h"
#include "shoot/CShotRunner.h"
#include "theme/CQmsStyle.h"

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

/** @brief Replace the application palette with Fusion's light or dark one, whatever the desktop is set to. */
void pinColorScheme(bool dark) {
  QGuiApplication::styleHints()->setColorScheme(dark ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light);

  // Fusion's palette; QStyle::standardPalette() would ask the platform theme for the scheme.
  const QColor windowText = dark ? QColor(240, 240, 240) : QColor(Qt::black);
  const QColor button = dark ? QColor(50, 50, 50) : QColor(239, 239, 239);
  const QColor light = button.lighter(150);
  const QColor mid = button.darker(130);
  const QColor shade = button.darker(150);
  const QColor base = dark ? button.darker(140) : QColor(Qt::white);
  const QColor highlight(48, 140, 198);
  const QColor disabledText = dark ? QColor(130, 130, 130) : QColor(190, 190, 190);
  const QColor shadow = shade.darker(135);
  QColor placeholder = windowText;
  placeholder.setAlpha(128);

  // The seven-colour ctor takes Window, which paletteIsDark() reads, from `button`.
  QPalette palette(windowText, button, light, shade, mid, windowText, base);
  palette.setBrush(QPalette::Midlight, mid.lighter(110));
  palette.setBrush(QPalette::Button, button);
  palette.setBrush(QPalette::Shadow, shadow);
  palette.setBrush(QPalette::HighlightedText, dark ? windowText : QColor(Qt::white));
  // The seven-colour ctor takes BrightText from `light`, too dark on a dark window.
  palette.setBrush(QPalette::BrightText, QColor(Qt::white));
  palette.setBrush(QPalette::PlaceholderText, placeholder);

  palette.setBrush(QPalette::Disabled, QPalette::Text, disabledText);
  palette.setBrush(QPalette::Disabled, QPalette::WindowText, disabledText);
  palette.setBrush(QPalette::Disabled, QPalette::ButtonText, disabledText);
  palette.setBrush(QPalette::Disabled, QPalette::Base, button);
  // Light arm: Fusion's literal. Dark arm derived, the literal is a near-white edge there.
  palette.setBrush(QPalette::Disabled, QPalette::Dark, dark ? shade.lighter(120) : QColor(209, 209, 209).darker(110));
  palette.setBrush(QPalette::Disabled, QPalette::Shadow, shadow.lighter(150));

  const QColor disabledHighlight(145, 145, 145);
  palette.setBrush(QPalette::Active, QPalette::Highlight, highlight);
  palette.setBrush(QPalette::Inactive, QPalette::Highlight, highlight);
  palette.setBrush(QPalette::Disabled, QPalette::Highlight, disabledHighlight);
  palette.setBrush(QPalette::Active, QPalette::Accent, highlight);
  palette.setBrush(QPalette::Inactive, QPalette::Accent, highlight);
  palette.setBrush(QPalette::Disabled, QPalette::Accent, disabledHighlight);

  // Qt::blue, the default, is unreadable on a dark window.
  if (dark) {
    palette.setBrush(QPalette::Link, highlight);
  }

  QApplication::setPalette(palette);
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
  if (!isDocArgv(argc, argv)) {
    return;
  }
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
    pinColorScheme("dark" == colorScheme);
    qDebug() << "documentation run: colour scheme pinned to" << colorScheme;
  } else {
    qWarning() << "unknown --color-scheme" << colorScheme << "- expected light or dark";
  }
}
