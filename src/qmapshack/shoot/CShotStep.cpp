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
#include <QMap>
#include <QVariant>

QJsonObject CShotStep::click(const QString& widget) { return QJsonObject{{"do", "click"}, {"widget", widget}}; }

QJsonObject CShotStep::clickSection(const QString& widget, qint32 section) {
  return QJsonObject{{"do", "click"}, {"widget", widget}, {"section", section}};
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

QJsonObject CShotStep::endEdit(const QString& widget) { return QJsonObject{{"do", "endedit"}, {"widget", widget}}; }

QJsonObject CShotStep::key(const QString& widget, const QString& text, bool enter) {
  QJsonObject step{{"do", "key"}, {"widget", widget}, {"text", text}};
  if (enter) {
    step["enter"] = true;
  }
  return step;
}

namespace {
/** @return the widget an address ends at, as the writer sees it */
QString widgetName(const QString& address) {
  static const QMap<QString, QString> kKnown = {{"CCanvas", "the map"},
                                                {"CGisListWks", "the workspace"},
                                                {"CGisListDB", "the database list"},
                                                {"CMapList", "the map list"},
                                                {"CMapTreeWidget", "the map list"},
                                                {"CDemList", "the DEM list"},
                                                {"CDemTreeWidget", "the DEM list"}};
  QString last = address.section('/', -1);
  last = last.section('#', 0, 0);
  return kKnown.value(last, QString("'%1'").arg(last));
}

QString verbOf(const QJsonObject& step) {
  static const QMap<QString, QString> kVerbs = {{"click", "click"},
                                                {"dclick", "double-click"},
                                                {"mouseclick", "click"},
                                                {"mousedclick", "double-click"},
                                                {"press", "press the mouse button"},
                                                {"release", "release the mouse button"},
                                                {"move", "move the pointer"},
                                                {"drag", "drag"},
                                                {"wheel", "turn the mouse wheel"},
                                                {"select", "select"},
                                                {"expand", "expand"},
                                                {"collapse", "collapse"},
                                                {"scroll", "scroll"},
                                                {"menu", "open the context menu"},
                                                {"openmenu", "open the menu"},
                                                {"close", "close a tab"},
                                                {"endedit", "finish editing"},
                                                {"arrange", "arrange the docks"}};
  const QString& verb = step["do"].toString();
  if ("keypress" == verb) {
    return QString("press %1").arg(step["key"].toString());
  }
  if ("key" == verb) {
    return QString("type '%1'").arg(step["text"].toString());
  }
  if ("set" == verb) {
    return QString("set %1 to %2").arg(step["property"].toString(), step["value"].toVariant().toString());
  }
  if ("trigger" == verb) {
    return QString("choose the entry '%1'").arg(step["action"].toString());
  }
  return kVerbs.value(verb, verb);
}
}  // namespace

QString CShotStep::describe(const QJsonObject& step) {
  QString text = verbOf(step);
  const QString& row = step["row"].toString();
  if (!row.isEmpty()) {
    text += QString(" the row '%1'").arg(row);
  }
  if (step.contains("hit")) {
    text += QString(" on '%1'").arg(step["hit"].toString());
  }
  if (step.contains("lat") && step.contains("lon")) {
    text += QString(" at %1, %2").arg(step["lat"].toDouble(), 0, 'f', 5).arg(step["lon"].toDouble(), 0, 'f', 5);
  }
  if (step.contains("widget")) {
    text += " in " + widgetName(step["widget"].toString());
  }
  return text;
}

QJsonObject CShotStep::surface(const QString& event, const QString& widget, const QJsonObject& where) {
  QJsonObject step{{"do", event}, {"widget", widget}};
  for (auto it = where.constBegin(); it != where.constEnd(); ++it) {
    step[it.key()] = it.value();
  }
  return step;
}
