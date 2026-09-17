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

#include <QApplication>

#include "shoot/CShotEntry.h"

/// A build without the documentation subsystem.
bool CShotEntry::isDocRun(const CAppOpts&) { return false; }
bool CShotEntry::showsMainWindow(const CAppOpts&) { return true; }

void CShotEntry::pinEnvironment(int, char**) {}

std::unique_ptr<QApplication> CShotEntry::createApplication(int& argc, char** argv) {
  return std::make_unique<QApplication>(argc, argv);
}

bool CShotEntry::prepare(const CAppOpts&) { return true; }

std::optional<qint32> CShotEntry::run(const CAppOpts&, CMainWindow&) { return std::nullopt; }
