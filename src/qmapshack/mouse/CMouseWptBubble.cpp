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
#include "gis/CGisWorkspace.h"
#include "gis/wpt/CGisItemWpt.h"
#include "mouse/CMouseWptBubble.h"

#include <QtWidgets>

CMouseWptBubble::CMouseWptBubble(const IGisItem::key_t &key, CGisDraw * gis, CCanvas * canvas, CMouseAdapter *mouse)
    : IMouse(gis, canvas, mouse)
    , key(key)
{
    cursor = QCursor(QPixmap("://cursors/cursorArrow.png"), 0, 0);
}

CMouseWptBubble::~CMouseWptBubble()
{
}

void CMouseWptBubble::draw(QPainter&, CCanvas::redraw_e, const QRect&)
{
}

void CMouseWptBubble::leftClicked(const QPoint &pos)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
    if(wpt)
    {
        wpt->leftClicked(pos);
    }
    else
    {
        canvas->resetMouse();
    }
}

void CMouseWptBubble::mouseMoved(const QPoint &pos)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
    if(wpt)
    {
        wpt->mouseMove(pos);
    }
    else
    {
        canvas->resetMouse();
    }
}

void CMouseWptBubble::mouseDragged(const QPoint& start, const QPoint &last, const QPoint&end)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
    if(wpt)
    {
        wpt->mouseDragged(start, last, end);
    }
    else
    {
        canvas->resetMouse();
    }
}

void CMouseWptBubble::dragFinished(const QPoint& pos)
{
    QMutexLocker lock(&IGisItem::mutexItems);

    CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(CGisWorkspace::self().getItemByKey(key));
    if(wpt)
    {
        wpt->dragFinished(pos);
    }
    else
    {
        canvas->resetMouse();
    }
}
