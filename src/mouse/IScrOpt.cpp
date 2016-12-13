/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "helpers/CDraw.h"
#include "mouse/IMouse.h"
#include "mouse/IScrOpt.h"
#include "units/IUnit.h"
#include <QtWidgets>


IScrOpt::IScrOpt(IMouse *mouse)
    : QWidget(mouse->getCanvas())
    , mouse(mouse)
{
    setFocusPolicy(Qt::WheelFocus);
}

IScrOpt::~IScrOpt()
{
    if(hasFocus() && !mouse.isNull())
    {
        CCanvas::setOverrideCursor(*mouse,"IScrOpt::~IScrOpt");
    }
}

void IScrOpt::mouseMoveEvent(QMouseEvent * e)
{
    mousePos = e->pos();
}

void IScrOpt::enterEvent(QEvent * e)
{
    QWidget::enterEvent(e);
    CCanvas::restoreOverrideCursor("IScrOpt::enterEvent");
}

void IScrOpt::leaveEvent(QEvent * e)
{
    QWidget::leaveEvent(e);
    if(!mouse.isNull())
    {
        CCanvas::setOverrideCursor(*mouse,"IScrOpt::leaveEvent");
    }
}

