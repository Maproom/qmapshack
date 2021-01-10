/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "poi/CPoiDraw.h"
#include "poi/CPoiPOI.h"

#include <QtWidgets>

CPoiPOI::CPoiPOI(const QString &filename, CPoiDraw *parent)
    : IPoi(parent)
    , filename(filename)
{
    // Set true if the file could be open and loaded successfully
    // If not set true the system will take care to destroy this object
    isActivated = true;
}


void CPoiPOI::draw(IDrawContext::buffer_t& buf)
{
    // !!!! NOTE !!!!
    // This is running in it's own thread, not the main thread.
    // Use CPoiPOI::mutex whenever you access data shared between
    // the main thread and this thread. Do not block it more than
    // neccessary.


    // do that on a regular base while you draw the buffer
    // to abort drawing ass soon as possible if there is
    // another draw request pending
    if(poi->needsRedraw())
    {
        return;
    }

    QPainter p(&buf.image);
    USE_ANTI_ALIASING(p, true);
    p.setOpacity(getOpacity() / 100.0);

    QPointF pp = buf.ref1;
    poi->convertRad2Px(pp);
    p.translate(-pp);

    // draw POI
    QPointF myPoi(12.329699 * DEG_TO_RAD, 45.437951 * DEG_TO_RAD);
    poi->convertRad2Px(myPoi);
    QPixmap icon("://icons/waypoints/32x32/FlagBlue.png");
    p.drawPixmap(myPoi, icon);
}
