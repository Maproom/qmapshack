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

#include "CScrOptRange.h"

#include <QtWidgets>

CScrOptRange::CScrOptRange(const QPointF &point, QWidget *parent)
    : IScrOpt(parent)
{
    setupUi(this);
    setOrigin(point.toPoint());

    move(point.toPoint() + QPoint(SCR_OPT_OFFSET,SCR_OPT_OFFSET));
    adjustSize();
}

CScrOptRange::~CScrOptRange()
{


}

void CScrOptRange::draw(QPainter& p)
{
    drawBubble(origin, p);
}
