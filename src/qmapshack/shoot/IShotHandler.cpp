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

#include "shoot/IShotHandler.h"

#include <QJsonArray>
#include <QPoint>
#include <QWidget>

#include "shoot/CShotRecorder.h"
#include "shoot/CShotStep.h"

void IShotHandler::input(QWidget* target, const QEvent* event, CShotRecorder& recorder) const {
  Q_UNUSED(target)
  Q_UNUSED(event)
  Q_UNUSED(recorder)
}

QJsonObject IShotHandler::contextMenu(QWidget* target, const QPoint& pos, const CShotRecorder& recorder) const {
  return menuAt(target, pos, recorder);
}

QJsonObject IShotHandler::menuAt(QWidget* target, const QPoint& pos, const CShotRecorder& recorder) {
  const std::optional<QString>& address = recorder.address(target);
  if (!address.has_value()) {
    return QJsonObject();
  }
  // A fraction, so it survives another widget size.
  const QJsonArray at({double(pos.x()) / qMax(1, target->width()), double(pos.y()) / qMax(1, target->height())});
  return CShotStep::menuAt(*address, at);
}
