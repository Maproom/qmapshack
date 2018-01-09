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

#include "widgets/CLineEdit.h"

#include <QtWidgets>

void CLineEdit::initialize()
{
    initialized  = true;

    paletteEdit = QPalette(palette());
    paletteRO   = QPalette(palette());
    paletteRW   = QPalette(palette());
    paletteRW.setColor(QPalette::Text, QColor(0, 0, 255));

    fontNoUnderline = QFont(font());
    fontUnderline   = QFont(font());
    fontUnderline.setUnderline(true);
}

void CLineEdit::updateStyle()
{
    if(!initialized)
    {
        initialize();
    }

    if(isReadOnly())
    {
        setPalette(paletteRO);
        setFont(fontNoUnderline);
    }
    else if(hasFocus())
    {
        setPalette(paletteEdit);
        setFont(fontNoUnderline);
    }
    else
    {
        setPalette(paletteRW);
        setFont(fontUnderline);
    }
}

CLineEdit::CLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    // initialization has to be done deferred,
    // as the correct palette is set after construction
    initialized = false;
}

void CLineEdit::setReadOnly(bool r)
{
    QLineEdit::setReadOnly(r);
    updateStyle();
}

void CLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);

    updateStyle();
    if(!isReadOnly())
    {
        QTimer::singleShot(0, this, SLOT(slotSelectAll()));
    }
}

void CLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    updateStyle();
}
