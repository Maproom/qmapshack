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

#include "gis/CGisItemRte.h"
#include "gis/CGisProject.h"

#include <QtXml>

CGisItemRte::CGisItemRte(const QDomNode& xml, CGisProject * parent)
    : IGisItem(parent)
{
    readRte(xml, rte);
    setText(0, rte.name);
    setIcon(0, QIcon("://icons/32x32/Route.png"));
    genKey();
}

CGisItemRte::~CGisItemRte()
{

}

void CGisItemRte::genKey()
{
    if(key.isEmpty())
    {
        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData((const char*)&rte, sizeof(rte));
        key = md5.result().toHex();
    }
}

void CGisItemRte::draw(QPainter& p, const QRectF& viewport, CGisDraw *gis)
{

}


void CGisItemRte::readRte(const QDomNode& xml, rte_t& rte)
{
    readXml(xml, "name", rte.name);
    readXml(xml, "cmt", rte.cmt);
    readXml(xml, "desc", rte.desc);
    readXml(xml, "src", rte.src);
    readXml(xml, "link", rte.links);
    readXml(xml, "number", rte.number);
    readXml(xml, "type", rte.type);

    const QDomNodeList& rtepts = xml.toElement().elementsByTagName("rtept");
    int M = rtepts.count();
    rte.pts.resize(M);
    for(int m = 0; m < M; ++m)
    {
        const QDomNode& rtept = rtepts.item(m);
        readWpt(rtept, rte.pts[m]);
    }

    // decode some well known extensions
    if(xml.namedItem("extensions").isElement())
    {
        const QDomNode& ext = xml.namedItem("extensions");
        readXml(ext, "ql:key", key);
    }


}
