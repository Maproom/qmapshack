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
#include "canvas/CCanvas.h"
#include "GeoMath.h"

#include <QtXml>
#include <QtWidgets>

QMutex IGisItem::mutexItems(QMutex::Recursive);

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
    , readOnlyMode(true)
{

}

IGisItem::~IGisItem()
{

}

void IGisItem::setReadOnlyMode(bool readOnly)
{
    if(readOnlyMode && !readOnly)
    {
        QString str = QObject::tr("This element is probably read-only because it was not created within QMapShack. Usually you should not want to change imported data. But if you think that is ok press'Ok'.");
        if(QMessageBox::warning(0, tr("Read Only Mode..."), str, QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok) != QMessageBox::Ok)
        {
            return;
        }
    }

    readOnlyMode = readOnly;
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

    // some GPX 1.0 backward compatibility
    QString url;
    readXml(xml, "url", url);
    if(!url.isEmpty())
    {
        link_t link;
        link.uri.setUrl(url);
        readXml(xml, "urlname", link.text);

        wpt.links << link;
    }

}


void IGisItem::writeWpt(QDomElement& xml, const wpt_t& wpt)
{
    QString str;

    str.sprintf("%1.8f", wpt.lat);
    xml.setAttribute("lat",str);
    str.sprintf("%1.8f", wpt.lon);
    xml.setAttribute("lon",str);

    writeXml(xml, "ele", wpt.ele);
    writeXml(xml, "time", wpt.time);
    writeXml(xml, "magvar", wpt.magvar);
    writeXml(xml, "geoidheight", wpt.geoidheight);
    writeXml(xml, "name", wpt.name);
    writeXml(xml, "cmt", wpt.cmt);
    writeXml(xml, "desc", wpt.desc);
    writeXml(xml, "src", wpt.src);
    writeXml(xml, "link", wpt.links);
    writeXml(xml, "sym", wpt.sym);
    writeXml(xml, "type", wpt.type);
    writeXml(xml, "fix", wpt.fix);
    writeXml(xml, "sat", wpt.sat);
    writeXml(xml, "hdop", wpt.hdop);
    writeXml(xml, "vdop", wpt.vdop);
    writeXml(xml, "pdop", wpt.pdop);
    writeXml(xml, "ageofdgpsdata", wpt.ageofdgpsdata);
    writeXml(xml, "dgpsid", wpt.dgpsid);

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

void IGisItem::splitLineToViewport(const QPolygonF& line, const QRectF& extViewport, QList<QPolygonF>& lines)
{
    int i;
    QPointF pt, ptt, pt1;
    QPolygonF subline;
    const int size = line.size();

    pt = line[0];
    subline << pt;

    for(i = 1; i < size; i++)
    {
        pt1 = line[i];

        if(!GPS_Math_LineCrossesRect(pt, pt1, extViewport))
        {
            pt = pt1;
            if(subline.size() > 1)
            {
                lines << subline;
            }
            subline.clear();
            subline << pt;
            continue;
        }

        ptt = pt1 - pt;
        if(ptt.manhattanLength() < 5)
        {
            continue;
        }

        subline << pt1;
        pt = pt1;
    }

    if(subline.size() > 1)
    {
        lines << subline;
    }

}

void IGisItem::drawArrows(const QPolygonF& line, const QRectF& extViewport, QPainter& p)
{
    QPointF arrow[4] =
    {
        QPointF( 20.0, 7.0),     //front
        QPointF( 0.0, 0.0),      //upper tail
        QPointF( 5.0, 7.0),      //mid tail
        QPointF( 0.0, 15.0)      //lower tail
    };

    QPointF  pt, pt1, ptt;

    // draw direction arrows
    bool    start = true;
    double  heading;

    //generate arrow pic on-the-fly
    QImage arrow_pic(21,16, QImage::Format_ARGB32);
    arrow_pic.fill( qRgba(0,0,0,0));
    QPainter t_paint(&arrow_pic);
    USE_ANTI_ALIASING(t_paint, true);
    t_paint.setPen(QPen(Qt::white, 2));
    t_paint.setBrush(p.brush());
    t_paint.drawPolygon(arrow, 4);
    t_paint.end();

    foreach(pt,line)
    {
        if(start)                // no arrow on  the first loop
        {
            start = false;
        }
        else
        {
            if(!extViewport.contains(pt))
            {
                pt1 = pt;
                continue;
            }
            if((qAbs(pt.x() - pt1.x()) + qAbs(pt.y() - pt1.y())) < 7)
            {
                pt1 = pt;
                continue;
            }
            // keep distance
            if((qAbs(pt.x() - ptt.x()) + qAbs(pt.y() - ptt.y())) > 100)
            {
                if(0 != pt.x() - pt1.x() && (pt.y() - pt1.y()))
                {
                    heading = ( atan2((double)(pt.y() - pt1.y()), (double)(pt.x() - pt1.x())) * 180.) / M_PI;

                    p.save();
                    // draw arrow between bullets
                    p.translate((pt.x() + pt1.x())/2,(pt.y() + pt1.y())/2);
                    p.rotate(heading);
                    p.drawImage(-11, -7, arrow_pic);
                    p.restore();
                    //remember last point
                    ptt = pt;
                }
            }
        }
        pt1 = pt;
    }
}

QString IGisItem::removeHtml(const QString &str)
{
    QTextDocument html;
    html.setHtml(str);
    return html.toPlainText();
}
