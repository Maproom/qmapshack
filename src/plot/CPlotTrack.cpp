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
    , pjsrc(0)
    , pjtar(0)
    , needsRedraw(true)
    , trk(0)
    , xoff(0)
    , yoff(0)
    , pos(NOPOINTF)
{
    pjtar = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");
}

CPlotTrack::~CPlotTrack()
{
    if(pjtar) pj_free(pjtar);
    if(pjsrc) pj_free(pjsrc);

}

void CPlotTrack::setupProjection(const QRectF& boundingBox)
{
    if(pjsrc)
    {
        pj_free(pjsrc);
        pjsrc = 0;
    }

    if(boundingBox.top() > (60*DEG_TO_RAD))
    {
        pjsrc =  pj_init_plus("+init=epsg:32661");
    }
    else if(boundingBox.bottom() < (-60*DEG_TO_RAD))
    {
        pjsrc =  pj_init_plus("+init=epsg:32761");
    }
    else
    {
        pjsrc =  pj_init_plus("+init=epsg:3857");
    }
}

void CPlotTrack::setTrack(CGisItemTrk * track)
{
    trk = track;

    setupProjection(trk->getBoundingRect());

    updateData();
}

void CPlotTrack::setTrack(const QPolygonF& track)
{
    coords = track;

    setupProjection(coords.boundingRect());

    updateData();
}

void CPlotTrack::updateData()
{
    if((pjsrc == 0) || (trk == 0 && coords.isEmpty()))
    {
        return;
    }

    QRectF boundingRect;
    if(trk)
    {
        coords.clear();
        const CGisItemTrk::trk_t& t = trk->getTrackData();
        foreach (const CGisItemTrk::trkseg_t& seg, t.segs)
        {
            foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
            {
                if(trkpt.flags & CGisItemTrk::trkpt_t::eHidden)
                {
                    continue;
                }

                coords << QPointF(trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
            }
        }
    }

    line.clear();
    foreach(const QPointF& trkpt, coords)
    {
        QPointF pt(trkpt.x(), trkpt.y());
        pj_transform(pjtar, pjsrc, 1, 0, &pt.rx(), &pt.ry(), 0);
        line << pt;
    }

    QRectF r1 = line.boundingRect();
    qreal  w1 = r1.width();
    qreal  h1 = r1.height();

    QRectF r2 = buffer.rect();
    qreal  w2 = r2.width();
    qreal  h2 = r2.height();

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

void CPlotTrack::setMouseMoveFocus(qreal lon, qreal lat)
{
    pos.rx() = lon * DEG_TO_RAD;
    pos.ry() = lat * DEG_TO_RAD;

    pj_transform(pjtar, pjsrc, 1, 0, &pos.rx(), &pos.ry(), 0);

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

    p.setPen(Qt::red);
    p.setBrush(Qt::red);
    p.scale(scale.x(), scale.y());
    p.translate(-xoff, -yoff);
    p.drawEllipse(pos,5/scale.x(),5/scale.x());
}

void CPlotTrack::draw()
{
    QPainter p(&buffer);
    USE_ANTI_ALIASING(p, true);
    p.setPen(Qt::black);
    p.setBrush(Qt::white);
    p.drawRect(buffer.rect());


    p.setPen(QPen(Qt::darkBlue,2/scale.x()));
    p.scale(scale.x(), scale.y());
    p.translate(-xoff,-yoff);
    p.drawPolyline(line);
}
