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
#include "gis/wpt/CDetailsWpt.h"
#include "gis/wpt/CDetailsGeoCache.h"
#include "gis/wpt/CScrOptWpt.h"
#include "gis/CGisProject.h"
#include "gis/CGisDraw.h"
#include "gis/WptIcons.h"
#include "gis/CGisListWks.h"
#include "canvas/CCanvas.h"
#include "mouse/IMouse.h"
#include "units/IUnit.h"
#include "GeoMath.h"


#include <QtWidgets>
#include <QtXml>

QString CGisItemWpt::keyUserFocus;

CGisItemWpt::CGisItemWpt(const QPointF& pos, const CGisItemWpt& parentWpt, CGisProject * project)
    : IGisItem(project)
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    *this = parentWpt;
    wpt.lon     = pos.x();
    wpt.lat     = pos.y();
    wpt.time    = QDateTime::currentDateTimeUtc();

    key.clear();
    flags = eFlagCreatedInQms|eFlagWriteAllowed;

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);
    setText(1, "*");
    setText(0, wpt.name);
    setIcon();
    setToolTip(0, getInfo());
    genKey();
}

CGisItemWpt::CGisItemWpt(const QDomNode &xml, CGisProject *project)
    : IGisItem(project)
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    // --- start read and process data ----
    readWpt(xml, wpt);
    // decode some well known extensions
    if(xml.namedItem("extensions").isElement())
    {        
        const QDomNode& ext = xml.namedItem("extensions");
        readXml(ext, "ql:key", key);
        readXml(ext, "ql:flags", flags);
        readXml(ext, history);

        const QDomNode& wptx1 = ext.namedItem("wptx1:WaypointExtension");
        readXml(wptx1, "wptx1:Proximity", proximity);

        const QDomNode& xmlCache = ext.namedItem("cache");
        if(!xmlCache.isNull())
        {
            // read OC cache extensions
        }
    }

    const QDomNode& xmlCache = xml.namedItem("groundspeak:cache");
    if(!xmlCache.isNull() && !geocache.hasData)
    {
        readGcExt(xmlCache);
    }
    // --- stop read and process data ----

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);
    setText(0, wpt.name);
    setIcon();
    setToolTip(0, getInfo());
    genKey();
}

CGisItemWpt::~CGisItemWpt()
{

}

CGisItemWpt& CGisItemWpt::operator=(const CGisItemWpt& w)
{
    wpt             = w.wpt;
    proximity       = w.proximity;
    geocache        = w.geocache;
    focus           = w.focus;
    posScreen       = w.posScreen;


    flags           = w.flags;
    key             = w.key;
    icon            = w.icon;
    boundingRect    = w.boundingRect;

    return *this;
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

QString CGisItemWpt::getInfo()
{
    QString str = "<div>" + getName() + "</div>";

    if(geocache.hasData)
    {
        str += QString(" %4 (%1, D %2, T %3)").arg(geocache.container).arg(geocache.difficulty, 0,'f',1).arg(geocache.terrain, 0,'f',1).arg(geocache.name);
    }

    if(wpt.time.isValid())
    {
        if(!str.isEmpty()) str += "<br/>\n";

        str += IUnit::datetime2string(wpt.time, QPointF(wpt.lon*DEG_TO_RAD, wpt.lat*DEG_TO_RAD));
    }

    if(wpt.ele != NOINT)
    {
        if(!str.isEmpty()) str += "<br/>\n";
        QString val, unit;
        IUnit::self().meter2elevation(wpt.ele, val, unit);
        str += QObject::tr("Elevation: %1 %2").arg(val).arg(unit);
    }

    if(proximity != NOFLOAT)
    {
        if(!str.isEmpty()) str += "<br/>\n";
        QString val, unit;
        IUnit::self().meter2distance(proximity, val, unit);
        str += QObject::tr("Proximity: %1 %2").arg(val).arg(unit);
    }

    QString desc = removeHtml(wpt.desc);
    if(desc.count())
    {
        if(!str.isEmpty()) str += "<br/>\n";

        if(desc.count() < 200)
        {
            str += desc;
        }
        else
        {
            str += desc.left(197) + "...";
        }
    }
    else
    {
        QString cmt = removeHtml(wpt.cmt);
        if(cmt.count())
        {
            if(!str.isEmpty()) str += "<br/>\n";

            if(cmt.count() < 200)
            {
                str += cmt;
            }
            else
            {
                str += cmt.left(197) + "...";
            }
        }
    }
    return str;
}

IScrOpt * CGisItemWpt::getScreenOptions(const QPoint& origin, IMouse * mouse)
{
    return new CScrOptWpt(this, origin, mouse);
}

void CGisItemWpt::setIcon()
{
    if(geocache.hasData)
    {
        icon = getWptIconByName(geocache.type, focus);
    }
    else
    {
        icon = getWptIconByName(wpt.sym, focus);
    }

    QTreeWidgetItem::setIcon(0,icon);
}

void CGisItemWpt::setName(const QString& str)
{
    setText(0, str);
    wpt.name = str;
    changed(QObject::tr("Changed name"));
}

void CGisItemWpt::setPosition(const QPointF& pos)
{
    wpt.lon = pos.x();
    wpt.lat = pos.y();
    changed(QObject::tr("Changed position"));
}

void CGisItemWpt::setElevation(qint32 val)
{
    wpt.ele = val;
    changed(QObject::tr("Changed elevation"));
}

void CGisItemWpt::setProximity(qreal val)
{
    proximity = val;
    changed(QObject::tr("Changed proximity"));
}

void CGisItemWpt::setIcon(const QString& name)
{    
    wpt.sym = name;
    setIcon();
    changed(QObject::tr("Changed icon"));
}

void CGisItemWpt::setComment(const QString& str)
{
    wpt.cmt = str;
    changed(QObject::tr("Changed comment"));
}

void CGisItemWpt::setDescription(const QString& str)
{
    wpt.desc = str;
    changed(QObject::tr("Changed description"));
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
    writeXml(xmlExt, "ql:flags", flags);
    writeXml(xmlExt, history);

    // write other well known extensions
    QDomElement wptx1  = doc.createElement("wptx1:WaypointExtension");
    xmlExt.appendChild(wptx1);
    writeXml(wptx1, "wptx1:Proximity", proximity);

    if(geocache.hasData && geocache.service == eGC)
    {
        QDomElement xmlCache = doc.createElement("groundspeak:cache");
        writeGcExt(xmlCache);
        xmlWpt.appendChild(xmlCache);
    }

    setText(1,"");
}

void CGisItemWpt::readGcExt(const QDomNode& xmlCache)
{
    geocache.service = eGC;
    const QDomNamedNodeMap& attr = xmlCache.attributes();
    geocache.id = attr.namedItem("id").nodeValue().toInt();

    geocache.archived   = attr.namedItem("archived").nodeValue().toLocal8Bit() == "True";
    geocache.available  = attr.namedItem("available").nodeValue().toLocal8Bit() == "True";
    if(geocache.archived)
    {
        geocache.status = QObject::tr("Archived");
    }
    else if(geocache.available)
    {
        geocache.status = QObject::tr("Available");
    }
    else
    {
        geocache.status = QObject::tr("Not Available");
    }

    readXml(xmlCache, "groundspeak:name", geocache.name);
    readXml(xmlCache, "groundspeak:placed_by", geocache.owner);
    readXml(xmlCache, "groundspeak:type", geocache.type);
    readXml(xmlCache, "groundspeak:container", geocache.container);
    readXml(xmlCache, "groundspeak:difficulty", geocache.difficulty);
    readXml(xmlCache, "groundspeak:terrain", geocache.terrain);
    readXml(xmlCache, "groundspeak:short_description", geocache.shortDesc, geocache.shortDescIsHtml);
    readXml(xmlCache, "groundspeak:long_description", geocache.longDesc, geocache.longDescIsHtml);
    readXml(xmlCache, "groundspeak:encoded_hints", geocache.hint);
    readXml(xmlCache, "groundspeak:country", geocache.country);
    readXml(xmlCache, "groundspeak:state", geocache.state);

    const QDomNodeList& logs = xmlCache.toElement().elementsByTagName("groundspeak:log");
    uint N = logs.count();

    for(uint n = 0; n < N; ++n)
    {
        const QDomNode& xmlLog = logs.item(n);
        const QDomNamedNodeMap& attr = xmlLog.attributes();

        geocachelog_t log;
        log.id = attr.namedItem("id").nodeValue().toUInt();
        readXml(xmlLog, "groundspeak:date", log.date);
        readXml(xmlLog, "groundspeak:type", log.type);
        if(xmlLog.namedItem("groundspeak:finder").isElement())
        {
            const QDomNamedNodeMap& attr = xmlLog.namedItem("groundspeak:finder").attributes();
            log.finderId = attr.namedItem("id").nodeValue();
        }

        readXml(xmlLog, "groundspeak:finder", log.finder);
        readXml(xmlLog, "groundspeak:text", log.text, log.textIsHtml);

        geocache.logs << log;

    }
    geocache.hasData = true;
}

void CGisItemWpt::writeGcExt(QDomNode& xmlCache)
{
    QString str;
    xmlCache.toElement().setAttribute("xmlns:groundspeak", "http://www.groundspeak.com/cache/1/0");
    xmlCache.toElement().setAttribute("id", geocache.id);
    xmlCache.toElement().setAttribute("archived", geocache.archived ? "True" : "False");
    xmlCache.toElement().setAttribute("available", geocache.available ? "True" : "False");

    writeXml(xmlCache, "groundspeak:name", geocache.name);
    writeXml(xmlCache, "groundspeak:placed_by", geocache.owner);
    writeXml(xmlCache, "groundspeak:type", geocache.type);
    writeXml(xmlCache, "groundspeak:container", geocache.container);

    if(geocache.difficulty == int(geocache.difficulty))
    {
        str.sprintf("%1.0f", geocache.difficulty);
    }
    else
    {
        str.sprintf("%1.1f", geocache.difficulty);
    }
    writeXml(xmlCache, "groundspeak:difficulty", str);

    if(geocache.terrain == int(geocache.terrain))
    {
        str.sprintf("%1.0f", geocache.terrain);
    }
    else
    {
        str.sprintf("%1.1f", geocache.terrain);
    }
    writeXml(xmlCache, "groundspeak:terrain", str);
    writeXml(xmlCache, "groundspeak:short_description", geocache.shortDesc, geocache.shortDescIsHtml);
    writeXml(xmlCache, "groundspeak:long_description", geocache.longDesc, geocache.longDescIsHtml);
    writeXml(xmlCache, "groundspeak:encoded_hints", geocache.hint);

    if(!geocache.logs.isEmpty())
    {
        QDomElement xmlLogs = xmlCache.ownerDocument().createElement("groundspeak:logs");
        xmlCache.appendChild(xmlLogs);

        foreach(const geocachelog_t& log, geocache.logs)
        {
            QDomElement xmlLog = xmlCache.ownerDocument().createElement("groundspeak:log");
            xmlLogs.appendChild(xmlLog);

            xmlLog.setAttribute("id", log.id);
            writeXml(xmlLog, "groundspeak:date", log.date);
            writeXml(xmlLog, "groundspeak:type", log.type);

            QDomElement xmlFinder = xmlCache.ownerDocument().createElement("groundspeak:finder");
            xmlLog.appendChild(xmlFinder);

            QDomText _finder_ = xmlCache.ownerDocument().createCDATASection(log.finder);
            xmlFinder.appendChild(_finder_);
            xmlFinder.setAttribute("id", log.finderId);

            writeXml(xmlLog, "groundspeak:text", log.text, log.textIsHtml);
        }
    }
}

bool CGisItemWpt::isCloseTo(const QPointF& pos)
{
    if(posScreen == NOPOINTF)
    {
        return false;
    }

    return ((pos - posScreen).manhattanLength() < 22);
}

void CGisItemWpt::gainUserFocus()
{
    keyUserFocus = key;
}

void CGisItemWpt::edit()
{
    if(geocache.hasData)
    {
        CDetailsGeoCache dlg(*this, 0);
        dlg.exec();
    }
    else
    {
        CDetailsWpt dlg(*this, 0);
        dlg.exec();
    }
}

void CGisItemWpt::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    posScreen = QPointF(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
    if(!viewport.contains(posScreen))
    {
        posScreen = NOPOINTF;
        return;
    }
    gis->convertRad2Px(posScreen);
    p.drawPixmap(posScreen - focus, icon);

    if(proximity != NOFLOAT)
    {
        QPointF pt1(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
        pt1 = GPS_Math_Wpt_Projection(pt1, proximity, 90 * DEG_TO_RAD);
        gis->convertRad2Px(pt1);

        double r = pt1.x() - posScreen.x();

        p.save();
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::white,3));
        p.drawEllipse(QRect(posScreen.x() - r - 1, posScreen.y() - r - 1, 2*r + 1, 2*r + 1));
        p.setPen(QPen(Qt::red,1));
        p.drawEllipse(QRect(posScreen.x() - r - 1, posScreen.y() - r - 1, 2*r + 1, 2*r + 1));
        p.restore();
    }

    blockedAreas << QRectF(posScreen - focus, icon.size());
}

void CGisItemWpt::drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
{
    if(posScreen == NOPOINTF)
    {
        return;
    }

    QPointF pt = posScreen - focus;

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

void CGisItemWpt::drawHighlight(QPainter& p)
{
    if(posScreen == NOPOINTF)
    {
        return;
    }

    p.drawImage(posScreen - QPointF(31,31), QImage("://cursors/wptHighlight.png"));

}
