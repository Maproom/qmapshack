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

#include "plot/CPlotTrack.h"
#include "gis/trk/CGisItemTrk.h"
#include "canvas/IDrawContext.h"

#include <QtWidgets>
#include <proj_api.h>

CPlotTrack::CPlotTrack(QWidget *parent)
    : QWidget(parent)
    , needsRedraw(true)
    , trk(0)
    , xoff(0)
    , yoff(0)
{    
}

CPlotTrack::~CPlotTrack()
{

}

void CPlotTrack::setTrack(CGisItemTrk * track)
{
    trk = track;

    updateData();
}

void CPlotTrack::updateData()
{
    if(trk == 0)
    {
        return;
    }

    const QRectF& boundingRect = trk->getBoundingRect();
    QRectF r = buffer.rect();
    r.adjust(5,5,-5,-5);

    topLeft = boundingRect.topLeft() * RAD_TO_DEG;
    if(qAbs(boundingRect.width()) > qAbs(boundingRect.height() * 2))
    {
        scale.rx()  = r.width() / boundingRect.width();
        scale.ry() = -scale.x() * 2;
    }
    else
    {
        scale.ry() = r.height() / boundingRect.height();
        scale.rx() = -scale.y() / 2;
    }
    scale      /= RAD_TO_DEG;

    line.clear();
    const CGisItemTrk::trk_t& t = trk->getTrackData();
    foreach (const CGisItemTrk::trkseg_t& seg, t.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
        {
            if(trkpt.flags & CGisItemTrk::trkpt_t::eDeleted)
            {
                continue;
            }

            line << (QPointF(trkpt.lon, trkpt.lat) - topLeft) * scale;
        }
    }

    xoff = qRound((buffer.width()  - boundingRect.width()  * scale.x() * RAD_TO_DEG) / 2);
    yoff = qRound((buffer.height() - boundingRect.height() * scale.y() * RAD_TO_DEG) / 2);

    needsRedraw = true;
}

void CPlotTrack::setMouseMoveFocus(qreal lon, qreal lat)
{
    pos.rx() = lon;
    pos.ry() = lat;

    pos = (pos - topLeft) * scale;
    update();
}

void CPlotTrack::resizeEvent(QResizeEvent * e)
{
    QSize s = e->size();
    setMinimumWidth(s.height());

    buffer = QImage(s.height(), s.height(), QImage::Format_ARGB32);

    updateData();
}

void CPlotTrack::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    USE_ANTI_ALIASING(p, true);

    if(needsRedraw)
    {
        draw();
        needsRedraw = false;
    }

    p.drawImage(0,0,buffer);
    p.translate(xoff, yoff);
    p.setPen(Qt::red);
    p.setBrush(Qt::red);
    p.drawEllipse(pos,5,5);
}

void CPlotTrack::draw()
{
    QPainter p(&buffer);
    USE_ANTI_ALIASING(p, true);
    p.setPen(Qt::black);
    p.setBrush(Qt::white);
    p.drawRect(buffer.rect());

    p.translate(xoff,yoff);
    p.setPen(QPen(Qt::darkBlue,2));
    p.drawPolyline(line);
}
