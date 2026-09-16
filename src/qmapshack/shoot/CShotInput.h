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

#ifndef CSHOTINPUT_H
#define CSHOTINPUT_H

#include <QString>

class QWidget;

/**
   @brief What typing into an input leaves in it, and which inputs a replay cannot reach.
 */
namespace CShotInput {
/** @return true for an item view's cell editor, deleted when the edit ends; an index widget is none */
bool isCellEditor(const QWidget* input);

/** @return what a `key` step types into @p input once all of it is selected; a spin box's without prefix and suffix */
QString keyValueOf(const QWidget* input);
}  // namespace CShotInput

#endif  // CSHOTINPUT_H
