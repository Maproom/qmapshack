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

#ifndef CSHOTSTEP_H
#define CSHOTSTEP_H

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class QVariant;

/**
   @brief The steps a recording is made of, spelled in one place.

   `widget` is an address, `row` an item view row, a position a fraction of a widget or a surface's own units - never a
   window pixel. The handler of the target's class replays a step, so verbs mean different things per handler.
 */
namespace CShotStep {
/** @return @p widget clicked */
QJsonObject click(const QString& widget);

/** @return the section @p section of the header @p widget clicked */
QJsonObject clickSection(const QString& widget, qint32 section);

/** @return the row @p row of the item view @p widget clicked */
QJsonObject select(const QString& widget, const QString& row);

/** @return the row @p row of the item view @p widget double clicked */
QJsonObject dclick(const QString& widget, const QString& row);

/** @return the row @p row of the item view @p widget expanded */
QJsonObject expand(const QString& widget, const QString& row);

/** @return the row @p row of the item view @p widget collapsed */
QJsonObject collapse(const QString& widget, const QString& row);

/** @return @p value put into @p property of @p widget */
QJsonObject set(const QString& widget, const QString& property, const QVariant& value);

/** @return the action named @p action triggered */
QJsonObject trigger(const QString& action);

/** @return the tab @p index of the tab bar @p widget asked to close */
QJsonObject closeTab(const QString& widget, qint32 index);

/** @return a context menu asked for on the row @p row of the item view @p widget */
QJsonObject menu(const QString& widget, const QString& row);

/** @return a context menu asked for at @p at, a fraction of @p widget */
QJsonObject menuAt(const QString& widget, const QJsonArray& at);

/** @return @p text typed into @p widget once all of it is selected, then Enter when @p enter */
QJsonObject key(const QString& widget, const QString& text, bool enter);

/**
   @brief What was done to a surface, where: `click`, `drag`, `dclick`, `move`, `wheel` or `keypress`.

   @param where  the surface's own units: `lat`/`lon` on the map, `x` or `at` on a plot, `icon` on the icon grid
 */
QJsonObject surface(const QString& event, const QString& widget, const QJsonObject& where);
}  // namespace CShotStep

#endif  // CSHOTSTEP_H
