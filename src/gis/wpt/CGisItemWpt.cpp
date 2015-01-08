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
#include "gis/prj/IGisProject.h"
#include "gis/CGisDraw.h"
#include "gis/WptIcons.h"
#include "gis/CGisListWks.h"
#include "canvas/CCanvas.h"
#include "mouse/IMouse.h"
#include "units/IUnit.h"
#include "helpers/CWptIconDialog.h"
#include "helpers/CPositionDialog.h"
#include "GeoMath.h"
#include "CMainWindow.h"



#include <QtWidgets>
#include <QtXml>

IGisItem::key_t CGisItemWpt::keyUserFocus;
QString CGisItemWpt::lastName;
QString CGisItemWpt::lastIcon;

/// used to add a new waypoint
CGisItemWpt::CGisItemWpt(const QPointF& pos, const QString& name, const QString &icon, IGisProject *project)
    : IGisItem(project, eTypeWpt, -1)
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{

    wpt.name    = name;
    wpt.sym     = icon;
    wpt.lon     = pos.x();
    wpt.lat     = pos.y();
    wpt.time    = QDateTime::currentDateTimeUtc();

    flags = eFlagCreatedInQms|eFlagWriteAllowed;
    qreal ele = CMainWindow::self().getEelevationAt(pos * DEG_TO_RAD);
    wpt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    genKey();
    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to move a copy of waypoint
CGisItemWpt::CGisItemWpt(const QPointF& pos, const CGisItemWpt& parentWpt, IGisProject *project)
    : IGisItem(project, eTypeWpt, -1)
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    *this = parentWpt;
    wpt.lon     = pos.x();
    wpt.lat     = pos.y();
    wpt.time    = QDateTime::currentDateTimeUtc();

    key.item.clear();
    history.events.clear();
    flags = eFlagCreatedInQms|eFlagWriteAllowed;

    qreal ele = CMainWindow::self().getEelevationAt(pos * DEG_TO_RAD);
    wpt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    genKey();
    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create a copy of waypoint with new parent
CGisItemWpt::CGisItemWpt(const CGisItemWpt &parentWpt, IGisProject *project, int idx, bool clone)
    : IGisItem(project, eTypeWpt, idx)
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    *this = parentWpt;
    key.project = project->getKey();

    if(clone)
    {
        wpt.name += QObject::tr("_Clone");
        key.item.clear();
        history.events.clear();
    }

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create waypoint from GPX file
CGisItemWpt::CGisItemWpt(const QDomNode &xml, IGisProject *project)
    : IGisItem(project, eTypeWpt, project->childCount())
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    readGpx(xml);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    genKey();
    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::CGisItemWpt(const history_t& hist, IGisProject * project)
    : IGisItem(project, eTypeWpt, project->childCount())
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);    
}

CGisItemWpt::CGisItemWpt(quint64 id, QSqlDatabase& db, IGisProject * project)
    : IGisItem(project, eTypeWpt, -1)
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    loadFromDb(id, db);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);
}

CGisItemWpt::~CGisItemWpt()
{

}

void CGisItemWpt::setSymbol()
{
    setIcon();
}


const QString& CGisItemWpt::getNewName()
{
    const int s = lastName.size();
    if(s != 0)
    {
        int idx;
        for(idx = s; idx > 0; idx--)
        {
            if(!lastName[idx - 1].isDigit())
            {
                break;
            }
        }

        if(idx == 0)
        {
            lastName = QString::number(lastName.toInt() + 1);
        }
        else
        {
            lastName = lastName.left(idx) + QString::number(lastName.mid(idx).toInt() + 1);
        }

    }
    lastName = QInputDialog::getText(0, QObject::tr("Edit name..."), QObject::tr("Enter new waypoint name."), QLineEdit::Normal, lastName);
    return lastName;
}

const QString& CGisItemWpt::getNewIcon()
{
    QToolButton but;
    but.setObjectName(lastIcon);
    CWptIconDialog dlg(&but);
    dlg.exec();
    lastIcon = but.objectName();

    return lastIcon;
}

void CGisItemWpt::getNewPosition(QPointF& pos)
{
    CPositionDialog dlg(0, pos);
    if(dlg.exec() == QDialog::Rejected)
    {
        pos = NOPOINTF;
    }
}


QString CGisItemWpt::getInfo() const
{
    QString str = "<div style='font-weight: bold;'>" + getName() + "</div>";

    if(geocache.hasData)
    {
        str += QString(" %4 (%1, D %2, T %3)").arg(geocache.container).arg(geocache.difficulty, 0,'f',1).arg(geocache.terrain, 0,'f',1).arg(geocache.name);
    }

    if(wpt.time.isValid())
    {
        if(!str.isEmpty()) str += "<br/>\n";

        str += IUnit::datetime2string(wpt.time, false, QPointF(wpt.lon*DEG_TO_RAD, wpt.lat*DEG_TO_RAD));
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

    QString desc = removeHtml(wpt.desc).simplified();
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
        QString cmt = removeHtml(wpt.cmt).simplified();
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
    if(scrOpt.isNull())
    {
        scrOpt = new CScrOptWpt(this, origin, mouse);
    }
    return scrOpt;
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

    QTreeWidgetItem::setIcon(CGisListWks::eColumnName,icon);
}

void CGisItemWpt::setName(const QString& str)
{    
    setText(CGisListWks::eColumnName, str);
    lastName = str;
    wpt.name = str;
    changed(QObject::tr("Changed name"),"://icons/48x48/EditText.png");
}

void CGisItemWpt::setPosition(const QPointF& pos)
{
    wpt.lon = pos.x();
    wpt.lat = pos.y();
    changed(QObject::tr("Changed position"),"://icons/48x48/WptMove.png");
}

void CGisItemWpt::setElevation(qint32 val)
{
    wpt.ele = val;
    changed(QObject::tr("Changed elevation"),"://icons/48x48/SetEle.png");
}

void CGisItemWpt::setProximity(qreal val)
{
    proximity = val;
    changed(QObject::tr("Changed proximity"),"://icons/48x48/WptProx.png");
}

void CGisItemWpt::setIcon(const QString& name)
{    
    lastIcon = name;
    wpt.sym  = name;

    QPointF focus;
    QString path;
    getWptIconByName(name, focus, &path);

    changed(QObject::tr("Changed icon"), path);
}

void CGisItemWpt::setComment(const QString& str)
{
    wpt.cmt = str;
    changed(QObject::tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemWpt::setDescription(const QString& str)
{
    wpt.desc = str;
    changed(QObject::tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemWpt::setLinks(const QList<link_t>& links)
{
    wpt.links = links;
    changed(QObject::tr("Changed links"), "://icons/48x48/Link.png");
}


bool CGisItemWpt::isCloseTo(const QPointF& pos)
{
    if(posScreen == NOPOINTF)
    {
        return false;
    }

    return ((pos - posScreen).manhattanLength() < 22);
}

void CGisItemWpt::gainUserFocus(bool yes)
{
    keyUserFocus = yes ? key : key_t();
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

void CGisItemWpt::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    posScreen = QPointF(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
    if(!isVisible(posScreen, viewport, gis))
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

        qreal r = pt1.x() - posScreen.x();

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

void CGisItemWpt::drawLabel(QPainter& p, const QPolygonF &viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
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
