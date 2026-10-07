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

/** @return false when the main window refuses the `layout` step's saveState() */
bool applyLayout(CMainWindow& main, const QJsonObject& layout);

/**
   @brief Apply a `layout` step's tab index and splitter states.

   Before the steps: they are taken when the recording starts, so applied after the steps they undo one. An index past a
   tab widget's last page is dropped, so the arrangement names only what exists before the steps.

   @return the failures
 */
qint32 applyArrangement(CMainWindow& main, const QJsonObject& layout);

/** @return false when the canvas did not take @p view */
bool applyView(CCanvas* canvas, const QJsonObject& view);

/**
   @brief Take one shot, in its scenario when it names one and that is not CShotContext::liveScenario().

   @param scenarios  the shot file's recordings by name
   @return the failures
 */
qint32 shootOne(const QJsonObject& shot, CShotContext& ctx, const QJsonObject& scenarios = QJsonObject());

/**
   @brief Take the shots of a shot file.

   @param only      an id glob; empty takes every shot
   @param scenario  the shots of this scenario, kBaseScenario for those of none, empty for all when none of the
                    shots @p only matches is in a scenario
   @return the failures; a file that cannot be read, a filter that matches nothing, or an empty @p scenario matching
           a shot in a scenario is one
 */
qint32 run(const QString& file, CShotContext& ctx, const QString& only, const QString& scenario);
}  // namespace CShotPage

#endif  // CSHOTPAGE_H
