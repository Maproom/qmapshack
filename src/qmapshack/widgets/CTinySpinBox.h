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

/** @brief A QWidget derived from QSpinBox, aiming to be tiny with regard to spaced consumed
           on screen.

    By default, a readonly CTinySpinBox looks like an ordinary label, whereas a non-readonly
    CTinySpinBox contains underlined text in the Info colour. As soon as the non-readonly widget
    receives the focus, the text falls back to the palette, the underline disappears and the
    modifiable text is selected.
 */

#ifndef CTINYSPINBOX_H
#define CTINYSPINBOX_H

#include <QSpinBox>

class CTinySpinBox : public QSpinBox {
  Q_OBJECT

 private:
  /** @brief True while updateStyle() applies its palette and font, to break the event feedback */
  bool applyingStyle = false;

  void updateStyle();

 public slots:
  void slotSelectAll() { selectAll(); }

 signals:
  void valueChangedByStep(int val);

 public:
  CTinySpinBox(QWidget* parent = nullptr);

  void setReadOnly(bool r);
  void stepBy(int steps) override;

 protected:
  void changeEvent(QEvent* event) override;
  void focusInEvent(QFocusEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;
};
#endif  // CTINYSPINBOX_H
