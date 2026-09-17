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

#ifndef CSHOTDOCSELFTEST_H
#define CSHOTDOCSELFTEST_H

#include <QtGlobal>

/**
   @brief The writer's session without a writer: page operations on a scratch checkout, and the state process and
          `shots.py` job handles against real processes, including the orders in which they fail.
 */
namespace CShotDocSelfTest {
/** @return the number of failed cases */
qint32 run();
}  // namespace CShotDocSelfTest

#endif  // CSHOTDOCSELFTEST_H
