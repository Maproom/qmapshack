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

#include "dem/CDemDraw.h"
#include "gis/CGisDraw.h"
#include "gis/IGisLine.h"


IGisLine::subpt_t::subpt_t(const QPointF& pt)
    : coord(pt)
{
}

IGisLine::point_t::point_t(const QPointF& pt)
{
    coord = pt;
    pixel = pt;
}

void IGisLine::point_t::resetElevation()
{
    ele = NOINT;
    for(int i = 0; i < subpts.size(); i++)
    {
        subpts[i].ele = NOINT;
    }
}

void SGisLine::updateElevation(CDemDraw * dem)
{
    for(int i = 0; i < size(); i++)
    {
        IGisLine::point_t& pt = (*this)[i];
        qreal ele = dem->getElevationAt(pt.coord);
        pt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);

        for(int n = 0; n < pt.subpts.size(); n++)
        {
            IGisLine::subpt_t& sub = pt.subpts[n];
            qreal ele = dem->getElevationAt(pt.coord);
            sub.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);
        }
    }
}


void SGisLine::updatePixel(CGisDraw * gis)
{
    for(int i = 0; i < size(); i++)
    {
        IGisLine::point_t& pt = (*this)[i];

        pt.pixel = pt.coord;
        gis->convertRad2Px(pt.pixel);

        for(int n = 0; n < pt.subpts.size(); n++)
        {
            IGisLine::subpt_t& sub = pt.subpts[n];

            sub.pixel = sub.coord;
            gis->convertRad2Px(sub.pixel);
        }
    }
}

