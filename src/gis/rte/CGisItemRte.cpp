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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/WptIcons.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/CScrOptRte.h"

#include <QtWidgets>
#include <QtXml>
#include <proj_api.h>

const QPen CGisItemRte::penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
IGisItem::key_t CGisItemRte::keyUserFocus;

/// used to create a copy of route with new parent
CGisItemRte::CGisItemRte(const CGisItemRte& parentRte, IGisProject * project, int idx, bool clone)
    : IGisItem(project, eTypeRte, idx)
    , penForeground(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    *this = parentRte;
    key.project = project->getKey();
    key.device  = project->getDeviceKey();

    if(clone)
    {
        rte.name += QObject::tr("_Clone");
        key.clear();
        history.events.clear();
    }

    if(parentRte.isOnDevice())
    {
        flags |= eFlagWriteAllowed;
    }
    else if(!parentRte.isReadOnly())
    {
        flags |= eFlagWriteAllowed;
    }
    else
    {
        flags &= ~eFlagWriteAllowed;
    }



    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create route from GPX file
CGisItemRte::CGisItemRte(const QDomNode& xml, IGisProject *parent)
    : IGisItem(parent, eTypeRte, parent->childCount())
    , penForeground(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    // --- start read and process data ----
    readRte(xml, rte);
    // --- stop read and process data ----

    setupHistory();
    deriveSecondaryData();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemRte::CGisItemRte(const history_t& hist, IGisProject * project)
    : IGisItem(project, eTypeRte, project->childCount())
    , penForeground(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
    deriveSecondaryData();
}

CGisItemRte::CGisItemRte(quint64 id, QSqlDatabase& db, IGisProject * project)
    : IGisItem(project, eTypeRte, NOIDX)
    , penForeground(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    loadFromDb(id, db);
}

CGisItemRte::CGisItemRte(const QPolygonF& l, const QString &name, IGisProject *project, int idx)
    : IGisItem(project, eTypeRte, idx)
    , penForeground(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    rte.name = name;
    readRouteDataFromPolyLine(l);

    flags |=  eFlagCreatedInQms|eFlagWriteAllowed;

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemRte::~CGisItemRte()
{
}

void CGisItemRte::deriveSecondaryData()
{
    qreal north = -90;
    qreal east  = -180;
    qreal south =  90;
    qreal west  =  180;

    foreach(const rtept_t &rtept, rte.pts)
    {
        if(rtept.lon < west)
        {
            west    = rtept.lon;
        }
        if(rtept.lon > east)
        {
            east    = rtept.lon;
        }
        if(rtept.lat < south)
        {
            south   = rtept.lat;
        }
        if(rtept.lat > north)
        {
            north   = rtept.lat;
        }
    }

    boundingRect = QRectF(QPointF(west * DEG_TO_RAD, north * DEG_TO_RAD), QPointF(east * DEG_TO_RAD,south * DEG_TO_RAD));
}

void CGisItemRte::setSymbol()
{
    icon = QPixmap("://icons/32x32/Route.png").scaled(22,22, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setIcon(CGisListWks::eColumnIcon, icon);
}

void CGisItemRte::setComment(const QString& str)
{
    rte.cmt = str;
    changed(QObject::tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemRte::setDescription(const QString& str)
{
    rte.desc = str;
    changed(QObject::tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemRte::setLinks(const QList<link_t>& links)
{
    rte.links = links;
    changed(QObject::tr("Changed links"), "://icons/48x48/Link.png");
}



QString CGisItemRte::getInfo(bool allowEdit) const
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
    foreach(const QPointF &point, line)
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
    foreach(const QPointF &pt, line)
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
    keyUserFocus = yes ? key : key_t();
}


void CGisItemRte::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    line.clear();
    if(!isVisible(boundingRect, viewport, gis))
    {
        return;
    }

    QPointF p1 = viewport[0];
    QPointF p2 = viewport[2];
    gis->convertRad2Px(p1);
    gis->convertRad2Px(p2);
    QRectF extViewport(p1,p2);

    QVector<qint32> points;

    foreach(const rtept_t &rtept, rte.pts)
    {
        QPointF pt(rtept.lon * DEG_TO_RAD, rtept.lat * DEG_TO_RAD);

        gis->convertRad2Px(pt);

        line    << pt;
        points  << 1;

        blockedAreas << QRectF(pt - rtept.focus, rtept.icon.size());
        foreach(const subpt_t& subpt, rtept.subpts)
        {
            QPointF pt(subpt.lon, subpt.lat);
            gis->convertRad2Px(pt);
            line    << pt;
            if(subpt.type != subpt_t::eTypeNone)
            {
                points << 2;
            }
            else
            {
                points << 0;
            }
        }
    }

    p.setPen(penBackground);
    p.drawPolyline(line);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    for(int i = 0; i < line.size(); i++)
    {
        switch(points[i])
        {
        case 1:
            p.drawEllipse(line[i],7,7);
            break;
        case 2:
            p.drawEllipse(line[i],4,4);
            break;
        }
    }

    p.setPen(penForeground);
    p.setBrush(penForeground.color());
    drawArrows(line, extViewport, p);
    p.drawPolyline(line);

    p.setPen(Qt::NoPen);
    for(int i = 0; i < line.size(); i++)
    {
        switch(points[i])
        {
        case 1:
            p.setBrush(Qt::red);
            p.drawEllipse(line[i],5,5);
            break;
        case 2:
            p.setBrush(Qt::cyan);
            p.drawEllipse(line[i],2,2);
            break;
        }
    }
}

void CGisItemRte::drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
{
    if(!isVisible(boundingRect, viewport, gis))
    {
        return;
    }


    foreach(const rtept_t &rtept, rte.pts)
    {
        QPointF pt(rtept.lon * DEG_TO_RAD, rtept.lat * DEG_TO_RAD);

        gis->convertRad2Px(pt);
        pt = pt - rtept.focus;
        p.drawPixmap(pt, rtept.icon);

        QRectF rect = fm.boundingRect(rtept.name);
        rect.adjust(-2,-2,2,2);

        // place label on top
        rect.moveCenter(pt + QPointF(rtept.icon.width()/2, -fm.height()));
        if(isBlocked(rect, blockedAreas))
        {
            // place label on bottom
            rect.moveCenter(pt + QPointF( rtept.icon.width()/2, +fm.height() + rtept.icon.height()));
            if(isBlocked(rect, blockedAreas))
            {
                // place label on right
                rect.moveCenter(pt + QPointF( rtept.icon.width() + rect.width()/2, +fm.height()));
                if(isBlocked(rect, blockedAreas))
                {
                    // place label on left
                    rect.moveCenter(pt + QPointF( -rect.width()/2, +fm.height()));
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
}

void CGisItemRte::readRouteDataFromPolyLine(const QPolygonF &l)
{
    rte.pts.clear();
    rte.pts.resize(l.size());

    QPolygonF ele(l.size());
    CMainWindow::self().getEelevationAt(l, ele);

    for(int i = 0; i < l.size(); i++)
    {
        rtept_t& rtept      = rte.pts[i];
        const QPointF& pt   = l[i];

        rtept.lon = pt.x() * RAD_TO_DEG;
        rtept.lat = pt.y() * RAD_TO_DEG;
        rtept.ele = ele[i].y();
    }

    deriveSecondaryData();
}

void CGisItemRte::setDataFromPolyline(const QPolygonF& l)
{
//    delete dlgDetails;

    readRouteDataFromPolyLine(l);

    flags |= eFlagTainted;
    changed(QObject::tr("Changed route points."), "://icons/48x48/LineMove.png");
}

void CGisItemRte::getPolylineFromData(QPolygonF& l)
{
    l.clear();
    foreach(const rtept_t &pt, rte.pts)
    {
        l << QPointF(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD);
    }
}

void CGisItemRte::calc()
{
    CRouterSetup::self().calcRoute(getKey());
}

void CGisItemRte::setResult(T_RoutinoRoute * route)
{
    qint32 idxRtept = -1;
    rtept_t * rtept = 0;

    T_RoutinoRoute * next = route;
    while(next)
    {
        if(next->type == IMP_WAYPOINT)
        {
            idxRtept++;
            rtept = &rte.pts[idxRtept];
            rtept->subpts.clear();
        }

        if(rtept != 0)
        {
            rtept->subpts << subpt_t();
            subpt_t& subpt = rtept->subpts.last();
            subpt.lon = next->lon;
            subpt.lat = next->lat;
            if(next->type > IMP_JUNCT_CONT)
            {
                subpt.type = subpt_t::eTypeJunct;
            }
            else
            {
                subpt.type = subpt_t::eTypeNone;
            }
        }

        next = next->next;
    }

    updateHistory();
}
