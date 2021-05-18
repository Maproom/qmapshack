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

#ifndef POI_H
#define POI_H

#include "IGisItem.h"
#include "units/IUnit.h"
#include <QPointF>
#include <QSize>

struct poi_t
{
    poi_t() : pos(NOPOINTF){}
    QString name;
    QString desc;
    /// in radians
    QPointF pos;
    QString icon;
    QList<IGisItem::link_t> links;
    quint32 ele = NOINT;
};

inline bool operator==(const poi_t& poi1, const poi_t& poi2)
{
    return poi1.name == poi2.name
           && poi1.desc == poi2.desc
           && poi1.pos == poi2.pos;
}

inline uint qHash(const poi_t& poi, uint seed)
{
    return qHash(poi.name, seed) ^ qHash(poi.desc, seed) ^ qHash(poi.pos.x(), seed) ^ qHash(poi.pos.y(), seed);
}

#endif //POI_H

