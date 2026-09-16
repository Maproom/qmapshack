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

#include "shoot/CShotInput.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QWidget>

#include "shoot/CShotAddress.h"

bool CShotInput::isCellEditor(const QWidget* input) {
  const QAbstractItemView* view = CShotAddress::itemViewOf(input->parentWidget());
  return nullptr != view && view->indexWidget(view->indexAt(input->geometry().center())) != input;
}

QString CShotInput::keyValueOf(const QWidget* input) {
  if (const QSpinBox* spin = qobject_cast<const QSpinBox*>(input); nullptr != spin) {
    return spin->cleanText();
  }
  if (const QDoubleSpinBox* spin = qobject_cast<const QDoubleSpinBox*>(input); nullptr != spin) {
    return spin->cleanText();
  }
  if (const QAbstractSpinBox* spin = qobject_cast<const QAbstractSpinBox*>(input); nullptr != spin) {
    return spin->text();
  }
  const QLineEdit* edit = qobject_cast<const QLineEdit*>(input);
  return (nullptr == edit) ? QString() : edit->text();
}
