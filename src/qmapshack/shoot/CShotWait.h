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

#ifndef CSHOTWAIT_H
#define CSHOTWAIT_H

#include <QtGlobal>
#include <functional>

/** @brief Waiting with the event loop running and user input held back; processEvents() does not wait. */
namespace CShotWait {
/** CMainWindow maximizes a window with no stored geometry after 500 ms. */
constexpr qint32 kStartupMs = 1000;

/** @brief Run the event loop for @p ms. */
void wait(qint32 ms);

/** @return false when @p done is still false after @p timeoutMs; checked every @p pollMs */
bool waitFor(const std::function<bool()>& done, qint32 timeoutMs, qint32 pollMs);
}  // namespace CShotWait

#endif  // CSHOTWAIT_H
