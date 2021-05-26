/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "plot/CPlotTrack.h"

#include <QtWidgets>

CPlotTrack::CPlotTrack(QWidget* parent)
    : QWidget(parent)
{
}

CPlotTrack::CPlotTrack(CGisItemTrk* trk, QWidget* parent)
    : QWidget(parent)
    , pos(NOPOINTF)
{
    setTrack(trk);
}

CPlotTrack::~CPlotTrack()
{
}

void CPlotTrack::setMouseFocus(qreal lon, qreal lat)
{
    pos.rx() = lon * DEG_TO_RAD;
    pos.ry() = lat * DEG_TO_RAD;

    proj.transform(pos, PJ_INV);

    update();
}

void CPlotTrack::resizeEvent(QResizeEvent* e)
{
    QSize s = e->size();
    setMinimumWidth(s.height());
    setSize(s.height(), s.height());
}

void CPlotTrack::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    USE_ANTI_ALIASING(p, true);

    draw(p);

    p.setPen(Qt::red);
    p.setBrush(Qt::red);
    p.scale(scale.x(), scale.y());
    p.translate(-xoff, -yoff);
    p.drawEllipse(pos, 5 / scale.x(), 5 / scale.x());
}

