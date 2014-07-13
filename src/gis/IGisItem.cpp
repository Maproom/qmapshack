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

#include "gis/IGisItem.h"
#include "units/IUnit.h"

#include <QtXml>

IGisItem::IGisItem(QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent)
{

}

IGisItem::~IGisItem()
{

}

void IGisItem::readWpt(const QDomNode& xml, wpt_t& wpt)
{
    const QDomNamedNodeMap& attr = xml.attributes();
    wpt.lon = attr.namedItem("lon").nodeValue().toDouble();
    wpt.lat = attr.namedItem("lat").nodeValue().toDouble();

    if(xml.namedItem("ele").isElement())
    {
        wpt.ele = xml.namedItem("ele").toElement().text().toInt();
    }
    if(xml.namedItem("time").isElement())
    {
        QString time = xml.namedItem("time").toElement().text();
        IUnit::parseTimestamp(time, wpt.time);
    }
    if(xml.namedItem("magvar").isElement())
    {
        wpt.magvar = xml.namedItem("magvar").toElement().text().toInt();
    }
    if(xml.namedItem("geoidheight").isElement())
    {
        wpt.geoidheight = xml.namedItem("geoidheight").toElement().text().toInt();
    }
    if(xml.namedItem("name").isElement())
    {
        wpt.name = xml.namedItem("name").toElement().text();
    }
    if(xml.namedItem("cmt").isElement())
    {
        wpt.cmt = xml.namedItem("cmt").toElement().text();
    }
    if(xml.namedItem("desc").isElement())
    {
        wpt.desc = xml.namedItem("desc").toElement().text();
    }
    if(xml.namedItem("src").isElement())
    {
        wpt.src = xml.namedItem("src").toElement().text();
    }

    if(xml.namedItem("link").isElement())
    {
        const QDomNodeList& links = xml.toElement().elementsByTagName("link");
        int N = links.count();
        for(int n = 0; n < N; ++n)
        {
            link_t tmp;
            const QDomNode& link = links.item(n);
            tmp.uri.setUrl(link.attributes().namedItem("href").nodeValue());
            if(link.namedItem("text").isElement())
            {
                tmp.text = link.namedItem("text").toElement().text();
            }
            if(link.namedItem("type").isElement())
            {
                tmp.type = link.namedItem("type").toElement().text();
            }

            wpt.links << tmp;
        }
    }

    if(xml.namedItem("sym").isElement())
    {
        wpt.sym = xml.namedItem("sym").toElement().text();
    }
    if(xml.namedItem("type").isElement())
    {
        wpt.type = xml.namedItem("type").toElement().text();
    }
    if(xml.namedItem("fix").isElement())
    {
        wpt.fix = xml.namedItem("fix").toElement().text();
    }
    if(xml.namedItem("sat").isElement())
    {
        wpt.sat = xml.namedItem("sat").toElement().text().toInt();
    }
    if(xml.namedItem("hdop").isElement())
    {
        wpt.hdop = xml.namedItem("hdop").toElement().text().toInt();
    }
    if(xml.namedItem("vdop").isElement())
    {
        wpt.vdop = xml.namedItem("vdop").toElement().text().toInt();
    }
    if(xml.namedItem("pdop").isElement())
    {
        wpt.pdop = xml.namedItem("pdop").toElement().text().toInt();
    }
    if(xml.namedItem("ageofdgpsdata").isElement())
    {
        wpt.ageofdgpsdata = xml.namedItem("ageofdgpsdata").toElement().text().toInt();
    }
    if(xml.namedItem("dgpsid").isElement())
    {
        wpt.dgpsid = xml.namedItem("dgpsid").toElement().text().toInt();
    }

}

void IGisItem::genKey()
{
    if(key.isEmpty())
    {
        QCryptographicHash md5(QCryptographicHash::Md5);

        QByteArray tmp((const char *)this, sizeof(*this));
        md5.addData(tmp);
        key = md5.result().toHex();
    }
}

const QString& IGisItem::getKey()
{
    if(key.isEmpty())
    {
        genKey();
    }
    return key;


}
