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

#ifndef CCLOCK_H
#define CCLOCK_H

#include <QDateTime>

/**
   @brief The time items, projects and filters are stamped with.

   A documentation run pins it, so an image showing such a time is the same in every run.
 */
class CClock {
 public:
  /** @return the current time in UTC, or the pinned time */
  static QDateTime nowUtc() { return pinned.isValid() ? pinned : QDateTime::currentDateTimeUtc(); }

  /**
     @brief Freeze the clock at @p utc; call before any thread reads it.
     @param utc  the time every later call returns
   */
  static void pin(const QDateTime& utc) { pinned = utc; }

 private:
  static inline QDateTime pinned;
};

#endif  // CCLOCK_H
