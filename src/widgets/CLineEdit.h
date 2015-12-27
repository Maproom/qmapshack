/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

/** @brief A QWidget derived from QLineEdit, aiming to provide a simple-to-use wrapper around QLineEdit

    By default, a readonly CLineEdit looks like an ordinary label, whereas a non-readonly
    CLineEdit contains blue, underlined text. As soon as the non-readonly widget receives
    the focus, the color is changed to black, the underline disappears and the modifiable text
    is selected.
 */

#ifndef CLINEEDIT_H
#define CLINEEDIT_H

#include <QFont>
#include <QPalette>
#include <QLineEdit>

class CLineEdit : public QLineEdit
{
    Q_OBJECT

private:
    bool initialized;

    QPalette paletteEdit;
    QPalette paletteRO;
    QPalette paletteRW;

    QFont fontNoUnderline;
    QFont fontUnderline;

    void initialize();
    void updateStyle();

public slots:
    void slotSelectAll()
    {
        selectAll();
    }

public:
    CLineEdit(QWidget *parent = nullptr);

    void setReadOnly(bool r);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
};
#endif // CLINEEDIT_H
