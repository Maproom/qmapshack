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

#include "widgets/CLineEdit.h"

#include <QtWidgets>

#include "theme/CUiTheme.h"

CLineEdit::CLineEdit(QWidget* parent) : QLineEdit(parent) { updateStyle(); }

void CLineEdit::updateStyle() {
  applyingStyle = true;

  // A default-constructed font resolves to nothing, so clearing first makes font() report what is
  // inherited. The point size has to be read after that: taken from our own previous override the
  // bump would compound on every pass.
  setFont(QFont());

  const bool editable = !isReadOnly() && !hasFocus();
  const int inheritedPointSize = font().pointSize();

  QFont f;
  f.setBold(true);
  if (inheritedPointSize > 0) {  // -1 for a font sized in pixels
    f.setPointSize(inheritedPointSize + 1);
  }
  f.setUnderline(editable);
  setFont(f);

  // The field has to read as a label, so Base and Window are transparent in every colour group and
  // the panel behind it shows through. It belongs here and not in a .ui <palette>: setPalette()
  // replaces the widget's override wholesale, so anything the designer set would be dropped on the
  // first state change. Only these roles and Text are frozen; every other role keeps following the
  // application palette.
  QPalette pal;
  for (const QPalette::ColorGroup group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
    pal.setBrush(group, QPalette::Base, Qt::transparent);
    pal.setBrush(group, QPalette::Window, Qt::transparent);
  }
  if (editable) {
    pal.setColor(QPalette::Text, CUiTheme::foreground(CUiTheme::Role::eInfo));
  }
  setPalette(pal);

  applyingStyle = false;
}

void CLineEdit::changeEvent(QEvent* event) {
  QLineEdit::changeEvent(event);

  // setPalette()/setFont() in updateStyle() re-enter with the same event - hence the guard.
  // FontChange matters as well: the point size is derived from the inherited font.
  if (!applyingStyle && (CUiTheme::isPaletteChange(event) || event->type() == QEvent::FontChange)) {
    updateStyle();
  }
}

void CLineEdit::setReadOnly(bool r) {
  QLineEdit::setReadOnly(r);
  updateStyle();
}

void CLineEdit::focusInEvent(QFocusEvent* event) {
  QLineEdit::focusInEvent(event);

  updateStyle();
  if (!isReadOnly()) {
    QTimer::singleShot(0, this, &CLineEdit::slotSelectAll);
  }
}

void CLineEdit::focusOutEvent(QFocusEvent* event) {
  QLineEdit::focusOutEvent(event);
  updateStyle();
}
