/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "canvas/CCanvas.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/IMouse.h"

IMouse::IMouse(CGisDraw * gis, CCanvas * canvas, CMouseAdapter * mouse)
    : QObject(mouse),
    canvas(canvas),
    gis(gis),
    mouse(mouse)
{
}

IMouse::~IMouse()
{
}

void IMouse::mouseDragged(const QPoint &start, const QPoint &last, const QPoint &end)
{
    canvas->moveMap(end - last);
}

void IMouse::rightButtonDown(const QPoint &pos)
{
    canvas->resetMouse();
    canvas->update();
}

void IMouse::startMouseMove(const QPoint &pos)
{
    mouse->startMouseMove(pos);
}
