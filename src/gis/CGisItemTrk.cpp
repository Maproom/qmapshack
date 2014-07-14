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

#include "gis/CGisItemTrk.h"
#include "gis/CGisProject.h"

#include <QtXml>


CGisItemTrk::CGisItemTrk(const QDomNode& xml, CGisProject * parent)
    : IGisItem(parent)
{
    readTrk(xml, trk);
    setText(0, trk.name);
    genKey();
}

CGisItemTrk::~CGisItemTrk()
{

}

void CGisItemTrk::genKey()
{
    if(key.isEmpty())
    {
        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData((const char*)&trk, sizeof(trk));
        key = md5.result().toHex();
    }
}

void CGisItemTrk::readTrk(const QDomNode& xml, trk_t& trk)
{
    readXml(xml, "name", trk.name);
    readXml(xml, "cmt", trk.cmt);
    readXml(xml, "desc", trk.desc);
    readXml(xml, "src", trk.src);
    if(xml.namedItem("link").isElement())
    {
        const QDomNodeList& links = xml.toElement().elementsByTagName("link");
        int N = links.count();
        for(int n = 0; n < N; ++n)
        {
            const QDomNode& link = links.item(n);

            link_t tmp;
            tmp.uri.setUrl(link.attributes().namedItem("href").nodeValue());
            readXml(link, "text", tmp.text);
            readXml(link, "type", tmp.type);

            trk.links << tmp;
        }
    }

    readXml(xml, "number", trk.number);
    readXml(xml, "type", trk.type);

    const QDomNodeList& trksegs = xml.toElement().elementsByTagName("trkseg");
    int N = trksegs.count();
    trk.segs.resize(N);
    for(int n = 0; n < N; ++n)
    {
        const QDomNode& trkseg = trksegs.item(n);
        trkseg_t& seg = trk.segs[n];

        const QDomNodeList& trkpts = trkseg.toElement().elementsByTagName("trkpt");
        int M = trksegs.count();
        seg.pts.resize(M);
        for(int m = 0; m < M; ++m)
        {
            const QDomNode& trkpt = trkpts.item(m);
            readWpt(trkpt, seg.pts[m]);
        }
    }
}
