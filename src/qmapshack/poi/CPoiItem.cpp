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

#include "../common/gis/proj_x.h"
#include "gis/gpx/serialization.h"
#include "poi/CPoiItem.h"

CPoiItem::CPoiItem(const QDomNode& gpx) : gpxMode(true), gpx(gpx)
{
    const QDomNamedNodeMap& attr = gpx.attributes();
    pos = QPointF(attr.namedItem("lon").nodeValue().toDouble(), attr.namedItem("lat").nodeValue().toDouble()) * DEG_TO_RAD;
    readXml(gpx, "name", name);
    readXml(gpx, "desc", desc);
    readXml(gpx, "sym", icon);
    readXml(gpx, "type", category);
    if(category.isEmpty())
    {
        category = icon;
    }
    if(icon == "Geocache")
    {
        // special treatment for geocaches, as we want to show the icon
        icon = category.split("|").last();
    }
}

QString CPoiItem::getName() const
{
    bool fallback = true;
    return getName(fallback);
}

QString CPoiItem::getName(bool& fallback) const
{
    if(name.isEmpty() && fallback)
    {
        fallback = true;
        return getCategory();
    }
    fallback = false;
    return name;
}

QString CPoiItem::getCategory() const
{
    bool fallback = true;
    return getCategory(fallback);
}

QString CPoiItem::getCategory(bool& fallback) const
{
    if(category.isEmpty() && fallback)
    {
        fallback = true;
        return getIcon();
    }
    fallback = false;
    return category;
}

uint CPoiItem::getKey() const
{
    return qHash(*this, 42);
}
