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

#include "gis/IGisLine.h"
#include "gis/CGisDraw.h"
#include "dem/CDemDraw.h"

IGisLine::subpt_t::subpt_t()
    : ele(NOINT)
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
        pt.ele = dem->getElevationAt(pt.coord);

        for(int n = 0; n < pt.subpts.size(); n++)
        {
            IGisLine::subpt_t& sub = pt.subpts[n];
            sub.ele = dem->getElevationAt(sub.coord);
        }
    }
}

IGisLine::IGisLine()
{
}

IGisLine::~IGisLine()
{
}

