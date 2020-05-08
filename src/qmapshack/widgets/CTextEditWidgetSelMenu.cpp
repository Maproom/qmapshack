/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler <code@christian-eichler.de>

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


#include "CTextEditWidgetSelMenu.h"
#include "helpers/Signals.h"

#include <QtWidgets>

CTextEditWidgetSelMenu::CTextEditWidgetSelMenu(QWidget *parent,
                                               QAction *actionTextBold, QAction *actionTextItalic, QAction *actionTextUnderline,
                                               QAction *actionCut,      QAction *actionCopy,       QAction *actionPaste
                                               )
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint)
{
    setupUi(this);

    toolBold->setDefaultAction(actionTextBold);
    toolItalic->setDefaultAction(actionTextItalic);
    toolUnder->setDefaultAction(actionTextUnderline);

    toolCut->setDefaultAction(actionCut);
    toolCopy->setDefaultAction(actionCopy);
    toolPaste->setDefaultAction(actionPaste);

    QRect geo = childrenRect();
    geo.adjust(0, 0, 0, 1);
    setGeometry(geo);
}

