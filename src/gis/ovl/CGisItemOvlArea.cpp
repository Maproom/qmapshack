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

#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/ovl/CScrOptOvlArea.h"
#include "gis/ovl/CDetailsOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/CGisDraw.h"
#include "GeoMath.h"

#include <QtWidgets>
#include <proj_api.h>

#define DEFAULT_COLOR       4
#define MIN_DIST_CLOSE_TO   10


const QColor CGisItemOvlArea::lineColors[OVL_N_COLORS] =
{
     Qt::black                    // 0
    ,Qt::darkRed                 // 1
    ,Qt::darkGreen               // 2
    ,Qt::darkYellow              // 3
    ,Qt::darkBlue                // 4
    ,Qt::darkMagenta             // 5
    ,Qt::darkCyan                // 6
    ,Qt::gray                    // 7
    ,Qt::darkGray                // 8
    ,Qt::red                     // 9
    ,Qt::green                   // 10
    ,Qt::yellow                  // 11
    ,Qt::blue                    // 12
    ,Qt::magenta                 // 13
    ,Qt::cyan                    // 14
    ,Qt::white                   // 15
    ,Qt::transparent             // 16
};

const QString CGisItemOvlArea::bulletColors[OVL_N_COLORS] =
{

                                 // 0
    QString("://icons/8x8/bullet_black.png")
                                 // 1
    ,QString("://icons/8x8/bullet_dark_red.png")
                                 // 2
    ,QString("://icons/8x8/bullet_dark_green.png")
                                 // 3
    ,QString("://icons/8x8/bullet_dark_yellow.png")
                                 // 4
    ,QString("://icons/8x8/bullet_dark_blue.png")
                                 // 5
    ,QString("://icons/8x8/bullet_dark_magenta.png")
                                 // 6
    ,QString("://icons/8x8/bullet_dark_cyan.png")
                                 // 7
    ,QString("://icons/8x8/bullet_gray.png")
                                 // 8
    ,QString("://icons/8x8/bullet_dark_gray.png")
                                 // 9
    ,QString("://icons/8x8/bullet_red.png")
                                 // 10
    ,QString("://icons/8x8/bullet_green.png")
                                 // 11
    ,QString("://icons/8x8/bullet_yellow.png")
                                 // 12
    ,QString("://icons/8x8/bullet_blue.png")
                                 // 13
    ,QString("://icons/8x8/bullet_magenta.png")
                                 // 14
    ,QString("://icons/8x8/bullet_cyan.png")
                                 // 15
    ,QString("://icons/8x8/bullet_white.png")
    ,QString("")                 // 16
};


const CGisItemOvlArea::width_t CGisItemOvlArea::lineWidths[OVL_N_WIDTHS] =
{
    {3, QObject::tr("thin")}
    ,{5, QObject::tr("normal")}
    ,{9, QObject::tr("wide")}
    ,{13, QObject::tr("strong")}
};

const Qt::BrushStyle CGisItemOvlArea::brushStyles[OVL_N_STYLES] =
{
    Qt::NoBrush
    , Qt::HorPattern
    , Qt::VerPattern
    , Qt::CrossPattern
    , Qt::BDiagPattern
    , Qt::FDiagPattern
    , Qt::DiagCrossPattern
    , Qt::SolidPattern
};


IGisItem::key_t CGisItemOvlArea::keyUserFocus;

CGisItemOvlArea::CGisItemOvlArea(const QPolygonF& line, const QString &name, IGisProject * project, int idx)
    : IGisItem(project, eTypeOvl, idx)
    , penForeground(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    area.name = name;
    readLine(line);

    flags |=  eFlagCreatedInQms|eFlagWriteAllowed;

    setColor(str2color(""));
    genKey();

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemOvlArea::CGisItemOvlArea(const CGisItemOvlArea& parentArea, IGisProject * project, int idx)
    : IGisItem(project, eTypeOvl, idx)
    , penForeground(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    *this = parentArea;
    key.project = project->getKey();

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemOvlArea::CGisItemOvlArea(const QDomNode &xml, IGisProject *project)
    : IGisItem(project, eTypeOvl, project->childCount())
    , penForeground(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    // --- start read and process data ----
    setColor(penForeground.color());
    readArea(xml, area);
    // --- stop read and process data ----
    genKey();
    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemOvlArea::CGisItemOvlArea(const history_t& hist, IGisProject * project)
    : IGisItem(project, eTypeOvl, project->childCount())
    , penForeground(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
}

CGisItemOvlArea::CGisItemOvlArea(quint64 id, QSqlDatabase& db, IGisProject * project)
    : IGisItem(project, eTypeOvl, -1)
    , penForeground(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , penBackground(Qt::white, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
{
    loadFromDb(id, db);
}

CGisItemOvlArea::~CGisItemOvlArea()
{
    // reset user focus if focused on this track
    if(key == keyUserFocus)
    {
        keyUserFocus.clear();
    }
}

void CGisItemOvlArea::setSymbol()
{
    setColor(str2color(area.color));
}

bool CGisItemOvlArea::isCloseTo(const QPointF& pos)
{
    foreach(const QPointF& pt, line)
    {
        if((pt - pos).manhattanLength() < MIN_DIST_CLOSE_TO)
        {
            return true;
        }
    }
    return false;
}

QPointF CGisItemOvlArea::getPointCloseBy(const QPoint& screenPos)
{
    qint32 i    = 0;
    qint32 idx  = -1;
    qint32  d   = NOINT;
    foreach(const QPointF& point, line)
    {
        int tmp = (screenPos - point).manhattanLength();
        if(tmp < d)
        {
            idx = i;
            d   = tmp;
        }
        i++;
    }

    if(idx < 0)
    {
        return NOPOINTF;
    }

    return line[idx];
}

void CGisItemOvlArea::readLine(const QPolygonF &line)
{
    area.pts.clear();
    area.pts.resize(line.size());

    for(int i = 0; i < line.size(); i++)
    {
        pt_t& areapt        = area.pts[i];
        const QPointF& pt   = line[i];

        areapt.lon = pt.x() * RAD_TO_DEG;
        areapt.lat = pt.y() * RAD_TO_DEG;
    }

    deriveSecondaryData();
}


void CGisItemOvlArea::edit()
{
    CDetailsOvlArea dlg(*this, 0);
    dlg.exec();

    deriveSecondaryData();
}

void CGisItemOvlArea::deriveSecondaryData()
{
    qreal north = -90;
    qreal east  = -180;
    qreal south =  90;
    qreal west  =  180;

    foreach(const pt_t& pt, area.pts)
    {
        if(pt.lon < west)  west    = pt.lon;
        if(pt.lon > east)  east    = pt.lon;
        if(pt.lat < south) south   = pt.lat;
        if(pt.lat > north) north   = pt.lat;
    }

    boundingRect = QRectF(QPointF(west * DEG_TO_RAD, north * DEG_TO_RAD), QPointF(east * DEG_TO_RAD,south * DEG_TO_RAD));

    QPolygonF line(area.pts.size());
    for(int i = 1; i < area.pts.size(); i++)
    {
        qreal a1, a2, d;
        const pt_t& pt11 = area.pts[i - 1];
        const pt_t& pt12 = area.pts[i];

        QPointF& pt21    = line[i - 1];
        QPointF& pt22    = line[i];

        d = GPS_Math_Distance(pt11.lon * DEG_TO_RAD, pt11.lat * DEG_TO_RAD, pt12.lon * DEG_TO_RAD, pt12.lat * DEG_TO_RAD, a1, a2);

        pt22.rx() = pt21.x() + cos(a1 * DEG_TO_RAD) * d;
        pt22.ry() = pt21.y() + sin(a1 * DEG_TO_RAD) * d;
    }

    area.area = 0;
    int j = line.size() - 1;
    for(int i = 0; i < line.size(); i++)
    {
        area.area += (line[j].x() + line[i].x())*(line[j].y() - line[i].y());
        j = i;
    }

    area.area = area.area/2;
}

void CGisItemOvlArea::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis)
{
    line.clear();
    if(!viewport.intersects(boundingRect))
    {
        return;
    }

    QPointF pt1;

    foreach(const pt_t& pt, area.pts)
    {
        pt1.setX(pt.lon);
        pt1.setY(pt.lat);
        pt1 *= DEG_TO_RAD;
        line << pt1;
    }

    gis->convertRad2Px(line);

    p.save();
    p.setOpacity(area.opacity ? 0.3 : 1.0);

    penBackground.setWidth(area.width + 2);
    p.setBrush(Qt::NoBrush);
    p.setPen(penBackground);
    p.drawPolygon(line);

    penForeground.setColor(color);
    penForeground.setWidth(area.width);
    p.setBrush(QBrush(color, (Qt::BrushStyle)area.style));
    p.setPen(penForeground);
    p.drawPolygon(line);
    p.restore();
}

void CGisItemOvlArea::drawLabel(QPainter& p, const QRectF& viewport,QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis)
{
    if(line.isEmpty())
    {
        return;
    }
    QPointF pt  = getPolygonCentroid(line);
    QRectF rect = fm.boundingRect(area.name);
    rect.adjust(-2,-2,2,2);
    rect.moveCenter(pt);

    CCanvas::drawText(getName(), p, pt.toPoint(), Qt::darkBlue);
    blockedAreas << rect;
}

void CGisItemOvlArea::drawHighlight(QPainter& p)
{
    if(line.isEmpty() || key == keyUserFocus)
    {
        return;
    }
    p.setPen(QPen(QColor(255,0,0,100),11,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolygon(line);

}

void CGisItemOvlArea::gainUserFocus(bool yes)
{
    keyUserFocus = yes ? key : key_t();
}

QPointF CGisItemOvlArea::getPolygonCentroid(const QPolygonF& polygon)
{
    int i, len;
    qreal x = 0, y = 0;

    len = polygon.size();

    for(i = 0; i < len; i++)
    {
        x = x + polygon[i].x();
        y = y + polygon[i].y();
    }
    x = x / len;
    y = y / len;

    return QPointF(x,y);
}

IScrOpt * CGisItemOvlArea::getScreenOptions(const QPoint& origin, IMouse * mouse)
{
    if(scrOpt.isNull())
    {
        scrOpt = new CScrOptOvlArea(this, origin, mouse);
    }
    return scrOpt;
}

const QString& CGisItemOvlArea::getName()
{
    return area.name;
}

QString CGisItemOvlArea::getInfo()
{
    QString unit, val;
    QString str = getName();

    IUnit::self().meter2area(area.area, val, unit);
    str += "\n" + QObject::tr("Area: %1%2").arg(val).arg(unit);

    QString desc = removeHtml(area.desc).simplified();
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
        QString cmt = removeHtml(area.cmt).simplified();
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

void CGisItemOvlArea::getPolylineFromData(QPolygonF& line)
{    
    line.clear();
    foreach(const pt_t& pt, area.pts)
    {
        line << QPointF(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD);
    }
}

void CGisItemOvlArea::setDataFromPolyline(const QPolygonF& line)
{
    readLine(line);

    flags |= eFlagTainted;

    changed(QObject::tr("Changed area shape."), "://icons/48x48/AreaMove.png");
    updateDecoration(eMarkChanged, eMarkNone);
}

void CGisItemOvlArea::setName(const QString& str)
{
    setText(0, str);
    area.name = str;
    changed(QObject::tr("Changed name."), "://icons/48x48/EditText.png");
}

void CGisItemOvlArea::setWidth(qint32 w)
{
    area.width = w;
    changed(QObject::tr("Changed border width."), "://icons/48x48/TextBold.png");
}

void CGisItemOvlArea::setStyle(qint32 s)
{
    area.style = s;
    changed(QObject::tr("Changed fill pattern."), "://icons/48x48/Pattern.png");
}

void CGisItemOvlArea::setOpacity(bool yes)
{
    area.opacity = yes;
    changed(QObject::tr("Changed opacity."), "://icons/48x48/Opacity.png");
}

void CGisItemOvlArea::setComment(const QString& str)
{
    area.cmt = str;
    changed(QObject::tr("Changed comment."), "://icons/48x48/EditText.png");
}

void CGisItemOvlArea::setDescription(const QString& str)
{
    area.desc = str;
    changed(QObject::tr("Changed description."), "://icons/48x48/EditText.png");
}

void CGisItemOvlArea::setLinks(const QList<link_t>& links)
{
    area.links = links;
    changed(QObject::tr("Changed links"), "://icons/48x48/Link.png");
}


void CGisItemOvlArea::setColor(int idx)
{
    int N = sizeof(lineColors)/sizeof(QColor);
    if(idx >= N)
    {
        return;
    }
    setColor(lineColors[idx]);
    changed(QObject::tr("Changed color"), "://icons/48x48/SelectColor.png");
}

void CGisItemOvlArea::setColor(const QColor& c)
{
    int n;
    int N = sizeof(lineColors)/sizeof(QColor);

    for(n = 0; n < N; n++)
    {
        if(lineColors[n] == c)
        {
            colorIdx    = n;
            color       = lineColors[n];
            bullet      = QPixmap(bulletColors[n]);
            break;
        }
    }

    if(n == N)
    {
        colorIdx    = DEFAULT_COLOR;
        color       = lineColors[DEFAULT_COLOR];
        bullet      = QPixmap(bulletColors[DEFAULT_COLOR]);
    }

    setIcon(color.name());
}

void CGisItemOvlArea::setIcon(const QString& c)
{
    area.color   = c;
    icon        = QPixmap("://icons/48x48/Area.png");

    QPixmap mask( icon.size() );
    mask.fill( str2color(c) );
    mask.setMask( icon.createMaskFromColor( Qt::transparent ) );
    icon = mask.scaled(22,22, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QTreeWidgetItem::setIcon(0,icon);
}
