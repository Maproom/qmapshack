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

#ifndef CSHOTHANDLERS_H
#define CSHOTHANDLERS_H

#include <QJsonObject>
#include <QString>
#include <QtGlobal>

class IShotHandler;
class QObject;
class QWidget;

/** @brief The handler per recordable class, and how a step resolves its target. Add new classes here only. */
namespace CShotHandlers {
/** @return the handler of @p object's nearest class; nullptr when none covers it */
const IShotHandler* of(const QObject* object);

/** @return a viewport's scroll area, else @p widget */
QWidget* ownerOf(QWidget* widget);

/** @return @p step's action or widget below @p root, nullptr when not there */
QObject* resolve(QWidget* root, const QJsonObject& step);

/** @return @p key as a portable name, empty for a modifier on its own or a key without a name */
QString keyName(int key);

/** @brief Put @p mods into @p step as names, if any */
void putMods(QJsonObject& step, Qt::KeyboardModifiers mods);

/**
   @brief Perform @p step below @p root through its target's handler; `layout` and `view` are applied beforehand.

   @return the failures
 */
qint32 replay(QWidget* root, const QJsonObject& step);
}  // namespace CShotHandlers

#endif  // CSHOTHANDLERS_H
