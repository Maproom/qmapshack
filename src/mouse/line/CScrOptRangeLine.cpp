/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "CScrOptRangeLine.h"

CScrOptRangeLine::CScrOptRangeLine(const QPointF &point, IMouse *mouse, QWidget * parent)
    : IScrOpt(mouse)
{
    if(parent != 0)
    {
        setParent(parent);
    }

    setupUi(this);
//    label->setFont(CMainWindow::self().getMapFont());
//    label->setText(trk->getInfoRange());
    adjustSize();

    setOrigin(point.toPoint());

    move(point.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();
}

CScrOptRangeLine::~CScrOptRangeLine()
{
}

void CScrOptRangeLine::draw(QPainter& p)
{
    drawBubble2(origin, p);
}
