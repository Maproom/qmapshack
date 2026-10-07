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

#ifndef CSHOTENTRY_H
#define CSHOTENTRY_H

#include <QString>
#include <memory>
#include <optional>

class CAppOpts;
class CMainWindow;
class QApplication;

/** @brief All main() knows about the documentation subsystem; the stub build answers false and does nothing. */
class CShotEntry {
 public:
  /** @return true for a shoot or a documentation run */
  static bool isDocRun(const CAppOpts& opts);

  /**
     @brief Set QT_QPA_PLATFORMTHEME=generic so on-screen and headless runs render alike. Before QApplication.

     Not on Windows, which has no desktop theme.
   */
  static void pinEnvironment(int argc, char** argv);

  /**
     @brief The application object, created before anything else.

     A documentation run gets a subclass that wraps input delivery in notify(); any other run a plain QApplication.
   */
  static std::unique_ptr<QApplication> createApplication(int& argc, char** argv);

  /**
     @brief Redirect a documentation run's data and pin its appearance. Once, before CMainWindow.

     @return false when the run must not start
   */
  static bool prepare(const CAppOpts& opts);

  /** @return false for the documentation launcher, whose main window only initialises the singletons */
  static bool showsMainWindow(const CAppOpts& opts);

  /**
     @brief Start a `--doc` launcher or state process, or run a `--shoot` run to its end.

     @return the exit code of a `--shoot` run (failures, capped at 255), nothing for any other run
   */
  static std::optional<qint32> run(const CAppOpts& opts, CMainWindow& window);

 private:
  /** @return the workspace database path, empty when @p dir could not be created */
  static QString scratchWorkspace(const QString& dir, const QString& name);

  /** @return false when the scratch directory could not be created */
  static bool redirectUserData(const QString& cache, const QString& name);

  /** @brief The offscreen platform on Windows has no fonts. */
  static void registerFonts();

  static void pinAppearance(const QString& colorScheme);

  /** @return true when the raw @p argv asks for a shoot or documentation run */
  static bool isDocArgv(int argc, char** argv);
};

#endif  // CSHOTENTRY_H
