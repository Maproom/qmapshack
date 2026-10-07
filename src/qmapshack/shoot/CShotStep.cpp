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

#include "shoot/CShotStep.h"

#include <QJsonValue>
#include <QVariant>

QJsonObject CShotStep::click(const QString& widget) { return QJsonObject{{"do", "click"}, {"widget", widget}}; }

QJsonObject CShotStep::clickSection(const QString& widget, qint32 section) {
  return QJsonObject{{"do", "click"}, {"widget", widget}, {"section", section}};
}

QJsonObject CShotStep::select(const QString& widget, const QString& row) {
  return QJsonObject{{"do", "select"}, {"widget", widget}, {"row", row}};
}

QJsonObject CShotStep::dclick(const QString& widget, const QString& row) {
  return QJsonObject{{"do", "dclick"}, {"widget", widget}, {"row", row}};
}

QJsonObject CShotStep::expand(const QString& widget, const QString& row) {
  return QJsonObject{{"do", "expand"}, {"widget", widget}, {"row", row}};
}

QJsonObject CShotStep::collapse(const QString& widget, const QString& row) {
  return QJsonObject{{"do", "collapse"}, {"widget", widget}, {"row", row}};
}

QJsonObject CShotStep::set(const QString& widget, const QString& property, const QVariant& value) {
  return QJsonObject{
      {"do", "set"}, {"widget", widget}, {"property", property}, {"value", QJsonValue::fromVariant(value)}};
}

QJsonObject CShotStep::trigger(const QString& action) { return QJsonObject{{"do", "trigger"}, {"action", action}}; }

QJsonObject CShotStep::closeTab(const QString& widget, qint32 index) {
  return QJsonObject{{"do", "close"}, {"widget", widget}, {"index", index}};
}

QJsonObject CShotStep::menu(const QString& widget, const QString& row) {
  return QJsonObject{{"do", "menu"}, {"widget", widget}, {"row", row}};
}

QJsonObject CShotStep::menuAt(const QString& widget, const QJsonArray& at) {
  return QJsonObject{{"do", "menu"}, {"widget", widget}, {"at", at}};
}

QJsonObject CShotStep::key(const QString& widget, const QString& text, bool enter) {
  QJsonObject step{{"do", "key"}, {"widget", widget}, {"text", text}};
  if (enter) {
    step["enter"] = true;
  }
  return step;
}

QJsonObject CShotStep::surface(const QString& event, const QString& widget, const QJsonObject& where) {
  QJsonObject step{{"do", event}, {"widget", widget}};
  for (auto it = where.constBegin(); it != where.constEnd(); ++it) {
    step[it.key()] = it.value();
  }
  return step;
}
