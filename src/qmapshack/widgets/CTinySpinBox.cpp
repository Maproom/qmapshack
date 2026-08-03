/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler <code@christian-eichler.de>

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

#include "widgets/CTinySpinBox.h"

#include <QtWidgets>

#include "theme/CUiTheme.h"

CTinySpinBox::CTinySpinBox(QWidget* parent) : QSpinBox(parent) {}

void CTinySpinBox::updateStyle() {
  applyingStyle = true;

  if (isReadOnly() || hasFocus()) {
    // a default-constructed palette/font resolves to nothing, so both fall back to inheritance
    setPalette(QPalette());
    setFont(QFont());
  } else {
    QPalette pal;
    pal.setColor(QPalette::Text, CUiTheme::foreground(CUiTheme::Role::eInfo));
    setPalette(pal);

    QFont f;
    f.setUnderline(true);
    setFont(f);
  }

  applyingStyle = false;
}

void CTinySpinBox::changeEvent(QEvent* event) {
  QSpinBox::changeEvent(event);

  // setPalette() in updateStyle() re-enters with the same event - hence the guard.
  if (!applyingStyle && CUiTheme::isPaletteChange(event)) {
    updateStyle();
  }
}

void CTinySpinBox::stepBy(int steps) {
  QSpinBox::stepBy(steps);
  emit valueChangedByStep(value());
}

void CTinySpinBox::setReadOnly(bool r) {
  QSpinBox::setReadOnly(r);
  updateStyle();
}

void CTinySpinBox::focusInEvent(QFocusEvent* event) {
  updateStyle();
  if (!isReadOnly()) {
    QTimer::singleShot(0, this, &CTinySpinBox::slotSelectAll);
  }

  QSpinBox::focusInEvent(event);
}

void CTinySpinBox::focusOutEvent(QFocusEvent* event) {
  updateStyle();
  QSpinBox::focusOutEvent(event);
}
