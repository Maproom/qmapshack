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

#ifndef CSHOTREPLAY_H
#define CSHOTREPLAY_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QtGlobal>
#include <functional>

class CShotContext;
class QWidget;

/**
   @brief Performs a recording as a queue: each step is queued from the event loop before it runs.

   A step that opens a modal dialog, a popup menu or a nested progress loop returns only once it is closed; the loop
   inside it delivers the steps after it.
 */
namespace CShotReplay {
/** A replay whose steps have not all run by then ends the process with kDeadlineExitCode. */
constexpr qint32 kDeadlineMs = 60000;
constexpr qint32 kDeadlineExitCode = 1;

/**
   @brief Perform @p steps below @p root through their handlers.

   @param whenReady  runs after the last step, while whatever a step opened is still up; returns its failures. With
   steps, popups and modal dialogs are closed after it.
   @return the failures
 */
qint32 perform(QWidget* root, const QList<QJsonObject>& steps, const std::function<qint32()>& whenReady = {});

/**
   @brief Take back what a replay of @p scenario leaves: screen options, mouse delegate, the selection's map hint and
          the mouse focus of every track a step hit.
 */
void clear(const QJsonArray& scenario, CShotContext& ctx);

/**
   @brief Clear, apply the leading `layout` with its tab and splitters and the `view`, then perform the steps.

   Nothing to perform clears nothing.

   @param whenReady  takes the picture; see perform()
   @return the failures
 */
qint32 replay(const QJsonArray& scenario, CShotContext& ctx, const std::function<qint32()>& whenReady);
}  // namespace CShotReplay

#endif  // CSHOTREPLAY_H
