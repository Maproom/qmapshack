/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#include "CRawPoi.h"
#include "gis/Poi.h"

#include <QRegularExpression>

CRawPoi::CRawPoi(QStringList data, QPointF coordinates, quint64 key, const QString& category)
    : category(category), coordinates(coordinates), data(data), key(key)
{
    for(const QRegularExpression& regex : {QRegularExpression("name:" + QLocale::system().name() + "=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("name:en=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("name=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("name:\\w\\w=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("brand=(.+)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("operator=(.+)", QRegularExpression::UseUnicodePropertiesOption)})
    {
        const QStringList& matches = data.filter(regex);
        if (!matches.isEmpty())
        {
            name = regex.match(matches[0]).captured(1).trimmed();
            break;
        }
    }
}

const QString &CRawPoi::getCategory() const
{
    return category;
}

const QPointF &CRawPoi::getCoordinates() const
{
    return coordinates;
}

const QStringList &CRawPoi::getData() const
{
    return data;
}

const quint64 &CRawPoi::getKey() const
{
    return key;
}

const QString &CRawPoi::getName(bool replaceEmptyByCategory) const
{
    if(replaceEmptyByCategory && name.isEmpty())
    {
        return category;
    }
    return name;
}

poi_t CRawPoi::toPoi() const
{
    poi_t poi;
    poi.pos = coordinates;
    poi.name = getName();
    poi.desc = data.join("<br>\n");
    return poi;
}

