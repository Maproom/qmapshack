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
namespace CShotEntry {
/** @return true for a shoot or a documentation run */
bool isDocRun(const CAppOpts& opts);

/**
   @brief Prepare a documentation run's process before QApplication.

   Sets QT_QPA_PLATFORMTHEME=generic so on-screen and headless runs render alike; on Windows, which has no desktop
   theme, attaches the GUI-subsystem build to the parent's console instead.
 */
void pinEnvironment(int argc, char** argv);

/**
   @brief The application object, created before anything else.

   A documentation run gets a subclass that wraps input delivery in notify(); any other run a plain QApplication.
 */
std::unique_ptr<QApplication> createApplication(int& argc, char** argv);

/**
   @brief Redirect a documentation run's data and pin its appearance. Once, before CMainWindow.

   @return false when the run must not start
 */
bool prepare(const CAppOpts& opts);

/** @return false for the documentation launcher, whose main window only initialises the singletons */
bool showsMainWindow(const CAppOpts& opts);

/**
   @brief Start a `--doc` launcher or state process, or run a `--shoot` run to its end.

   @return the exit code of a `--shoot` run (failures, capped at CShotReplay::kMaxFailureExitCode), nothing for any
   other run
 */
std::optional<qint32> run(const CAppOpts& opts, CMainWindow& window);
}  // namespace CShotEntry

#endif  // CSHOTENTRY_H
