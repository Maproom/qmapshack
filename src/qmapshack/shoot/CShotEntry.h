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

class CAppOpts;

/** @brief All main() knows about the documentation subsystem; the stub build answers false and does nothing. */
class CShotEntry {
 public:
  /** @return true for a shoot or a documentation run */
  static bool isDocRun(const CAppOpts& opts);

  /**
     @brief Set QT_QPA_PLATFORMTHEME=generic so on-screen and headless runs render alike. Before QApplication.

     Qt takes icons and a few style hints from the platform theme, so the same dialog renders Breeze
     on a KDE desktop and Qt's own icons headless. QT_QPA_PLATFORMTHEME=generic makes both render the
     same bytes, and Qt reads it while QApplication is constructed - hence here, before the command
     line has been parsed.
   */
  static void pinEnvironment(int argc, char** argv);

  /**
     @brief Redirect a documentation run's data and pin its appearance. Once, before CMainWindow.

     @return false when the run must not start
   */
  static bool prepare(const CAppOpts& opts);

 private:
  /** @return the workspace database path, empty when @p dir could not be created */
  static QString scratchWorkspace(const QString& dir, const QString& name);

  /** @return false when the scratch directory could not be created */
  static bool redirectUserData(const QString& cache, const QString& name);

  /** @brief The offscreen platform on Windows has no fonts. */
  static void registerFonts();

  static void pinAppearance(const QString& colorScheme);
};

#endif  // CSHOTENTRY_H
