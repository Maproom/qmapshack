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

#include "gis/rte/CGisItemRte.h"
#include "gis/rte/CScrOptRte.h"
#include "gis/CGisProject.h"
#include "gis/WptIcons.h"
#include "gis/CGisDraw.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>
#include <QtXml>
#include <proj_api.h>

const QPen CGisItemRte::penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
QString CGisItemRte::keyUserFocus;

CGisItemRte::CGisItemRte(const QDomNode& xml, CGisProject * parent)
    : IGisItem(parent)
    , penForeground(Qt::magenta, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)    
{
    // --- start read and process data ----
    readRte(xml, rte);    
    icon = QPixmap("://icons/32x32/Route.png").scaled(22,22, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // --- stop read and process data ----
    setText(0, rte.name);
    setIcon(0, icon);
    setToolTip(0,getInfo());
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

QString CGisItemRte::getInfo()
{
    QString str = getName();


    return str;
}

IScrOpt * CGisItemRte::getScreenOptions(IMouse * mouse)
{
    return new CScrOptRte(this, mouse);
}

QPointF CGisItemRte::getPointCloseBy(const QPoint& screenPos)
{
    qint32 d    = NOINT;
    QPointF pt  = NOPOINT;
    foreach(const QPointF& point, line)
    {
        int tmp = (screenPos - point).manhattanLength();
        if(tmp < d)
        {
            pt  = point;
            d   = tmp;
        }
    }

    return pt;
}

void CGisItemRte::readRte(const QDomNode& xml, rte_t& rte)
{
    qreal north = -90;
    qreal east  = -180;
    qreal south =  90;
    qreal west  =  180;

    readXml(xml, "name", rte.name);
    readXml(xml, "cmt", rte.cmt);
    readXml(xml, "desc", rte.desc);
    readXml(xml, "src", rte.src);
    readXml(xml, "link", rte.links);
    readXml(xml, "number", rte.number);
    readXml(xml, "type", rte.type);

    const QDomNodeList& xmlRtepts = xml.toElement().elementsByTagName("rtept");
    int M = xmlRtepts.count();
    rte.pts.resize(M);
    for(int m = 0; m < M; ++m)
    {
        rtept_t& rtept = rte.pts[m];
        const QDomNode& xmlRtept = xmlRtepts.item(m);
        readWpt(xmlRtept, rtept);
        rtept.icon = getWptIconByName(rtept.sym, rtept.focus);

        if(rtept.lon < west)  west    = rtept.lon;
        if(rtept.lon > east)  east    = rtept.lon;
        if(rtept.lat < south) south   = rtept.lat;
        if(rtept.lat > north) north   = rtept.lat;

    }

    boundingRect = QRectF(QPointF(west * DEG_TO_RAD, north * DEG_TO_RAD), QPointF(east * DEG_TO_RAD,south * DEG_TO_RAD));

    // decode some well known extensions
    if(xml.namedItem("extensions").isElement())
    {
        const QDomNode& ext = xml.namedItem("extensions");
        readXml(ext, "ql:key", key);
    }
}


void CGisItemRte::save(QDomNode& gpx)
{
    QDomDocument doc = gpx.ownerDocument();

    QDomElement xmlRte = doc.createElement("rte");
    gpx.appendChild(xmlRte);

    writeXml(xmlRte, "name", rte.name);
    writeXml(xmlRte, "cmt", rte.cmt);
    writeXml(xmlRte, "desc", rte.desc);
    writeXml(xmlRte, "src", rte.src);
    writeXml(xmlRte, "link", rte.links);
    writeXml(xmlRte, "number", rte.number);
    writeXml(xmlRte, "type", rte.type);

    // write the key as extension tag
    QDomElement xmlExt  = doc.createElement("extensions");
    xmlRte.appendChild(xmlExt);
    writeXml(xmlExt, "ql:key", key);

    foreach(const rtept_t& pt, rte.pts)
    {
        QDomElement xmlRtept = doc.createElement("rtept");
        xmlRte.appendChild(xmlRtept);
        writeWpt(xmlRtept, pt);
    }

}


bool CGisItemRte::isCloseTo(const QPointF& pos)
{
    foreach(const QPointF& pt, line)
    {
        if((pt - pos).manhattanLength() < 10)
        {
            return true;
        }
    }

    return false;
}

void CGisItemRte::gainUserFocus()
{
    keyUserFocus = key;
}


void CGisItemRte::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    line.clear();
    if(!viewport.intersects(boundingRect))
    {
        return;
    }

    QPointF p1 = viewport.topLeft();
    QPointF p2 = viewport.bottomRight();
    gis->convertRad2Px(p1);
    gis->convertRad2Px(p2);
    QRectF extViewport(p1,p2);

    foreach(const rtept_t& rtept, rte.pts)
    {
        QPointF pt(rtept.lon * DEG_TO_RAD, rtept.lat * DEG_TO_RAD);
        //        if(!viewport.contains(pt))
        //        {
        //            return;
        //        }
        gis->convertRad2Px(pt);

        line << pt;

        blockedAreas << QRectF(pt - rtept.focus, rtept.icon.size());
    }

    QList<QPolygonF> lines;
    splitLineToViewport(line, extViewport, lines);

    p.setPen(penBackground);
    foreach(const QPolygonF& line, lines)
    {
        p.drawPolyline(line);
    }
    p.setPen(penForeground);
    p.setBrush(penForeground.color());
    foreach(const QPolygonF& line, lines)
    {
        p.drawPolyline(line);
        drawArrows(line, extViewport, p);
    }


}

void CGisItemRte::drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
{
    if(!viewport.intersects(boundingRect))
    {
        return;
    }


    foreach(const rtept_t& rtept, rte.pts)
    {

        QPointF pt(rtept.lon * DEG_TO_RAD, rtept.lat * DEG_TO_RAD);
//        if(!viewport.contains(pt))
//        {
//            return;
//        }
        gis->convertRad2Px(pt);
        pt = pt - rtept.focus;
        p.drawPixmap(pt, rtept.icon);

        QRectF rect = fm.boundingRect(rtept.name);
        rect.adjust(-2,-2,2,2);

        // place label on top
        rect.moveCenter(pt + QPointF(rtept.icon.width()/2, - fm.height()));
        if(isBlocked(rect, blockedAreas))
        {
            // place label on bottom
            rect.moveCenter(pt + QPointF( rtept.icon.width()/2, + fm.height() + rtept.icon.height()));
            if(isBlocked(rect, blockedAreas))
            {
                // place label on right
                rect.moveCenter(pt + QPointF( rtept.icon.width() + rect.width()/2, +fm.height()));
                if(isBlocked(rect, blockedAreas))
                {
                    // place label on left
                    rect.moveCenter(pt + QPointF( - rect.width()/2, +fm.height()));
                    if(isBlocked(rect, blockedAreas))
                    {
                        // failed to place label anywhere
                        return;
                    }
                }
            }
        }

        CCanvas::drawText(rtept.name, p, rect.toRect(), Qt::darkBlue);
        blockedAreas << rect;
    }
}

void CGisItemRte::drawHighlight(QPainter& p)
{
    if(line.isEmpty())
    {
        return;
    }

    p.setPen(QPen(QColor(255,0,0,100),11,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(line);

    foreach(const QPointF& pt, line)
    {
        p.drawImage(pt - QPointF(31,31), QImage("://cursors/wptHighlight.png"));
    }
}
