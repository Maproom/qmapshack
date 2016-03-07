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
#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "gis/prj/IGisProject.h"
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
    QString name;
    qreal x = 0;
    qreal y = 0;
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

    for(const trkseg_t &srcseg : srctrk.segs)
    {
        trkseg_t seg;
        for(const trkpt_t &srcpt : srcseg.pts)
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
        trk.name += tr("_Clone");
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
    , trk(std::move(trkdata))
{
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

template<typename T>
static inline bool isInRange(const T &val, const T &rangeStart, const T &rangeEnd)
{
    return (rangeStart <= val) && (val <= rangeEnd);
}

IGisItem * CGisItemTrk::createClone()
{
    int idx = -1;
    IGisProject * project = getParentProject();
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
    changed(tr("Changed trackpoints, sacrificed all previous data."), "://icons/48x48/LineMove.png");
}

void CGisItemTrk::getPolylineFromData(QPolygonF &l) const
{
    QMutexLocker lock(&mutexItems);

    l.clear();
    for(const trkseg_t &seg : trk.segs)
    {
        for(const trkpt_t &pt : seg.pts)
        {
            if(!pt.isHidden())
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
    for(const trkseg_t &seg : trk.segs)
    {
        for(const trkpt_t &pt : seg.pts)
        {
            if(!pt.isHidden())
            {
                l << point_t(QPointF(pt.lon*DEG_TO_RAD, pt.lat * DEG_TO_RAD));
            }
        }
    }
}

bool CGisItemTrk::getNameAndProject(QString &name, IGisProject *&project)
{
    name = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), tr("Edit name..."), tr("Enter new track name."), QLineEdit::Normal, name);
    if(name.isEmpty())
    {
        return false;
    }

    project = CGisWidget::self().selectProject();
    return nullptr != project;
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

    if(cntVisiblePoints == 0)
    {
        return showName ? QString("<div><b>%1</b></div>").arg(getName()) : QString("<div></div>");
    }

    QString str = "<div>";

    if(showName)
    {
        str += "<b>" + getName() + "</b><br />";
    }

    IUnit::self().meter2distance(totalDistance, val1, unit1);
    str += tr("Length: %1 %2").arg(val1).arg(unit1);

    if(totalAscend != NOFLOAT && totalDescend != NOFLOAT)
    {
        IUnit::self().meter2elevation(totalAscend,  val1, unit1);
        IUnit::self().meter2elevation(totalDescend, val2, unit2);

        str += tr(", %1%2 %3, %4%5 %6").arg(QChar(0x2197)).arg(val1).arg(unit1).arg(QChar(0x2198)).arg(val2).arg(unit2);
    }
    str += "<br />";

    if(totalElapsedSeconds != NOTIME)
    {
        IUnit::self().seconds2time(totalElapsedSeconds, val1, unit1);
        IUnit::self().meter2speed(totalDistance / totalElapsedSeconds, val2, unit2);
        str += tr("Time: %1, Speed: %2 %3").arg(val1).arg(val2).arg(unit2) + "<br />";
    }

    if(totalElapsedSecondsMoving != NOTIME)
    {
        IUnit::self().seconds2time(totalElapsedSecondsMoving, val1, unit1);
        IUnit::self().meter2speed(totalDistance / totalElapsedSecondsMoving, val2, unit2);
        str += tr("Moving: %1, Speed: %2 %3").arg(val1).arg(val2).arg(unit2) + "<br />";
    }

    if(timeStart.isValid())
    {
        str += tr("Start: %1").arg(IUnit::datetime2string(timeStart, false, boundingRect.center())) + "<br />";
    }
    if(timeEnd.isValid())
    {
        str += tr("End: %1").arg(IUnit::datetime2string(timeEnd, false, boundingRect.center())) + "<br />";
    }

    str += tr("Points: %1 (%2)").arg(cntVisiblePoints).arg(cntTotalPoints);
    return str + "</div>";
}

QString CGisItemTrk::getInfoRange() const
{
    qreal tmp, slope1, slope2;
    QString str, val, unit;
    if(mouseRange1 == nullptr || mouseRange2 == nullptr)
    {
        return str;
    }

    int idx1 = mouseRange1->idxTotal;
    int idx2 = mouseRange2->idxTotal;
    const trkpt_t *pt1 = mouseRange1;
    const trkpt_t *pt2 = mouseRange2;
    if(idx1 >= idx2)
    {
        pt1 = mouseRange2;
        pt2 = mouseRange1;
    }

    for(; pt1->isHidden() && (pt1->idxTotal < cntTotalPoints); ++pt1)
    {
    }
    for(; pt2->isHidden() && (pt2->idxTotal > 0); --pt2)
    {
    }

    bool timeIsValid = pt1->time.isValid() && pt2->time.isValid();
    qreal deltaTime  = pt2->time.toTime_t() - pt1->time.toTime_t();

    const qreal distance = pt2->distance - pt1->distance;

    IUnit::self().meter2distance(distance, val, unit);
    str += QString("%3 %1%2 ").arg(val).arg(unit).arg(QChar(0x21A6));
    if(timeIsValid)
    {
        quint32 t  = pt2->time.toTime_t() - pt1->time.toTime_t();
        quint32 hh =  t / 3600;
        quint32 mm = (t % 3600) / 60;
        quint32 ss =  t % 60;

        str += QString("%4 %1:%2:%3").arg(hh, 2, 10, QChar('0')).arg(mm, 2, 10, QChar('0')).arg(ss, 2, 10, QChar('0')).arg(QChar(0x231a));

        IUnit::self().meter2speed(distance/deltaTime, val, unit);
        str += QString(", %3 %1%2").arg(val).arg(unit).arg(QChar(0x21A3));
    }
    str += "\n";

    qreal deltaAscend  = pt2->ascend  - pt1->ascend;
    qreal deltaDescend = pt2->descend - pt1->descend;

    tmp    = qAtan(deltaAscend/distance);
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

    tmp    = qAtan(deltaDescend/distance);
    slope1 = qAbs(tmp * 360.0/(2 * M_PI));
    slope2 = qTan(slope1 * DEG_TO_RAD) * 100;

    IUnit::self().meter2elevation(deltaDescend, val, unit);
    str += QString("%3 %1%2 (%4%5, %6%)").arg(val).arg(unit).arg(QChar(0x2198)).arg(qRound(slope1)).arg(QChar(0260)).arg(qRound(slope2));
    if(timeIsValid)
    {
        IUnit::self().meter2speed(deltaDescend/deltaTime, val, unit);
        str += QString(", %1%2").arg(val).arg(unit);
    }

    return str + "\n";
}

QString CGisItemTrk::getInfoTrkPt(const trkpt_t& pt) const
{
    QString str, val1, unit1;
    if(totalElapsedSeconds != 0)
    {
        str += IUnit::datetime2string(pt.time, false, QPointF(pt.lon, pt.lat) * DEG_TO_RAD);
    }
    str += "\n";

    IUnit::self().meter2elevation(pt.ele, val1, unit1);
    str += tr("Ele.: %1 %2").arg(val1).arg(unit1);
    if(pt.slope1 != NOFLOAT)
    {
        str += tr(" slope: %1%3 (%2%)").arg(pt.slope1, 2, 'f', 0).arg(pt.slope2, 2, 'f', 0).arg(QChar(0260));
    }
    if(pt.speed != NOFLOAT)
    {
        IUnit::self().meter2speed(pt.speed, val1, unit1);
        str += tr(" speed: %1%2").arg(val1).arg(unit1);
    }

    QStringList keys = pt.extensions.keys();
    keys.sort();

    qint32 more = keys.size() - 10;
    if(more > 0)
    {
        keys = keys.mid(0, 10);
    }

    for(const QString &key : keys)
    {
        const CKnownExtension &ext = CKnownExtension::get(key);
        if(ext.known)
        {
            str += "\n" + ext.name + ": " + QString("%1%2").arg(ext.valueFunc(pt)*ext.factor, 0, 'f', 1).arg(ext.unit);
        }
        else
        {
            QStringList tags = key.split("|");
            str += "\n" + tags.last() + ": " + pt.extensions[key].toString();
        }
    }

    if(more > 0)
    {
        str += "\n" + tr("... and %1 tags not displayed").arg(more);
    }

    return str;
}

QString CGisItemTrk::getInfoProgress(const trkpt_t& pt) const
{
    QString val, unit;

    QString asc = tr("Ascend: - (-)");
    QString dsc = tr("Descend: - (-)");
    QString dst = tr("Distance: - (-)");
    QString mov = tr("Moving: - (-)");

    if(pt.ascend != NOFLOAT)
    {
        IUnit::self().meter2elevation(pt.ascend, val, unit);
        asc = tr("Ascend: %1%2 (%3%)").arg(val).arg(unit).arg(pt.ascend * 100/totalAscend, 2, 'f', 0);
    }

    if(pt.descend != NOFLOAT)
    {
        IUnit::self().meter2elevation(pt.descend, val, unit);
        dsc = tr(" Descend: %1%2 (%3%)").arg(val).arg(unit).arg(pt.descend * 100/totalDescend, 2, 'f', 0);
    }

    if(pt.distance != NOFLOAT)
    {
        IUnit::self().meter2distance(pt.distance, val, unit);
        dst = tr("Distance: %1%2 (%3%)").arg(val).arg(unit).arg(pt.distance * 100/totalDistance, 2, 'f', 0);
    }

    if(pt.elapsedSeconds != NOFLOAT)
    {
        IUnit::self().seconds2time(pt.elapsedSecondsMoving, val, unit);
        mov = tr(" Moving: %1%2 (%3%)").arg(val).arg(unit).arg(pt.elapsedSecondsMoving * 100/totalElapsedSecondsMoving, 2, 'f', 0);
    }

    return QString("%1 %2\n%3 %4").arg(asc).arg(dsc).arg(dst).arg(mov);
}

QString CGisItemTrk::getInfoRange(const trkpt_t& pt1, const trkpt_t& pt2) const
{
    QString val, unit;
    qreal dt = NOFLOAT;

    if(pt1.time.isValid() && pt2.time.isValid())
    {
        dt = pt2.time.toTime_t() - pt1.time.toTime_t();
    }

    QString asc = tr("Ascend: -");
    QString dsc = tr("Descend: -");

    if((pt1.ascend != NOFLOAT) && (pt2.ascend != NOFLOAT))
    {
        IUnit::self().meter2elevation(pt2.ascend - pt1.ascend, val, unit);
        asc = tr("Ascend: %1%2").arg(val).arg(unit);

        if(dt != NOFLOAT)
        {
            IUnit::self().meter2speed((pt2.ascend - pt1.ascend)/dt, val, unit);
            asc += tr(", %1%2").arg(val).arg(unit);
        }
    }

    if((pt1.descend != NOFLOAT) && (pt2.descend != NOFLOAT))
    {
        IUnit::self().meter2elevation(pt2.descend - pt1.descend, val, unit);
        dsc = tr(" Descend: %1%2").arg(val).arg(unit);

        if(dt != NOFLOAT)
        {
            IUnit::self().meter2speed((pt2.descend - pt1.descend)/dt, val, unit);
            dsc += tr(", %1%2").arg(val).arg(unit);
        }
    }

    IUnit::self().meter2distance(pt2.distance - pt1.distance, val, unit);
    QString dsttme = tr("Distance: %1%2").arg(val).arg(unit);

    if(dt != NOFLOAT)
    {
        IUnit::self().seconds2time(dt, val, unit);
        dsttme += tr(" Time: %1%2").arg(val).arg(unit);
    }

    return QString("%1 %2\n%3").arg(asc).arg(dsc).arg(dsttme);
}

IScrOpt * CGisItemTrk::getScreenOptions(const QPoint& origin, IMouse * mouse)
{
    if(scrOpt.isNull())
    {
        scrOpt = new CScrOptTrk(this, origin, mouse);
    }
    return scrOpt;
}

static qint32 getIdxPointCloseBy(const QPoint &pos, const QPolygonF &line)
{
    qint32 idx     = 0;
    qint32 bestIdx = NOIDX;
    qint32 bestDst = NOINT;
    for(const QPointF &pt : line)
    {
        int dst = (pos - pt).manhattanLength();
        if(dst < bestDst)
        {
            bestIdx = idx;
            bestDst = dst;
        }
        ++idx;
    }

    return bestIdx;
}

QPointF CGisItemTrk::getPointCloseBy(const QPoint& screenPos)
{
    QMutexLocker lock(&mutexItems);

    qint32 bestIdx = getIdxPointCloseBy(screenPos, lineSimple);
    return (NOIDX == bestIdx) ? NOPOINTF : lineSimple[bestIdx];
}

void CGisItemTrk::getSelectedVisiblePoints(qint32& idx1, qint32& idx2) const
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
    limits_t extremaSpeed    = { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };
    limits_t extremaSlope    = { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };
    limits_t extremaEle      = { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };
    limits_t extremaProgress = { numeric_limits<qreal>::max(), numeric_limits<qreal>::lowest() };

    existingExtensions = QSet<QString>();
    QSet<QString> nonRealExtensions;

    for(const trkseg_t &seg : trk.segs)
    {
        for(const trkpt_t &pt : seg.pts)
        {
            if(pt.isHidden())
            {
                continue;
            }

            existingExtensions.unite(pt.extensions.keys().toSet());

            for(const QString &key : pt.extensions.keys())
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

            updateExtrema(extremaSpeed,    pt.speed);
            updateExtrema(extremaEle,      pt.ele);
            updateExtrema(extremaSlope,    pt.slope1);
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
    resetMouseRange();

    delete dlgDetails;
}

void CGisItemTrk::deriveSecondaryData()
{
    qreal north = -90;
    qreal east  = -180;
    qreal south =  90;
    qreal west  =  180;

    // reset all secondary data
    cntTotalPoints            = 0;
    cntVisiblePoints          = 0;
    timeStart                 = QDateTime();
    timeEnd                   = QDateTime();
    totalDistance             = NOFLOAT;
    totalAscend               = NOFLOAT;
    totalDescend              = NOFLOAT;
    totalElapsedSeconds       = NOTIME;
    totalElapsedSecondsMoving = NOTIME;


    // remove empty segments
    QVector<trkseg_t>::iterator it = trk.segs.begin();
    while(it != trk.segs.end())
    {
        if(it->pts.isEmpty())
        {
            it = trk.segs.erase(it);
        }
        else
        {
            ++it;
        }
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
            if(trkpt.isHidden())
            {
                trkpt.reset();
                continue;
            }
            trkpt.idxVisible = cntVisiblePoints++;
            lintrk << &trkpt;

            west  = qMin(west,  trkpt.lon);
            east  = qMax(east,  trkpt.lon);
            south = qMin(south, trkpt.lat);
            north = qMax(north, trkpt.lat);

            if(lastTrkpt != nullptr)
            {
                trkpt.deltaDistance  = GPS_Math_Distance(lastTrkpt->lon * DEG_TO_RAD, lastTrkpt->lat * DEG_TO_RAD, trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
                trkpt.distance       = lastTrkpt->distance + trkpt.deltaDistance;
                trkpt.elapsedSeconds = trkpt.time.toMSecsSinceEpoch()/1000.0 - timestampStart;

                // ascend descend
                if(lastEle != NOFLOAT)
                {
                    qreal delta   = trkpt.ele - lastEle;

                    trkpt.ascend  = lastTrkpt->ascend;
                    trkpt.descend = lastTrkpt->descend;

                    if(qAbs(delta) > ASCEND_THRESHOLD)
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
                timeStart      = trkpt.time;
                timestampStart = timeStart.toMSecsSinceEpoch()/1000.0;
                lastEle        = trkpt.ele;

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

    updateVisuals(eVisualPlot|eVisualDetails|eVisualProject|eVisualColorAct, "deriveSecondaryData()");

//    qDebug() << "--------------" << getName() << "------------------";
//    qDebug() << "totalDistance" << totalDistance;
//    qDebug() << "totalAscend" << totalAscend;
//    qDebug() << "totalDescend" << totalDescend;
//    qDebug() << "totalElapsedSeconds" << totalElapsedSeconds;
//    qDebug() << "totalElapsedSecondsMoving" << totalElapsedSecondsMoving;
}


void CGisItemTrk::findWaypointsCloseBy(CProgressDialog& progress, quint32& current)
{
    IGisProject * project = getParentProject();
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
            if(pt.isHidden())
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
        if(wpt == nullptr)
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

    for(const trkwpt_t &trkwpt : trkwpts)
    {
        qreal minD   = WPT_FOCUS_DIST_IN;
        qint32 index = NOIDX;

        for(const pointDP &pt : line)
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

    return GPS_Math_DistPointPolyline(lineSimple, pos) < 20;
}

bool CGisItemTrk::isWithin(const QRectF& area, selflags_t flags)
{
    return (flags & eSelectionTrk) ? IGisItem::isWithin(area, flags, lineSimple) : false;
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
        dlgDetails = new CDetailsTrk(*this, nullptr);
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
            QString name = getName() + QString(" (%1 - %2)").arg(0).arg(idxMouse);
            IGisProject *project = nullptr;
            if(!getNameAndProject(name, project))
            {
                return false;
            }

            new CGisItemTrk(name, 0, idxMouse, trk, project);
        }

        // clone second part?
        if((mode & (CCutTrk::eModeKeepBoth|CCutTrk::eModeKeepSecond)) != 0)
        {
            QString name = getName() + QString(" (%1 - %2)").arg(idxMouse).arg(cntTotalPoints-1);
            IGisProject *project = nullptr;
            if(!getNameAndProject(name, project))
            {
                return false;
            }

            new CGisItemTrk(name, idxMouse, cntTotalPoints-1, trk, project);
        }
    }
    else
    {
        // if the result is not a clone, the track's list of segments and trackpoints
        // has to be reduced. This is done by copying points into a new trackpoint list
        // that replaces the original one.
        int removeStart = ((mode & CCutTrk::eModeKeepFirst) != 0) ? idxMouse + 1   : 0;
        int removeEnd   = ((mode & CCutTrk::eModeKeepFirst) != 0) ? cntTotalPoints : idxMouse - 1;

        for(int i = 0; i < trk.segs.size(); ++i)
        {
            trkseg_t& seg = trk.segs[i];

            if(seg.pts.empty())
            {
                continue;
            }

            if(removeStart <= seg.pts.first().idxTotal && seg.pts.last().idxTotal <= removeEnd)
            {
                // remove all points for segments, that are completely in [removeStart; removeEnd]
                seg.pts.clear();
            }
            else if( isInRange(removeStart, seg.pts.first().idxTotal, seg.pts.last().idxTotal)
                     || isInRange(removeEnd,   seg.pts.first().idxTotal, seg.pts.last().idxTotal) )
            {
                QVector<trkpt_t> pts;

                for(int n = 0; n < seg.pts.size(); n++)
                {
                    trkpt_t& pt = seg.pts[n];

                    if(!(removeStart <= pt.idxTotal && pt.idxTotal <= removeEnd) )
                    {
                        pts << pt;
                    }
                }

                seg.pts = pts;
            }
            // else: keep any segments, that are not in [removeStart; removeEnd]
        }
        deriveSecondaryData();
        changed(tr("Permanently removed points %1..%2").arg(removeStart).arg(removeEnd), "://icons/48x48/TrkCut.png");
    }

    return askToDeleteOriginal;
}

void CGisItemTrk::reverse()
{
    QString name = getName() + "_rev";
    IGisProject *project = nullptr;
    if(!getNameAndProject(name, project))
    {
        return;
    }

    // start with a 1:1 copy of the first track
    CGisItemTrk * trk1 = new CGisItemTrk(*this, project, NOIDX, false);
    trk1->trk.name = name;
    /*
        clear track data, item key and history. To clear the history is important as
        the original track's history would restore the original key

     */
    trk1->trk.segs.clear();
    trk1->key.clear();
    trk1->history.events.clear();

    for(const trkseg_t &seg : trk.segs)
    {
        trkseg_t seg1;
        for(const trkpt_t &pt : seg.pts)
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

void CGisItemTrk::combine(const QList<IGisItem::key_t>& keys)
{
    if(keys.isEmpty())
    {
        return;
    }

    QString name = getName() + " & other";
    IGisProject *projectNew = nullptr;
    if(!getNameAndProject(name, projectNew))
    {
        return;
    }

    // start with a 1:1 copy of the first track
    CGisItemTrk * trk1 = new CGisItemTrk(*this, projectNew, NOIDX, false);
    // replace name
    trk1->trk.name = name;

    /*
        clear track data, item key and history. To clear the history is important as
        the original track's history would restore the original key

     */
    trk1->trk.segs.clear();
    trk1->key.clear();
    trk1->history.events.clear();

    // copy the segments of all tracks to new track
    CGisWidget& gis = CGisWidget::self();
    for(const IGisItem::key_t &key : keys)
    {
        CGisItemTrk * trk2 = dynamic_cast<CGisItemTrk*>(gis.getItemByKey(key));
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

    // abort if there is no point between idx1 and idx2
    if(idx1 + 1 == idx2)
    {
        return;
    }

    // special case for a single point
    if(idx1 == idx2)
    {
        --idx1;
        ++idx2;
    }

    // iterate over all segments and delete points between idx1 and idx2
    for(int s = 0; s < trk.segs.size(); s++)
    {
        trkseg_t& seg = trk.segs[s];
        for(int i = 0; i < seg.pts.size(); i++)
        {
            trkpt_t& trkpt = seg.pts[i];

            if((idx1 < trkpt.idxTotal) && (trkpt.idxTotal < idx2))
            {
                trkpt.setFlag(trkpt_t::eHidden);
            }
        }
    }
    resetMouseRange();
    deriveSecondaryData();
    if(idx1 + 1 == idx2 - 1)
    {
        changed(tr("Hide point %1.").arg(idx1 + 1), "://icons/48x48/PointHide.png");
    }
    else
    {
        changed(tr("Hide points %1..%2.").arg(idx1 + 1).arg(idx2 - 1), "://icons/48x48/PointHide.png");
    }
}

void CGisItemTrk::showSelectedPoints()
{
    if(!setReadOnlyMode(false))
    {
        return;
    }

    if((mouseRange1 == nullptr) || (mouseRange2 == nullptr))
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
                trkpt.unsetFlag(trkpt_t::eHidden);
            }
        }
    }

    resetMouseRange();
    deriveSecondaryData();
    changed(tr("Show points."), "://icons/48x48/PointShow.png");
}

void CGisItemTrk::copySelectedPoints() const
{
    if((mouseRange1 == nullptr) || (mouseRange2 == nullptr))
    {
        return;
    }

    quint32 idx1 = mouseRange1->idxTotal;
    quint32 idx2 = mouseRange2->idxTotal;

    if(idx1 > idx2)
    {
        qSwap(idx1,idx2);
    }

    QString name = getName() + QString(" (%1 - %2)").arg(idx1).arg(idx2);
    IGisProject *project = nullptr;
    if(!getNameAndProject(name, project))
    {
        return;
    }

    new CGisItemTrk(name, idx1, idx2, trk, project);
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
        for(const trkseg_t &seg : trk.segs)
        {
            for(const trkpt_t &pt : seg.pts)
            {
                if(pt.isHidden())
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
        for(const trkseg_t &seg : trk.segs)
        {
            for(const trkpt_t &pt : seg.pts)
            {
                pt1.setX(pt.lon);
                pt1.setY(pt.lat);
                pt1 *= DEG_TO_RAD;

                lineFull << pt1;

                if(pt.isHidden())
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

        for(const QPolygonF &l : lines)
        {
            p.drawPolyline(l);
        }

        QPixmap bullet("://icons/8x8/bullet_dark_gray.png");
        for(const QPolygonF &l : lines)
        {
            for(const QPointF &pt : l)
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
        for(const QPolygonF &l : lines)
        {
            p.drawPolyline(l);
        }
    }

    p.setBrush(color);
    p.setPen(penBackground);
    for(const QPolygonF &l : lines)
    {
        p.drawPolyline(l);
        if(showArrows.val().toBool())
        {
            CDraw::arrows(l, extViewport, p, 10, 80, lineScale.val().toDouble());
        }
    }

    if(getColorizeSource().isEmpty())
    {
        // use the track's ordinary color
        penForeground.setColor(color);
        p.setPen(penForeground);
        for(const QPolygonF &l : lines)
        {
            p.drawPolyline(l);
        }
    }
    else if(getColorizeSource() == "activity")
    {
        drawColorizedByActivity(p);
    }
    else
    {
        drawColorized(p);
    }
    // -------------------------
}

void CGisItemTrk::setPen(QPainter& p, QPen& pen, quint32 flag) const
{
    flag &= trkpt_t::eActMask;
    pen.setColor((flag == 0) ? color : CActivityTrk::getDescriptor(flag).color);
    p.setPen(pen);
}

void CGisItemTrk::drawColorizedByActivity(QPainter& p) const
{
    QPen pen;
    pen.setWidth(penWidthFg);
    pen.setCapStyle(Qt::RoundCap);

    for(const trkseg_t &segment : trk.segs)
    {
        const trkpt_t *ptPrev = nullptr;

        for(const trkpt_t &pt : segment.pts)
        {
            if(pt.isHidden())
            {
                continue;
            }
            if(nullptr == ptPrev)
            {
                setPen(p, pen, pt.flags);
                ptPrev = &pt;
                continue;
            }

            if((ptPrev->flags & trkpt_t::eActMask) != (pt.flags & trkpt_t::eActMask))
            {
                setPen(p, pen, pt.flags);
            }

            p.drawLine(lineSimple[ptPrev->idxVisible], lineSimple[pt.idxVisible]);
            ptPrev  = &pt;
        }
    }
}

void CGisItemTrk::drawColorized(QPainter &p) const
{
    auto valueFunc = CKnownExtension::get(getColorizeSource()).valueFunc;

    QImage colors(1, 256, QImage::Format_RGB888);
    QPainter colorsPainter(&colors);

    QLinearGradient colorsGradient(colors.rect().topLeft(), colors.rect().bottomLeft());
    colorsGradient.setColorAt(1.00, QColor(  0,   0, 255)); // blue
    colorsGradient.setColorAt(0.60, QColor(  0, 255,   0)); // green
    colorsGradient.setColorAt(0.40, QColor(255, 255,   0)); // yellow
    colorsGradient.setColorAt(0.00, QColor(255,   0,   0)); // red
    colorsPainter.fillRect(colors.rect(), colorsGradient);

    const qreal factor = CKnownExtension::get(getColorizeSource()).factor;

    for(const trkseg_t &segment : trk.segs)
    {
        const trkpt_t *ptPrev = nullptr;
        QColor colorStart;

        for(const trkpt_t &pt : segment.pts)
        {
            if(pt.isHidden())
            {
                continue;
            }
            if(nullptr == ptPrev)
            {
                ptPrev = &pt;
                continue;
            }

            float colorAt = ( factor * valueFunc(pt) - getColorizeLimitLow() ) / (getColorizeLimitHigh() - getColorizeLimitLow());
            colorAt = qMin(qMax(colorAt, 0.f), 1.f);

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

    for(const QString &key : existingExtensions)
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
    if(src != getColorizeSource())
    {
        colorSourceLimit.setSource(src);
        updateHistory(eVisualColorLegend|eVisualDetails);
    }
}

void CGisItemTrk::setColorizeLimitLow(qreal limit)
{
    colorSourceLimit.setMin(limit);
    updateHistory(eVisualColorLegend|eVisualDetails);
}

void CGisItemTrk::setColorizeLimitHigh(qreal limit)
{
    colorSourceLimit.setMax(limit);
    updateHistory(eVisualColorLegend|eVisualDetails);
}

void CGisItemTrk::setColorizeLimits(qreal low, qreal high)
{
    colorSourceLimit.setMin(low);
    colorSourceLimit.setMax(high);
    updateHistory(eVisualColorLegend|eVisualDetails);
}

const QString CGisItemTrk::getColorizeUnit() const
{
    return CKnownExtension::get(getColorizeSource()).unit;
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
        QString val1, unit1, val2, unit2;
        QString str = getInfoTrkPt(*mouseMoveFocus);

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

void CGisItemTrk::drawLabel(QPainter&, const QPolygonF&, QList<QRectF>&, const QFontMetricsF&, CGisDraw*)
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


    // draw the reduced track line
    QList<QPolygonF> lines;
    splitLineToViewport(lineSimple, p.viewport(), lines);

    p.setPen(QPen(QColor(255,0,0,100), penWidthHi, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    for(const QPolygonF &line : lines)
    {
        p.drawPolyline(line);
    }
}

void CGisItemTrk::drawRange(QPainter& p)
{
    QMutexLocker lock(&mutexItems);

    if((mouseRange1 != nullptr) && (mouseRange2 != nullptr))
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
    resetMouseRange();
    mouseFocusOwner = (mode == eModeRange) ? owner : "";


    CCanvas * canvas = CMainWindow::self().getVisibleCanvas();
    if(nullptr != canvas)
    {
        canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
    }

    return true;
}

void CGisItemTrk::setName(const QString& str)
{
    setText(CGisListWks::eColumnName, str);
    trk.name = str;
    changed(tr("Changed name"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setComment(const QString& str)
{
    trk.cmt = str;
    changed(tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setDescription(const QString& str)
{
    trk.desc = str;
    changed(tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setLinks(const QList<link_t>& links)
{
    trk.links = links;
    changed(tr("Changed links"), "://icons/48x48/Link.png");
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
            trkpt.unsetFlag(trkpt_t::eActMask);
            trkpt.setFlag((enum CGisItemTrk::trkpt_t::flag_e) flag);
        }
    }

    deriveSecondaryData();
    changed(tr("Changed activity to '%1' for complete track.").arg(name), icon);
}

void CGisItemTrk::setActivity()
{
    if((mouseRange1 == nullptr) && (mouseRange2 == nullptr))
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
        --idx1;
    }

    // iterate over all segments and set activity flag for points between idx1 and idx2
    for(int s = 0; s < trk.segs.size(); s++)
    {
        trkseg_t& seg = trk.segs[s];
        for(int i = 0; i < seg.pts.size(); i++)
        {
            trkpt_t& trkpt = seg.pts[i];

            if((idx1 < trkpt.idxTotal) && (trkpt.idxTotal <= idx2))
            {
                trkpt.unsetFlag(trkpt_t::eActMask);
                trkpt.setFlag((enum CGisItemTrk::trkpt_t::flag_e) flag);
            }
        }
    }

    resetMouseRange();
    deriveSecondaryData();
    changed(tr("Changed activity to '%1' for range(%2..%3).").arg(name).arg(idx1).arg(idx2), icon);
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

        for(const trkseg_t &seg : trk.segs)
        {
            for(const trkpt_t &pt : seg.pts)
            {
                if(pt.isHidden())
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

        for(const trkseg_t &seg : trk.segs)
        {
            for(const trkpt_t &pt : seg.pts)
            {
                if(pt.isHidden())
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

        idx = getIdxPointCloseBy(pt, line);
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
    const trkpt_t *newPointOfFocus = getTrkPtByTotalIndex(idx);
    if(nullptr != newPointOfFocus)
    {
        return publishMouseFocus(newPointOfFocus, fmode, owner);
    }

    return false;
}

const CGisItemTrk::trkpt_t* CGisItemTrk::getTrkPtByCondition(std::function<bool(const trkpt_t&)> cond) const
{
    for(const trkseg_t &seg : trk.segs)
    {
        for(const trkpt_t &pt : seg.pts)
        {
            if(cond(pt))
            {
                return &pt;
            }
        }
    }
    return nullptr;
}

const CGisItemTrk::trkpt_t * CGisItemTrk::getTrkPtByVisibleIndex(qint32 idx) const
{
    if(idx == NOIDX)
    {
        return nullptr;
    }

    auto condition = [idx](const trkpt_t &pt) { return pt.idxVisible == idx;  };
    return getTrkPtByCondition(condition);
}

const CGisItemTrk::trkpt_t * CGisItemTrk::getTrkPtByTotalIndex(qint32 idx) const
{
    auto condition = [idx](const trkpt_t &pt) { return pt.idxTotal == idx;  };
    return getTrkPtByCondition(condition);
}

bool CGisItemTrk::isTrkPtLastVisible(qint32 idxTotal) const
{
    auto condition = [idxTotal](const trkpt_t &pt) { return (pt.idxTotal > idxTotal) && !pt.isHidden();  };
    return nullptr == getTrkPtByCondition(condition);
}

bool CGisItemTrk::isTrkPtFirstVisible(qint32 idxTotal) const
{
    for(const trkseg_t &seg : trk.segs)
    {
        for(const trkpt_t &pt : seg.pts)
        {
            if((pt.idxTotal < idxTotal))
            {
                if(!pt.isHidden())
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

void CGisItemTrk::resetMouseRange()
{
    mouseRange1 = nullptr;
    mouseRange2 = nullptr;
    rangeState  = eRangeStateIdle;
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
        if((fmode == eFocusMouseClick) && (pt != nullptr))
        {
            mouseRange1 = pt;
            rangeState  = eRangeState1st;
        }

        break;
    }

    case eRangeState1st:
    {
        mouseRange2 = pt;
        if((fmode == eFocusMouseClick) && (pt != nullptr))
        {
            rangeState  = eRangeState2nd;
        }
        break;
    }

    case eRangeState2nd:
    {
        if(fmode == eFocusMouseClick)
        {
            resetMouseRange();
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

    for(INotifyTrk * visual : registeredVisuals)
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

    for(INotifyTrk * visual : registeredVisuals)
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

    for(INotifyTrk * visual : registeredVisuals)
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

    for(INotifyTrk * visual : registeredVisuals)
    {
        visual->setMouseClickFocus(pt);
    }
}
