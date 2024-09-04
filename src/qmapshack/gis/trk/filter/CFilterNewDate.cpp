/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/trk/filter/CFilterNewDate.h"

#include "canvas/CCanvas.h"
#include "gis/trk/CGisItemTrk.h"
#include "units/IUnit.h"

CFilterNewDate::CFilterNewDate(CGisItemTrk& trk, QWidget* parent) : QWidget(parent), trk(trk) {
  setupUi(this);

  IUnit::tz_mode_e mode;
  QByteArray zone;
  bool format;
  IUnit::self().getTimeZoneSetup(mode, zone, format);

  switch (mode) {
    case IUnit::eTZUtc: {
      labelTimeZone->setText(QTimeZone::utc().abbreviation(QDateTime::currentDateTimeUtc()));
      dateTimeEdit->setDateTime(QDateTime::currentDateTimeUtc());
      break;
    }

    case IUnit::eTZLocal: {
      labelTimeZone->setText(QDateTime::currentDateTime().timeZone().abbreviation(QDateTime::currentDateTime()));
      dateTimeEdit->setDateTime(QDateTime::currentDateTime());
      break;
    }

    case IUnit::eTZAuto: {
      CTrackData::trkpt_t trkpt = *trk.getTrackData().begin();
      zone = IUnit::pos2timezone(trkpt.radPoint() * DEG_TO_RAD);
      // break; // intended
    }

    case IUnit::eTZSelected: {
      const QDateTime& datetime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone(zone));
      labelTimeZone->setText(datetime.timeZone().abbreviation(datetime));
      dateTimeEdit->setDateTime(datetime);
      break;
    }
  }

  connect(toolApply, &QToolButton::clicked, this, &CFilterNewDate::slotApply);
}

void CFilterNewDate::slotApply() {
  CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
  trk.filterNewDate(dateTimeEdit->dateTime().toUTC());
}
