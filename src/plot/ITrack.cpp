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

#include "gis/trk/CGisItemTrk.h"
#include "helpers/CDraw.h"
#include "plot/ITrack.h"

#include <QtWidgets>

ITrack::ITrack()
{
    pjtar = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");
}

ITrack::~ITrack()
{
    if(pjtar)
    {
        pj_free(pjtar);
    }
    if(pjsrc)
    {
        pj_free(pjsrc);
    }
}

void ITrack::save(QImage& image)
{
    setSize(image.width(), image.height());
    draw();
    image = buffer;
}

void ITrack::setSize(int w, int h)
{
    buffer = QImage(w, h, QImage::Format_ARGB32);
    updateData();
}

void ITrack::setupProjection(const QRectF& boundingBox)
{
    if(pjsrc)
    {
        pj_free(pjsrc);
        pjsrc = nullptr;
    }

    if(boundingBox.top() > (60*DEG_TO_RAD))
    {
        pjsrc = pj_init_plus("+init=epsg:32661");
    }
    else if(boundingBox.bottom() < (-60*DEG_TO_RAD))
    {
        pjsrc = pj_init_plus("+init=epsg:32761");
    }
    else
    {
        pjsrc = pj_init_plus("+init=epsg:3857");
    }
}

void ITrack::setTrack(CGisItemTrk * track)
{
    trk = track;

    setupProjection(trk->getBoundingRect());

    updateData();
}

void ITrack::setTrack(const QPolygonF& track)
{
    coords = track;

    setupProjection(coords.boundingRect());

    updateData();
}

void ITrack::updateData()
{
    if((pjsrc == nullptr) || (nullptr == trk && coords.isEmpty()))
    {
        return;
    }

    if(trk)
    {
        coords.clear();
        const trk_t& t = trk->getTrackData();
        for(const trkpt_t& trkpt : t)
        {
            if(trkpt.isHidden())
            {
                continue;
            }

            coords << trkpt.radPoint();
        }
    }

    line.clear();
    for(const QPointF &trkpt : coords)
    {
        QPointF pt(trkpt.x(), trkpt.y());
        pj_transform(pjtar, pjsrc, 1, 0, &pt.rx(), &pt.ry(), 0);
        line << pt;
    }

    QRectF r1 = line.boundingRect();
    qreal w1 = r1.width();
    qreal h1 = r1.height();

    QRectF r2 = buffer.rect();
    qreal w2 = r2.width();
    qreal h2 = r2.height();

    if(qAbs(w1) > qAbs(h1))
    {
        scale.rx() = (w2 - 10) / w1;
        scale.ry() = -scale.x();
        xoff = 0;
        yoff = -((h2 - 10)/scale.y() + h1) / 2;
    }
    else
    {
        scale.ry() = (-h2 + 10) / h1;
        scale.rx() = -scale.y();
        xoff = -((w2 - 10)/scale.x() - w1) / 2;
        yoff = 0;
    }

    xoff += r1.left()   - 5/scale.x();
    yoff += r1.bottom() - 5/scale.y();

    needsRedraw = true;
}


void ITrack::draw(QPainter& p)
{
    if(needsRedraw)
    {
        draw();
        needsRedraw = false;
    }

    p.drawImage(0, 0, buffer);
}

void ITrack::draw()
{
    buffer.fill(Qt::transparent);
    QPainter p(&buffer);
    USE_ANTI_ALIASING(p, true);


    p.setPen(CDraw::penBorderBlack);
    p.setBrush(QColor(255, 255, 255, 255));
    PAINT_ROUNDED_RECT(p, buffer.rect().adjusted(1,1,-1,-1));

    p.setPen(QPen(Qt::darkBlue, 2/scale.x()));
    p.scale(scale.x(), scale.y());
    p.translate(-xoff, -yoff);
    p.drawPolyline(line);
}
