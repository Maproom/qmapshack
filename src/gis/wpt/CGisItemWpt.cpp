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


#include <QtWidgets>
#include <QtXml>

inline bool isBlocked(const QRectF& rect, const QList<QRectF> &blockedAreas)
{
    foreach(const QRectF& r, blockedAreas)
    {
        if(rect.intersects(r))
        {
            return true;
        }
    }
    return false;
}


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

        QDomChildMap childmap = mapChildElements(ext);
        if(childmap.contains("wptx1:WaypointExtension"))
        {
            QDomNode wptx1 = childmap["wptx1:WaypointExtension"];
            readXml(wptx1, "wptx1:Proximity", proximity);
        }
    }

    icon = getWptIconByName(wpt.sym, focus);
    // Limit icon size to 22 pixel max.
    if(icon.width() > 22 || icon.height() > 22)
    {
        qreal s;
        if(icon.width() > icon.height())
        {
            s = 22.0/icon.width();
        }
        else
        {
            s = 22.0/icon.height();
        }

        focus = focus * s;
        icon  = icon.scaled(icon.size()*s,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
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
    QDomNode xmlWpt     = writeWpt(gpx, wpt);
    // write the key as extension tag
    QDomElement xmlExt  = gpx.ownerDocument().createElement("extensions");
    xmlWpt.appendChild(xmlExt);
    writeXml(xmlExt, "ql:key", key);

    // write other well known extensions
    QDomElement wptx1  = gpx.ownerDocument().createElement("wptx1:WaypointExtension");
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

