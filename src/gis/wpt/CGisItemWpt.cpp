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

#include "gis/wpt/CGisItemWpt.h"
#include "gis/CGisProject.h"
#include "gis/CGisDraw.h"
#include "gis/WptIcons.h"
#include "canvas/CCanvas.h"
#include "GeoMath.h"


#include <QtWidgets>
#include <QtXml>



CGisItemWpt::CGisItemWpt(const QDomNode &xml, CGisProject *parent)
    : IGisItem(parent)
    , proximity(NOFLOAT)
{
    // --- start read and process data ----
    readWpt(xml, wpt);
    // decode some well known extensions
    if(xml.namedItem("extensions").isElement())
    {        
        const QDomNode& ext = xml.namedItem("extensions");
        readXml(ext, "ql:key", key);

        const QDomNode& wptx1 = ext.namedItem("wptx1:WaypointExtension");
        readXml(wptx1, "wptx1:Proximity", proximity);
    }

    // translate icon from kind of well known string to pixmap object
    icon = getWptIconByName(wpt.sym, focus);
    // --- stop read and process data ----

    setText(0, wpt.name);
    setIcon(0, icon);
    genKey();
}

CGisItemWpt::~CGisItemWpt()
{

}

void CGisItemWpt::save(QDomNode& gpx)
{
    QDomDocument doc = gpx.ownerDocument();

    QDomElement xmlWpt = doc.createElement("wpt");
    gpx.appendChild(xmlWpt);
    writeWpt(xmlWpt, wpt);

    // write the key as extension tag
    QDomElement xmlExt  = doc.createElement("extensions");
    xmlWpt.appendChild(xmlExt);
    writeXml(xmlExt, "ql:key", key);

    // write other well known extensions
    QDomElement wptx1  = doc.createElement("wptx1:WaypointExtension");
    xmlExt.appendChild(wptx1);
    writeXml(wptx1, "wptx1:Proximity", proximity);
}

void CGisItemWpt::genKey()
{
    if(key.isEmpty())
    {
        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData((const char*)&wpt, sizeof(wpt));
        key = md5.result().toHex();
    }
}

void CGisItemWpt::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    QPointF pt(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
    if(!viewport.contains(pt))
    {
        return;
    }
    gis->convertRad2Px(pt);
    p.drawPixmap(pt - focus, icon);

    if(proximity != NOFLOAT)
    {
        QPointF pt1(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
        pt1 = GPS_Math_Wpt_Projection(pt1, proximity, 90 * DEG_TO_RAD);
        gis->convertRad2Px(pt1);

        double r = pt1.x() - pt.x();

        p.save();
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::white,3));
        p.drawEllipse(QRect(pt.x() - r - 1, pt.y() - r - 1, 2*r + 1, 2*r + 1));
        p.setPen(QPen(Qt::red,1));
        p.drawEllipse(QRect(pt.x() - r - 1, pt.y() - r - 1, 2*r + 1, 2*r + 1));
        p.restore();
    }

    blockedAreas << QRectF(pt - focus, icon.size());
}

void CGisItemWpt::drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
{
    QPointF pt(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
    if(!viewport.contains(pt))
    {
        return;
    }
    gis->convertRad2Px(pt);
    pt = pt - focus;


    QRectF rect = fm.boundingRect(wpt.name);
    rect.adjust(-2,-2,2,2);

    // place label on top
    rect.moveCenter(pt + QPointF(icon.width()/2, - fm.height()));
    if(isBlocked(rect, blockedAreas))
    {
        // place label on bottom
        rect.moveCenter(pt + QPointF( icon.width()/2, + fm.height() + icon.height()));
        if(isBlocked(rect, blockedAreas))
        {
            // place label on right
            rect.moveCenter(pt + QPointF( icon.width() + rect.width()/2, +fm.height()));
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

    CCanvas::drawText(wpt.name,p,rect.toRect(), Qt::darkBlue);
    blockedAreas << rect;
}

