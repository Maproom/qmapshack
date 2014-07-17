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

QMutex IGisItem::mutexItems;




const IGisItem::color_t IGisItem::colorMap[] =
{
     {"Black",       QColor(Qt::black)}
    ,{"DarkRed",     QColor(Qt::darkRed)}
    ,{"DarkGreen",   QColor(Qt::darkGreen)}
    ,{"DarkYellow",  QColor(Qt::darkYellow)}
    ,{"DarkBlue",    QColor(Qt::darkBlue)}
    ,{"DarkMagenta", QColor(Qt::darkMagenta)}
    ,{"DarkCyan",    QColor(Qt::darkCyan)}
    ,{"LightGray",   QColor(Qt::gray)}
    ,{"DarkGray",    QColor(Qt::darkGray)}
    ,{"Red",         QColor(Qt::red)}
    ,{"Green",       QColor(Qt::green)}
    ,{"Yellow",      QColor(Qt::yellow)}
    ,{"Blue",        QColor(Qt::blue)}
    ,{"Magenta",     QColor(Qt::magenta)}
    ,{"Cyan",        QColor(Qt::cyan)}
    ,{"White",       QColor(Qt::white)}
    ,{"Transparent", QColor(Qt::transparent)}
    ,{0, QColor()}

};

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
    wpt.lat = attr.namedItem("lat").nodeValue().toDouble();
    wpt.lon = attr.namedItem("lon").nodeValue().toDouble();

    readXml(xml, "ele", wpt.ele);
    readXml(xml, "time", wpt.time);
    readXml(xml, "magvar", wpt.magvar);
    readXml(xml, "geoidheight", wpt.geoidheight);
    readXml(xml, "name", wpt.name);
    readXml(xml, "cmt", wpt.cmt);
    readXml(xml, "desc", wpt.desc);
    readXml(xml, "src", wpt.src);
    readXml(xml, "link", wpt.links);
    readXml(xml, "sym", wpt.sym);
    readXml(xml, "type", wpt.type);
    readXml(xml, "fix", wpt.fix);
    readXml(xml, "sat", wpt.sat);
    readXml(xml, "hdop", wpt.hdop);
    readXml(xml, "vdop", wpt.vdop);
    readXml(xml, "pdop", wpt.pdop);
    readXml(xml, "ageofdgpsdata", wpt.ageofdgpsdata);
    readXml(xml, "dgpsid", wpt.dgpsid);
}


QDomNode IGisItem::writeWpt(QDomNode& gpx, const wpt_t& wpt)
{
    QString str;

    QDomDocument doc = gpx.ownerDocument();

    QDomElement xmlWpt = doc.createElement("wpt");
    gpx.appendChild(xmlWpt);

    str.sprintf("%1.8f", wpt.lat);
    xmlWpt.setAttribute("lat",str);
    str.sprintf("%1.8f", wpt.lon);
    xmlWpt.setAttribute("lon",str);

    writeXml(xmlWpt, "ele", wpt.ele);
    writeXml(xmlWpt, "time", wpt.time);
    writeXml(xmlWpt, "magvar", wpt.magvar);
    writeXml(xmlWpt, "geoidheight", wpt.geoidheight);
    writeXml(xmlWpt, "name", wpt.name);
    writeXml(xmlWpt, "cmt", wpt.cmt);
    writeXml(xmlWpt, "desc", wpt.desc);
    writeXml(xmlWpt, "src", wpt.src);
    writeXml(xmlWpt, "link", wpt.links);
    writeXml(xmlWpt, "sym", wpt.sym);
    writeXml(xmlWpt, "type", wpt.type);
    writeXml(xmlWpt, "fix", wpt.fix);
    writeXml(xmlWpt, "sat", wpt.sat);
    writeXml(xmlWpt, "hdop", wpt.hdop);
    writeXml(xmlWpt, "vdop", wpt.vdop);
    writeXml(xmlWpt, "pdop", wpt.pdop);
    writeXml(xmlWpt, "ageofdgpsdata", wpt.ageofdgpsdata);
    writeXml(xmlWpt, "dgpsid", wpt.dgpsid);


    return xmlWpt;
}

const QString& IGisItem::getKey()
{
    if(key.isEmpty())
    {
        genKey();
    }
    return key;
}

QColor IGisItem::str2color(const QString& name)
{
    const color_t * p = colorMap;
    while(p->name)
    {
        if(p->name == name)
        {
            return p->color;
        }
        p++;
    }

    return QColor();
}

QString IGisItem::color2str(const QColor& color)
{
    const color_t * p = colorMap;
    while(p->name)
    {
        if(p->color == color)
        {
            return p->name;
        }
        p++;
    }

    return "";
}
