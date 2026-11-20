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

#include <QStringList>
#include <QtGlobal>

class CShotContext;

/**
   @brief Recorder self test: real window-system input is recorded, compared with the expected steps, and replayed.
 */
namespace CShotSelfTest {
/** @return the names of the groups of cases, in the order a run of all takes them; `shots.py` keeps the same list */
QStringList groups();

/**
   @brief Run the group @p group, or every group and the coverage check for an empty one.

   A single group reports the handlers it did not replay as `shoot: unreplayed <names>`: coverage spans the groups.
   @return the number of failed cases
 */
qint32 run(CShotContext& ctx, const QString& group);
}  // namespace CShotSelfTest

#endif  // CSHOTSELFTEST_H
