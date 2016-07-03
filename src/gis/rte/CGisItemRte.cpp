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
#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/WptIcons.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CDetailsRte.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/CScrOptRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CDraw.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <QtXml>
#include <proj_api.h>

const QPen CGisItemRte::penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
IGisItem::key_t CGisItemRte::keyUserFocus;

#define MIN_DIST_FOCUS 200

void CGisItemRte::rtept_t::updateIcon()
{
    if(sym.isEmpty())
    {
        icon  = QPixmap();
        focus = NOPOINTF;
    }
    else
    {
        icon = getWptIconByName(sym, focus);
    }
}
/// used to create a copy of route with new parent
CGisItemRte::CGisItemRte(const CGisItemRte& parentRte, IGisProject * project, int idx, bool clone)
    : IGisItem(project, eTypeRte, idx)
{
    history = parentRte.history;
    loadHistory(history.histIdxCurrent);


    if(clone)
    {
        rte.name += tr("_Clone");
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
    deriveSecondaryData();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create route from GPX file
CGisItemRte::CGisItemRte(const QDomNode& xml, IGisProject *parent)
    : IGisItem(parent, eTypeRte, parent->childCount())
{
    // --- start read and process data ----
    readRte(xml, rte);
    // --- stop read and process data ----

    setupHistory();
    deriveSecondaryData();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemRte::CGisItemRte(const history_t& hist, const QString &dbHash, IGisProject * project)
    : IGisItem(project, eTypeRte, project->childCount())
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
    deriveSecondaryData();
    if(!dbHash.isEmpty())
    {
        lastDatabaseHash = dbHash;
    }
}

CGisItemRte::CGisItemRte(quint64 id, QSqlDatabase& db, IGisProject * project)
    : IGisItem(project, eTypeRte, NOIDX)
{
    loadFromDb(id, db);
}

CGisItemRte::CGisItemRte(const SGisLine &l, const QString &name, IGisProject *project, int idx)
    : IGisItem(project, eTypeRte, idx)
{
    rte.name = name;
    readRouteDataFromGisLine(l);

    flags |=  eFlagCreatedInQms|eFlagWriteAllowed;

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemRte::CGisItemRte(CFitStream& stream, IGisProject * project)
    : IGisItem(project, eTypeRte, project->childCount())
{
    // --- start read and process data ----
    readRteFromFit(stream);
    // --- stop read and process data ----

    setupHistory();
    deriveSecondaryData();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemRte::~CGisItemRte()
{
    // reset user focus if focused on this track
    if(key == keyUserFocus)
    {
        keyUserFocus.clear();
    }
}

IGisItem * CGisItemRte::createClone()
{
    int idx = -1;
    IGisProject * project = getParentProject();
    if(project)
    {
        idx = project->indexOfChild(this);
    }
    return new CGisItemRte(*this, project, idx, true);
}


bool CGisItemRte::isCalculated()
{
    bool yes = true;
    for(const rtept_t &pt : rte.pts)
    {
        if((pt.fakeSubpt.lat == NOFLOAT) || (pt.fakeSubpt.lon == NOFLOAT))
        {
            yes = false;
        }
    }

    return yes;
}

void CGisItemRte::setElevation(qreal ele, subpt_t& subpt, qreal& lastEle)
{
    if(ele == NOFLOAT)
    {
        subpt.ele = NOINT;
        return;
    }

    subpt.ele = qRound(ele);

    if(lastEle != NOFLOAT)
    {
        qreal delta   = subpt.ele - lastEle;

        if(qAbs(delta) > ASCEND_THRESHOLD)
        {
            if(delta > 0)
            {
                rte.ascend  += delta;
            }
            else
            {
                rte.descend -= delta;
            }
            lastEle = subpt.ele;
        }
    }
    else
    {
        lastEle = subpt.ele;
    }
}

void CGisItemRte::deriveSecondaryData()
{
    QPolygonF pos;
    QPolygonF ele;
    qreal north = -90;
    qreal east  = -180;
    qreal south =  90;
    qreal west  =  180;

    for(rtept_t &rtept : rte.pts)
    {
        west  = qMin(west,  rtept.lon);
        east  = qMax(east,  rtept.lon);
        south = qMin(south, rtept.lat);
        north = qMax(north, rtept.lat);

        pos << (QPointF(rtept.lon, rtept.lat) * DEG_TO_RAD);
        rtept.ele = NOINT;
        rtept.fakeSubpt.ele = NOINT;

        for(subpt_t &subpt : rtept.subpts)
        {
            west  = qMin(west,  subpt.lon);
            east  = qMax(east,  subpt.lon);
            south = qMin(south, subpt.lat);
            north = qMax(north, subpt.lat);

            pos << (QPointF(subpt.lon, subpt.lat) * DEG_TO_RAD);
            subpt.ele = NOINT;
        }
        rtept.updateIcon();
    }

    ele.resize(pos.size());
    ele.fill(NOPOINTF);
    CMainWindow::self().getElevationAt(pos, ele);

    if(!ele.isEmpty())
    {
        qreal lastEle = NOFLOAT;
        int i = 0;
        rte.descend = 0;
        rte.ascend = 0;

        for(rtept_t &rtept : rte.pts)
        {
            setElevation(ele[i++].y(), rtept.fakeSubpt, lastEle);
            rtept.ele = rtept.fakeSubpt.ele;

            for(subpt_t &subpt : rtept.subpts)
            {
                setElevation(ele[i++].y(), subpt, lastEle);
            }
        }
    }

    boundingRect = QRectF(QPointF(west * DEG_TO_RAD, north * DEG_TO_RAD), QPointF(east * DEG_TO_RAD,south * DEG_TO_RAD));
}

void CGisItemRte::edit()
{
    CDetailsRte dlg(*this, CMainWindow::getBestWidgetForParent());
    dlg.exec();
}

void CGisItemRte::toTrack()
{
    QString name;
    IGisProject * project;

    if(!getNameAndProject(name, project, tr("track")))
    {
        return;
    }

    SGisLine line;
    getPolylineFromData(line);

    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->getElevationAt(line);
    }

    new CGisItemTrk(line, name, project, -1);
}

void CGisItemRte::setSymbol()
{
    icon = QPixmap("://icons/32x32/Route.png").scaled(22,22, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setIcon(CGisListWks::eColumnIcon, icon);
}

void CGisItemRte::setName(const QString& str)
{
    setText(CGisListWks::eColumnName, str);
    rte.name = str;
    changed(tr("Changed name."), "://icons/48x48/EditText.png");
}

void CGisItemRte::setComment(const QString& str)
{
    rte.cmt = str;
    changed(tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemRte::setDescription(const QString& str)
{
    rte.desc = str;
    changed(tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemRte::setLinks(const QList<link_t>& links)
{
    rte.links = links;
    changed(tr("Changed links"), "://icons/48x48/Link.png");
}



QString CGisItemRte::getInfo(bool showName) const
{
    QString val1, unit1, val2, unit2;
    QString str = "<div>";

    if(showName)
    {
        str += "<b>" + getName() + "</b><br />";
    }

    if(rte.totalDistance != NOFLOAT)
    {
        IUnit::self().meter2distance(rte.totalDistance, val1, unit1);
        str += tr("Length: %1 %2").arg(val1).arg(unit1);
    }
    else
    {
        str += tr("Length: -");
    }

    str += "<br/>\n";
    if(rte.totalTime != 0)
    {
        IUnit::self().seconds2time(rte.totalTime, val1, unit1);
        str += tr("Time: %1 %2").arg(val1).arg(unit1);
    }
    else
    {
        str += tr("Time: -");
    }

    if((rte.ascend != NOFLOAT) && (rte.descend != NOFLOAT))
    {
        str += "<br/>\n";
        QString val1, val2, unit1, unit2;
        IUnit::self().meter2elevation(rte.ascend,  val1, unit1);
        IUnit::self().meter2elevation(rte.descend, val2, unit2);

        str += tr("%1%2 %3, %4%5 %6").arg(QChar(0x2197)).arg(val1).arg(unit1).arg(QChar(0x2198)).arg(val2).arg(unit2);
    }

    if(!rte.lastRoutedWith.isEmpty())
    {
        str += "<br/>\n";
        str += tr("Last time routed:<br/>%1").arg(IUnit::datetime2string(rte.lastRoutedTime, false, boundingRect.center()));
        str += "<br/>\n";
        str += tr("with %1").arg(rte.lastRoutedWith);
    }
    return str;
}

IScrOpt * CGisItemRte::getScreenOptions(const QPoint& origin, IMouse * mouse)
{
    if(scrOpt.isNull())
    {
        scrOpt = new CScrOptRte(this, origin, mouse);
    }
    return scrOpt;
}

QPointF CGisItemRte::getPointCloseBy(const QPoint& screenPos)
{
    QMutexLocker lock(&mutexItems);

    qint32 d    = NOINT;
    QPointF pt  = NOPOINTF;
    for(const QPointF &point : line)
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
    QMutexLocker lock(&mutexItems);

    qreal dist = GPS_Math_DistPointPolyline(line, pos);
    return dist < 20;
}

bool CGisItemRte::isWithin(const QRectF& area, selflags_t flags)
{
    return (flags & eSelectionRte) ? IGisItem::isWithin(area, flags, line) : false;
}


void CGisItemRte::gainUserFocus(bool yes)
{
    keyUserFocus = yes ? key : key_t();
}


void CGisItemRte::looseUserFocus()
{
    if(keyUserFocus == key)
    {
        keyUserFocus.clear();
    }
}



void CGisItemRte::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    QMutexLocker lock(&mutexItems);

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

    QVector<qint32>  points;
    QVector<QPixmap> icons;
    QVector<QPointF> focus;

    for(const rtept_t &rtept : rte.pts)
    {
        QPointF pt(rtept.lon * DEG_TO_RAD, rtept.lat * DEG_TO_RAD);

        gis->convertRad2Px(pt);

        line << pt;
        points << 1;
        icons << rtept.icon;
        focus << rtept.focus;

        blockedAreas << QRectF(pt - rtept.focus, rtept.icon.size());
        for(const subpt_t &subpt : rtept.subpts)
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

    p.setPen(hasUserFocus() ? penForegroundFocus : penForeground);
    p.setBrush(hasUserFocus() ? penForegroundFocus.color() : penForeground.color());
    CDraw::arrows(line, extViewport, p, 10, 80, 1.0);
    p.drawPolyline(line);

    p.setPen(Qt::NoPen);
    for(int i = 0, n = 0; i < line.size(); i++)
    {
        switch(points[i])
        {
        case 1:
            p.setBrush(Qt::red);
            p.drawEllipse(line[i],5,5);
            if(focus[n] != NOPOINTF)
            {
                p.drawPixmap(line[i] - focus[n], icons[n]);
            }
            n++;
            break;

        case 2:
            p.setBrush(Qt::cyan);
            p.drawEllipse(line[i],3,3);
            break;
        }
    }
}

void CGisItemRte::drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    QMutexLocker lock(&mutexItems);
    if(rte.pts.isEmpty())
    {
        return;
    }

    QDateTime startTime = rte.pts.first().fakeSubpt.time;

    if(hasUserFocus() && mouseMoveFocus && mouseMoveFocus->lon != NOFLOAT && mouseMoveFocus->lat != NOFLOAT)
    {
        QPointF anchor(mouseMoveFocus->lon, mouseMoveFocus->lat);
        anchor *= DEG_TO_RAD;
        gis->convertRad2Px(anchor);
        p.drawEllipse(anchor, 5, 5);

        QString str, val, unit;
        IUnit::self().seconds2time((mouseMoveFocus->time.toTime_t() - startTime.toTime_t()), val, unit);
        str += tr("Time: %1 %2").arg(val).arg(unit) + " ";
        IUnit::self().meter2distance(mouseMoveFocus->distance, val, unit);
        str += tr("Distance: %1 %2").arg(val).arg(unit);
        str += "\n" + mouseMoveFocus->instruction;

        // calculate bounding box of text
        QFont f = CMainWindow::self().getMapFont();
        QFontMetrics fm(f);
        QRect rectText = fm.boundingRect(QRect(0,0,500,0), Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, str);
        rectText.adjust(-5, -5, 5, 5);
        rectText.moveBottomLeft(anchor.toPoint() + QPoint(-50,-50));

        p.setFont(f);
        CDraw::bubble(p, rectText, anchor.toPoint(), 18 /* px */, 21 /* px */);

        p.save();
        p.translate(5,5);
        p.setPen(Qt::darkBlue);
        p.drawText(rectText, str);
        p.restore();
    }
}

void CGisItemRte::drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
{
    QMutexLocker lock(&mutexItems);
    if(!isVisible(boundingRect, viewport, gis))
    {
        return;
    }


    for(const rtept_t &rtept : rte.pts)
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

        CDraw::text(rtept.name, p, rect.toRect(), Qt::darkBlue);
        blockedAreas << rect;
    }
}

void CGisItemRte::drawHighlight(QPainter& p)
{
    QMutexLocker lock(&mutexItems);

    if(line.isEmpty() || hasUserFocus())
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
    QMutexLocker lock(&mutexItems);
    mouseMoveFocus = nullptr;

    readRouteDataFromGisLine(l);

    flags |= eFlagTainted;
    changed(tr("Changed route points."), "://icons/48x48/LineMove.png");
}

void CGisItemRte::getPolylineFromData(SGisLine& l)
{
    QMutexLocker lock(&mutexItems);
    l.clear();
    for(const rtept_t &rtept : rte.pts)
    {
        l << point_t(QPointF(rtept.lon * DEG_TO_RAD, rtept.lat * DEG_TO_RAD));

        point_t& pt = l.last();

        pt.subpts.clear();
        for(const subpt_t &subpt : rtept.subpts)
        {
            pt.subpts << IGisLine::subpt_t(QPointF(subpt.lon * DEG_TO_RAD, subpt.lat * DEG_TO_RAD));
        }
    }
}

void CGisItemRte::calc()
{
    QMutexLocker lock(&mutexItems);
    mouseMoveFocus = nullptr;
    for(int i = 0; i < rte.pts.size(); i++)
    {
        rte.pts[i].subpts.clear();
    }
    CRouterSetup::self().calcRoute(getKey());
}

void CGisItemRte::reset()
{
    QMutexLocker lock(&mutexItems);
    for(int i = 0; i < rte.pts.size(); i++)
    {
        rtept_t& pt = rte.pts[i];
        pt.subpts.clear();
        pt.fakeSubpt = subpt_t();
    }

    mouseMoveFocus  = nullptr;
    rte.totalDistance   = NOFLOAT;
    rte.totalTime       = 0;
    rte.lastRoutedTime  = QDateTime();
    rte.lastRoutedWith  = "";

    deriveSecondaryData();
    updateHistory();

    if(key == keyUserFocus)
    {
        gainUserFocus(false);
    }
}


QPointF CGisItemRte::setMouseFocusByPoint(const QPoint& pt, focusmode_e fmode, const QString &owner)
{
    QMutexLocker lock(&mutexItems);

    const subpt_t * newPointOfFocus = nullptr;
    quint32 idx = 0;

    if(pt != NOPOINT && GPS_Math_DistPointPolyline(line, pt) < MIN_DIST_FOCUS)
    {
        quint32 i = 0;
        qint32 d1 = NOINT;

        for(const QPointF &point : line)
        {
            int tmp = (pt - point).manhattanLength();
            if(tmp <= d1)
            {
                idx = i;
                d1  = tmp;
            }
            i++;
        }

        newPointOfFocus = getSubPtByIndex(idx);
    }

    if(newPointOfFocus && (newPointOfFocus->type == subpt_t::eTypeNone))
    {
        newPointOfFocus = nullptr;
    }


    mouseMoveFocus = newPointOfFocus;

    return newPointOfFocus ? ((int)idx < line.size() ? line[idx] : NOPOINTF) : NOPOINTF;
}

const CGisItemRte::subpt_t * CGisItemRte::getSubPtByIndex(quint32 idx)
{
    quint32 cnt = 0;
    for(const rtept_t &rtept : rte.pts)
    {
        if(cnt == idx)
        {
            return &rtept.fakeSubpt;
        }

        for(const subpt_t &subpt : rtept.subpts)
        {
            cnt++;
            if(cnt == idx)
            {
                return &subpt;
            }
        }
        cnt++;
    }

    return nullptr;
}

void CGisItemRte::setResult(Routino_Output * route, const QString& options)
{
    QMutexLocker lock(&mutexItems);

    qint32 idxRtept = -1;
    rtept_t * rtept = nullptr;

    QDateTime time = QDateTime::currentDateTimeUtc();

    Routino_Output * next = route;
    while(next)
    {
        if(next->type == ROUTINO_POINT_WAYPOINT)
        {
            idxRtept++;
            rtept = &rte.pts[idxRtept];
            rtept->subpts.clear();
            rtept->fakeSubpt.lon       = next->lon * RAD_TO_DEG;
            rtept->fakeSubpt.lat       = next->lat * RAD_TO_DEG;

            rtept->fakeSubpt.turn      = next->turn;
            rtept->fakeSubpt.bearing   = next->bearing;
            rtept->fakeSubpt.distance  = next->dist * 1000;
            rtept->fakeSubpt.time      = time.addSecs(next->time * 60);

            rtept->fakeSubpt.type      = subpt_t::eTypeWpt;
            rtept->fakeSubpt.instruction = QString(next->desc1) + ".\n" + QString(next->desc2) + ".";
        }
        else if(rtept != nullptr)
        {
            rtept->subpts << subpt_t();
            subpt_t& subpt  = rtept->subpts.last();
            subpt.lon       = next->lon * RAD_TO_DEG;
            subpt.lat       = next->lat * RAD_TO_DEG;

            subpt.turn      = next->turn;
            subpt.bearing   = next->bearing;
            subpt.distance  = next->dist * 1000;
            subpt.time      = time.addSecs(next->time * 60);

            if(next->name != 0)
            {
                subpt.streets << next->name;
            }

            if(next->type > ROUTINO_POINT_CHANGE)
            {
                subpt.type = subpt_t::eTypeJunct;
            }
            else
            {
                subpt.type = subpt_t::eTypeNone;
            }

            rte.totalDistance = subpt.distance;
            rte.totalTime     = subpt.time.toTime_t() - time.toTime_t();
            subpt.instruction = QString(next->desc1) + ".\n" + QString(next->desc2) + ".";
        }

        next = next->next;
    }

    rte.lastRoutedTime = QDateTime::currentDateTimeUtc();
    rte.lastRoutedWith = "Routino, " + options;

    deriveSecondaryData();
    updateHistory();
}

struct maneuver_t
{
    QStringList streets;
    QString instruction;
    quint32 time;
    qreal dist;
    qint32 bearing;
    qint32 turn;
};

static const qint32 idx2bearing[] =
{
    NOINT
    , 0
    , -45
    , 45
    , 180
    , 135
    , -135
    , -90
    , 90
};


void CGisItemRte::setResult(const QDomDocument& xml, const QString &options)
{
    QMutexLocker lock(&mutexItems);

    QDateTime localtime = QDateTime::currentDateTimeUtc();

    QDomElement response    = xml.firstChildElement("response");
    QDomElement route       = response.firstChildElement("route");

    // get time of travel
    QDomElement xmlTime     = route.firstChildElement("time");
    rte.totalTime = xmlTime.text().toUInt();


    // build list of maneuvers
    QDomNodeList xmlLegs       = route.firstChildElement("legs").elementsByTagName("leg");
    const qint32 L = xmlLegs.size();

    QList<maneuver_t> maneuvers;

    for(int l = 0; l < L; l++)
    {
        QDomNode xmlLeg = xmlLegs.item(l);
        QDomNodeList xmlManeuvers = xmlLeg.firstChildElement("maneuvers").elementsByTagName("maneuver");
        const qint32 M = xmlManeuvers.size();
        for(int m = 0; m < M; m++)
        {
            maneuvers << maneuver_t();
            maneuver_t& maneuver    = maneuvers.last();
            QDomNode xmlManeuver    = xmlManeuvers.item(m);
            maneuver.instruction    = xmlManeuver.firstChildElement("narrative").text();
            maneuver.time           = xmlManeuver.firstChildElement("time").text().toUInt();
            maneuver.dist           = xmlManeuver.firstChildElement("distance").text().toFloat();

            maneuver.bearing        = idx2bearing[xmlManeuver.firstChildElement("direction").text().toUInt()];
            maneuver.turn           = xmlManeuver.firstChildElement("turnType").text().toInt();

            QDomNodeList xmlStreets = xmlManeuver.toElement().elementsByTagName("streets");
            const int S = xmlStreets.size();
            for(int s = 0; s < S; s++)
            {
                QDomNode xmlStreet = xmlStreets.item(s);
                maneuver.streets << xmlStreet.toElement().text();
            }
        }
    }

    QVector<subpt_t> shape;

    // read the shape
    QDomElement xmlShape        = route.firstChildElement("shape");
    QDomElement xmlShapePoints  = xmlShape.firstChildElement("shapePoints");
    QDomNodeList xmlLatLng      = xmlShapePoints.elementsByTagName("latLng");
    const qint32 N = xmlLatLng.size();
    for(int n = 0; n < N; n++)
    {
        QDomNode elem   = xmlLatLng.item(n);
        QDomElement lat = elem.firstChildElement("lat");
        QDomElement lng = elem.firstChildElement("lng");

        shape << subpt_t();
        subpt_t& subpt = shape.last();
        subpt.lon = lng.text().toFloat();
        subpt.lat = lat.text().toFloat();
    }


    QVector<quint32> idxLegs;
    QDomElement xmlLegIndexes = xmlShape.firstChildElement("legIndexes");
    QDomNodeList xmlIndex     = xmlLegIndexes.elementsByTagName("index");
    const qint32 I = xmlIndex.size();
    for(int i = 0; i < I; i++)
    {
        QDomNode elem = xmlIndex.item(i);
        idxLegs << elem.toElement().text().toUInt();
    }

    quint32 time = 0;
    quint32 dist = 0;
    QDomElement xmlManeuverIndexes = xmlShape.firstChildElement("maneuverIndexes");
    xmlIndex                       = xmlManeuverIndexes.elementsByTagName("index");
    qint32 M = xmlIndex.size();
    for(int m = 0; m < M; m++)
    {
        QDomNode elem           = xmlIndex.item(m);
        quint32 idx             = elem.toElement().text().toUInt();
        subpt_t& subpt          = shape[idx];
        maneuver_t& maneuver    = maneuvers[m];
        subpt.type              = subpt_t::eTypeJunct;
        subpt.instruction       = maneuver.instruction;

        subpt.time              = localtime.addSecs(time);
        time += maneuver.time;

        subpt.distance          = dist;
        dist += maneuver.dist * 1000;

        subpt.bearing           = maneuver.bearing;
        subpt.turn              = maneuver.turn;

        subpt.streets           = maneuver.streets;
    }

    for(int i = 0; i < rte.pts.size() - 1; i++ )
    {
        quint32 idx1 = idxLegs[i];
        quint32 idx2 = idxLegs[i+1];

        rtept_t& rtept      = rte.pts[i];
        rtept.subpts        = shape.mid(idx1, idx2 - idx1 + 1);
        rtept.fakeSubpt.lon = rtept.lon;
        rtept.fakeSubpt.lat = rtept.lat;
    }

    rtept_t& rtept = rte.pts.last();
    rtept.fakeSubpt.lon = rtept.lon;
    rtept.fakeSubpt.lat = rtept.lat;

    rte.totalDistance  = dist;
    rte.lastRoutedTime = QDateTime::currentDateTimeUtc();
    rte.lastRoutedWith = "MapQuest" + options;

    deriveSecondaryData();
    updateHistory();
}


