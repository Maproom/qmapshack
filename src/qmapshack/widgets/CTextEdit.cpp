/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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

#include "widgets/CTextEdit.h"

#include <QtWidgets>

CTextEdit::CTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
}

void CTextEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Paste))
    {
        event->ignore();
        paste();
    }
    else
    {
        QTextEdit::keyPressEvent(event);
    }
}

void CTextEdit::paste()
{
    if(pastePlain)
    {
        QClipboard *clip = QApplication::clipboard();
        insertPlainText( clip->text() );
    }
    else
    {
        QTextEdit::paste();
    }
}

void CTextEdit::setPastePlain(bool plain)
{
    pastePlain = plain;
}
