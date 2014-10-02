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

void CPlotTrack::setTrack(CGisItemTrk * track, const QString& proj)
{
    trk = track;
    pjsrc = pj_init_plus(proj.toLatin1());

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

    pt1 = boundingRect.topLeft();
    pt2 = boundingRect.bottomRight();

    pj_transform(pjtar, pjsrc, 1, 0, &pt1.rx(), &pt1.ry(), 0);
    pj_transform(pjtar, pjsrc, 1, 0, &pt2.rx(), &pt2.ry(), 0);

    qreal w = pt2.x() - pt1.x();
    qreal h = pt2.y() - pt1.y();

    if(qAbs(w) > qAbs(h))
    {
        scale.rx() = r.width() / w;
        scale.ry() = -scale.x();
    }
    else
    {
        scale.ry() = r.height() / h;
        scale.rx() = -scale.y();
    }

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

            QPointF pt(trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
            pj_transform(pjtar, pjsrc, 1, 0, &pt.rx(), &pt.ry(), 0);

            line << (pt - pt1) * scale;
        }
    }

    xoff = qRound((buffer.width()  - w * scale.x()) / 2);
    yoff = qRound((buffer.height() - h * scale.y()) / 2);



    needsRedraw = true;
}

void CPlotTrack::setMouseMoveFocus(qreal lon, qreal lat)
{
    pos.rx() = lon * DEG_TO_RAD;
    pos.ry() = lat * DEG_TO_RAD;

    pj_transform(pjtar, pjsrc, 1, 0, &pos.rx(), &pos.ry(), 0);
    pos = (pos - pt1) * scale;
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
