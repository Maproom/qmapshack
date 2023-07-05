/**********************************************************************************************
    Copyright (C) 2023 Gunnar Skjold <gunnar.skjold@gmail.com>

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

#ifndef CRTAISRECORD_H
#define CRTAISRECORD_H

#include "realtime/IRtRecord.h"
#include "realtime/ais/CRtAis.h"

class CRtAisRecord : public IRtRecord {
  Q_OBJECT
 public:
  CRtAisRecord(QObject* parent);
  virtual ~CRtAisRecord() = default;

  bool writeEntry(const CRtAis::ship_t& ship);

 protected:
  using IRtRecord::writeEntry;
};

#endif  // CRTAISRECORD_H
