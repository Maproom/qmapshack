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
#include <QtWidgets>

#define DEFAULT_COLOR 4

const QColor CGisItemTrk::lineColors[] =
{
     Qt::black                    // 0
    ,Qt::darkRed                 // 1
    ,Qt::darkGreen               // 2
    ,Qt::darkYellow              // 3
    ,Qt::darkBlue                // 4
    ,Qt::darkMagenta             // 5
    ,Qt::darkCyan                // 6
    ,Qt::gray                    // 7
    ,Qt::darkGray                // 8
    ,Qt::red                     // 9
    ,Qt::green                   // 10
    ,Qt::yellow                  // 11
    ,Qt::blue                    // 12
    ,Qt::magenta                 // 13
    ,Qt::cyan                    // 14
    ,Qt::white                   // 15
    ,Qt::transparent             // 16
};

const QString CGisItemTrk::bulletColors[] =
{

                                 // 0
    QString("icons/8x8/bullet_black.png")
                                 // 1
    ,QString("icons/8x8/bullet_dark_red.png")
                                 // 2
    ,QString("icons/8x8/bullet_dark_green.png")
                                 // 3
    ,QString("icons/8x8/bullet_dark_yellow.png")
                                 // 4
    ,QString("icons/8x8/bullet_dark_blue.png")
                                 // 5
    ,QString("icons/8x8/bullet_dark_magenta.png")
                                 // 6
    ,QString("icons/8x8/bullet_dark_cyan.png")
                                 // 7
    ,QString("icons/8x8/bullet_gray.png")
                                 // 8
    ,QString("icons/8x8/bullet_dark_gray.png")
                                 // 9
    ,QString("icons/8x8/bullet_red.png")
                                 // 10
    ,QString("icons/8x8/bullet_green.png")
                                 // 11
    ,QString("icons/8x8/bullet_yellow.png")
                                 // 12
    ,QString("icons/8x8/bullet_blue.png")
                                 // 13
    ,QString("icons/8x8/bullet_magenta.png")
                                 // 14
    ,QString("icons/8x8/bullet_cyan.png")
                                 // 15
    ,QString("icons/8x8/bullet_white.png")
    ,QString("")                 // 16
};


CGisItemTrk::CGisItemTrk(const QDomNode& xml, CGisProject * parent)
    : IGisItem(parent)
{
    setColor(DEFAULT_COLOR);
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

void CGisItemTrk::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{

}

void CGisItemTrk::drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
{

}


void CGisItemTrk::readTrk(const QDomNode& xml, trk_t& trk)
{
    readXml(xml, "name", trk.name);
    readXml(xml, "cmt", trk.cmt);
    readXml(xml, "desc", trk.desc);
    readXml(xml, "src", trk.src);
    readXml(xml, "link", trk.links);
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
        int M = trkpts.count();
        seg.pts.resize(M);
        for(int m = 0; m < M; ++m)
        {
            const QDomNode& trkpt = trkpts.item(m);
            readWpt(trkpt, seg.pts[m]);
        }
    }

    // decode some well known extensions
    if(xml.namedItem("extensions").isElement())
    {
        const QDomNode& ext = xml.namedItem("extensions");
        readXml(ext, "ql:key", key);
    }

}

void CGisItemTrk::setColor(const QColor& c)
{
    int n;
    int N = sizeof(lineColors)/sizeof(QColor);

    for(n = 0; n < N; n++)
    {
        if(lineColors[n] == c)
        {
            colorIdx    = n;
            color       = lineColors[n];
            bullet      = QPixmap(bulletColors[n]);
            break;
        }
    }

    if(n == N)
    {
        colorIdx    = DEFAULT_COLOR;
        color       = lineColors[DEFAULT_COLOR];
        bullet      = QPixmap(bulletColors[DEFAULT_COLOR]);
    }

    setIcon(color.name());

}


void CGisItemTrk::setColor(unsigned i)
{
    if(i>16) i = DEFAULT_COLOR;
    colorIdx    = i;
    color       = lineColors[i];
    bullet      = QPixmap(bulletColors[i]);

    setIcon(color.name());
}

void CGisItemTrk::setIcon(const QString& c)
{
    QPixmap icon(32,32);
    icon.fill(Qt::transparent);
    QPainter p(&icon);
    p.setBrush(QColor(c));
    p.setPen(Qt::NoPen);
    p.drawRect(3,3,26,26);

    QTreeWidgetItem::setIcon(0,QIcon(icon));
}
