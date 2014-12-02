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
#include "gis/prj/IGisProject.h"
#include "gis/WptIcons.h"
#include "gis/CGisDraw.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>
#include <QtXml>
#include <proj_api.h>

const QPen CGisItemRte::penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
QString CGisItemRte::keyUserFocus;

/// used to create a copy of route with new parent
CGisItemRte::CGisItemRte(const CGisItemRte& parentRte, IGisProject * project, int idx)
    : IGisItem(project, eTypeRte, idx)
    , penForeground(Qt::magenta, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    *this = parentRte;

    key.clear();
    genKey();
    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create route from GPX file
CGisItemRte::CGisItemRte(const QDomNode& xml, IGisProject *parent)
    : IGisItem(parent, eTypeRte, parent->childCount())
    , penForeground(Qt::magenta, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)    
{
    // --- start read and process data ----
    readRte(xml, rte);    
    // --- stop read and process data ----

    genKey();
    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemRte::CGisItemRte(const history_t& hist, IGisProject * project)
    : IGisItem(project, eTypeRte, project->childCount())
    , penForeground(Qt::magenta, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
}



CGisItemRte::~CGisItemRte()
{

}

void CGisItemRte::setSymbol()
{
    icon = QPixmap("://icons/32x32/Route.png").scaled(22,22, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setIcon(0, icon);
}


QString CGisItemRte::getInfo()
{
    QString str = getName();


    return str;
}

IScrOpt * CGisItemRte::getScreenOptions(const QPoint& origin, IMouse * mouse)
{
    return new CScrOptRte(this, origin, mouse);
}

QPointF CGisItemRte::getPointCloseBy(const QPoint& screenPos)
{
    qint32 d    = NOINT;
    QPointF pt  = NOPOINTF;
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

void CGisItemRte::gainUserFocus(bool yes)
{
    keyUserFocus = yes ? key : "";
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
