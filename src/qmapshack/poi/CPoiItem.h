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

#ifndef CPOIITEM_H
#define CPOIITEM_H

#include "gis/IGisItem.h"
#include "units/IUnit.h"
#include <QException>
#include <QPointF>
#include <QSize>

class CPoiItem
{
public:
    CPoiItem() : pos(NOPOINTF){}
    CPoiItem(QDomNode gpx) : gpxMode(true), gpx(gpx), pos(NOPOINTF){}
    CPoiItem(const QString& name, const QString& desc, const QPointF& pos, const QString& icon = "", const QList<IGisItem::link_t>& links = QList<IGisItem::link_t>(), quint32 ele = NOINT)
        : gpxMode(false), name(name), desc(desc), pos(pos), icon(icon), links(links), ele(ele){}
    virtual ~CPoiItem(){}

    // no const refs for getters, as methods in derived classes may create return values on the fly
    virtual bool getGpxMode() const {return gpxMode;}
    virtual QDomNode getGpx() const {return gpx;}
    virtual QString getName() const {return name;}
    virtual QString getDesc() const {return desc;}
    /// Get position in radians
    virtual QPointF getPos() const {return pos;}
    virtual QString getIcon() const {return icon;}
    virtual QList<IGisItem::link_t> getLinks() const {return links;}
    virtual quint32 getEle() const {return ele;}

protected:
    bool gpxMode = false;
    QDomNode gpx;

    QString name;
    QString desc;
    /// in radians
    QPointF pos;
    QString icon;
    QList<IGisItem::link_t> links;
    quint32 ele = NOINT;
};


inline bool operator==(const CPoiItem& poi1, const CPoiItem& poi2)
{
    if(poi1.getGpxMode() != poi2.getGpxMode())
    {
        return false;
    }
    else if (poi1.getGpxMode())
    {
        return poi1.getGpx() == poi2.getGpx();
    }
    else
    {
        return poi1.getName() == poi2.getName()
               && poi1.getDesc() == poi2.getDesc()
               && poi1.getPos() == poi2.getPos();
    }
}

inline uint qHash(const CPoiItem& poi, uint seed)
{
    return qHash(poi.getName(), seed) ^ qHash(poi.getDesc(), seed) ^ qHash(poi.getPos().x(), seed) ^ qHash(poi.getPos().y(), seed);
}
#endif //CPOIITEM_H

