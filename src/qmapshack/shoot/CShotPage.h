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

#ifndef CSHOTPAGE_H
#define CSHOTPAGE_H

#include <QJsonObject>
#include <QString>

class CCanvas;
class CMainWindow;
class CShotContext;
class QObject;
class QVariant;
class QWidget;

/** @brief A page's shot file, `doc/shots/<page>.json`, and taking its shots. */
namespace CShotPage {
/** The --shoot-scenario value for shots outside any scenario. */
inline const QString kBaseScenario = QStringLiteral("-");

/** @brief Set a property and read it back: setProperty() does not say whether the value took. */
bool driveProperty(QObject* target, const QString& property, const QVariant& value);

/** @return the canvas' view as a `view` step: the centre in degrees and the zoom level */
QJsonObject viewOf(const CCanvas* canvas);

/**
   @brief The window arrangement as a `layout` step.

   No saveGeometry(): the window size belongs to the shot's `size` alone.
 */
QJsonObject layoutOf(const CMainWindow& main);

/** @return false when the canvas did not take @p view */
bool applyView(CCanvas* canvas, const QJsonObject& view);

/** @return the failures of one shot */
qint32 shootOne(const QJsonObject& shot, CShotContext& ctx);

/**
   @brief Take the shots of a shot file.

   @param only      an id glob; empty takes every shot
   @param scenario  the shots of this scenario, kBaseScenario for those of none, empty for all
   @return the failures; a file that cannot be read or a filter that matches nothing is one
 */
qint32 run(const QString& file, CShotContext& ctx, const QString& only, const QString& scenario);
}  // namespace CShotPage

#endif  // CSHOTPAGE_H
