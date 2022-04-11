/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2022 Henri Hornburg <pingurus@t-online.de>
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

#ifndef IPOIITEM_H
#define IPOIITEM_H

#include "gis/IGisItem.h"
#include "units/IUnit.h"
#include <QException>
#include <QPointF>
#include <QSize>

class IPoiItem
{
public:
    IPoiItem() = default;
    virtual ~IPoiItem() = default;

    // no const refs for getters, as methods in derived classes may create return values on the fly
    /// bool fallback=true; return getName(fallback);
    QString getName() const;
    /// if fallback is true, the usage of a fallback is allowed. After the method returned, fallback indicates whether a fallback value was used or not
    virtual QString getName(bool& fallback) const = 0;
    virtual QString getDesc() const = 0;
    /// Get position in radians (lon,lat)
    virtual QPointF getPos() const = 0;
    virtual QString getIcon() const {return "";}
    virtual QList<IGisItem::link_t> getLinks() const {return QList<IGisItem::link_t>();}
    virtual quint32 getEle() const {return NOINT;}
    /// bool fallback=true; return getCategory(fallback);
    QString getCategory() const;
    /// if fallback is true, the usage of a fallback is allowed. After the method returned, fallback indicates whether a fallback value was used or not
    virtual QString getCategory(bool& fallback) const {return "";}

    virtual uint getKey() const;
};


inline bool operator==(const IPoiItem& poi1, const IPoiItem& poi2)
{
    return poi1.getName() == poi2.getName()
           && poi1.getDesc() == poi2.getDesc()
           && poi1.getPos() == poi2.getPos();
}

inline uint qHash(const IPoiItem& poi, uint seed = 0)
{
    return qHash(poi.getName(), seed) ^ qHash(poi.getDesc(), seed) ^ qHash(poi.getPos().x(), seed) ^ qHash(poi.getPos().y(), seed);
}

#endif //IPOIITEM_H
