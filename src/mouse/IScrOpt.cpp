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

#include "IScrOpt.h"
#include <QtWidgets>


IScrOpt::IScrOpt(QObject *parent)
    : QObject(parent)
    , doSpecialCursor(false)
{

}

IScrOpt::~IScrOpt()
{

}

void IScrOpt::clear()
{
    if(doSpecialCursor)
    {
        QApplication::restoreOverrideCursor();
        doSpecialCursor = false;
    }
    items.clear();
}

void IScrOpt::mouseMoveEvent(QMouseEvent * e)
{
    mousePos = e->pos();

    foreach(const item_t& item, items)
    {
        if(item.area.contains(mousePos))
        {
            if(!doSpecialCursor)
            {
                QApplication::setOverrideCursor(Qt::PointingHandCursor);
                doSpecialCursor = true;
            }
            return;
        }
    }

    if(doSpecialCursor)
    {
        QApplication::restoreOverrideCursor();
        doSpecialCursor = false;
        return;
    }
}
