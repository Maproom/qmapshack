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

#include "gis/Poi.h"
#include "poi/CRawPoi.h"

#include <QDebug>
#include <QRegularExpression>

CRawPoi::CRawPoi(const QStringList& data, const QPointF& coordinates, const quint64& key, const QString& category, const QString& garminIcon)
    : category(category), coordinates(coordinates), rawData(data), garminIcon(garminIcon), key(key)
{
    for(const QString& line : data)
    {
        const QStringList& keyValue = line.split("=");
        if(keyValue[0].contains("wikipedia", Qt::CaseInsensitive))
        {
            wikipediaRelatedKeys += keyValue[0];
        }
        if(keyValue[0].contains("wikidata", Qt::CaseInsensitive))
        {
            wikidataRelatedKeys += keyValue[0];
        }
        if(keyValue[0] == "name" || keyValue[0].contains("name:\\w\\w", Qt::CaseInsensitive) || keyValue[0].contains(QRegularExpression("[A-z]+_name")))
        {
            nameRelatedKeys += keyValue[0];
        }
        this->data[keyValue[0]] = keyValue[1];
    }

    //find name
    //TODO doesn't match official_name without other name and such stuff
    for(const QRegularExpression& regex : {QRegularExpression("(name:" + QLocale::system().name() + ")", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("(name:en)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("(name)", QRegularExpression::UseUnicodePropertiesOption),
                                           QRegularExpression("(name:\\w\\w)", QRegularExpression::UseUnicodePropertiesOption)})
    {
        const QStringList& matches = nameRelatedKeys.filter(regex);
        if (!matches.isEmpty())
        {
            name = this->data[regex.match(matches[0]).captured(1)];
            break;
        }
    }
    if(name.isEmpty() && data.contains("operator"))
    {
        name = this->data["operator"];
    }
    if(name.isEmpty() && data.contains("brand"))
    {
        name = this->data["brand"];
    }
}

const QString& CRawPoi::getCategory() const
{
    return category;
}

const QString& CRawPoi::getName(bool replaceEmptyByCategory) const
{
    if(replaceEmptyByCategory && name.isEmpty())
    {
        return category;
    }
    return name;
}

const QPointF& CRawPoi::getCoordinates() const
{
    return coordinates;
}

const quint64& CRawPoi::getKey() const
{
    return key;
}

const QMap<QString, QString>& CRawPoi::getData() const
{
    return data;
}

const QStringList& CRawPoi::getRawData() const
{
    return rawData;
}

QString CRawPoi::getDesc() const
{
    //No need to add the wiki* keys, as they are already covered through their lists
    static const QList<QString> skipKeys = {
        "contact:email", "email",
        "contact:phone", "phone", "contact:mobile",
        "contact:website", "website", "url",
        "osm_id",
        "wikimedia_commons",
        "image",
        "ele"
    };

    // for now address keys to skip, as they've been included in the address.
    // Don't skip all address keys, as there might be some non-western address schemes that use address tags not displayed in the address field by QMS
    QList<QString> tempSkipKeys;

    QString desc = "";
    // Build Adress field

    //Dear translators, please reformat this the way you want which is appropriate for your locale. The osm key will be replaced by the value.
    //Note that a non-letter character is needed between a key and another key or some arbitrary letter that is part of the address scheme
    //You can find all avalable address keys at: https://wiki.openstreetmap.org/wiki/Key:addr
    QString addr = tr("addr:housenumber addr:street<br>\naddr:city addr:postcode<br>\naddr:country");
    static const QRegularExpression addressRegex("(addr:[A-z]+)", QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator i = addressRegex.globalMatch(addr);
    while (i.hasNext())
    {
        const QRegularExpressionMatch& match = i.next();
        const QString& key = match.captured(1);
        if(data.contains(key))
        {
            addr.replace(key, data[key]);
            tempSkipKeys.append(key);
        }
        else
        {
            addr.replace(key, "");
        }
    }
    static const QRegularExpression addrCleanup("(.*?)(\\s*<br>\n\\s*)+\\Z", QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = addrCleanup.match(addr);
    if(match.hasMatch())
    {
        addr = match.captured(1);
    }
    desc += addr;

    if(nameRelatedKeys.count() > 1)
    {
        if(!desc.isEmpty())
        {
            desc += "<br>\n";
        }
        desc += "<b>" + tr("Alternative names: ") + "</b>";
        bool firstName = true;
        for(const QString& key : nameRelatedKeys)
        {
            if(data[key] == name)
            {
                continue;
            }

            if(firstName)
            {
                firstName = false;
            }
            else
            {
                desc += ", ";
            }
            desc += data[key];
        }
    }
    if(data.contains("phone"))
    {
        if(!desc.isEmpty())
        {
            desc += "<br>\n";
        }
        desc += "<b>" + tr("Phone number: ") + "</b>" + data["phone"];
    }
    if(data.contains("contact:phone"))
    {
        if(!desc.isEmpty())
        {
            desc += "<br>\n";
        }
        desc += "<b>" + tr("Phone number: ") + "</b>" + data["contact:phone"];
    }
    if(data.contains("contact:mobile"))
    {
        if(!desc.isEmpty())
        {
            desc += "<br>\n";
        }
        desc += "<b>" + tr("Mobile phone number: ") + "</b>" + data["contact:mobile"];
    }
    if(data.contains("fax"))
    {
        if(!desc.isEmpty())
        {
            desc += "<br>\n";
        }
        desc += "<b>" + tr("Fax: ") + "</b>" + data["fax"];
    }
    if(!desc.isEmpty())
    {
        desc += "<br>\n" "<br>\n";
    }
    // Append all other tags
    bool firstTag = true;
    const QList<QString>& keys = data.keys();
    for(const QString& key : keys)
    {
        if(skipKeys.contains(key)
           || tempSkipKeys.contains(key)
           || wikidataRelatedKeys.contains(key)
           || wikipediaRelatedKeys.contains(key)
           || nameRelatedKeys.contains(key)
           || key == "phone"
           || key == "contact:phone"
           || key == "contact:mobile"
           || key == "fax")
        {
            continue;
        }

        if(firstTag)
        {
            firstTag = false;
        }
        else
        {
            desc += "<br>\n";
        }
        desc += key + ": " + data[key];
    }

    return desc;
}

QList<IGisItem::link_t> CRawPoi::getLinks() const
{
    QList<IGisItem::link_t> links;
    if(data.contains("contact:email"))
    {
        links += {"mailto:" + data["contact:email"], tr("Email"), "text/html"};
    }
    if(data.contains("email"))
    {
        links += {"mailto:" + data["email"], tr("Email"), "text/html"};
    }
    if(data.contains("url"))
    {
        links += {data["url"], tr("Website"), "text/html"};
    }
    if(data.contains("contact:website"))
    {
        links += {data["contact:website"], tr("Website"), "text/html"};
    }
    if(data.contains("website"))
    {
        links += {data["website"], tr("Website"), "text/html"};
    }
    if(data.contains("osm_id"))
    {
        QString uri = "https://www.openstreetmap.org/";
        QRegularExpressionMatch match;
        if(data["osm_id"].contains(QRegularExpression("(P|W|R)/(.+)"), &match))
        {
            if(match.captured(1) == "P")
            {
                uri += "node/";
            }
            else if(match.captured(1) == "W")
            {
                uri += "way/";
            }
            else
            {
                uri += "relation/";
            }
            uri += match.captured(2);
            links += {uri, tr("OpenStreetMap"), "text/html"};
        }
        else
        {
            qDebug() << "Encountered unsupported osm_id: " + data["osm_id"];
        }
    }
    if(data.contains("wikimedia_commons"))
    {
        links += {"https://wikipedia.org/wiki/commons:" + data["wikimedia_commons"], tr("wikimedia"), "text/html"};
    }
    if(data.contains("image"))
    {
        links += {data["image"], tr("Image"), "text/html"};
    }

    static const QRegularExpression wikipediaRegex ("(.*:)?(wikipedia)(:[A-z]{2})?", QRegularExpression::UseUnicodePropertiesOption);
    for(const QString& key : wikipediaRelatedKeys)
    {
        const QRegularExpressionMatch& match = wikipediaRegex.match(key);
        if (match.hasMatch())
        {
            QString uri;
            if(match.captured(3).isEmpty())
            {
                uri += "https://wikipedia.org/wiki/";
            }
            else
            {
                uri += "https://" + match.captured(3).remove(0, 1) + ".wikipedia.org/wiki/";
            }
            uri += data[key];

            QString title = tr("Wikipedia");
            if(!match.captured(1).isEmpty() || !match.captured(3).isEmpty())
            {
                title += "(";
                if(!match.captured(1).isEmpty())
                {
                    title += match.captured(1);
                }
                if(!match.captured(1).isEmpty() && !match.captured(3).isEmpty())
                {
                    title += ",";
                }
                if(!match.captured(3).isEmpty())
                {
                    title += match.captured(3).remove(0, 1);
                }
                title += ")";
            }
            links += {uri, title, "text/html"};
        }
    }

    static const QRegularExpression wikidataRegex ("(.*:)?(wikidata)=(.+)", QRegularExpression::UseUnicodePropertiesOption);
    for(const QString& key : wikidataRelatedKeys)
    {
        const QRegularExpressionMatch& match = wikidataRegex.match(key);
        if(match.hasMatch())
        {
            QString title = tr("Wikidata");
            if(!match.captured(1).isEmpty())
            {
                title += "(" + match.captured(1) + ")";
            }
            links += {"https://wikidata.org/wiki/" + data[key], title, "text/html"};
        }
    }

    return links;
}

quint32 CRawPoi::getEle() const
{
    bool ok = true;
    quint32 ele = data["ele"].toInt(&ok);
    return ok ? ele : NOINT;
}



poi_t CRawPoi::toPoi() const
{
    poi_t poi;
    poi.pos = coordinates;
    poi.name = getName();
    poi.icon = garminIcon;
    poi.desc = getDesc();
    poi.links = getLinks();
    poi.ele = getEle();
    return poi;
}


