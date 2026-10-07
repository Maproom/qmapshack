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

#ifndef CSHOTFIXTURE_H
#define CSHOTFIXTURE_H

#include <QString>

class CShotContext;

/** @brief Loads the example project into the workspace; maps, DEM, POI and database come from the configuration. */
class CShotFixture {
 public:
  /**
     @brief Load the project the configuration names as `Shoot/fixtureProject`: the page fixture's.

     @return the failures
   */
  static qint32 load(CShotContext& ctx);
};

#endif  // CSHOTFIXTURE_H
