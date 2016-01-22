﻿/**********************************************************************************************
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
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/prj/IGisProject.h"
#include "gis/trk/CCombineTrk.h"
#include "gis/trk/CCutTrk.h"
#include "gis/trk/CDetailsTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CPropertyTrk.h"
#include "gis/trk/CScrOptTrk.h"
#include "gis/trk/CSelectActivity.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"
#include "plot/IPlot.h"

#include <QtWidgets>
#include <QtXml>
#include <proj_api.h>

using std::numeric_limits;

#define DEFAULT_COLOR       4
#define MIN_DIST_CLOSE_TO   10
#define MIN_DIST_FOCUS      200

#define WPT_FOCUS_DIST_IN   (50*50)
#define WPT_FOCUS_DIST_OUT  (200*200)

struct trkwpt_t
{
    trkwpt_t() : x(0), y(0)
    {
    }

    QString name;
    qreal x;
    qreal y;
    IGisItem::key_t key;
};

struct activity_t
{
    QString name;
    QString icon;
};

IGisItem::key_t CGisItemTrk::keyUserFocus;


CGisItemTrk::CGisItemTrk(const QString &name, qint32 idx1, qint32 idx2, const trk_t& srctrk, IGisProject * project)
    : IGisItem(project, eTypeTrk, NOIDX)
{
    flags = eFlagCreatedInQms;

    foreach(const trkseg_t &srcseg, srctrk.segs)
    {
        trkseg_t seg;
        foreach(const trkpt_t &srcpt, srcseg.pts)
        {
            if(srcpt.idxTotal < idx1)
            {
                continue;
            }
            if(srcpt.idxTotal > idx2)
            {
                break;
            }

            seg.pts << srcpt;
        }

        if(!seg.pts.isEmpty())
        {
            trk.segs << seg;
        }
    }

    trk.name   = name;
    trk.cmt    = srctrk.cmt;
    trk.desc   = srctrk.desc;
    trk.src    = srctrk.src;
    trk.links  = srctrk.links;
    trk.number = srctrk.number;
    trk.type   = srctrk.type;

    deriveSecondaryData();
    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemTrk::CGisItemTrk(const CGisItemTrk& parentTrk, IGisProject *project, int idx, bool clone)
    : IGisItem(project, eTypeTrk, idx)
{
    // copy track via history
    history = parentTrk.history;
    loadHistory(history.histIdxCurrent);

    // if track should be a clone clear history and key and
    // build new ones.
    if(clone)
    {
        trk.name += QObject::tr("_Clone");
        key.clear();
        history.events.clear();
        setupHistory();
    }


    if(parentTrk.isOnDevice() || !parentTrk.isReadOnly())
    {
        flags |= eFlagWriteAllowed;
    }
    else
    {
        flags &= ~eFlagWriteAllowed;
    }

    deriveSecondaryData();
    updateDecoration(eMarkChanged, eMarkNone);
}


CGisItemTrk::CGisItemTrk(const SGisLine& l, const QString& name, IGisProject * project, int idx)
    : IGisItem(project, eTypeTrk, idx)
{
    trk.name = name;
    readTrackDataFromGisLine(l);

    flags |= eFlagCreatedInQms | eFlagWriteAllowed;
    setColor(str2color(""));

    setupHistory();
    updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemTrk::CGisItemTrk(const QDomNode& xml, IGisProject *project)
    : IGisItem(project, eTypeTrk, project->childCount())
{
    // --- start read and process data ----
    setColor(penForeground.color());
    readTrk(xml, trk);
    // --- stop read and process data ----

    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemTrk::CGisItemTrk(const QString& filename, IGisProject * project)
    : IGisItem(project, eTypeTrk, project->childCount())
{
    // --- start read and process data ----
    setColor(penForeground.color());
    if(!readTwoNav(filename))
    {
        throw -1;
    }
    // --- stop read and process data ----

    setupHistory();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemTrk::CGisItemTrk(const history_t& hist, const QString &dbHash, IGisProject * project)
    : IGisItem(project, eTypeTrk, project->childCount())
{
    history = hist;
    loadHistory(hist.histIdxCurrent);
    if(!dbHash.isEmpty())
    {
        lastDatabaseHash = dbHash;
    }
}

CGisItemTrk::CGisItemTrk(quint64 id, QSqlDatabase& db, IGisProject * project)
    : IGisItem(project, eTypeTrk, NOIDX)
{
    loadFromDb(id, db);
}

CGisItemTrk::CGisItemTrk(trk_t& trkdata, IGisProject *project)
    : IGisItem(project, eTypeTrk, NOIDX)
{
    trk = std::move(trkdata);

    setupHistory();
    deriveSecondaryData();
    updateDecoration(eMarkNone, eMarkNone);
}

CGisItemTrk::CGisItemTrk(CFitStream& stream, IGisProject * project)
    : IGisItem(project, eTypeTrk, project->childCount())
{
    // --- start read and process data ----
    setColor(penForeground.color());
    readTrkFromFit(stream);
    // --- stop read and process data ----

    setupHistory();
    deriveSecondaryData();
    updateDecoration(eMarkNone, eMarkNone);
}


CGisItemTrk::~CGisItemTrk()
{
    // reset user focus if focused on this track
    if(key == keyUserFocus)
    {
        keyUserFocus.clear();
    }

    /*
        Delete all registered INotifyTrk as they can't exist without the item.
        As the INotifyTrk objects will unregister via unregisterVisual() in their
        destructor things will get a bit complicated here. Better create
        a copy of the list before we start to delete.
     */
    qDeleteAll(registeredVisuals.toList());

    // now it is save to destroy the details dialog
    delete dlgDetails;

    // delete it after the detail dialog as it is used by the detail dialog
    delete propHandler;
}

IGisItem * CGisItemTrk::createClone()
{
    int idx = -1;
    IGisProject * project = dynamic_cast<IGisProject*>(parent());
    if(project)
    {
        idx = project->indexOfChild(this);
    }
    return new CGisItemTrk(*this, project, idx, true);
}

void CGisItemTrk::updateFromDB(quint64 id, QSqlDatabase& db)
{
    IGisItem::updateFromDB(id, db);

    /*
        as this will change the line significantly we better stop
        all focus operations and close the detail dialog.
     */
    resetInternalData();
}


void CGisItemTrk::setSymbol()
{
    setColor(str2color(trk.color));
}


void CGisItemTrk::setDataFromPolyline(const SGisLine &l)
{
    QMutexLocker lock(&mutexItems);

    /*
        as this will change the line significantly we better stop
        all focus operations and close the detail dialog.
     */
    resetInternalData();

    readTrackDataFromGisLine(l);

    flags |= eFlagTainted;
    changed(QObject::tr("Changed trackpoints, sacrificed all previous data."), "://icons/48x48/LineMove.png");
}

void CGisItemTrk::getPolylineFromData(QPolygonF &l)
{
    QMutexLocker lock(&mutexItems);

    l.clear();
    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt, seg.pts)
        {
            if(!(pt.flags & trkpt_t::eHidden))
            {
                l << QPointF(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD);
            }
        }
    }
}

void CGisItemTrk::getPolylineFromData(SGisLine &l)
{
    QMutexLocker lock(&mutexItems);

    l.clear();
    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt, seg.pts)
        {
            if(!(pt.flags & trkpt_t::eHidden))
            {
                l << point_t(QPointF(pt.lon*DEG_TO_RAD, pt.lat * DEG_TO_RAD));
            }
        }
    }
}

void CGisItemTrk::readTrackDataFromGisLine(const SGisLine &l)
{
    QMutexLocker lock(&mutexItems);

    trk.segs.clear();
    trk.segs.resize(1);
    trkseg_t& seg = trk.segs.first();

    for(int i = 0; i < l.size(); i++)
    {
        seg.pts << trkpt_t();

        trkpt_t& trkpt    = seg.pts.last();
        const point_t& pt = l[i];

        trkpt.lon = pt.coord.x() * RAD_TO_DEG;
        trkpt.lat = pt.coord.y() * RAD_TO_DEG;
        trkpt.ele = pt.ele;

        for(int n = 0; n < pt.subpts.size(); n++)
        {
            seg.pts << trkpt_t();

            trkpt_t& trkpt     = seg.pts.last();
            const subpt_t& sub = pt.subpts[n];

            trkpt.lon = sub.coord.x() * RAD_TO_DEG;
            trkpt.lat = sub.coord.y() * RAD_TO_DEG;
            trkpt.ele = sub.ele;
        }
    }

    deriveSecondaryData();
}

void CGisItemTrk::registerVisual(INotifyTrk * visual)
{
    registeredVisuals << visual;
}

void CGisItemTrk::unregisterVisual(INotifyTrk * visual)
{
    registeredVisuals.remove(visual);
}

QString CGisItemTrk::getInfo(bool showName) const
{
    QString val1, unit1, val2, unit2;
    QString str = "<div>";

    if(showName)
    {
        str += "<b>" + getName() + "</b>";
    }

    if(cntVisiblePoints == 0)
    {
        return str + "</div>";
    }

    if(showName)
    {
        str += "<br />";
    }

    IUnit::self().meter2distance(totalDistance, val1, unit1);
    str += QObject::tr("Length: %1 %2").arg(val1).arg(unit1);

    if(totalAscend != NOFLOAT && totalDescend != NOFLOAT)
    {
        IUnit::self().meter2elevation(totalAscend, val1, unit1);
        IUnit::self().meter2elevation(totalDescend, val2, unit2);

        str += QObject::tr(", %1%2 %3, %4%5 %6").arg(QChar(0x2197)).arg(val1).arg(unit1).arg(QChar(0x2198)).arg(val2).arg(unit2);
    }


    if(totalElapsedSeconds != NOTIME)
    {
        IUnit::self().seconds2time(totalElapsedSeconds, val1, unit1);
        str += "<br/>\n";
        str += QObject::tr("Time: %1").arg(val1);

        IUnit::self().meter2speed(totalDistance / totalElapsedSeconds, val1, unit1);
        str += QObject::tr(", Speed: %1 %2").arg(val1).arg(unit1);
    }

    if(totalElapsedSecondsMoving != NOTIME)
    {
        IUnit::self().seconds2time(totalElapsedSecondsMoving, val1, unit1);
        str += "<br/>\n";
        str += QObject::tr("Moving: %1").arg(val1);

        IUnit::self().meter2speed(totalDistance / totalElapsedSecondsMoving, val1, unit1);
        str += QObject::tr(", Speed: %1 %2").arg(val1).arg(unit1);
    }

    if(timeStart.isValid())
    {
        str += "<br/>\n";
        str += QObject::tr("Start: %1").arg(IUnit::datetime2string(timeStart, false, boundingRect.center()));
    }
    if(timeEnd.isValid())
    {
        str += "<br/>\n";
        str += QObject::tr("End: %1").arg(IUnit::datetime2string(timeEnd, false, boundingRect.center()));
    }

    str += "<br/>\n";
    str += QObject::tr("Points: %1 (%2)").arg(cntVisiblePoints).arg(cntTotalPoints);

    str += "</div>";
    return str;
}

QString CGisItemTrk::getInfoRange()
{
    qreal tmp, d, slope1, slope2;
    QString str, val, unit;
    if(mouseRange1 == 0 || mouseRange2 == 0)
    {
        return str;
    }

    int idx1 = mouseRange1->idxTotal;
    int idx2 = mouseRange2->idxTotal;
    const trkpt_t * pt1, * pt2;
    if(idx1 < idx2)
    {
        pt1 = mouseRange1;
        pt2 = mouseRange2;
    }
    else
    {
        pt1 = mouseRange2;
        pt2 = mouseRange1;
    }

    while(pt1->flags & trkpt_t::eHidden)
    {
        if(pt1->idxTotal == (cntTotalPoints - 1))
        {
            break;
        }
        pt1++;
    }

    while(pt2->flags & trkpt_t::eHidden)
    {
        if(pt2->idxTotal == 0)
        {
            break;
        }
        pt2--;
    }

    bool timeIsValid    = pt1->time.isValid() && pt2->time.isValid();
    qreal deltaTime     = pt2->time.toTime_t() - pt1->time.toTime_t();

    d = tmp = pt2->distance - pt1->distance;

    IUnit::self().meter2distance(tmp, val, unit);
    str += QString("%3 %1%2 ").arg(val).arg(unit).arg(QChar(0x21A6));
    if(timeIsValid)
    {
        quint32 t  = pt2->time.toTime_t() - pt1->time.toTime_t();
        quint32 hh = t / 3600;
        quint32 mm = (t - hh * 3600) / 60;
        quint32 ss = (t - hh * 3600 - mm * 60);

        str += QString("%4 %1:%2:%3").arg(hh,2,10,QChar('0')).arg(mm,2,10,QChar('0')).arg(ss,2,10,QChar('0')).arg(QChar(0x231a));

        IUnit::self().meter2speed(d/deltaTime, val, unit);
        str += QString(", %3 %1%2").arg(val).arg(unit).arg(QChar(0x21A3));
    }
    str += "\n";

    qreal deltaAscend  = pt2->ascend  - pt1->ascend;
    qreal deltaDescend = pt2->descend - pt1->descend;

    tmp    = qAtan(deltaAscend/d);
    slope1 = qAbs(tmp * 360.0/(2 * M_PI));
    slope2 = qTan(slope1 * DEG_TO_RAD) * 100;

    IUnit::self().meter2elevation(deltaAscend, val, unit);
    str += QString("%3 %1%2 (%4%5, %6%)").arg(val).arg(unit).arg(QChar(0x2197)).arg(qRound(slope1)).arg(QChar(0260)).arg(qRound(slope2));
    if(timeIsValid)
    {
        IUnit::self().meter2speed(deltaAscend/deltaTime, val, unit);
        str += QString(", %1%2").arg(val).arg(unit);
    }
    str += "\n";

    tmp       = qAtan(deltaDescend/d);
    slope1    = qAbs(tmp * 360.0/(2 * M_PI));
    slope2    = qTan(slope1 * DEG_TO_RAD) * 100;

    IUnit::self().meter2elevation(deltaDescend, val, unit);
    str += QString("%3 %1%2 (%4%5, %6%)").arg(val).arg(unit).arg(QChar(0x2198)).arg(qRound(slope1)).arg(QChar(0260)).arg(qRound(slope2));
    if(timeIsValid)
    {
        IUnit::self().meter2speed(deltaDescend/deltaTime, val, unit);
        str += QString(", %1%2").arg(val).arg(unit);
    }

    return str + "\n";
}

QString CGisItemTrk::getInfoTrkPt(const trkpt_t& pt)
{
    QString str, val1, unit1;
    if(totalElapsedSeconds != 0)
    {
        str += IUnit::datetime2string(pt.time, false, QPointF(pt.lon, pt.lat) * DEG_TO_RAD);
    }
    str += "\n";

    IUnit::self().meter2elevation(pt.ele, val1, unit1);
    str += QObject::tr("Ele.: %1 %2").arg(val1).arg(unit1);
    if(pt.slope1 != NOFLOAT)
    {
        str += QObject::tr(" slope: %1%3 (%2%)").arg(pt.slope1, 2, 'f', 0).arg(pt.slope2, 2, 'f', 0).arg(QChar(0260));
    }
    if(pt.speed != NOFLOAT)
    {
        IUnit::self().meter2speed(pt.speed, val1, unit1);
        str += QObject::tr(" speed: %1%2").arg(val1).arg(unit1);
    }

    QStringList keys = pt.extensions.keys();
    keys.sort();

    qint32 more = keys.size() - 10;
    if(more > 0)
    {
        keys = keys.mid(0, 10);
    }

    foreach(const QString &key, keys)
    {
        const CKnownExtension &ext = CKnownExtension::get(key);
        if(ext.known)
        {
            str += "\n" + ext.name + ": " + QString("%1%2").arg(ext.valueFunc(pt)*ext.factor,0,'f',1).arg(ext.unit);
        }
        else
        {
            QStringList tags = key.split("|");
            str += "\n" + tags.last() + ": " + pt.extensions[key].toString();
        }
    }

    if(more > 0)
    {
        str += "\n" + QObject::tr("... and %1 tags not displayed").arg(more);
    }

    return str;
}

QString CGisItemTrk::getInfoProgress(const trkpt_t& pt)
{
    QString str, val, unit;
    if(pt.ascend != NOFLOAT)
    {
        IUnit::self().meter2elevation(pt.ascend, val, unit);
        str += QObject::tr("Ascend: %1%2 (%3%)").arg(val).arg(unit).arg(pt.ascend * 100/totalAscend, 2,'f',0);
    }
    else
    {
        str += QObject::tr("Ascend: - (-)");
    }

    if(pt.descend != NOFLOAT)
    {
        IUnit::self().meter2elevation(pt.descend, val, unit);
        str += QObject::tr(" Descend: %1%2 (%3%)").arg(val).arg(unit).arg(pt.descend * 100/totalDescend, 2,'f',0);
    }
    else
    {
        str += QObject::tr(" Descend: - (-) ");
    }

    str += "\n";
    if(pt.distance != NOFLOAT)
    {
        IUnit::self().meter2distance(pt.distance, val, unit);
        str += QObject::tr("Dist.: %1%2 (%3%)").arg(val).arg(unit).arg(pt.distance * 100/totalDistance, 2,'f',0);
    }
    else
    {
        str += QObject::tr("Dist.: - (-)");
    }

    if(pt.elapsedSeconds != NOFLOAT)
    {
        IUnit::self().seconds2time(pt.elapsedSecondsMoving, val, unit);
        str += QObject::tr(" Moving: %1%2 (%3%)").arg(val).arg(unit).arg(pt.elapsedSecondsMoving * 100/totalElapsedSecondsMoving, 2,'f',0);
    }
    else
    {
        str += QObject::tr(" Moving: - (-) ");
    }

    return str;
}

QString CGisItemTrk::getInfoRange(const trkpt_t& pt1, const trkpt_t& pt2)
{
    QString str, val, unit;
    qreal dt = NOFLOAT;

    if(pt1.time.isValid() && pt2.time.isValid())
    {
        dt = pt2.time.toTime_t() - pt1.time.toTime_t();
    }

    if((pt1.ascend != NOFLOAT) && (pt2.ascend != NOFLOAT))
    {
        IUnit::self().meter2elevation(pt2.ascend - pt1.ascend, val, unit);
        str += QObject::tr("Ascend: %1%2").arg(val).arg(unit);

        if(dt != NOFLOAT)
        {
            IUnit::self().meter2speed((pt2.ascend - pt1.ascend)/dt, val, unit);
            str += QObject::tr(", %1%2").arg(val).arg(unit);
        }
    }
    else
    {
        str += QObject::tr("Ascend: -");
    }


    if((pt1.descend != NOFLOAT) && (pt2.descend != NOFLOAT))
    {
        IUnit::self().meter2elevation(pt2.descend - pt1.descend, val, unit);
        str += QObject::tr(" Descend: %1%2").arg(val).arg(unit);

        if(dt != NOFLOAT)
        {
            IUnit::self().meter2speed((pt2.descend - pt1.descend)/dt, val, unit);
            str += QObject::tr(", %1%2").arg(val).arg(unit);
        }
    }
    else
    {
        str += QObject::tr("Descend: -");
    }

    str += "\n";

    IUnit::self().meter2distance(pt2.distance - pt1.distance, val, unit);
    str += QObject::tr("Dist.: %1%2").arg(val).arg(unit);

    if(dt != NOFLOAT)
    {
        IUnit::self().seconds2time(dt, val, unit);
        str += QObject::tr(" Time: %1%2").arg(val).arg(unit);
    }

    return str;
}

IScrOpt * CGisItemTrk::getScreenOptions(const QPoint& origin, IMouse * mouse)
{
    if(scrOpt.isNull())
    {
        scrOpt = new CScrOptTrk(this, origin, mouse);
    }
    return scrOpt;
}

QPointF CGisItemTrk::getPointCloseBy(const QPoint& screenPos)
{
    QMutexLocker lock(&mutexItems);

    qint32 i    = 0;
    qint32 idx  = NOIDX;
    qint32 d    = NOINT;
    foreach(const QPointF &point, lineSimple)
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

    return lineSimple[idx];
}

void CGisItemTrk::getSelectedVisiblePoints(qint32& idx1, qint32& idx2)
{
    if((nullptr == mouseRange1) || (nullptr == mouseRange2))
    {
        idx1 = NOIDX;
        idx2 = NOIDX;
        return;
    }

    idx1 = mouseRange1->idxVisible;
    idx2 = mouseRange2->idxVisible;

    if(idx1 > idx2)
    {
        qSwap(idx1,idx2);
    }
}

static inline void updateExtrema(CGisItemTrk::limits_t &extrema, qreal val)
{
    if(NOFLOAT != val)
    {
        extrema = { qMin(extrema.min, val), qMax(extrema.max, val) };
    }
}

void CGisItemTrk::updateExtremaAndExtensions()
{
    extrema = QHash<QString, limits_t>();
    limits_t extremaSpeed = { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };
    limits_t extremaSlope = { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };
    limits_t extremaEle   = { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };
    limits_t extremaProgress= { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };

    existingExtensions = QSet<QString>();
    QSet<QString> nonRealExtensions;

    foreach(const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt, seg.pts)
        {
            if(pt.flags & trkpt_t::eHidden)
            {
                continue;
            }

            existingExtensions.unite(pt.extensions.keys().toSet());

            foreach(const QString &key, pt.extensions.keys())
            {
                bool isReal = false;
                qreal val = pt.extensions.value(key).toReal(&isReal);

                if(isReal)
                {
                    const limits_t &current = extrema.value(key, { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() });
                    extrema[key] = { qMin(current.min, val), qMax(current.max, val) };
                }
                else
                {
                    nonRealExtensions << key;
                }
            }

            updateExtrema(extremaSpeed, pt.speed);
            updateExtrema(extremaEle,   pt.ele);
            updateExtrema(extremaSlope, pt.slope1);
            updateExtrema(extremaProgress, pt.distance);
        }
    }

    if(extremaEle.min < extremaEle.max)
    {
        existingExtensions << CKnownExtension::internalEle;
        extrema[CKnownExtension::internalEle] = extremaEle;
    }

    if(extremaSlope.min < extremaSlope.max)
    {
        existingExtensions << CKnownExtension::internalSlope;
        extrema[CKnownExtension::internalSlope] = extremaSlope;
    }

    if(numeric_limits<qreal>::max() != extremaSpeed.min)
    {
        existingExtensions << CKnownExtension::internalSpeed;
        extrema[CKnownExtension::internalSpeed] = extremaSpeed;
    }

    if(numeric_limits<qreal>::max() != extremaProgress.min)
    {
        existingExtensions << CKnownExtension::internalProgress;
        extrema[CKnownExtension::internalProgress] = extremaProgress;
    }


    existingExtensions.subtract(nonRealExtensions);
}

void CGisItemTrk::resetInternalData()
{
    mouseClickFocus = nullptr;
    mouseMoveFocus  = nullptr;
    mouseRange1     = nullptr;
    mouseRange2     = nullptr;
    rangeState      = eRangeStateIdle;

    delete dlgDetails;
}

void CGisItemTrk::deriveSecondaryData()
{
    qreal north = -90;
    qreal east  = -180;
    qreal south =  90;
    qreal west  =  180;

    // reset all secondary data
    cntTotalPoints          = 0;
    cntVisiblePoints        = 0;
    timeStart               = QDateTime();
    timeEnd                 = QDateTime();
    totalDistance           = NOFLOAT;
    totalAscend             = NOFLOAT;
    totalDescend            = NOFLOAT;
    totalElapsedSeconds     = NOTIME;
    totalElapsedSecondsMoving = NOTIME;


    // remove empty segments
    QVector<trkseg_t>::iterator i = trk.segs.begin();
    while(i != trk.segs.end())
    {
        if((*i).pts.isEmpty())
        {
            i = trk.segs.erase(i);
            continue;
        }
        i++;
    }

    // no segments -> no data -> nothing to do
    if(trk.segs.isEmpty())
    {
        return;
    }

    trkpt_t * lastTrkpt  = nullptr;
    qreal timestampStart = NOFLOAT;
    qreal lastEle        = NOFLOAT;

    // linear list of pointers to visible track points
    QVector<trkpt_t*> lintrk;

    for(int s = 0; s < trk.segs.size(); s++)
    {
        trkseg_t& seg = trk.segs[s];

        for(int p = 0; p < seg.pts.size(); p++)
        {
            trkpt_t& trkpt = seg.pts[p];

            trkpt.idxTotal = cntTotalPoints++;
            if(trkpt.flags & trkpt_t::eHidden)
            {
                trkpt.reset();
                continue;
            }
            trkpt.idxVisible = cntVisiblePoints++;
            lintrk << &trkpt;

            if(trkpt.lon < west)
            {
                west    = trkpt.lon;
            }
            if(trkpt.lon > east)
            {
                east    = trkpt.lon;
            }
            if(trkpt.lat < south)
            {
                south   = trkpt.lat;
            }
            if(trkpt.lat > north)
            {
                north   = trkpt.lat;
            }

            if(lastTrkpt != 0)
            {
                trkpt.deltaDistance  = GPS_Math_Distance(lastTrkpt->lon * DEG_TO_RAD, lastTrkpt->lat * DEG_TO_RAD, trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
                trkpt.distance       = lastTrkpt->distance + trkpt.deltaDistance;
                trkpt.elapsedSeconds = trkpt.time.toMSecsSinceEpoch()/1000.0 - timestampStart;

                // ascend descend
                if(lastEle != NOFLOAT)
                {
                    qreal delta     = trkpt.ele - lastEle;
                    qreal absDelta  = qAbs(delta);

                    trkpt.ascend  = lastTrkpt->ascend;
                    trkpt.descend = lastTrkpt->descend;

                    if(absDelta > ASCEND_THRESHOLD)
                    {
                        if(delta > 0)
                        {
                            trkpt.ascend  += delta;
                        }
                        else
                        {
                            trkpt.descend -= delta;
                        }
                        lastEle = trkpt.ele;
                    }
                }

                // time moving
                trkpt.elapsedSecondsMoving = lastTrkpt->elapsedSecondsMoving;
                qreal dt = (trkpt.time.toMSecsSinceEpoch() - lastTrkpt->time.toMSecsSinceEpoch()) / 1000.0;
                if(dt > 0 && ((trkpt.deltaDistance / dt) > 0.2))
                {
                    trkpt.elapsedSecondsMoving += dt;
                }
            }
            else
            {
                timeStart       = trkpt.time;
                timestampStart  = timeStart.toMSecsSinceEpoch()/1000.0;
                lastEle         = trkpt.ele;

                trkpt.deltaDistance        = 0;
                trkpt.distance             = 0;
                trkpt.ascend               = 0;
                trkpt.descend              = 0;
                trkpt.elapsedSeconds       = 0;
                trkpt.elapsedSecondsMoving = 0;
            }

            lastTrkpt = &trkpt;
        }
    }

    boundingRect = QRectF(QPointF(west * DEG_TO_RAD, north * DEG_TO_RAD), QPointF(east * DEG_TO_RAD,south * DEG_TO_RAD));

    for(int p = 0; p < lintrk.size(); p++)
    {
        trkpt_t& trkpt = *lintrk[p];

        qreal d1 = trkpt.distance;
        qreal e1 = trkpt.ele;
        qreal t1 = trkpt.time.toMSecsSinceEpoch() / 1000.0;
        int n = p;

        while(n>0)
        {
            trkpt_t & trkpt2 = *lintrk[n];
            if(trkpt2.ele == NOINT)
            {
                n--;
                continue;
            }

            if(trkpt.distance - trkpt2.distance >= 25)
            {
                d1 = trkpt2.distance;
                e1 = trkpt2.ele;
                t1 = trkpt2.time.toMSecsSinceEpoch()/1000.0;
                break;
            }
            n--;
        }

        qreal d2 = trkpt.distance;
        qreal e2 = trkpt.ele;
        qreal t2 = trkpt.time.toMSecsSinceEpoch() / 1000.0;
        n = p;
        while(n < lintrk.size())
        {
            trkpt_t & trkpt2 = *lintrk[n];
            if(trkpt2.ele == NOINT)
            {
                n++;
                continue;
            }

            if(trkpt2.distance - trkpt.distance >= 25)
            {
                d2 = trkpt2.distance;
                e2 = trkpt2.ele;
                t2 = trkpt2.time.toMSecsSinceEpoch() / 1000.0;
                break;
            }
            n++;
        }

        qreal a      = qAtan((e2 - e1)/(d2 - d1));
        trkpt.slope1 = a * 360.0/(2 * M_PI);
        trkpt.slope2 = qTan(trkpt.slope1 * DEG_TO_RAD) * 100;

        if((t2 - t1) > 0)
        {
            trkpt.speed = (d2 - d1) / (t2 - t1);
        }
        else
        {
            trkpt.speed = NOFLOAT;
        }
    }


    if(nullptr != lastTrkpt)
    {
        timeEnd                   = lastTrkpt->time;
        totalDistance             = lastTrkpt->distance;
        totalAscend               = lastTrkpt->ascend;
        totalDescend              = lastTrkpt->descend;
        totalElapsedSeconds       = lastTrkpt->elapsedSeconds;
        totalElapsedSecondsMoving = lastTrkpt->elapsedSecondsMoving;
    }

    activities.update();

    updateExtremaAndExtensions();
    // make sure we have a graph properties object by now
    if(propHandler == nullptr)
    {
        propHandler = new CPropertyTrk(*this);
        limitsGraph1.setSource(CKnownExtension::internalEle);
    }
    else
    {
        propHandler->setupData();
    }

    updateVisuals(eVisualPlot|eVisualDetails|eVisualProject, "deriveSecondaryData()");

//    qDebug() << "--------------" << getName() << "------------------";
//    qDebug() << "totalDistance" << totalDistance;
//    qDebug() << "totalAscend" << totalAscend;
//    qDebug() << "totalDescend" << totalDescend;
//    qDebug() << "totalElapsedSeconds" << totalElapsedSeconds;
//    qDebug() << "totalElapsedSecondsMoving" << totalElapsedSecondsMoving;
}


void CGisItemTrk::findWaypointsCloseBy(CProgressDialog& progress, quint32& current)
{
    IGisProject * project = dynamic_cast<IGisProject*>(parent());
    if(nullptr == project)
    {
        return;
    }

    quint32 lastCurrent = current;

    bool withDoubles = project->getSorting() != IGisProject::eSortTrackWithoutDouble;

    QVector<pointDP> line;
    // combine all segments to a single line
    const int M = trk.segs.size();
    for(int m = 0; m < M; m++)
    {
        trkseg_t& seg = trk.segs[m];

        const int N = seg.pts.size();
        for(int n = 0; n < N; n++)
        {
            trkpt_t& pt = seg.pts[n];
            pt.keyWpt.clear();
            if(pt.flags & CGisItemTrk::trkpt_t::eHidden)
            {
                continue;
            }
            pointDP dp;
            dp.x    = pt.lon * DEG_TO_RAD;
            dp.y    = pt.lat * DEG_TO_RAD;
            dp.idx  = pt.idxVisible;
            line << dp;
        }
    }

    if(line.isEmpty())
    {
        return;
    }

    // convert coordinates of all waypoints into meter coordinates relative to the first track point
    point3D pt0 = line[0];
    QList<trkwpt_t> trkwpts;
    for(int i=0; i < project->childCount(); i++)
    {
        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(project->child(i));
        if(wpt == 0)
        {
            continue;
        }

        QPointF pos;
        pos = wpt->getPosition();

        trkwpt_t trkwpt;
        trkwpt.x      = pos.x() * DEG_TO_RAD;
        trkwpt.y      = pos.y() * DEG_TO_RAD;
        trkwpt.key    = wpt->getKey();

        qreal a1, a2;
        qreal d = GPS_Math_Distance(pt0.x, pt0.y, trkwpt.x, trkwpt.y, a1, a2);

        trkwpt.x = qCos(a1 * DEG_TO_RAD) * d;
        trkwpt.y = qSin(a1 * DEG_TO_RAD) * d;
        trkwpt.name = wpt->getName();

        trkwpts << trkwpt;
    }

    // convert all coordinates into meter relative to the first track point.
    for(int i = 0; i < line.size(); i++)
    {
        qreal d, a1 = 0, a2 = 0;
        pointDP& pt1 = line[i];

        d = GPS_Math_Distance(pt0.x, pt0.y, pt1.x, pt1.y, a1, a2);

        pt1.x = qCos(a1 * DEG_TO_RAD) * d;
        pt1.y = qSin(a1 * DEG_TO_RAD) * d;
    }

    foreach(const trkwpt_t &trkwpt, trkwpts)
    {
        qreal minD   = WPT_FOCUS_DIST_IN;
        qint32 index = NOIDX;

        foreach(const pointDP &pt, line)
        {
            current++;
            qreal d = (trkwpt.x - pt.x)*(trkwpt.x - pt.x) + (trkwpt.y - pt.y)*(trkwpt.y - pt.y);

            if(d < WPT_FOCUS_DIST_IN)
            {
                if(d < minD)
                {
                    index = pt.idx;
                    minD  = d;
                }
            }
            else if(withDoubles && (d > WPT_FOCUS_DIST_OUT))
            {
                trkpt_t * trkpt = const_cast<trkpt_t*>(getTrkPtByVisibleIndex(index));
                if(trkpt)
                {
                    trkpt->keyWpt = trkwpt.key;
                }


                index = NOIDX;
                minD  = WPT_FOCUS_DIST_IN;
            }

            if(current - lastCurrent > 100)
            {
                lastCurrent = current;
                PROGRESS(current, return );
            }
        }

        if(index != NOIDX)
        {
            trkpt_t * trkpt = const_cast<trkpt_t*>(getTrkPtByVisibleIndex(index));
            if(trkpt)
            {
                trkpt->keyWpt = trkwpt.key;
            }
        }
    }

    updateVisuals(eVisualDetails|eVisualPlot|eVisualProject, "findWaypointsCloseBy()");
}

bool CGisItemTrk::isCloseTo(const QPointF& pos)
{
    QMutexLocker lock(&mutexItems);

    qreal dist = GPS_Math_DistPointPolyline(lineSimple, pos);
    return dist < 20;
}

void CGisItemTrk::gainUserFocus(bool yes)
{
    keyUserFocus = yes ? key : key_t();
}

void CGisItemTrk::looseUserFocus()
{
    if(keyUserFocus == key)
    {
        keyUserFocus.clear();
    }
}


void CGisItemTrk::edit()
{
    if(dlgDetails.isNull())
    {
        dlgDetails = new CDetailsTrk(*this, 0);
        dlgDetails->setObjectName(getName());
    }

    CMainWindow::self().addWidgetToTab(dlgDetails);
}


bool CGisItemTrk::cut()
{
    if(nullptr == mouseClickFocus)
    {
        return false;
    }

    CCutTrk dlg(CMainWindow::getBestWidgetForParent());
    if(dlg.exec() == QDialog::Rejected)
    {
        return false;
    }

    qint32 idxMouse      = mouseClickFocus->idxTotal;
    CCutTrk::mode_e mode = dlg.getMode();

    // if the cut action results into cloning a track, the calling method should
    // ask if the original track should be removed. As a track can't delete itself
    // this has to be done from the outside of this method.
    bool askToDeleteOriginal    = dlg.createClone() || (mode == CCutTrk::eModeKeepBoth);

    // askToDeleteOriginal = store result as clone
    if(askToDeleteOriginal)
    {
        // clone first part?
        if((mode & (CCutTrk::eModeKeepBoth|CCutTrk::eModeKeepFirst)) != 0)
        {
            QString name1 = getName() + QString(" (%1 - %2)").arg(0).arg(idxMouse);
            name1 = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, name1);
            if(name1.isEmpty())
            {
                return false;
            }

            IGisProject *project = CGisWidget::self().selectProject();
            if(nullptr == project)
            {
                return false;
            }

            new CGisItemTrk(name1, 0, idxMouse, trk, project);
        }

        // clone second part?
        if((mode & (CCutTrk::eModeKeepBoth|CCutTrk::eModeKeepSecond)) != 0)
        {
            QString name1 = getName() + QString(" (%1 - %2)").arg(idxMouse).arg(cntTotalPoints-1);
            name1 = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, name1);
            if(name1.isEmpty())
            {
                return false;
            }

            IGisProject *project = CGisWidget::self().selectProject();
            if(nullptr == project)
            {
                return false;
            }

            new CGisItemTrk(name1, idxMouse, cntTotalPoints-1, trk, project);
        }
    }
    else
    {
        // if the result is not a clone, the track's list of segments and trackpoints
        // has to be reduced. This is done by copying points into a new trackpoint list
        // that replaces the original one.

        if((mode & CCutTrk::eModeKeepFirst) != 0)
        {
            for(int i = 0; i < trk.segs.size(); i++)
            {
                QVector<trkpt_t> pts;
                trkseg_t& seg = trk.segs[i];

                for(int n = 0; n < seg.pts.size(); n++)
                {
                    trkpt_t& pt = seg.pts[n];

                    if(pt.idxTotal > idxMouse)
                    {
                        break;
                    }

                    pts << pt;
                }

                seg.pts = pts;
            }
            deriveSecondaryData();
            changed(QObject::tr("Permanently removed points %1..%2").arg(idxMouse+1).arg(cntTotalPoints-1), "://icons/48x48/TrkCut.png");
        }
        else if((mode & CCutTrk::eModeKeepSecond) != 0)
        {
            for(int i = 0; i < trk.segs.size(); i++)
            {
                QVector<trkpt_t> pts;
                trkseg_t& seg = trk.segs[i];

                for(int n = 0; n < seg.pts.size(); n++)
                {
                    trkpt_t& pt = seg.pts[n];

                    if(pt.idxTotal < idxMouse)
                    {
                        continue;
                    }

                    pts << pt;
                }

                seg.pts = pts;
            }
            deriveSecondaryData();
            changed(QObject::tr("Permanently removed points %1..%2").arg(0).arg(idxMouse-1), "://icons/48x48/TrkCut.png");
        }
    }

    return askToDeleteOriginal;
}

void CGisItemTrk::reverse()
{
    QString name1 = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, getName() + "_rev");
    if(name1.isEmpty())
    {
        return;
    }

    IGisProject * project = CGisWidget::self().selectProject();
    if(nullptr == project)
    {
        return;
    }

    // start with a 1:1 copy of the first track
    CGisItemTrk * trk1 = new CGisItemTrk(*this, project, NOIDX, false);
    trk1->trk.name = name1;
    /*
        clear track data, item key and history. To clear the history is important as
        the original track's history would restore the original key

     */
    trk1->trk.segs.clear();
    trk1->key.clear();
    trk1->history.events.clear();

    foreach(const trkseg_t &seg, trk.segs)
    {
        trkseg_t seg1;
        foreach(const trkpt_t &pt, seg.pts)
        {
            trkpt_t pt1     = pt;
            pt1.time        = QDateTime();

            seg1.pts.push_front(pt1);
        }
        trk1->trk.segs.push_front(seg1);
    }
    // restore secondary data and create a new history
    trk1->deriveSecondaryData();
    trk1->setupHistory();
    trk1->updateDecoration(eMarkChanged, eMarkNone);
}

void CGisItemTrk::combine(const QList<IGisItem::key_t>& keysPreSel)
{
    IGisProject * project = dynamic_cast<IGisProject*>(parent());
    if(nullptr == project)
    {
        return;
    }

    CCombineTrk dlg(*this, keysPreSel, *project, CMainWindow::getBestWidgetForParent());
    dlg.exec();

    QList<IGisItem::key_t> keys = dlg.getTrackKeys();
    if(keys.isEmpty())
    {
        return;
    }

    QString name1 = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, getName() + " & other");
    if(name1.isEmpty())
    {
        return;
    }

    IGisProject * projectNew = CGisWidget::self().selectProject();
    if(nullptr == projectNew)
    {
        return;
    }

    // start with a 1:1 copy of the first track
    CGisItemTrk * trk1 = new CGisItemTrk(*this, projectNew, NOIDX, false);
    // replace name
    trk1->trk.name = name1;

    /*
        clear track data, item key and history. To clear the history is important as
        the original track's history would restore the original key

     */
    trk1->trk.segs.clear();
    trk1->key.clear();
    trk1->history.events.clear();

    // copy the segments of all tracks to new track
    foreach(const IGisItem::key_t &key, keys)
    {
        CGisItemTrk * trk2 = dynamic_cast<CGisItemTrk*>(project->getItemByKey(key));
        if(nullptr == trk2)
        {
            continue;
        }

        trk1->trk.segs += trk2->trk.segs;
    }

    // restore secondary data and create a new history
    trk1->deriveSecondaryData();
    trk1->setupHistory();
    trk1->updateDecoration(eMarkChanged, eMarkNone);
}

void CGisItemTrk::hideSelectedPoints()
{
    if(!setReadOnlyMode(false))
    {
        return;
    }

    if((nullptr == mouseRange1) || (nullptr == mouseRange2))
    {
        return;
    }

    // read start/stop indices
    qint32 idx1 = mouseRange1->idxTotal;
    qint32 idx2 = mouseRange2->idxTotal;

    if(idx1 > idx2)
    {
        qSwap(idx1,idx2);
    }

    // if first index is the first point adjust index to hide it, too
    if(isTrkPtFirstVisible(idx1))
    {
        idx1--;
    }
    // if second index is the last point adjust index to hide it, too
    if(isTrkPtLastVisible(idx2))
    {
        idx2++;
    }

    // special case for a single point
    if(idx1 == idx2)
    {
        for(int s = 0; s < trk.segs.size(); s++)
        {
            trkseg_t& seg = trk.segs[s];
            for(int i = 0; i < seg.pts.size(); i++)
            {
                trkpt_t& trkpt = seg.pts[i];

                if(idx1 == trkpt.idxTotal)
                {
                    trkpt.flags |= trkpt_t::eHidden;
                }
            }
        }
    }
    else
    {
        // iterate over all segments and delete points between idx1 and idx2
        for(int s = 0; s < trk.segs.size(); s++)
        {
            trkseg_t& seg = trk.segs[s];
            for(int i = 0; i < seg.pts.size(); i++)
            {
                trkpt_t& trkpt = seg.pts[i];

                if((idx1 < trkpt.idxTotal) && (trkpt.idxTotal < idx2))
                {
                    trkpt.flags |= trkpt_t::eHidden;
                }
            }
        }
    }
    mouseRange1 = 0;
    mouseRange2 = 0;
    rangeState  = eRangeStateIdle;
    deriveSecondaryData();
    changed(QObject::tr("Hide points."), "://icons/48x48/PointHide.png");
}

void CGisItemTrk::showSelectedPoints()
{
    if(!setReadOnlyMode(false))
    {
        return;
    }

    if((mouseRange1 == 0) || (mouseRange2 == 0))
    {
        return;
    }

    qint32 idx1 = mouseRange1->idxTotal;
    qint32 idx2 = mouseRange2->idxTotal;

    if(idx1 > idx2)
    {
        qSwap(idx1,idx2);
    }

    for(int s = 0; s < trk.segs.size(); s++)
    {
        trkseg_t& seg = trk.segs[s];
        for(int i = 0; i < seg.pts.size(); i++)
        {
            trkpt_t& trkpt = seg.pts[i];

            if((idx1 <= trkpt.idxTotal) && (trkpt.idxTotal <= idx2))
            {
                trkpt.flags &= ~trkpt_t::eHidden;
            }
        }
    }
    mouseRange1 = 0;
    mouseRange2 = 0;
    rangeState  = eRangeStateIdle;
    deriveSecondaryData();
    changed(QObject::tr("Show points."), "://icons/48x48/PointShow.png");
}

void CGisItemTrk::copySelectedPoints()
{
    if((mouseRange1 == 0) || (mouseRange2 == 0))
    {
        return;
    }

    quint32 idx1 = mouseRange1->idxTotal;
    quint32 idx2 = mouseRange2->idxTotal;

    if(idx1 > idx2)
    {
        qSwap(idx1,idx2);
    }

    IGisProject * project = CGisWidget::self().selectProject();
    if(nullptr == project)
    {
        return;
    }

    QString name1 = getName() + QString(" (%1 - %2)").arg(idx1).arg(idx2);
    name1 = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, name1);
    if(name1.isEmpty())
    {
        return;
    }
    new CGisItemTrk(name1, idx1, idx2, trk, project);
}

void CGisItemTrk::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF> &blockedAreas, CGisDraw *gis)
{
    QMutexLocker lock(&mutexItems);

    lineSimple.clear();
    lineFull.clear();

    if(!isVisible(boundingRect, viewport,gis))
    {
        return;
    }

    if(trk.segs.isEmpty())
    {
        return;
    }

    QPointF pt1;
    QPointF p1 = viewport[0];
    QPointF p2 = viewport[2];
    gis->convertRad2Px(p1);
    gis->convertRad2Px(p2);
    QRectF extViewport(p1,p2);

    if(mode == eModeNormal)
    {
        // in normal mode the trackline without points marked as deleted is drawn
        foreach (const trkseg_t &seg, trk.segs)
        {
            foreach(const trkpt_t &pt, seg.pts)
            {
                if(pt.flags & trkpt_t::eHidden)
                {
                    continue;
                }

                pt1.setX(pt.lon);
                pt1.setY(pt.lat);
                pt1 *= DEG_TO_RAD;
                lineSimple << pt1;
            }
        }
    }
    else
    {
        // in full mode the complete track including points marked as deleted
        // is drawn as gray line first. Then the track without points marked as
        // deleted is drawn with it's configured color
        foreach (const trkseg_t &seg, trk.segs)
        {
            foreach(const trkpt_t &pt, seg.pts)
            {
                pt1.setX(pt.lon);
                pt1.setY(pt.lat);
                pt1 *= DEG_TO_RAD;

                lineFull << pt1;

                if(pt.flags & trkpt_t::eHidden)
                {
                    continue;
                }

                lineSimple << pt1;
            }
        }
    }
    gis->convertRad2Px(lineSimple);
    gis->convertRad2Px(lineFull);

    // draw the full line first
    if(mode == eModeRange)
    {
        QList<QPolygonF> lines;
        splitLineToViewport(lineFull, extViewport, lines);

        p.setPen(QPen(Qt::lightGray, penWidthBg, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        foreach(const QPolygonF &l, lines)
        {
            p.drawPolyline(l);
        }

        QPixmap bullet("://icons/8x8/bullet_dark_gray.png");
        foreach(const QPolygonF &l, lines)
        {
            foreach(const QPointF &pt, l)
            {
                p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
            }
        }
    }
    // -------------------------

    // draw the reduced track line
    QList<QPolygonF> lines;
    splitLineToViewport(lineSimple, extViewport, lines);

    if(key == keyUserFocus)
    {
        p.setPen(QPen(Qt::red, penWidthHi, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        foreach(const QPolygonF &l, lines)
        {
            p.drawPolyline(l);
        }
    }

    p.setBrush(color);
    p.setPen(penBackground);
    foreach(const QPolygonF &l, lines)
    {
        p.drawPolyline(l);
        if(showArrows.val().toBool())
        {
            CDraw::arrows(l, extViewport, p, 10, 80, lineScale.val().toDouble());
        }
    }

    if(colorSource.isEmpty())
    {
        // use the track's ordinary color
        penForeground.setColor(color);
        p.setPen(penForeground);
        foreach(const QPolygonF &l, lines)
        {
            p.drawPolyline(l);
        }
    }
    else
    {
        drawColorized(p);
    }
    // -------------------------
}

void CGisItemTrk::drawColorized(QPainter &p)
{
    auto valueFunc = CKnownExtension::get(colorSource).valueFunc;

    QImage colors(1, 256, QImage::Format_RGB888);
    QPainter colorsPainter(&colors);

    QLinearGradient colorsGradient(colors.rect().topLeft(), colors.rect().bottomLeft());
    colorsGradient.setColorAt(1.00, QColor(  0,   0, 255)); // blue
    colorsGradient.setColorAt(0.60, QColor(  0, 255,   0)); // green
    colorsGradient.setColorAt(0.40, QColor(255, 255,   0)); // yellow
    colorsGradient.setColorAt(0.00, QColor(255,   0,   0)); // red
    colorsPainter.fillRect(colors.rect(), colorsGradient);

    const qreal factor = CKnownExtension::get(colorSource).factor;

    foreach(const trkseg_t &segment, trk.segs)
    {
        const trkpt_t *ptPrev = NULL;
        QColor colorStart;

        foreach(const trkpt_t &pt, segment.pts)
        {
            if(pt.flags & trkpt_t::eHidden)
            {
                continue;
            }
            if(NULL == ptPrev)
            {
                ptPrev = &pt;
                continue;
            }

            float colorAt = ( factor * valueFunc(pt) - limitLow ) / (limitHigh - limitLow);
            if(colorAt > 1.f)
            {
                colorAt = 1.f;
            }
            if(colorAt < 0.f)
            {
                colorAt = 0.f;
            }

            const QColor &colorEnd = colors.pixel(0, ((1.f - colorAt) * 255.f));
            if(!colorStart.isValid())
            {
                colorStart = colorEnd;
            }

            QLinearGradient grad(lineSimple[ptPrev->idxVisible], lineSimple[pt.idxVisible]);
            grad.setColorAt(0.f, colorStart);
            grad.setColorAt(1.f, colorEnd);

            QPen pen;
            pen.setBrush(QBrush(grad));
            pen.setWidth(penWidthFg);
            pen.setCapStyle(Qt::RoundCap);

            p.setPen(pen);
            p.drawLine(lineSimple[ptPrev->idxVisible], lineSimple[pt.idxVisible]);

            ptPrev = &pt;
            colorStart = colorEnd;
        }
    }
}


qreal CGisItemTrk::getMin(const QString& source) const
{
    return extrema.value(source).min * CKnownExtension::get(source).factor;
}

qreal CGisItemTrk::getMax(const QString& source) const
{
    return extrema.value(source).max * CKnownExtension::get(source).factor;
}

qreal CGisItemTrk::getMinProp(const QString& source) const
{
    if(propHandler == nullptr)
    {
        return NOFLOAT;
    }
    return propHandler->propBySource(source).min;
}

qreal CGisItemTrk::getMaxProp(const QString& source) const
{
    if(propHandler == nullptr)
    {
        return NOFLOAT;
    }
    return propHandler->propBySource(source).max;
}

QString CGisItemTrk::getUnitProp(const QString& source) const
{
    if(propHandler == nullptr)
    {
        return QString();
    }
    return propHandler->propBySource(source).unit;
}


QStringList CGisItemTrk::getExistingDataSources() const
{
    QStringList known;
    QStringList unknown;

    foreach(const QString &key, existingExtensions)
    {
        if(CKnownExtension::isKnown(key))
        {
            known << key;
        }
        else
        {
            unknown << key;
        }
    }

    auto stringSort = [] (const QString &s1, const QString &s2)
                      {
                          return s1.toLower() < s2.toLower();
                      };

    qSort(known.begin(),   known.end(),   stringSort);
    qSort(unknown.begin(), unknown.end(), stringSort);

    return known + unknown;
}

void CGisItemTrk::setColorizeSource(QString src)
{
    if(src != colorSource)
    {
        colorSource = src;

        const CKnownExtension ext = CKnownExtension::get(src);
        if(ext.known)
        {
            limitLow    = ext.defLimitLow;
            limitHigh   = ext.defLimitHigh;
        }
        else
        {
            limitLow    = getMin(src);
            limitHigh   = getMax(src);

            if(limitHigh - limitLow < 0.1)
            {
                limitHigh = limitLow + 0.1;
            }
        }
        updateHistory(eVisualColorLegend|eVisualDetails);
    }
}

void CGisItemTrk::setColorizeLimitLow(qreal limit)
{
    limitLow = limit;
    updateHistory(eVisualColorLegend|eVisualDetails);
}

void CGisItemTrk::setColorizeLimitHigh(qreal limit)
{
    limitHigh = limit;
    updateHistory(eVisualColorLegend|eVisualDetails);
}

const QString CGisItemTrk::getColorizeUnit() const
{
    return CKnownExtension::get(colorSource).unit;
}

void CGisItemTrk::drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    QMutexLocker lock(&mutexItems);

    if(trk.segs.isEmpty())
    {
        return;
    }

    if(hasUserFocus() && mouseMoveFocus && (mode != eModeRange))
    {
        // derive anchor
        QPointF anchor(mouseMoveFocus->lon, mouseMoveFocus->lat);
        anchor *= DEG_TO_RAD;
        gis->convertRad2Px(anchor);

        // create trackpoint info text
        QString str, val1, unit1, val2, unit2;
        str = getInfoTrkPt(*mouseMoveFocus);

        // calculate bounding box of text
        QFont f = CMainWindow::self().getMapFont();
        QFontMetrics fm(f);
        QRect rectText = fm.boundingRect(QRect(0,0,500,0), Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, str);

        // create info box
        int w = rectText.width()  + 5 + 5;
        int h = rectText.height() + 5 + (fm.height() + 8);
        if(totalElapsedSeconds != 0)
        {
            h += 5 + fm.height() + 8;
        }

        p.setFont(f);

        // draw the bubble
        QRect box(0, 0, w, h);
        box.moveBottomLeft(anchor.toPoint() + QPoint(-50,-50));
        CDraw::bubble(p, box, anchor.toPoint(), 18 /* px */, 21 /* px */);

        p.save();
        p.translate(box.topLeft());

        // draw progress bar distance
        p.translate(5,5);
        QRect rectBar1(0,0,rectText.width(), fm.height());
        p.setPen(QColor(150,150,255));
        p.setBrush(QColor(150,150,255));
        p.drawRect(rectBar1);
        qreal d = mouseMoveFocus->distance * rectBar1.width() / totalDistance;
        p.setPen(QColor(150,255,150));
        p.setBrush(QColor(150,255,150));
        p.drawRect(0,0,d,fm.height());


        IUnit::self().meter2distance(mouseMoveFocus->distance, val1, unit1);
        IUnit::self().meter2distance(totalDistance - mouseMoveFocus->distance, val2, unit2);
        p.setPen(Qt::darkBlue);
        p.drawText(QRect(0,1,rectBar1.width(),fm.height()), Qt::AlignVCenter|Qt::AlignLeft, QString("%1%2").arg(val1).arg(unit1));
        p.drawText(QRect(0,1,rectBar1.width(),fm.height()), Qt::AlignCenter, QString("%1%").arg(mouseMoveFocus->distance * 100 / totalDistance, 0, 'f', 0));
        p.drawText(QRect(0,1,rectBar1.width(),fm.height()), Qt::AlignVCenter|Qt::AlignRight, QString("%1%2").arg(val2).arg(unit2));

        // draw progress bar time
        if(totalElapsedSeconds != 0)
        {
            p.translate(0,fm.height() + 5);
            QRect rectBar2(0,0,rectText.width(), fm.height());
            p.setPen(QColor(150,150,255));
            p.setBrush(QColor(150,150,255));
            p.drawRect(rectBar2);
            qreal t = mouseMoveFocus->elapsedSecondsMoving * rectBar2.width() / totalElapsedSecondsMoving;
            p.setPen(QColor(150,255,150));
            p.setBrush(QColor(150,255,150));
            p.drawRect(0,0,t,fm.height());

            IUnit::self().seconds2time(mouseMoveFocus->elapsedSecondsMoving, val1, unit1);
            IUnit::self().seconds2time(totalElapsedSecondsMoving - mouseMoveFocus->elapsedSecondsMoving, val2, unit2);
            p.setPen(Qt::darkBlue);
            p.drawText(QRect(0,1,rectBar1.width(),fm.height()), Qt::AlignVCenter|Qt::AlignLeft, QString("%1%2").arg(val1).arg(unit1));
            p.drawText(QRect(0,1,rectBar1.width(),fm.height()), Qt::AlignCenter, QString("%1%").arg(mouseMoveFocus->elapsedSecondsMoving * 100 / totalElapsedSecondsMoving, 0, 'f', 0));
            p.drawText(QRect(0,1,rectBar1.width(),fm.height()), Qt::AlignVCenter|Qt::AlignRight, QString("%1%2").arg(val2).arg(unit2));

            p.translate(0,fm.height() + 5);
        }
        else
        {
            p.translate(0, 5);
        }
        // draw text
        p.translate(0, 3);
        p.setPen(Qt::darkBlue);
        p.drawText(rectText, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,str);

        p.restore();
    }

    if(!scrOpt.isNull() && mouseClickFocus)
    {
        QPointF anchor(mouseClickFocus->lon, mouseClickFocus->lat);
        anchor *= DEG_TO_RAD;
        gis->convertRad2Px(anchor);

        p.drawPixmap(anchor - QPointF(4,4), QPixmap(IGisItem::colorMap[colorIdx].bullet));
    }

    drawRange(p);
}

void CGisItemTrk::drawLabel(QPainter& p, const QPolygonF &viewport, QList<QRectF> &blockedAreas, const QFontMetricsF &fm, CGisDraw *gis)
{
    // tracks have no labels
}


void CGisItemTrk::drawHighlight(QPainter& p)
{
    QMutexLocker lock(&mutexItems);

    if(lineSimple.isEmpty() || hasUserFocus())
    {
        return;
    }
    p.setPen(QPen(QColor(255,0,0,100), penWidthHi, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(lineSimple);
}

void CGisItemTrk::drawRange(QPainter& p)
{
    QMutexLocker lock(&mutexItems);

    if((mouseRange1 != 0) && (mouseRange2 != 0))
    {
        const QPolygonF& line = (mode == eModeRange) ? lineFull : lineSimple;
        int idx1 = (mode == eModeRange) ? mouseRange1->idxTotal : mouseRange1->idxVisible;
        int idx2 = (mode == eModeRange) ? mouseRange2->idxTotal : mouseRange2->idxVisible;

        if(idx1 > idx2)
        {
            qSwap(idx1,idx2);
        }

        QPolygonF seg = line.mid(idx1, idx2 - idx1 + 1);

        p.setPen(QPen(Qt::darkGreen, penWidthHi, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(seg);

        p.setPen(QPen(Qt::green, penWidthFg, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(seg);
    }
}

bool CGisItemTrk::setMode(mode_e m, const QString& owner)
{
    if(!mouseFocusOwner.isEmpty() && owner != mouseFocusOwner)
    {
        return false;
    }

    mode            = m;
    // always reset the range statemachine
    rangeState      = eRangeStateIdle;
    mouseRange1     = 0;
    mouseRange2     = 0;
    mouseFocusOwner = mode == eModeRange ? owner : "";


    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    return true;
}

void CGisItemTrk::setName(const QString& str)
{
    setText(CGisListWks::eColumnName, str);
    trk.name = str;
    changed(QObject::tr("Changed name"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setComment(const QString& str)
{
    trk.cmt = str;
    changed(QObject::tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setDescription(const QString& str)
{
    trk.desc = str;
    changed(QObject::tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setLinks(const QList<link_t>& links)
{
    trk.links = links;
    changed(QObject::tr("Changed links"), "://icons/48x48/Link.png");
}


void CGisItemTrk::setColor(int idx)
{
    if(idx < TRK_N_COLORS)
    {
        setColor(IGisItem::colorMap[idx].color);
        updateHistory(eVisualColorLegend|eVisualDetails);
    }
}

void CGisItemTrk::setActivity(quint32 flag, const QString& name, const QString& icon)
{
    for(int s = 0; s < trk.segs.size(); s++)
    {
        trkseg_t& seg = trk.segs[s];
        for(int i = 0; i < seg.pts.size(); i++)
        {
            trkpt_t& trkpt = seg.pts[i];
            trkpt.flags &= ~trkpt_t::eActMask;
            trkpt.flags |= flag;
        }
    }

    deriveSecondaryData();
    changed(QObject::tr("Changed activity to '%1' for complete track.").arg(name), icon);
}

void CGisItemTrk::setActivity()
{
    if((mouseRange1 == 0) && (mouseRange2 == 0))
    {
        return;
    }

    quint32 flag = 0;
    QString name;
    QString icon;

    CSelectActivity dlg(flag, name, icon, CMainWindow::getBestWidgetForParent());
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    if(!setReadOnlyMode(false))
    {
        return;
    }

    // read start/stop indices
    qint32 idx1 = mouseRange1->idxTotal;
    qint32 idx2 = mouseRange2->idxTotal;

    if(idx1 > idx2)
    {
        qSwap(idx1,idx2);
    }

    // special case for a single point
    if(idx1 == idx2)
    {
        for(int s = 0; s < trk.segs.size(); s++)
        {
            trkseg_t& seg = trk.segs[s];
            for(int i = 0; i < seg.pts.size(); i++)
            {
                trkpt_t& trkpt = seg.pts[i];

                if(idx1 == trkpt.idxTotal)
                {
                    trkpt.flags &= ~trkpt_t::eActMask;
                    trkpt.flags |= flag;
                }
            }
        }
    }
    else
    {
        // iterate over all segments and delete points between idx1 and idx2
        for(int s = 0; s < trk.segs.size(); s++)
        {
            trkseg_t& seg = trk.segs[s];
            for(int i = 0; i < seg.pts.size(); i++)
            {
                trkpt_t& trkpt = seg.pts[i];

                if((idx1 < trkpt.idxTotal) && (trkpt.idxTotal < idx2))
                {
                    trkpt.flags &= ~trkpt_t::eActMask;
                    trkpt.flags |= flag;
                }
            }
        }
    }
    mouseRange1 = 0;
    mouseRange2 = 0;
    rangeState  = eRangeStateIdle;
    deriveSecondaryData();
    changed(QObject::tr("Changed activity to '%1' for range(%2..%3).").arg(name).arg(idx1).arg(idx2), icon);
}


void CGisItemTrk::setColor(const QColor& c)
{
    colorIdx = DEFAULT_COLOR;
    for(int n = 0; n < TRK_N_COLORS; n++)
    {
        if(c == IGisItem::colorMap[n].color)
        {
            colorIdx = n;
            break;
        }
    }

    color  = IGisItem::colorMap[colorIdx].color;
    bullet = QPixmap(IGisItem::colorMap[colorIdx].bullet);

    setIcon(color2str(color));
}

void CGisItemTrk::setIcon(const QString& iconColor)
{
    trk.color   = iconColor;
    icon        = QPixmap("://icons/48x48/Track.png");

    QPixmap mask( icon.size() );
    mask.fill( str2color(iconColor) );
    mask.setMask( icon.createMaskFromColor( Qt::transparent ) );
    icon = mask.scaled(22,22, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QTreeWidgetItem::setIcon(CGisListWks::eColumnIcon,icon);
}



bool CGisItemTrk::setMouseFocusByDistance(qreal dist, focusmode_e fmode, const QString &owner)
{
    const trkpt_t * newPointOfFocus = nullptr;

    if(dist != NOFLOAT)
    {
        qreal delta = totalDistance;

        /// @todo: optimize search by single out segment and then do a binary search

        foreach (const trkseg_t &seg, trk.segs)
        {
            foreach(const trkpt_t &pt, seg.pts)
            {
                if(pt.flags & trkpt_t::eHidden)
                {
                    continue;
                }

                qreal d = qAbs(pt.distance - dist);
                if(d <= delta)
                {
                    newPointOfFocus = &pt;
                    delta = d;
                }
                else
                {
                    break;
                }
            }
        }
    }

    return publishMouseFocus(newPointOfFocus, fmode, owner);
}

bool CGisItemTrk::setMouseFocusByTime(quint32 time, focusmode_e fmode, const QString &owner)
{
    const trkpt_t * newPointOfFocus = nullptr;

    if(time != NOTIME)
    {
        /// @todo: optimize search by single out segment and then do a binary search

        qreal delta = totalElapsedSeconds;

        foreach (const trkseg_t &seg, trk.segs)
        {
            foreach(const trkpt_t &pt, seg.pts)
            {
                if(pt.flags & trkpt_t::eHidden)
                {
                    continue;
                }

                qreal d = qAbs(qreal(pt.time.toTime_t()) - qreal(time));
                if(d <= delta)
                {
                    newPointOfFocus = &pt;
                    delta = d;
                }
                else
                {
                    break;
                }
            }
        }
    }

    return publishMouseFocus(newPointOfFocus, fmode, owner);
}

QPointF CGisItemTrk::setMouseFocusByPoint(const QPoint& pt, focusmode_e fmode, const QString &owner)
{
    QMutexLocker lock(&mutexItems);

    const trkpt_t * newPointOfFocus = nullptr;
    quint32 idx = 0;

    const QPolygonF& line = (mode == eModeRange) ? lineFull : lineSimple;

    if(pt != NOPOINT && GPS_Math_DistPointPolyline(line, pt) < MIN_DIST_FOCUS)
    {
        /*
            Iterate over the polyline used to draw the track as it contains screen
            coordinates. The polyline is a linear representation of the segments in the
            track. That is why the index into the polyline can't be used directly.
            In a second step we have to iterate over all segments and points of the trk_t object
            until the index is reached. This is done by either getTrkPtByVisibleIndex(), or
            getTrkPtByTotalIndex(). Depending on the current mode.
         */

        quint32 i   = 0;
        qint32 d1   = NOINT;
        foreach(const QPointF &point, line)
        {
            int tmp = (pt - point).manhattanLength();
            if(tmp < d1)
            {
                idx = i;
                d1  = tmp;
            }
            i++;
        }

        newPointOfFocus = (mode == eModeRange) ? getTrkPtByTotalIndex(idx) : getTrkPtByVisibleIndex(idx);
    }
    if(!publishMouseFocus(newPointOfFocus, fmode, owner))
    {
        newPointOfFocus = nullptr;
    }

    /*
       Test for line size before applying index. This fixes random assertions because
       of an invalid index. The reason for this is unknown.
     */
    return newPointOfFocus ? ((int)idx < line.size() ? line[idx] : NOPOINTF) : NOPOINTF;
}


bool CGisItemTrk::setMouseFocusByTotalIndex(qint32 idx, focusmode_e fmode, const QString &owner)
{
    const trkpt_t * newPointOfFocus = nullptr;

    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt, seg.pts)
        {
            if(pt.idxTotal == idx)
            {
                newPointOfFocus = &pt;
                return publishMouseFocus(newPointOfFocus, fmode, owner);
            }
        }
    }
    return false;
}

const CGisItemTrk::trkpt_t * CGisItemTrk::getTrkPtByVisibleIndex(qint32 idx)
{
    if(idx == NOIDX)
    {
        return 0;
    }

    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt, seg.pts)
        {
            if(pt.idxVisible == idx)
            {
                return &pt;
            }
        }
    }
    return 0;
}

const CGisItemTrk::trkpt_t * CGisItemTrk::getTrkPtByTotalIndex(qint32 idx)
{
    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt, seg.pts)
        {
            if(pt.idxTotal == idx)
            {
                return &pt;
            }
        }
    }
    return 0;
}

bool CGisItemTrk::isTrkPtLastVisible(qint32 idxTotal)
{
    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt1, seg.pts)
        {
            if((pt1.idxTotal > idxTotal) && !(pt1.flags & trkpt_t::eHidden))
            {
                return false;
            }
        }
    }
    return true;
}

bool CGisItemTrk::isTrkPtFirstVisible(qint32 idxTotal)
{
    foreach (const trkseg_t &seg, trk.segs)
    {
        foreach(const trkpt_t &pt1, seg.pts)
        {
            if((pt1.idxTotal < idxTotal))
            {
                if(!(pt1.flags & trkpt_t::eHidden))
                {
                    return false;
                }
            }
            else
            {
                return true;
            }
        }
    }
    return true;
}


bool CGisItemTrk::publishMouseFocus(const trkpt_t * pt, focusmode_e fmode, const QString& owner)
{
    if(mode == eModeRange)
    {
        if(mouseFocusOwner != owner)
        {
            return false;
        }

        publishMouseFocusRangeMode(pt, fmode);
    }
    else
    {
        publishMouseFocusNormalMode(pt, fmode);
    }

    return true;
}

void CGisItemTrk::publishMouseFocusRangeMode(const trkpt_t * pt, focusmode_e fmode)
{
    switch(rangeState)
    {
    case eRangeStateIdle:
    {
        if((fmode == eFocusMouseClick) && (pt != 0))
        {
            mouseRange1 = pt;
            rangeState  = eRangeState1st;
        }

        break;
    }

    case eRangeState1st:
    {
        mouseRange2 = pt;
        if((fmode == eFocusMouseClick) && (pt != 0))
        {
            rangeState  = eRangeState2nd;
        }
        break;
    }

    case eRangeState2nd:
    {
        if(fmode == eFocusMouseClick)
        {
            mouseRange1 = 0;
            mouseRange2 = 0;
            rangeState  = eRangeStateIdle;
        }
        break;
    }
    }

    setMouseFocusVisuals(pt);
    setMouseRangeFocusVisuals(mouseRange1, mouseRange2);
}
void CGisItemTrk::publishMouseFocusNormalMode(const trkpt_t * pt, focusmode_e fmode)
{
    switch(fmode)
    {
    case eFocusMouseMove:
        if(pt != mouseMoveFocus)
        {
            mouseMoveFocus = pt;
            setMouseFocusVisuals(pt);
            setMouseRangeFocusVisuals(0, 0);
        }
        break;

    case eFocusMouseClick:
        if(pt != mouseClickFocus)
        {
            mouseClickFocus =  pt;
            setMouseClickFocusVisuals(pt);
        }

    default:
        ;
    }
}

void CGisItemTrk::changed(const QString& what, const QString& icon)
{
    IGisItem::changed(what, icon);
    updateVisuals(eVisualAll, "changed()");
}

void CGisItemTrk::updateHistory(quint32 visuals)
{
    IGisItem::updateHistory();
    updateVisuals(visuals, "updateHistory()");
}


void CGisItemTrk::updateVisuals(quint32 visuals, const QString& who)
{
    qDebug() << "CGisItemTrk::updateVisuals()" << getName() << who;

    if(!dlgDetails.isNull() && (visuals & eVisualDetails))
    {
        dlgDetails->updateData();
    }

    foreach(INotifyTrk * visual, registeredVisuals)
    {
        if(visuals & visual->mask)
        {
            visual->updateData();
        }
    }
}

void CGisItemTrk::setMouseFocusVisuals(const CGisItemTrk::trkpt_t * pt)
{
    if(!dlgDetails.isNull())
    {
        dlgDetails->setMouseFocus(pt);
    }

    foreach(INotifyTrk * visual, registeredVisuals)
    {
        visual->setMouseFocus(pt);
    }
}

void CGisItemTrk::setMouseRangeFocusVisuals(const CGisItemTrk::trkpt_t * pt1, const CGisItemTrk::trkpt_t * pt2)
{
    if(!dlgDetails.isNull())
    {
        dlgDetails->setMouseRangeFocus(pt1, pt2);
    }

    foreach(INotifyTrk * visual, registeredVisuals)
    {
        visual->setMouseRangeFocus(pt1, pt2);
    }
}

void CGisItemTrk::setMouseClickFocusVisuals(const CGisItemTrk::trkpt_t * pt)
{
    if(!dlgDetails.isNull())
    {
        dlgDetails->setMouseClickFocus(pt);
    }

    foreach(INotifyTrk * visual, registeredVisuals)
    {
        visual->setMouseClickFocus(pt);
    }
}
