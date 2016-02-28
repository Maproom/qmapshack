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
#include "gis/wpt/CDetailsGeoCache.h"
#include "gis/wpt/CDetailsWpt.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CScrOptWpt.h"
#include "gis/wpt/CSetupNewWpt.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "mouse/IMouse.h"
#include "units/IUnit.h"

#include <QtWidgets>
#include <QtXml>

IGisItem::key_t CGisItemWpt::keyUserFocus;

CGisItemWpt::CGisItemWpt(const QPointF &pos, qreal ele, const QDateTime &time, const QString &name, const QString &icon, IGisProject *project)
    : IGisItem(project, eTypeWpt, NOIDX)
{
    wpt.name    = name;
    wpt.sym     = icon;
    wpt.lon     = pos.x();
    wpt.lat     = pos.y();
    wpt.ele     = (ele == NOFLOAT) ? NOINT : qRound(ele);
    wpt.time    = time;

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

/// used to add a new waypoint
CGisItemWpt::CGisItemWpt(const QPointF& pos, const QString& name, const QString &icon, IGisProject *project)
    : CGisItemWpt(pos, NOFLOAT, QDateTime::currentDateTimeUtc(), name, icon, project)
{
    flags = eFlagCreatedInQms|eFlagWriteAllowed;
    qreal ele = CMainWindow::self().getElevationAt(pos * DEG_TO_RAD);
    wpt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to move a copy of waypoint
CGisItemWpt::CGisItemWpt(const QPointF& pos, const CGisItemWpt& parentWpt, IGisProject *project)
    : IGisItem(project, eTypeWpt, NOIDX)
{
    *this = parentWpt;
    wpt.lon     = pos.x();
    wpt.lat     = pos.y();
    wpt.time    = QDateTime::currentDateTimeUtc();

    key.clear();
    history.events.clear();
    flags = eFlagCreatedInQms|eFlagWriteAllowed;

    qreal ele = CMainWindow::self().getElevationAt(pos * DEG_TO_RAD);
    wpt.ele = (ele == NOFLOAT) ? NOINT : qRound(ele);

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create a copy of waypoint with new parent
CGisItemWpt::CGisItemWpt(const CGisItemWpt &parentWpt, IGisProject *project, int idx, bool clone)
    : IGisItem(project, eTypeWpt, idx)
{
    history = parentWpt.history;
    loadHistory(history.histIdxCurrent);

    if(clone)
    {
        wpt.name += tr("_Clone");
        key.clear();
        history.events.clear();
        setupHistory();
    }

    if(parentWpt.isOnDevice() || !parentWpt.isReadOnly())
    {
        flags |= eFlagWriteAllowed;
    }
    else
    {
        flags &= ~eFlagWriteAllowed;
    }

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);
    updateDecoration(eMarkChanged, eMarkNone);
}

/// used to create waypoint from GPX file
CGisItemWpt::CGisItemWpt(const QDomNode &xml, IGisProject *project)
    : IGisItem(project, eTypeWpt, project->childCount())
{
    readGpx(xml);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    genKey();
    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::CGisItemWpt(const history_t& hist, const QString &dbHash, IGisProject * project)
    : IGisItem(project, eTypeWpt, project->childCount())
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);
    if(!dbHash.isEmpty())
    {
        lastDatabaseHash = dbHash;
    }
}

CGisItemWpt::CGisItemWpt(quint64 id, QSqlDatabase& db, IGisProject * project)
    : IGisItem(project, eTypeWpt, NOIDX)
{
    loadFromDb(id, db);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);
}

CGisItemWpt::CGisItemWpt(const CTwoNavProject::wpt_t &tnvWpt, IGisProject * project)
    : IGisItem(project, eTypeWpt, NOIDX)
{
    readTwoNav(tnvWpt);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    genKey();
    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::CGisItemWpt(CFitStream& stream, IGisProject * project)
    : IGisItem(project, eTypeWpt, NOIDX)
    , proximity(NOFLOAT)
    , posScreen(NOPOINTF)
{
    readWptFromFit(stream);
    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    genKey();
    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemWpt::~CGisItemWpt()
{
}

IGisItem * CGisItemWpt::createClone()
{
    int idx = -1;
    IGisProject * project = dynamic_cast<IGisProject*>(parent());
    if(project)
    {
        idx = project->indexOfChild(this);
    }
    return new CGisItemWpt(*this, project, idx, true);
}


void CGisItemWpt::setSymbol()
{
    setIcon();
}

bool CGisItemWpt::getNewWptData(QPointF& pt, QString& icon, QString& name)
{
    SETTINGS;
    QString lastName = cfg.value("Waypoint/lastName", "wpt").toString();
    QString lastIcon = cfg.value("Waypoint/lastIcon", "Waypoint").toString();

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

    name = lastName;
    icon = lastIcon;

    CSetupNewWpt dlg(pt, icon, name, CMainWindow::getBestWidgetForParent());
    if(dlg.exec() != QDialog::Accepted)
    {
        return false;
    }

    cfg.setValue("Waypoint/lastName", name);
    cfg.setValue("Waypoint/lastIcon", icon);

    return true;
}

QString CGisItemWpt::getInfo(bool allowEdit) const
{
    QString str = "<div style='font-weight: bold;'>" + getName() + "</div>";

    if(geocache.hasData)
    {
        str += QString(" %4 (%1, D %2, T %3)").arg(geocache.container).arg(geocache.difficulty, 0,'f',1).arg(geocache.terrain, 0,'f',1).arg(geocache.name);
    }

    if(wpt.time.isValid())
    {
        if(!str.isEmpty())
        {
            str += "<br/>\n";
        }

        str += IUnit::datetime2string(wpt.time, false, QPointF(wpt.lon*DEG_TO_RAD, wpt.lat*DEG_TO_RAD));
    }

    if(wpt.ele != NOINT)
    {
        if(!str.isEmpty())
        {
            str += "<br/>\n";
        }
        QString val, unit;
        IUnit::self().meter2elevation(wpt.ele, val, unit);
        str += tr("Elevation: %1 %2").arg(val).arg(unit);
    }

    if(proximity != NOFLOAT)
    {
        if(!str.isEmpty())
        {
            str += "<br/>\n";
        }
        QString val, unit;
        IUnit::self().meter2distance(proximity, val, unit);
        str += tr("Proximity: %1 %2").arg(val).arg(unit);
    }

    QString desc = removeHtml(wpt.desc).simplified();
    if(desc.count())
    {
        if(!str.isEmpty())
        {
            str += "<br/>\n";
        }

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
            if(!str.isEmpty())
            {
                str += "<br/>\n";
            }

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

    QTreeWidgetItem::setIcon(CGisListWks::eColumnIcon,icon);
}

void CGisItemWpt::setName(const QString& str)
{
    SETTINGS;
    cfg.setValue("Waypoint/lastName", str);

    setText(CGisListWks::eColumnName, str);

    wpt.name = str;
    changed(tr("Changed name"),"://icons/48x48/EditText.png");
}

void CGisItemWpt::setPosition(const QPointF& pos)
{
    wpt.lon = pos.x();
    wpt.lat = pos.y();

    boundingRect = QRectF(QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD,QPointF(wpt.lon,wpt.lat)*DEG_TO_RAD);

    changed(tr("Changed position"),"://icons/48x48/WptMove.png");
}

void CGisItemWpt::setElevation(qint32 val)
{
    wpt.ele = val;
    changed(tr("Changed elevation"),"://icons/48x48/SetEle.png");
}

void CGisItemWpt::setProximity(qreal val)
{
    proximity = val;
    changed(tr("Changed proximity"),"://icons/48x48/WptProx.png");
}

void CGisItemWpt::setIcon(const QString& name)
{
    SETTINGS;
    cfg.setValue("Waypoint/lastIcon", name);

    wpt.sym  = name;

    QPointF focus;
    QString path;
    getWptIconByName(name, focus, &path);

    changed(tr("Changed icon"), path);
}

void CGisItemWpt::setComment(const QString& str)
{
    wpt.cmt = str;
    changed(tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemWpt::setDescription(const QString& str)
{
    wpt.desc = str;
    changed(tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemWpt::setLinks(const QList<link_t>& links)
{
    wpt.links = links;
    changed(tr("Changed links"), "://icons/48x48/Link.png");
}

void CGisItemWpt::setImages(const QList<image_t>& imgs)
{
    images = imgs;
    changed(tr("Changed images"), "://icons/48x48/Image.png");
}

void CGisItemWpt::addImage(const image_t& img)
{
    images.append(img);
    changed(tr("Add image"), "://icons/48x48/Image.png");
}


bool CGisItemWpt::isCloseTo(const QPointF& pos)
{
    if(posScreen == NOPOINTF)
    {
        return false;
    }

    return (pos - posScreen).manhattanLength() < 22;
}

bool CGisItemWpt::isWithin(const QRectF& area, selection_e)
{
    return area.contains(posScreen);
}


void CGisItemWpt::gainUserFocus(bool yes)
{
    keyUserFocus = yes ? key : key_t();
}

void CGisItemWpt::edit()
{
    if(geocache.hasData)
    {
        CDetailsGeoCache dlg(*this, CMainWindow::getBestWidgetForParent());
        dlg.exec();
    }
    else
    {
        CDetailsWpt dlg(*this, CMainWindow::getBestWidgetForParent());
        dlg.exec();
    }
}

void CGisItemWpt::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    posScreen = QPointF(wpt.lon * DEG_TO_RAD, wpt.lat * DEG_TO_RAD);
    if(!isVisible(posScreen, viewport, gis))
    {
        rectBubble  = QRect();
        posScreen   = NOPOINTF;
        return;
    }
    gis->convertRad2Px(posScreen);

    drawBubble(p);

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

void CGisItemWpt::drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    if(mouseIsOverBubble && !doBubbleMove && !doBubbleSize && rectBubble.isValid() && !isReadOnly())
    {
        QPainterPath clip;
        clip.addRoundedRect(rectBubble, 5, 5);
        p.setClipPath(clip);

        QRect barTop(rectBubble.topLeft(), QSize(rectBubble.width(), 26));
        QRect barBottom(barTop);
        barBottom.moveBottomLeft(rectBubble.bottomLeft());
        barBottom.adjust(1,0,-1,-1);
        barTop.adjust(1,1,-1,0);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(200,200,255,150));
        p.drawRect(barTop);
        p.drawRect(barBottom);

        p.setBrush(Qt::white);
        p.drawRoundedRect(rectBubbleMove.adjusted(-2,-2,2,2), 3,3);
        p.drawRoundedRect(rectBubbleEdit.adjusted(-2,-2,2,2), 3,3);
        p.drawRoundedRect(rectBubbleSize.adjusted(-2,-2,2,2), 3,3);

        p.drawPixmap(rectBubbleMove, QPixmap("://icons/32x32/MoveArrow.png"));
        p.drawPixmap(rectBubbleEdit, QPixmap("://icons/32x32/EditDetails.png"));
        p.drawPixmap(rectBubbleSize, QPixmap("://icons/32x32/SizeArrow.png"));
    }
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
    rect.moveCenter(pt + QPointF(icon.width()/2, -fm.height()));
    if(isBlocked(rect, blockedAreas))
    {
        // place label on bottom
        rect.moveCenter(pt + QPointF( icon.width()/2, +fm.height() + icon.height()));
        if(isBlocked(rect, blockedAreas))
        {
            // place label on right
            rect.moveCenter(pt + QPointF( icon.width() + rect.width()/2, +fm.height()));
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

    CDraw::text(wpt.name,p,rect.toRect(), Qt::darkBlue);
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

void CGisItemWpt::drawBubble(QPainter& p)
{
    if(!(flags & eFlagWptBubble))
    {
        return;
    }

    QString str = QString("<b>%1</b>").arg(getName());

    if(!removeHtml(wpt.desc).simplified().isEmpty())
    {
        str += QString("<p>%1</p>").arg(wpt.desc);
    }

    if(!removeHtml(wpt.cmt).simplified().isEmpty())
    {
        str += QString("<p>%1</p>").arg(wpt.cmt);
    }

    QTextDocument doc;
    doc.setHtml(str);
    doc.setTextWidth(widthBubble);

    rectBubble.setWidth(widthBubble);
    rectBubble.setHeight(doc.size().height());

    QPoint posBubble = posScreen.toPoint() + offsetBubble;
    rectBubble.moveTopLeft(posBubble);

    rectBubbleMove.moveTopLeft(rectBubble.topLeft() + QPoint(5,5));
    rectBubbleEdit.moveTopLeft(rectBubbleMove.topRight() + QPoint(7,0));
    rectBubbleSize.moveBottomRight(rectBubble.bottomRight() - QPoint(5,5));

    QPolygonF frame = makePolyline(posScreen, rectBubble);
    p.setPen(CDraw::penBorderGray);
    p.setBrush(CDraw::brushBackWhite);
    p.drawPolygon(frame);

    p.save();
    p.translate(posBubble);
    p.setPen(Qt::black);
    doc.drawContents(&p);
    p.restore();
}

QPolygonF CGisItemWpt::makePolyline(const QPointF& anchor, const QRectF& r)
{
    QPolygonF poly1, poly2;
    poly1 << r.topLeft() << r.topRight() << r.bottomRight() << r.bottomLeft();

    if(!r.contains(anchor))
    {
        qreal w = rectBubble.width()>>1;
        qreal h = rectBubble.height()>>1;

        if(w > 30)
        {
            w = 30;
        }
        if(h > 30)
        {
            h = 30;
        }

        w = h = qMin(w,h);

        if(anchor.x() < r.left())
        {
            poly2 << anchor << (r.center() + QPoint(0,-h)) << (r.center() + QPoint(0,h)) << anchor;
        }
        else if(r.right() < anchor.x())
        {
            poly2 << anchor << (r.center() + QPoint(0,-h)) << (r.center() + QPoint(0,h)) << anchor;
        }
        else if(anchor.y() < r.top())
        {
            poly2 << anchor << (r.center() + QPoint(-w,0)) << (r.center() + QPoint(w,0)) << anchor;
        }
        else if(r.bottom() < anchor.y())
        {
            poly2 << anchor << (r.center() + QPoint(-w,0)) << (r.center() + QPoint(w,0)) << anchor;
        }

        QPainterPath path1;
        path1.addRoundedRect(r,5,5);
        QPainterPath path2;
        path2.addPolygon(poly2);

        path1 = path1.united(path2);

        poly1 = path1.toFillPolygon();
    }

    return poly1;
}


void CGisItemWpt::removeLinksByType(const QString& type)
{
    QList<IGisItem::link_t>::iterator link = wpt.links.begin();

    while(link != wpt.links.end())
    {
        if(link->type == type)
        {
            link = wpt.links.erase(link);
            continue;
        }

        ++link;
    }
}

void CGisItemWpt::mouseMove(const QPointF& pos)
{
    if(!hasBubble() || isReadOnly())
    {
        return;
    }

    if(!processMouseOverBubble(pos.toPoint()))
    {
        if(mouseIsOverBubble)
        {
            if(!rectBubble.contains(pos.toPoint()))
            {
                CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
                if(canvas)
                {
                    doBubbleMove = doBubbleSize = false;
                    canvas->resetMouse();
                }
                mouseIsOverBubble = false;
            }
        }
        else
        {
            if(rectBubble.contains(pos.toPoint()))
            {
                CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
                if(canvas)
                {
                    doBubbleMove = doBubbleSize = false;
                    canvas->setMouseWptBubble(getKey());
                }
                mouseIsOverBubble = true;
            }
        }
    }
}

void CGisItemWpt::mousePress(const QPointF& pos)
{
    if(!mouseIsOverBubble)
    {
        return;
    }

    QPoint pos1 = pos.toPoint();

    if(rectBubbleMove.contains(pos1))
    {
        offsetMouse = pos1 - rectBubble.topLeft();
        doBubbleMove = true;
    }
    else if(rectBubbleEdit.contains(pos1))
    {
        CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
        if(canvas)
        {
            doBubbleMove = doBubbleSize = false;
            canvas->resetMouse();
        }
        mouseIsOverBubble = false;
        edit();
    }
    else if(rectBubbleSize.contains(pos1))
    {
        offsetMouse = pos1 - rectBubble.bottomRight();
        doBubbleSize = true;
    }
}

void CGisItemWpt::mouseRelease(const QPointF& pos)
{
    if(!mouseIsOverBubble)
    {
        return;
    }

    updateHistory();
    doBubbleMove = doBubbleSize = false;
}

void CGisItemWpt::toggleBubble()
{
    if(flags & eFlagWptBubble)
    {
        flags &= ~eFlagWptBubble;
    }
    else
    {
        flags |= eFlagWptBubble;
    }
    updateHistory();
}

bool CGisItemWpt::processMouseOverBubble(const QPoint &pos)
{
    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(!canvas || !mouseIsOverBubble)
    {
        return false;
    }

    if(doBubbleMove)
    {
        offsetBubble  = pos - posScreen.toPoint();
        offsetBubble -= offsetMouse;
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
        return true;
    }
    else if(doBubbleSize)
    {
        qDebug() << offsetMouse;
        int width  = pos.x() - rectBubble.left() - offsetMouse.x();
        if(width > 50)
        {
            widthBubble = width;
        }
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
        return true;
    }
    else if(rectBubbleMove.contains(pos) || rectBubbleEdit.contains(pos) || rectBubbleSize.contains(pos))
    {
        if(!doSpecialCursor)
        {
            CCanvas::setOverrideCursor(Qt::PointingHandCursor, "processMouseOverBubble");
            doSpecialCursor = true;
        }
    }
    else
    {
        if(doSpecialCursor)
        {
            CCanvas::restoreOverrideCursor("processMouseOverBubble");
            doSpecialCursor = false;
        }
    }

    return false;
}
