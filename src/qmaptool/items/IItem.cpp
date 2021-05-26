/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/IDrawContext.h"
#include "CMainWindow.h"
#include "items/IItem.h"

#include <QtWidgets>

IItem::IItem(const QString& filename)
    : filename(filename)
{
}

void IItem::saveSettings(QSettings& cfg)
{
    if(nullptr != drawContext)
    {
        drawContext->saveSettings(cfg);
    }
}

void IItem::loadSettings(QSettings& cfg)
{
    if(nullptr != drawContext)
    {
        drawContext->loadSettings(cfg);
    }
}

void IItem::reload()
{
    if(nullptr == drawContext)
    {
        return;
    }
    drawContext->setSourceFile(filename, false);
}

bool IItem::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    if(nullptr == drawContext)
    {
        return false;
    }

    drawContext->draw(p, needsRedraw);
    return true;
}

void IItem::mousePressEventFx(QMouseEvent* e)
{
    if(nullptr == drawContext)
    {
        return;
    }

    if(e->button() == Qt::LeftButton)
    {
        lastPos = e->pos();
        firstPos = lastPos;
        mapIsMoving = true;
        mapDidMove = false;
    }
}

void IItem::mouseMoveEventFx(QMouseEvent* e)
{
    if(nullptr == drawContext)
    {
        return;
    }

    const QPoint& point = e->pos();
    if(mapIsMoving)
    {
        if((point - firstPos).manhattanLength() >= 4)
        {
            drawContext->move(point - lastPos);

            lastPos = point;
            mapDidMove = true;

            drawContext->triggerCompleteUpdate(CCanvas::eRedrawMap);
        }
    }
}

void IItem::mouseReleaseEventFx(QMouseEvent* e)
{
    if(nullptr == drawContext)
    {
        return;
    }

    if(e->button() == Qt::LeftButton)
    {
        lastPos = e->pos();
        mapIsMoving = false;
        mapDidMove = false;
    }
}

void IItem::wheelEventFx(QWheelEvent* e)
{
    if(nullptr == drawContext)
    {
        return;
    }

    // angleDelta() returns the eighths of a degree
    // of the mousewheel
    // -> zoom in/out every 15 degress = every 120 eights
    const int EIGHTS_ZOOM = 15 * 8;
    zoomAngleDelta += e->angleDelta().y();
    if(abs(zoomAngleDelta) < EIGHTS_ZOOM)
    {
        return;
    }

    zoomAngleDelta = 0;

    drawContext->zoom(CMainWindow::self().flipMouseWheel() ? (e->delta() < 0) : (e->delta() > 0), e->posF());
    drawContext->triggerCompleteUpdate(CCanvas::eRedrawAll);
}

