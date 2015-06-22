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
    , totalDays(NOINT)
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
    , totalDays(NOINT)
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
    , totalDays(NOINT)
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
    deriveSecondaryData();
}

CGisItemRte::CGisItemRte(quint64 id, QSqlDatabase& db, IGisProject * project)
    : IGisItem(project, eTypeRte, NOIDX)
    , penForeground(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , totalDays(NOINT)
{
    loadFromDb(id, db);
}

CGisItemRte::CGisItemRte(const SGisLine &l, const QString &name, IGisProject *project, int idx)
    : IGisItem(project, eTypeRte, idx)
    , penForeground(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , totalDays(NOINT)
{
    rte.name = name;
    readRouteDataFromGisLine(l);

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

    totalDistance = NOFLOAT;
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
    QString val1, unit1, val2, unit2;
    QString str = "<div>";

    if(allowEdit)
    {
        str += "<b>" + toLink(isReadOnly(), "name", getName(), "") + "</b>";
    }
    else
    {
        str += "<div style='font-weight: bold;'>" + getName() + "</div>";
    }

    str += "<br/>\n";
    if(totalDistance != NOFLOAT)
    {
        IUnit::self().meter2distance(totalDistance, val1, unit1);
        str += QObject::tr("Length: %1 %2").arg(val1).arg(unit1);
    }
    else
    {
        str += QObject::tr("Length: -");
    }

    str += "<br/>\n";
    if(totalTime.isValid())
    {
        if(totalDays != 0 && totalDays != NOINT)
        {
            str += QObject::tr("Time: %2 days %1").arg(totalTime.toString()).arg(totalDays);
        }
        else
        {
            str += QObject::tr("Time: %1").arg(totalTime.toString());
        }
    }
    else
    {
        str += QObject::tr("Time: -");
    }

    if(!lastRoutedWith.isEmpty())
    {
        str += "<br/>\n";
        str += QObject::tr("Last time routed:<br/>%1").arg(IUnit::datetime2string(lastRoutedTime, false, boundingRect.center()));
        str += "<br/>\n";
        str += QObject::tr("with %1").arg(lastRoutedWith);
    }




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

        line << pt;
        points << 1;

        blockedAreas << QRectF(pt - rtept.focus, rtept.icon.size());
        foreach(const subpt_t &subpt, rtept.subpts)
        {
            QPointF pt(subpt.lon * DEG_TO_RAD, subpt.lat * DEG_TO_RAD);
            gis->convertRad2Px(pt);
            line << pt;
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
            p.drawEllipse(line[i],5,5);
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
            p.drawEllipse(line[i],3,3);
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
        //pt = pt - rtept.focus;
        //p.drawPixmap(pt, rtept.icon);

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

void CGisItemRte::readRouteDataFromGisLine(const SGisLine &l)
{
    bool doAutoRouting = !l.first().subpts.isEmpty();
    rte.pts.clear();

    for(int i = 0; i < l.size(); i++)
    {
        rte.pts << rtept_t();

        rtept_t& rtept      = rte.pts.last();
        const point_t& pt   = l[i];

        rtept.lon = pt.coord.x() * RAD_TO_DEG;
        rtept.lat = pt.coord.y() * RAD_TO_DEG;
        rtept.ele = pt.ele;
    }

    if(doAutoRouting)
    {
        calc();
    }
    deriveSecondaryData();
}

void CGisItemRte::setDataFromPolyline(const SGisLine &l)
{
//    delete dlgDetails;

    readRouteDataFromGisLine(l);

    flags |= eFlagTainted;
    changed(QObject::tr("Changed route points."), "://icons/48x48/LineMove.png");
}

void CGisItemRte::getPolylineFromData(SGisLine& l)
{
    l.clear();
    foreach(const rtept_t &rtept, rte.pts)
    {
        l << point_t(QPointF(rtept.lon * DEG_TO_RAD, rtept.lat * DEG_TO_RAD));

        point_t& pt = l.last();

        pt.subpts.clear();
        foreach(const subpt_t &subpt, rtept.subpts)
        {
            pt.subpts << IGisLine::subpt_t(QPointF(subpt.lon * DEG_TO_RAD, subpt.lat * DEG_TO_RAD));
        }
    }
}

void CGisItemRte::calc()
{
    for(int i = 0; i < rte.pts.size(); i++)
    {
        rte.pts[i].subpts.clear();
    }
    CRouterSetup::self().calcRoute(getKey());
}

void CGisItemRte::reset()
{
    for(int i = 0; i < rte.pts.size(); i++)
    {
        rte.pts[i].subpts.clear();
    }

    totalDays       = NOINT;
    totalDistance   = NOFLOAT;
    totalTime       = QTime();
    lastRoutedTime  = QDateTime();
    lastRoutedWith  = "";

    updateHistory();
}

void CGisItemRte::setResult(T_RoutinoRoute * route, const QString& options)
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
            subpt_t& subpt  = rtept->subpts.last();
            subpt.lon       = next->lon * RAD_TO_DEG;
            subpt.lat       = next->lat * RAD_TO_DEG;

            subpt.turn      = next->turn;
            subpt.bearing   = next->bearing;
            subpt.distance  = next->dist;
            subpt.time      = subpt.time.addSecs(next->time/10);

            if(next->string != 0)
            {
                subpt.names << next->string;
            }

            if(next->type > IMP_CHANGE)
            {
                subpt.type = subpt_t::eTypeJunct;
            }
            else
            {
                subpt.type = subpt_t::eTypeNone;
            }

            totalDistance = subpt.distance;
            totalTime     = subpt.time;
            totalDays     = qFloor(next->time/864000);
        }

        next = next->next;
    }

    lastRoutedTime = QDateTime::currentDateTimeUtc();
    lastRoutedWith = "Routino, " + options;

    updateHistory();
}
