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

#ifndef CSHOTSELFTEST_H
#define CSHOTSELFTEST_H

#include <QtGlobal>

class CShotContext;

/**
   @brief Recorder self test: real window-system input is recorded, compared with the expected steps, and replayed.
 */
namespace CShotSelfTest {
/** @return the number of failed cases */
qint32 run(CShotContext& ctx);
}  // namespace CShotSelfTest

#endif  // CSHOTSELFTEST_H
