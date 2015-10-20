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

#include "canvas/CCanvas.h"
#include "mouse/IMouse.h"
#include <gis/CGisDraw.h>

#include <QtWidgets>

IMouse::IMouse(CGisDraw *gis, CCanvas *canvas)
    : QObject(canvas)
    , gis(gis)
    , canvas(canvas)
{
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(50);

    connect(timer, SIGNAL(timeout()), this, SLOT(slotPanCanvas()));
}

IMouse::~IMouse()
{
}

void IMouse::setMouseTracking(bool enabled)
{
    canvas->setMouseTracking(enabled);
    if(!enabled)
    {
        timer->stop();
    }
}

void IMouse::slotPanCanvas()
{
    panCanvas(point);
}

#define SENSITIVE_FRAME 100
#define DAMPING_FACTOR  0.25
void IMouse::panCanvas(const QPoint& pos)
{
    if(pos.x() < SENSITIVE_FRAME)
    {
        int d = (SENSITIVE_FRAME - pos.x()) * DAMPING_FACTOR;
        canvas->moveMap(QPointF(d/2, 0));
        timer->start();
    }
    else if(pos.x() > canvas->width() - SENSITIVE_FRAME)
    {
        int d = (canvas->width() - SENSITIVE_FRAME - pos.x()) * DAMPING_FACTOR;
        canvas->moveMap(QPointF(d/2, 0));
        timer->start();
    }
    else if(pos.y() < SENSITIVE_FRAME)
    {
        int d = (SENSITIVE_FRAME - pos.y()) * DAMPING_FACTOR;
        canvas->moveMap(QPointF(0, d/2));
        timer->start();
    }
    else if(pos.y() > canvas->height() - SENSITIVE_FRAME)
    {
        int d = (canvas->height() - SENSITIVE_FRAME - pos.y()) * DAMPING_FACTOR;
        canvas->moveMap(QPointF(0, d/2));
        timer->start();
    }
    else
    {
        timer->stop();
    }

    canvas->update();
}
