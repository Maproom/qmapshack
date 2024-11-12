/**********************************************************************************************
    Copyright (C) 2022 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CTimeDialog.h"

#include <QTimeZone>

#include "CSettings.h"

CTimeDialog::CTimeDialog(QWidget* parent, const QDateTime& datetime)
    : QDialog(parent), timestamp(datetime), timestamp_utc0(datetime.toTimeZone(QTimeZone("UTC+00:00"))) {
  setupUi(this);

  SETTINGS;
  cfg.beginGroup("TimeDialog");
  const QByteArray& zone = cfg.value("timezone", QTimeZone::systemTimeZone().id()).toByteArray();
  cfg.endGroup();

  const QList<QByteArray>& ids = QTimeZone::availableTimeZoneIds();
  foreach (const QByteArray& id, ids) {
    comboTimezone->addItem(id.data());
  }

  comboTimezone->setCurrentText(zone.data());

  const QDateTime& newTime = timestamp_utc0.toTimeZone(QTimeZone(zone));
  dateTimeEdit->setDateTime(newTime);

  connect(comboTimezone, &QComboBox::currentTextChanged, this, [this](const QString& id) {
    SETTINGS;
    cfg.beginGroup("TimeDialog");
    cfg.setValue("timezone", id);
    cfg.endGroup();
    const QDateTime& newTime = timestamp_utc0.toTimeZone(QTimeZone(id.toUtf8()));
    dateTimeEdit->setDateTime(newTime);
  });
}

void CTimeDialog::accept() {
  const QDateTime& newTime = dateTimeEdit->getDateTime();
  timestamp = newTime.toTimeZone(timestamp.timeZone());
  QDialog::accept();
}
