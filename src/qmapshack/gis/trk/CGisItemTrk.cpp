/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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

#include "gis/trk/CGisItemTrk.h"

#include <QtWidgets>
#include <QtXml>

#include "CMainWindow.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/GeoMath.h"
#include "gis/prj/IGisProject.h"
#include "gis/proj_x.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CCutTrk.h"
#include "gis/trk/CDetailsTrk.h"
#include "gis/trk/CInvalidTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/trk/CPropertyTrk.h"
#include "gis/trk/CScrOptTrk.h"
#include "gis/trk/CTrkToRteDialog.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "helpers/CProgressDialog.h"
#include "misc.h"

#define DEFAULT_COLOR 4
#define MIN_DIST_CLOSE_TO 10
#define MIN_DIST_FOCUS 200

#define WPT_FOCUS_DIST_IN (50 * 50)
#define WPT_FOCUS_DIST_OUT (200 * 200)

namespace {
// helper to declutter and draw clusters of track info points
class cluster {
 public:
  cluster(int number, const QRect& r) : box(r) { elements.append({number}); }
  int size() const { return elements.size(); }

  static void addToCluster(const QRect& r, QList<cluster>& clusters, int number) {
    for (cluster& cluster : clusters) {
      if (cluster.box.intersects(r)) {
        cluster.box &= r;
        cluster.elements.append(number);
        return;
      }
    }
    clusters.append({number, r});
  }

  static void draw(const QList<cluster>& clusters, QPainter& p, int size) {
    for (const cluster& cluster : clusters) {
      const int N = cluster.size();
      if (N >= MAX_CONST_SIZE) {
        continue;
      }

      const QPointF* constellation = constellations[N];
      for (int n = 0; n < N; n++) {
        CDraw::number(cluster.elements[n], size, p, cluster.box.center() + constellation[n] * size, Qt::black);
      }
    }
  }

 private:
  QRect box;
  QList<int> elements;

  static const QPointF point1[];
  static const QPointF point2[];
  static const QPointF point3[];
  static const QPointF point4[];
  static const QPointF point5[];
  static const QPointF point6[];
  static const QPointF point7[];
  static const QPointF point8[];
  static const QPointF point9[];

  static constexpr int MAX_CONST_SIZE = 9;
  static const QPointF* constellations[MAX_CONST_SIZE];
};

const QPointF cluster::point1[] = {{0, 0}};

const QPointF cluster::point2[] = {{-0.5, 0}, {0.5, 0}};

const QPointF cluster::point3[] = {{-1, 0}, {0, 0}, {1, 0}};

const QPointF cluster::point4[] = {{-1, -0.5}, {0, -0.5}, {1, -0.5}, {-1, 0.5}};

const QPointF cluster::point5[] = {{-1, -0.5}, {0, -0.5}, {1, -0.5}, {-1, 0.5}, {0, 0.5}};

const QPointF cluster::point6[] = {{-1, -0.5}, {0, -0.5}, {1, -0.5}, {-1, 0.5}, {0, 0.5}, {1, 0.5}};

const QPointF cluster::point7[] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}, {-1, 1}};

const QPointF cluster::point8[] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {0, 1}};

const QPointF cluster::point9[] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};

const QPointF* cluster::constellations[MAX_CONST_SIZE] = {point1, point2, point3, point4, point5,
                                                          point6, point7, point8, point9};

struct trkwpt_t {
  QString name;
  qreal x = 0;
  qreal y = 0;
  IGisItem::key_t key;
};
}  // namespace

IGisItem::key_t CGisItemTrk::keyUserFocus;

CGisItemTrk::CGisItemTrk(const QString& name, qint32 idx1, qint32 idx2, const CTrackData& srctrk, IGisProject* project)
    : IGisItem(project, eTypeTrk, NOIDX), trk(name, srctrk, idx1, idx2) {
  flags = eFlagCreatedInQms;

  deriveSecondaryData();
  setupHistory();
  updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemTrk::CGisItemTrk(const CGisItemTrk& parentTrk, IGisProject* project, int idx, bool clone)
    : IGisItem(project, eTypeTrk, idx) {
  // copy track via history
  history = parentTrk.history;
  loadHistory(history.histIdxCurrent);

  // if track should be a clone clear history and key and
  // build new ones.
  if (clone) {
    trk.name += tr("_Clone");
    key.clear();
    history.events.clear();
    setupHistory();
  }

  if (parentTrk.isOnDevice() || !parentTrk.isReadOnly()) {
    flags |= eFlagWriteAllowed;
  } else {
    flags &= ~eFlagWriteAllowed;
  }

  setNogoFlag(parentTrk.isNogo());

  deriveSecondaryData();
  updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemTrk::CGisItemTrk(const SGisLine& l, const QString& name, IGisProject* project, int idx)
    : IGisItem(project, eTypeTrk, idx) {
  trk.name = name;
  readTrackDataFromGisLine(l);

  flags |= eFlagCreatedInQms | eFlagWriteAllowed;
  setColor(str2color(""));

  setupHistory();
  updateDecoration(eMarkChanged, eMarkNone);
}

CGisItemTrk::CGisItemTrk(const QDomNode& xml, IGisProject* project)
    : IGisItem(project, eTypeTrk, project->childCount()) {
  // --- start read and process data ----
  setColor(penForeground.color());
  readTrk(xml, trk);
  // --- stop read and process data ----

  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);

  checkForInvalidPoints();
}

CGisItemTrk::CGisItemTrk(const QString& filename, IGisProject* project)
    : IGisItem(project, eTypeTrk, project->childCount()) {
  // --- start read and process data ----
  setColor(penForeground.color());
  if (!readTwoNav(filename)) {
    throw -1;
  }
  // --- stop read and process data ----

  setupHistory();
  updateDecoration(eMarkNone, eMarkNone);

  checkForInvalidPoints();
}

CGisItemTrk::CGisItemTrk(const history_t& hist, const QString& dbHash, IGisProject* project)
    : IGisItem(project, eTypeTrk, project->childCount()) {
  history = hist;
  loadHistory(hist.histIdxCurrent);
  if (!dbHash.isEmpty()) {
    lastDatabaseHash = dbHash;
  }
}

CGisItemTrk::CGisItemTrk(quint64 id, QSqlDatabase& db, IGisProject* project) : IGisItem(project, eTypeTrk, NOIDX) {
  loadFromDb(id, db);
}

CGisItemTrk::CGisItemTrk(CTrackData& trkdata, IGisProject* project)
    : IGisItem(project, eTypeTrk, NOIDX), trk(std::move(trkdata)) {
  setupHistory();
  deriveSecondaryData();
  updateDecoration(eMarkNone, eMarkNone);

  checkForInvalidPoints();
}

CGisItemTrk::CGisItemTrk(CFitStream& stream, IGisProject* project)
    : IGisItem(project, eTypeTrk, project->childCount()) {
  // --- start read and process data ----
  setColor(penForeground.color());
  readTrkFromFit(stream);
  // --- stop read and process data ----

  setupHistory();
  deriveSecondaryData();
  updateDecoration(eMarkNone, eMarkNone);

  checkForInvalidPoints();
}

CGisItemTrk::~CGisItemTrk() {
  // reset user focus if focused on this track
  if (key == keyUserFocus) {
    keyUserFocus.clear();
  }

  /*
      Delete all registered INotifyTrk as they can't exist without the item.
      As the INotifyTrk objects will unregister via unregisterVisual() in their
      destructor things will get a bit complicated here. Additionally visuals
      can be parents of other visuals. Therefor destroying one might destroy
      others. To cover that we destroy the first object in registeredVisuals
      until there is none left
   */
  while (registeredVisuals.size()) {
    delete *registeredVisuals.begin();
  }

  // now it is save to destroy the details dialog
  delete dlgDetails;

  // delete it after the detail dialog as it is used by the detail dialog
  delete propHandler;
}

template <typename T>
static inline bool isInRange(const T& val, const T& rangeStart, const T& rangeEnd) {
  return (rangeStart <= val) && (val <= rangeEnd);
}

IGisItem* CGisItemTrk::createClone() {
  int idx = -1;
  IGisProject* project = getParentProject();
  if (project) {
    idx = project->indexOfChild(this);
  }
  return new CGisItemTrk(*this, project, idx, true);
}

void CGisItemTrk::updateFromDB(quint64 id, QSqlDatabase& db) {
  IGisItem::updateFromDB(id, db);

  /*
      as this will change the line significantly we better stop
      all focus operations and close the detail dialog.
   */
  resetInternalData();
}

void CGisItemTrk::setSymbol() { setColor(str2color(trk.color)); }

void CGisItemTrk::setDataFromPolyline(const SGisLine& l) {
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

void CGisItemTrk::getPolylineFromData(QPolygonF& l) const {
  QMutexLocker lock(&mutexItems);
  trk.getPolyline(l);
}

void CGisItemTrk::getPolylineFromData(SGisLine& l) const {
  QMutexLocker lock(&mutexItems);
  trk.getPolyline(l);
}

void CGisItemTrk::getPolylineRangeFromData(SGisLine& l, qint32 rangeStart, qint32 rangeEnd, bool getSubPixel) const {
  QMutexLocker lock(&mutexItems);
  trk.getPolylineRange(l, rangeStart, rangeEnd, getSubPixel);
}

void CGisItemTrk::getPolylineDegFromData(QPolygonF& l) const {
  QMutexLocker lock(&mutexItems);
  trk.getPolylineDeg(l);
}

void CGisItemTrk::readTrackDataFromGisLine(const SGisLine& l) {
  QMutexLocker lock(&mutexItems);
  trk.readFrom(l);
  deriveSecondaryData();
}

void CGisItemTrk::registerVisual(INotifyTrk* visual) { registeredVisuals << visual; }

void CGisItemTrk::unregisterVisual(INotifyTrk* visual) { registeredVisuals.remove(visual); }

static void addRowLimit(QString& str, const QString& name, const QString& min, const QString& max) {
  str += "<tr>";
  str += "<td align='left'>" + name + "</td>";
  str += "<td align='right'>" + min + "</td>";
  str += "<td align='right'>" + max + "</td>";
  str += "</tr>";
}

QString CGisItemTrk::getInfoLimits() const {
  QString str = "<table width='100%'>";

  str += "<tr><th align='left'></th><th align='right'>" + tr("min.") + "</th><th align='right'>" + tr("max.") +
         "</th></tr>";

  QStringList keys = extrema.keys();
  std::sort(keys.begin(), keys.end(), sortByString);

  for (const QString& key : std::as_const(keys)) {
    if (key == CKnownExtension::internalSpeedTime)  // Output of "Speed*" will already be covered by internalSpeedDist
    {                                               // No need to show it twice
      continue;
    }

    const CKnownExtension& ext = CKnownExtension::get(key);
    const limits_t& limit = extrema[key];

    const QString& labelMin = ext.toString(limit.min, false, key);
    const QString& labelMax = ext.toString(limit.max, false, key);

    if (!labelMin.isEmpty() && !labelMax.isEmpty()) {
      addRowLimit(str, ext.getName(key), labelMin, labelMax);
    }
  }

  str += "</table>";
  return str;
}

QString CGisItemTrk::getInfo(quint32 feature) const {
  QString val1, unit1, val2, unit2;

  if (cntVisiblePoints == 0) {
    return feature ? QString("<div><b>%1</b></div>").arg(getName()) : QString("<div></div>");
  }

  bool timeIsValid = (allValidFlags & CTrackData::trkpt_t::eInvalidTime) == 0;
  bool eleIsValid = (allValidFlags & CTrackData::trkpt_t::eInvalidEle) == 0;

  QString str = "<div>";

  qint32 actCnt = activities.getActivityCount();
  const QList<trkact_t>& acts = activities.getAllActivities().values();

  if (feature & eFeatureShowName) {
    if ((actCnt == 1) && (acts.first() != CTrackData::trkpt_t::eAct20None)) {
      const CActivityTrk::desc_t& desc = activities.getDescriptor(acts.first());
      str += QString("<img src='%1'/>&nbsp;").arg(desc.iconSmall);
    }
    str += "<b>" + getName() + "</b>";
  }

  if ((feature & eFeatureShowActivity) && (actCnt > 1)) {
    str += "<br/>";
    activities.printSummary(str);
    str += "<br/>";
  } else {
    str += "<br/>";
    IUnit::self().meter2distance(totalDistance, val1, unit1);
    str += tr("Length: %1%2").arg(val1, unit1);

    if (eleIsValid && totalAscent != NOFLOAT && totalDescent != NOFLOAT) {
      IUnit::self().meter2elevation(totalAscent, val1, unit1);
      IUnit::self().meter2elevation(totalDescent, val2, unit2);

      str += tr(", %1%2%3, %4%5%6").arg(QChar(0x2197)).arg(val1, unit1).arg(QChar(0x2198)).arg(val2, unit2);
    } else {
      str += tr(", %1-, %2-").arg(QChar(0x2197)).arg(QChar(0x2198));
    }
    str += "<br />";

    if (timeIsValid && (totalElapsedSeconds != NOTIME)) {
      IUnit::self().seconds2time(totalElapsedSeconds, val1, unit1);
      IUnit::self().meter2speed(totalDistance / totalElapsedSeconds, val2, unit2);
      str += tr("Time: %1%2, Speed: %3%4").arg(val1, unit1, val2, unit2);
    } else {
      str += tr("Time: -, Speed: -");
    }
    str += "<br />";

    if (timeIsValid && (totalElapsedSecondsMoving != NOTIME)) {
      IUnit::self().seconds2time(totalElapsedSecondsMoving, val1, unit1);
      IUnit::self().meter2speed(totalDistance / totalElapsedSecondsMoving, val2, unit2);
      str += tr("Moving: %1%2, Speed: %3%4").arg(val1, unit1, val2, unit2);
    } else {
      str += tr("Moving: -, Speed: -");
    }
    str += "<br />";
  }

  str += "<br/>";

  if (timeIsValid && timeStart.isValid()) {
    str += tr("Start: %1").arg(IUnit::datetime2string(timeStart, IUnit::eTimeFormatLong, boundingRect.center()));
  } else {
    str += tr("Start: -");
  }
  str += "<br />";

  if (timeIsValid && timeEnd.isValid()) {
    str += tr("End: %1").arg(IUnit::datetime2string(timeEnd, IUnit::eTimeFormatLong, boundingRect.center()));
  } else {
    str += tr("End: -");
  }
  str += "<br />";

  str += tr("Points: %1 (%2)").arg(cntVisiblePoints).arg(cntTotalPoints) + "<br />";

  qreal energyUseCycling = energyCycling.getEnergyUseCycling();
  if (energyCycling.isValid() && energyUseCycling != NOFLOAT) {
    str += tr("Energy Use Cycling: %L1").arg(energyUseCycling, 0, 'f', 0) + "kcal<br/>";
  }

  if ((allValidFlags & (CTrackData::trkpt_t::eValidEle | CTrackData::trkpt_t::eInvalidEle)) ==
      (CTrackData::trkpt_t::eValidEle | CTrackData::trkpt_t::eInvalidEle)) {
    str += "<b style='color: red;'>" + tr("Invalid elevations!") + "</b><br/>";
  }

  if ((allValidFlags & (CTrackData::trkpt_t::eValidTime | CTrackData::trkpt_t::eInvalidTime)) ==
      (CTrackData::trkpt_t::eValidTime | CTrackData::trkpt_t::eInvalidTime)) {
    str += "<b style='color: red;'>" + tr("Invalid timestamps!") + "</b><br/>";
  }

  if ((allValidFlags & (CTrackData::trkpt_t::eValidPos | CTrackData::trkpt_t::eInvalidPos)) ==
      (CTrackData::trkpt_t::eValidPos | CTrackData::trkpt_t::eInvalidPos)) {
    str += "<b style='color: red;'>" + tr("Invalid positions!") + "</b><br/>";
  }

  if ((allValidFlags & (CTrackData::trkpt_t::eValidSlope | CTrackData::trkpt_t::eInvalidSlope)) ==
      (CTrackData::trkpt_t::eValidSlope | CTrackData::trkpt_t::eInvalidSlope)) {
    str += "<b style='color: red;'>" + tr("Invalid slopes!") + "</b><br/>";
  }

  if (feature & eFeatureShowFullText) {
    QStringList actNames;
    activities.getActivityNames(actNames);
    if (!actNames.isEmpty()) {
      str += "</div><div>" + tr("Activities: %1").arg(actNames.join(", "));
    }

    QString desc = removeHtml(trk.desc).simplified();
    if (desc.count()) {
      if (!str.isEmpty()) {
        str += "<br/>\n";
      }
      str += desc;
    }

    QString cmt = removeHtml(trk.cmt).simplified();
    if ((cmt != desc) && cmt.count()) {
      if (!str.isEmpty()) {
        str += "<br/>\n";
      }
      str += cmt;
    }
  }

  if ((feature & eFeatureShowLinks) && !trk.links.isEmpty()) {
    for (const link_t& link : trk.links) {
      if (link.type.isEmpty() || (link.type == "text/html")) {
        str += "<br/>\n";
        str += QString("<a href='%1'>%2</a>").arg(link.uri.toString(), link.text);
      }
    }
  }

  str += getRatingKeywordInfo();

  return str + "</div>";
}

QString CGisItemTrk::getInfoRange() const {
  qreal tmp, slope1;
  QString str, val, unit;
  if ((mouseRange1 == nullptr) || (mouseRange2 == nullptr) || (mouseRange1 == mouseRange2)) {
    return str;
  }

  const CTrackData::trkpt_t* pt1 = mouseRange1;
  const CTrackData::trkpt_t* pt2 = mouseRange2;
  if (mouseRange1->idxTotal >= mouseRange2->idxTotal) {
    pt1 = mouseRange2;
    pt2 = mouseRange1;
  }

  for (; pt1->isHidden() && (pt1->idxTotal < (cntTotalPoints - 1)); ++pt1) {
  }
  for (; pt2->isHidden() && (pt2->idxTotal > 0); --pt2) {
  }

  bool timeIsValid = pt1->time.isValid() && pt2->time.isValid();
  qreal deltaTime = pt2->time.toSecsSinceEpoch() - pt1->time.toSecsSinceEpoch();

  const qreal distance = pt2->distance - pt1->distance;

  IUnit::self().meter2distance(distance, val, unit);
  str += QString("%3 %1%2 ").arg(val, unit).arg(QChar(0x21A6));
  if (timeIsValid) {
    quint32 t = pt2->time.toSecsSinceEpoch() - pt1->time.toSecsSinceEpoch();
    quint32 hh = t / 3600;
    quint32 mm = (t % 3600) / 60;
    quint32 ss = t % 60;

    str += QString("%4 %1:%2:%3")
               .arg(hh, 2, 10, QChar('0'))
               .arg(mm, 2, 10, QChar('0'))
               .arg(ss, 2, 10, QChar('0'))
               .arg(QChar(0x231a));

    IUnit::self().meter2speed(distance / deltaTime, val, unit);
    str += QString(", %3 %1%2").arg(val, unit).arg(QChar(0x21A3));
  }
  str += "\n";

  qreal deltaAscent = pt2->ascent - pt1->ascent;
  qreal deltaDescent = pt2->descent - pt1->descent;

  tmp = qAtan(deltaAscent / distance);
  slope1 = qAbs(tmp * 360.0 / (2 * M_PI));
  QString val2, unit2;
  IUnit::self().slope2string(slope1, val2, unit2);

  IUnit::self().meter2elevation(deltaAscent, val, unit);
  str += QString("%3 %1%2 (%4%5)").arg(val, unit).arg(QChar(0x2197)).arg(val2, unit2);
  if (timeIsValid) {
    IUnit::self().meter2speed(deltaAscent / deltaTime, val, unit);
    str += QString(", %1%2").arg(val, unit);
  }
  str += "\n";

  tmp = qAtan(deltaDescent / distance);
  slope1 = qAbs(tmp * 360.0 / (2 * M_PI));
  IUnit::self().slope2string(slope1, val2, unit2);

  IUnit::self().meter2elevation(deltaDescent, val, unit);
  str += QString("%3 %1%2 (%4%5)").arg(val, unit).arg(QChar(0x2198)).arg(val2, unit2);
  if (timeIsValid) {
    IUnit::self().meter2speed(deltaDescent / deltaTime, val, unit);
    str += QString(", %1%2").arg(val, unit);
  }

  return str + "\n";
}

QString CGisItemTrk::getInfoTrkPt(const CTrackData::trkpt_t& pt) const {
  QString str, val1, unit1;

  if (pt.idxTotal == pt.idxVisible) {
    str += tr("Index: %1").arg(pt.idxVisible);
  } else {
    str += tr("Index: visible %1, total %2").arg(pt.idxVisible).arg(pt.idxTotal);
  }
  str += "\n";

  if (totalElapsedSeconds != 0) {
    str += IUnit::datetime2string(pt.time, IUnit::eTimeFormatLong, QPointF(pt.lon, pt.lat) * DEG_TO_RAD);
    str += "\n";
  }

  IUnit::self().meter2elevation(pt.ele, val1, unit1);
  str += tr("Ele.: %1%2").arg(val1, unit1);
  if (pt.slope1 != NOFLOAT) {
    IUnit::self().slope2string(pt.slope1, val1, unit1);
    str += tr(", Slope: %1%2").arg(val1, unit1);
  }
  if (pt.speed != NOFLOAT) {
    IUnit::self().meter2speed(pt.speed, val1, unit1);
    str += tr(", Speed: %1%2").arg(val1, unit1);
  }

  QStringList keys = pt.extensions.keys();
  keys.sort();

  qint32 more = keys.size() - 10;
  if (more > 0) {
    keys = keys.mid(0, 10);
  }

  for (const QString& key : std::as_const(keys)) {
    const CKnownExtension& ext = CKnownExtension::get(key);
    if (ext.known) {
      str +=
          "\n" + ext.nameLongText + ": " + QString("%1%2").arg(ext.valueFunc(pt) * ext.factor, 0, 'f', 1).arg(ext.unit);
    } else {
      QStringList tags = key.split("|");
      str += "\n" + tags.last() + ": " + pt.extensions[key].toString();
    }
  }

  if (more > 0) {
    str += "\n" + tr("... and %1 tags not displayed").arg(more);
  }

  return str;
}

QString CGisItemTrk::getInfoProgress(const CTrackData::trkpt_t& pt) const {
  QString val, unit;

  QString asc = tr("Ascent: - (-)");
  QString dsc = tr("Descent: - (-)");
  QString dst = tr("Distance: - (-)");
  QString mov = tr("Moving: - (-)");

  if (pt.ascent != NOFLOAT) {
    IUnit::self().meter2elevation(pt.ascent, val, unit);
    asc = tr("Ascent: %1%2 (%3%)").arg(val, unit).arg(pt.ascent * 100 / totalAscent, 2, 'f', 0);
  }

  if (pt.descent != NOFLOAT) {
    IUnit::self().meter2elevation(pt.descent, val, unit);
    dsc = tr(", Descent: %1%2 (%3%)").arg(val, unit).arg(pt.descent * 100 / totalDescent, 2, 'f', 0);
  }

  if (pt.distance != NOFLOAT) {
    IUnit::self().meter2distance(pt.distance, val, unit);
    dst = tr("Distance: %1%2 (%3%)").arg(val, unit).arg(pt.distance * 100 / totalDistance, 2, 'f', 0);
  }

  if (pt.elapsedSeconds != NOFLOAT) {
    IUnit::self().seconds2time(pt.elapsedSecondsMoving, val, unit);
    mov = tr(", Moving: %1%2 (%3%)")
              .arg(val, unit)
              .arg(pt.elapsedSecondsMoving * 100 / totalElapsedSecondsMoving, 2, 'f', 0);
  }

  return QString("%1%2\n%3%4").arg(asc, dsc, dst, mov);
}

QString CGisItemTrk::getInfoRange(const CTrackData::trkpt_t& trkpt1, const CTrackData::trkpt_t& trkpt2) const {
  QString val, unit;
  qreal dt = NOFLOAT;

  CTrackData::trkpt_t pt1 = trkpt1;
  CTrackData::trkpt_t pt2 = trkpt2;
  if (pt1.idxTotal > pt2.idxTotal) {
    qSwap(pt1, pt2);
  }

  if (pt1.time.isValid() && pt2.time.isValid()) {
    dt = pt2.time.toSecsSinceEpoch() - pt1.time.toSecsSinceEpoch();
  }

  QString asc = tr("Ascent: -");
  QString dsc = tr("Descent: -");

  if ((pt1.ascent != NOFLOAT) && (pt2.ascent != NOFLOAT)) {
    IUnit::self().meter2elevation(pt2.ascent - pt1.ascent, val, unit);
    asc = tr("Ascent: %1%2").arg(val, unit);

    if (dt != NOFLOAT) {
      IUnit::self().meter2speed((pt2.ascent - pt1.ascent) / dt, val, unit);
      asc += tr(", %1%2").arg(val, unit);
    }
  }

  if ((pt1.descent != NOFLOAT) && (pt2.descent != NOFLOAT)) {
    IUnit::self().meter2elevation(pt2.descent - pt1.descent, val, unit);
    dsc = tr(", Descent: %1%2").arg(val, unit);

    if (dt != NOFLOAT) {
      IUnit::self().meter2speed((pt2.descent - pt1.descent) / dt, val, unit);
      dsc += tr(", %1%2").arg(val, unit);
    }
  }

  IUnit::self().meter2distance(pt2.distance - pt1.distance, val, unit);
  QString dsttme = tr("Distance: %1%2").arg(val, unit);

  if (dt != NOFLOAT) {
    IUnit::self().seconds2time(dt, val, unit);
    dsttme += tr(", Time: %1%2").arg(val, unit);
  }

  return QString("%1%2\n%3").arg(asc, dsc, dsttme);
}

qint32 CGisItemTrk::getElevation(qint32 idx) const {
  const CTrackData::trkpt_t* trkpt = trk.getTrkPtByTotalIndex(idx);
  return trkpt != nullptr ? trkpt->ele : NOINT;
}

IScrOpt* CGisItemTrk::getScreenOptions(const QPoint& origin, IMouse* mouse) {
  if (scrOpt.isNull()) {
    scrOpt = new CScrOptTrk(this, origin, mouse);
  }
  return scrOpt;
}

static qint32 getIdxPointCloseBy(const QPoint& pos, const QPolygonF& line) {
  qint32 idx = 0;
  qint32 bestIdx = NOIDX;
  qint32 bestDst = NOINT;
  for (const QPointF& pt : line) {
    int dst = (pos - pt).manhattanLength();
    if (dst < bestDst) {
      bestIdx = idx;
      bestDst = dst;
    }
    ++idx;
  }

  return bestIdx;
}

QPointF CGisItemTrk::getPointCloseBy(const QPoint& screenPos) {
  QMutexLocker lock(&mutexItems);

  qint32 bestIdx = getIdxPointCloseBy(screenPos, lineSimple);
  return (NOIDX == bestIdx) ? NOPOINTF : lineSimple[bestIdx];
}

bool CGisItemTrk::isRangeSelected() const { return mouseRange1 != mouseRange2; }

static inline void updateExtrema(CGisItemTrk::limits_t& extrema, qreal val, const QPointF& pos) {
  if (NOFLOAT != val) {
    extrema.setMin(val, pos);
    extrema.setMax(val, pos);
  }
}

void CGisItemTrk::updateExtremaAndExtensions() {
  extrema = QHash<QString, limits_t>();
  limits_t extremaSpeed;
  limits_t extremaSlope;
  limits_t extremaEle;
  limits_t extremaProgress;

  existingExtensions = QSet<QString>();
  QSet<QString> nonRealExtensions;

  for (const CTrackData::trkpt_t& pt : trk) {
    if (pt.isHidden()) {
      continue;
    }

    const QStringList& keys = pt.extensions.keys();
    existingExtensions.unite({keys.begin(), keys.end()});

    const QPointF& pos = {pt.lon, pt.lat};
    for (const QString& key : keys) {
      bool isReal = false;
      qreal val = pt.extensions.value(key).toReal(&isReal);

      if (isReal) {
        if (!extrema.contains(key)) {
          extrema[key] = limits_t();
        }
        updateExtrema(extrema[key], val, pos);
      } else {
        nonRealExtensions << key;
      }
    }

    updateExtrema(extremaSpeed, pt.speed, pos);
    updateExtrema(extremaEle, pt.ele, pos);
    updateExtrema(extremaSlope, pt.slope1, pos);
    updateExtrema(extremaProgress, pt.distance, pos);
  }

  if (extremaEle.min < extremaEle.max) {
    existingExtensions << CKnownExtension::internalEle;
    extrema[CKnownExtension::internalEle] = extremaEle;
  }

  if (extremaSlope.min < extremaSlope.max) {
    existingExtensions << CKnownExtension::internalSlope;
    extrema[CKnownExtension::internalSlope] = extremaSlope;
  }

  if (numeric_limits<qreal>::max() != extremaSpeed.min) {
    existingExtensions << CKnownExtension::internalSpeedDist;
    existingExtensions << CKnownExtension::internalSpeedTime;
    extrema[CKnownExtension::internalSpeedDist] = extremaSpeed;
    extrema[CKnownExtension::internalSpeedTime] = extremaSpeed;
  }

  if (numeric_limits<qreal>::max() != extremaProgress.min) {
    existingExtensions << CKnownExtension::internalProgress;
    extrema[CKnownExtension::internalProgress] = extremaProgress;
  }

  existingExtensions.subtract(nonRealExtensions);
}

void CGisItemTrk::resetInternalData() {
  mouseClickFocus = nullptr;
  mouseMoveFocus = nullptr;
  resetMouseRange();

  delete dlgDetails;
}

void CGisItemTrk::verifyTrkPt(CTrackData::trkpt_t*& last, CTrackData::trkpt_t& trkpt) {
  trkpt.valid =
      (trkpt.ele != NOINT) ? quint32(CTrackData::trkpt_t::eValidEle) : quint32(CTrackData::trkpt_t::eInvalidEle);
  trkpt.valid |= (trkpt.lat < -90) || (trkpt.lat > 90) || (trkpt.lon < -180) || (trkpt.lon > 180)
                     ? quint32(CTrackData::trkpt_t::eInvalidPos)
                     : quint32(CTrackData::trkpt_t::eValidPos);

  if (trkpt.time.isValid()) {
    if (last != nullptr) {
      trkpt.valid |= (trkpt.time.toMSecsSinceEpoch() - last->time.toMSecsSinceEpoch()) < 0
                         ? quint32(CTrackData::trkpt_t::eInvalidTime)
                         : quint32(CTrackData::trkpt_t::eValidTime);
    } else {
      trkpt.valid |= CTrackData::trkpt_t::eValidTime;
    }

    last = &trkpt;
  } else {
    trkpt.valid |= CTrackData::trkpt_t::eInvalidTime;
  }

  trkpt.valid |= (trkpt.slope1 == NOFLOAT) || (trkpt.slope2 == NOFLOAT) ? quint32(CTrackData::trkpt_t::eInvalidSlope)
                                                                        : quint32(CTrackData::trkpt_t::eValidSlope);
}

void CGisItemTrk::consolidatePoints() {
  for (CTrackData::trkseg_t& seg : trk.segs) {
    if (seg.pts.empty()) {
      continue;
    }

    seg.pts.first().unsetFlag(CTrackData::trkpt_t::eFlagSubpt);
    seg.pts.last().unsetFlag(CTrackData::trkpt_t::eFlagSubpt);
  }
}

void CGisItemTrk::deriveSecondaryData() {
  consolidatePoints();

  qreal north = -90;
  qreal east = -180;
  qreal south = 90;
  qreal west = 180;

  // reset all secondary data
  allValidFlags = 0;
  cntInvalidPoints = 0;
  cntTotalPoints = 0;
  cntVisiblePoints = 0;
  timeStart = QDateTime();
  timeEnd = QDateTime();
  totalDistance = NOFLOAT;
  totalAscent = NOFLOAT;
  totalDescent = NOFLOAT;
  totalElapsedSeconds = NOTIME;
  totalElapsedSecondsMoving = NOTIME;

  trk.removeEmptySegments();

  // no data -> nothing to do
  if (trk.isEmpty()) {
    return;
  }

  activities.updateFlags();

  CTrackData::trkpt_t* lastValid = nullptr;
  CTrackData::trkpt_t* lastTrkpt = nullptr;
  qreal timestampStart = NOFLOAT;
  qint32 lastEle = NOINT;

  // linear list of pointers to visible track points
  QVector<CTrackData::trkpt_t*> lintrk;

  for (CTrackData::trkpt_t& trkpt : trk) {
    trkpt.idxTotal = cntTotalPoints++;

    if (trkpt.isHidden()) {
      trkpt.reset();
      continue;
    }

    trkpt.idxVisible = cntVisiblePoints++;
    lintrk << &trkpt;

    west = qMin(west, trkpt.lon);
    east = qMax(east, trkpt.lon);
    south = qMin(south, trkpt.lat);
    north = qMax(north, trkpt.lat);

    if (lastTrkpt != nullptr) {
      trkpt.deltaDistance = lastTrkpt->distanceTo(trkpt);
      trkpt.distance = lastTrkpt->distance + trkpt.deltaDistance;
      trkpt.elapsedSeconds = trkpt.time.toMSecsSinceEpoch() / 1000.0 - timestampStart;

      // ascent descent
      if (lastEle != NOINT) {
        qint32 delta = trkpt.ele - lastEle;

        trkpt.ascent = lastTrkpt->ascent;
        trkpt.descent = lastTrkpt->descent;

        if (qAbs(delta) >= ASCENT_THRESHOLD) {
          const qint32 step = (delta / ASCENT_THRESHOLD) * ASCENT_THRESHOLD;

          if (delta > 0) {
            trkpt.ascent += step;
          } else {
            trkpt.descent -= step;
          }
          lastEle += step;
        }
      }

      // time moving
      trkpt.elapsedSecondsMoving = lastTrkpt->elapsedSecondsMoving;
      qreal dt = (trkpt.time.toMSecsSinceEpoch() - lastTrkpt->time.toMSecsSinceEpoch()) / 1000.0;
      if (dt > 0 && ((trkpt.deltaDistance / dt) > 0.2)) {
        trkpt.elapsedSecondsMoving += dt;
      }
    } else {
      timeStart = trkpt.time;
      timestampStart = timeStart.toMSecsSinceEpoch() / 1000.0;
      lastEle = trkpt.ele;

      trkpt.deltaDistance = 0;
      trkpt.distance = 0;
      trkpt.ascent = 0;
      trkpt.descent = 0;
      trkpt.elapsedSeconds = 0;
      trkpt.elapsedSecondsMoving = 0;
    }

    lastTrkpt = &trkpt;
  }

  constexpr qreal kMargin = 0.0001 * DEG_TO_RAD;  // ~5m
  boundingRect = QRectF(QPointF(west * DEG_TO_RAD - kMargin, north * DEG_TO_RAD + kMargin),
                        QPointF(east * DEG_TO_RAD + kMargin, south * DEG_TO_RAD - kMargin));

  for (int p = 0; p < lintrk.size(); p++) {
    CTrackData::trkpt_t& trkpt = *lintrk[p];

    qreal d1 = trkpt.distance;
    qreal e1 = trkpt.ele;
    qreal t1 = trkpt.time.toMSecsSinceEpoch() / 1000.0;
    for (int n = p; n > 0; --n) {
      CTrackData::trkpt_t& trkpt2 = *lintrk[n];
      if (trkpt2.ele == NOINT) {
        continue;
      }

      d1 = trkpt2.distance;
      e1 = trkpt2.ele;
      t1 = trkpt2.time.toMSecsSinceEpoch() / 1000.0;
      if (trkpt.distance - trkpt2.distance >= 25) {
        break;
      }
    }

    qreal d2 = trkpt.distance;
    qreal e2 = trkpt.ele;
    qreal t2 = trkpt.time.toMSecsSinceEpoch() / 1000.0;
    for (int n = p; n < lintrk.size(); ++n) {
      CTrackData::trkpt_t& trkpt2 = *lintrk[n];
      if (trkpt2.ele == NOINT) {
        continue;
      }

      d2 = trkpt2.distance;
      e2 = trkpt2.ele;
      t2 = trkpt2.time.toMSecsSinceEpoch() / 1000.0;
      if (trkpt2.distance - trkpt.distance >= 25) {
        break;
      }
    }

    if (d1 < d2) {
      qreal a = qAtan((e2 - e1) / (d2 - d1));
      trkpt.slope1 = a * 360.0 / (2 * M_PI);
      trkpt.slope2 = qTan(trkpt.slope1 * DEG_TO_RAD) * 100;
    } else {
      trkpt.slope1 = NOFLOAT;
      trkpt.slope2 = NOFLOAT;
    }

    if (t1 < t2) {
      trkpt.speed = (d2 - d1) / (t2 - t1);
    } else {
      trkpt.speed = NOFLOAT;
    }

    // verify data
    verifyTrkPt(lastValid, trkpt);
    // add current status to allValidFlags
    allValidFlags |= trkpt.valid;
    if ((trkpt.valid & 0xFFFF0000) != 0) {
      cntInvalidPoints++;
    }
  }

  if (nullptr != lastTrkpt) {
    timeEnd = lastTrkpt->time;
    totalDistance = lastTrkpt->distance;
    totalAscent = lastTrkpt->ascent;
    totalDescent = lastTrkpt->descent;
    totalElapsedSeconds = lastTrkpt->elapsedSeconds;
    totalElapsedSecondsMoving = lastTrkpt->elapsedSecondsMoving;
  }

  activities.update();

  updateExtremaAndExtensions();
  // make sure we have a graph properties object by now
  if (propHandler == nullptr) {
    propHandler = new CPropertyTrk(*this);
    limitsGraph1.setSource(CKnownExtension::internalEle);
  } else {
    propHandler->setupData();
  }

  setupInterpolation(interp.valid, interp.Q);

  energyCycling.compute();

  updateVisuals(eVisualAll, "deriveSecondaryData()");

  //    qDebug() << "--------------" << getName() << "------------------";
  //    qDebug() << "allValidFlags" << Qt::hex << allValidFlags;
  //    qDebug() << "totalDistance" << totalDistance;
  //    qDebug() << "totalAscent" << totalAscent;
  //    qDebug() << "totalDescent" << totalDescent;
  //    qDebug() << "totalElapsedSeconds" << totalElapsedSeconds;
  //    qDebug() << "totalElapsedSecondsMoving" << totalElapsedSecondsMoving;
}

void CGisItemTrk::findWaypointsCloseBy(CProgressDialog& progress, quint32& current) {
  IGisProject* project = getParentProject();
  if (nullptr == project) {
    return;
  }

  quint32 lastCurrent = current;

  bool withDoubles = project->getSortingRoadbook() != IGisProject::eSortRoadbookTrackWithoutDouble;

  qreal north = -90 * DEG_TO_RAD;
  qreal south = 90 * DEG_TO_RAD;
  qreal west = 180 * DEG_TO_RAD;
  qreal east = -180 * DEG_TO_RAD;
  QVector<pointDP> line;
  // combine all segments to a single line
  for (CTrackData::trkpt_t& pt : trk) {
    pt.keyWpt.clear();
    pointDP dp(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD, 0);
    dp.idx = pt.idxTotal;

    north = qMax(north, dp.y);
    south = qMin(south, dp.y);
    west = qMin(west, dp.x);
    east = qMax(east, dp.x);

    line << dp;
  }

  if (line.isEmpty()) {
    return;
  }

  constexpr qreal OFFSET = 0.1 * DEG_TO_RAD;
  QRectF _boundingRect(QPointF(west - OFFSET, north + OFFSET), QPointF(east + OFFSET, south - OFFSET));

  // convert coordinates of all waypoints into meter coordinates relative to the first track point
  point3D pt0 = line[0];
  QList<trkwpt_t> trkwpts;
  for (int i = 0; i < project->childCount(); i++) {
    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(project->child(i));
    if (wpt == nullptr) {
      continue;
    }

    if (!_boundingRect.contains(wpt->getBoundingRect().topLeft())) {
      continue;
    }

    QPointF pos = wpt->getPosition();

    qreal a1 = 0, a2 = 0;
    qreal d = GPS_Math_Distance(pt0.x, pt0.y, pos.x() * DEG_TO_RAD, pos.y() * DEG_TO_RAD, a1, a2);

    trkwpt_t trkwpt;
    trkwpt.x = qCos(a1 * DEG_TO_RAD) * d;
    trkwpt.y = qSin(a1 * DEG_TO_RAD) * d;
    trkwpt.name = wpt->getName();
    trkwpt.key = wpt->getKey();

    trkwpts << trkwpt;
  }

  // convert all coordinates into meter relative to the first track point.
  for (pointDP& pt1 : line) {
    qreal a1 = 0, a2 = 0;
    qreal d = GPS_Math_Distance(pt0.x, pt0.y, pt1.x, pt1.y, a1, a2);

    pt1.x = qCos(a1 * DEG_TO_RAD) * d;
    pt1.y = qSin(a1 * DEG_TO_RAD) * d;
  }

  bool doDeriveData = false;
  numberOfAttachedWpt = 0;
  for (const trkwpt_t& trkwpt : std::as_const(trkwpts)) {
    qreal minD = WPT_FOCUS_DIST_IN;
    qint32 index = NOIDX;

    for (const pointDP& pt : std::as_const(line)) {
      ++current;
      qreal d = (trkwpt.x - pt.x) * (trkwpt.x - pt.x) + (trkwpt.y - pt.y) * (trkwpt.y - pt.y);

      if (d < WPT_FOCUS_DIST_IN) {
        if (d < minD) {
          index = pt.idx;
          minD = d;
        }
      } else if (withDoubles && (d > WPT_FOCUS_DIST_OUT)) {
        CTrackData::trkpt_t* trkpt = const_cast<CTrackData::trkpt_t*>(trk.getTrkPtByTotalIndex(index));
        if (trkpt) {
          ++numberOfAttachedWpt;
          trkpt->keyWpt = trkwpt.key;
          if (trkpt->isHidden()) {
            trkpt->unsetFlag(CTrackData::trkpt_t::eFlagHidden);
            doDeriveData = true;
          }
        }

        index = NOIDX;
        minD = WPT_FOCUS_DIST_IN;
      }

      if (current - lastCurrent > 100) {
        lastCurrent = current;
        PROGRESS(current, return );
      }
    }

    if (index != NOIDX) {
      CTrackData::trkpt_t* trkpt = const_cast<CTrackData::trkpt_t*>(trk.getTrkPtByTotalIndex(index));
      if (trkpt) {
        ++numberOfAttachedWpt;
        trkpt->keyWpt = trkwpt.key;
        if (trkpt->isHidden()) {
          trkpt->unsetFlag(CTrackData::trkpt_t::eFlagHidden);
          doDeriveData = true;
        }
      }
    }
  }

  if (doDeriveData) {
    deriveSecondaryData();
  }
  updateVisuals(eVisualDetails | eVisualPlot, "findWaypointsCloseBy()");
}

bool CGisItemTrk::isCloseTo(const QPointF& pos) {
  QMutexLocker lock(&mutexItems);

  return GPS_Math_DistPointPolyline(lineSimple, pos) < 20;
}

bool CGisItemTrk::isWithin(const QRectF& area, selflags_t flags) {
  QPolygonF l;
  getPolylineDegFromData(l);
  return (flags & eSelectionTrk) ? IGisItem::isWithin(area, flags, l) : false;
}

void CGisItemTrk::gainUserFocus(bool yes) {
  keyUserFocus = yes ? key : key_t();
  widthInfoBox = MIN_WIDTH_INFO_BOX;
}

void CGisItemTrk::looseUserFocus() {
  if (keyUserFocus == key) {
    keyUserFocus.clear();
  }
}

void CGisItemTrk::edit() {
  if (dlgDetails.isNull()) {
    dlgDetails = new CDetailsTrk(*this);
    dlgDetails->setObjectName(getName());
  }

  CMainWindow::self().addWidgetToTab(dlgDetails);
}

bool CGisItemTrk::cut() {
  if (nullptr == mouseClickFocus) {
    return false;
  }

  CCutTrk dlg(CMainWindow::getBestWidgetForParent());
  if (dlg.exec() == QDialog::Rejected) {
    return false;
  }

  mouseMoveFocus = nullptr;

  qint32 idxMouse = mouseClickFocus->idxTotal;
  CCutTrk::mode_e mode = dlg.getMode();
  CCutTrk::cutmode_e cutMode = dlg.getCutMode();

  // if the cut action results into cloning a track, the calling method should
  // ask if the original track should be removed. As a track can't delete itself
  // this has to be done from the outside of this method.
  bool askToDeleteOriginal = dlg.createClone() || (mode == CCutTrk::eModeKeepBoth);

  // askToDeleteOriginal = store result as clone
  if (askToDeleteOriginal) {
    // clone first part?
    if ((mode & (CCutTrk::eModeKeepBoth | CCutTrk::eModeKeepFirst)) != 0) {
      int idx = cutMode == CCutTrk::eCutMode1 ? idxMouse - 1 : idxMouse;
      if (idx < 0) {
        idx = 0;
      }

      QString name = getName() + QString(" (%1 - %2)").arg(0).arg(idx);
      IGisProject* project = nullptr;
      if (!getNameAndProject(name, project, tr("track"))) {
        return false;
      }

      new CGisItemTrk(name, 0, idx, trk, project);
    }

    // clone second part?
    if ((mode & (CCutTrk::eModeKeepBoth | CCutTrk::eModeKeepSecond)) != 0) {
      QString name = getName() + QString(" (%1 - %2)").arg(idxMouse).arg(cntTotalPoints - 1);
      IGisProject* project = nullptr;
      if (!getNameAndProject(name, project, tr("track"))) {
        return false;
      }

      new CGisItemTrk(name, idxMouse, cntTotalPoints - 1, trk, project);
    }
  } else {
    // if the result is not a clone, the track's list of segments and trackpoints
    // has to be reduced. This is done by copying points into a new trackpoint list
    // that replaces the original one.
    int removeStart = ((mode & CCutTrk::eModeKeepFirst) != 0) ? idxMouse + 1 : 0;
    int removeEnd = ((mode & CCutTrk::eModeKeepFirst) != 0) ? cntTotalPoints : idxMouse - 1;

    for (CTrackData::trkseg_t& seg : trk.segs) {
      if (seg.isEmpty()) {
        continue;
      }

      if (removeStart <= seg.pts.first().idxTotal && seg.pts.last().idxTotal <= removeEnd) {
        // remove all points for segments, that are completely in [removeStart; removeEnd]
        seg.pts.clear();
      } else if (isInRange(removeStart, seg.pts.first().idxTotal, seg.pts.last().idxTotal) ||
                 isInRange(removeEnd, seg.pts.first().idxTotal, seg.pts.last().idxTotal)) {
        QVector<CTrackData::trkpt_t> pts;

        for (const CTrackData::trkpt_t& pt : std::as_const(seg.pts)) {
          if (!(removeStart <= pt.idxTotal && pt.idxTotal <= removeEnd)) {
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

bool CGisItemTrk::addTrkPtDesc() {
  if (nullptr == mouseClickFocus) {
    return false;
  }

  const QString& desc = QInputDialog::getText(CMainWindow::self().getBestWidgetForParent(), tr("Track Point Info..."),
                                              tr("Enter some text to be attached to this track point:"));

  if (desc.isEmpty()) {
    return false;
  }

  if (trk.setTrkPtDesc(mouseClickFocus->idxTotal, desc)) {
    changed(tr("Add track point desc.: %1").arg(desc), "://icons/48x48/I.png");
    return true;
  }
  return false;
}

bool CGisItemTrk::setTrkPtDesc(int idxTotal, const QString& desc) {
  if (trk.setTrkPtDesc(idxTotal, desc)) {
    changed(tr("Changed track point desc.: %1").arg(desc), "://icons/48x48/I.png");
    return true;
  }
  return false;
}

bool CGisItemTrk::delTrkPtDesc(const QList<int>& idxTotal) {
  if (trk.delTrkPtDesc(idxTotal)) {
    changed(tr("Removed track point desc."), "://icons/48x48/DeleteMultiple.png");
    return true;
  }
  return false;
}

void CGisItemTrk::reverse() {
  QString name = getName() + "_rev";
  IGisProject* project = nullptr;
  if (!getNameAndProject(name, project, tr("track"))) {
    return;
  }

  // start with a 1:1 copy of the first track
  CGisItemTrk* trk1 = new CGisItemTrk(*this, project, NOIDX, false);
  trk1->trk.name = name;
  /*
      clear track data, item key and history. To clear the history is important as
      the original track's history would restore the original key

   */
  trk1->trk.segs.clear();
  trk1->key.clear();
  trk1->history.events.clear();

  for (const CTrackData::trkseg_t& seg : std::as_const(trk.segs)) {
    CTrackData::trkseg_t seg1;
    for (const CTrackData::trkpt_t& pt : seg.pts) {
      CTrackData::trkpt_t pt1 = pt;
      pt1.time = QDateTime();

      seg1.pts.push_front(pt1);
    }
    trk1->trk.segs.push_front(seg1);
  }
  // restore secondary data and create a new history
  trk1->deriveSecondaryData();
  trk1->setupHistory();
  trk1->updateDecoration(eMarkChanged, eMarkNone);
}

void CGisItemTrk::combine(const QList<IGisItem::key_t>& keys) {
  if (keys.isEmpty()) {
    return;
  }

  QString name = getName() + " & other";
  IGisProject* projectNew = nullptr;
  if (!getNameAndProject(name, projectNew, tr("track"))) {
    return;
  }

  // start with a 1:1 copy of the first track
  CGisItemTrk* trk1 = new CGisItemTrk(*this, projectNew, NOIDX, false);
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
  CGisWorkspace& gis = CGisWorkspace::self();
  for (const IGisItem::key_t& key : keys) {
    CGisItemTrk* trk2 = dynamic_cast<CGisItemTrk*>(gis.getItemByKey(key));
    if (nullptr == trk2) {
      continue;
    }

    trk1->trk.segs += trk2->trk.segs;
  }

  // restore secondary data and create a new history
  trk1->deriveSecondaryData();
  trk1->setupHistory();
  trk1->updateDecoration(eMarkChanged, eMarkNone);
}

bool CGisItemTrk::adjustIndicesForRemoveOperations(qint32& idx1, qint32& idx2) {
  if (NOIDX == idx1) {
    return false;
  }

  // if first index is the first point adjust index to hide it, too
  if (trk.isTrkPtFirstVisible(idx1)) {
    idx1--;
  }
  // if second index is the last point adjust index to hide it, too
  if (trk.isTrkPtLastVisible(idx2)) {
    idx2++;
  }

  // abort if there is no point between idx1 and idx2
  if (idx1 + 1 == idx2) {
    return false;
  }

  // special case for a single point
  if (idx1 == idx2) {
    --idx1;
    ++idx2;
  }
  return true;
}

void CGisItemTrk::hideSelectedPoints() {
  if (!setReadOnlyMode(false)) {
    return;
  }

  // read start/stop indices
  qint32 idx1, idx2;
  getMouseRange(idx1, idx2, true);

  if (!adjustIndicesForRemoveOperations(idx1, idx2)) {
    return;
  }

  // iterate over all segments and delete points between idx1 and idx2
  for (CTrackData::trkpt_t& trkpt : trk) {
    if ((idx1 < trkpt.idxTotal) && (trkpt.idxTotal < idx2) && trkpt.desc.isEmpty() && trkpt.keyWpt.item.isEmpty()) {
      trkpt.setFlag(CTrackData::trkpt_t::eFlagHidden);
    }
  }
  deriveSecondaryData();
  if (idx1 + 1 == idx2 - 1) {
    changed(tr("Hide point %1.").arg(idx1 + 1), "://icons/48x48/PointHide.png");
  } else {
    changed(tr("Hide points %1..%2.").arg(idx1 + 1).arg(idx2 - 1), "://icons/48x48/PointHide.png");
  }
}

void CGisItemTrk::deleteSelectedPoints() {
  if (!setReadOnlyMode(false)) {
    return;
  }

  // read start/stop indices
  qint32 idx1, idx2;
  getMouseRange(idx1, idx2, true);

  if (!adjustIndicesForRemoveOperations(idx1, idx2)) {
    return;
  }

  for (CTrackData::trkseg_t& seg : trk.segs) {
    QVector<CTrackData::trkpt_t> pts;
    for (const CTrackData::trkpt_t& pt : std::as_const(seg.pts)) {
      if (idx1 < pt.idxTotal && pt.idxTotal < idx2) {
        continue;
      }

      pts << pt;
    }

    seg.pts = pts;
  }

  mouseRange1 = trk.getTrkPtByTotalIndex(idx1);
  mouseRange2 = trk.getTrkPtByTotalIndex(idx1 + 1);
  mouseMoveFocus = nullptr;

  deriveSecondaryData();
  if (idx1 + 1 == idx2 - 1) {
    changed(tr("Delete point %1.").arg(idx1 + 1), "://icons/48x48/DeleteOne.png");
  } else {
    changed(tr("Delete points %1..%2.").arg(idx1 + 1).arg(idx2 - 1), "://icons/48x48/DeleteMultiple.png");
  }
}

void CGisItemTrk::showSelectedPoints() {
  if (!setReadOnlyMode(false)) {
    return;
  }

  qint32 idx1, idx2;
  getMouseRange(idx1, idx2, true);

  if (NOIDX == idx1) {
    return;
  }

  for (CTrackData::trkpt_t& trkpt : trk) {
    if (isInRange(trkpt.idxTotal, idx1, idx2)) {
      trkpt.unsetFlag(CTrackData::trkpt_t::eFlagHidden);
    }
  }

  deriveSecondaryData();
  changed(tr("Show points."), "://icons/48x48/PointShow.png");
}

void CGisItemTrk::copySelectedPoints() const {
  qint32 idx1, idx2;
  getMouseRange(idx1, idx2, true);

  if (NOIDX == idx1) {
    return;
  }

  QString name = getName() + QString(" (%1 - %2)").arg(idx1).arg(idx2);
  IGisProject* project = nullptr;
  if (!getNameAndProject(name, project, tr("track"))) {
    return;
  }

  new CGisItemTrk(name, idx1, idx2, trk, project);
}

void CGisItemTrk::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) {
  QMutexLocker lock(&mutexItems);

  lineSimple.clear();
  lineFull.clear();

  if (!isVisible(boundingRect, viewport, gis)) {
    return;
  }

  if (trk.segs.isEmpty()) {
    return;
  }

  QPointF pt1;
  QPointF p1 = viewport[0];
  QPointF p2 = viewport[2];
  gis->convertRad2Px(p1);
  gis->convertRad2Px(p2);
  QRectF extViewport(p1, p2);

  if (mode == eModeNormal) {
    // in normal mode the trackline without points marked as deleted is drawn
    for (const CTrackData::trkpt_t& pt : trk) {
      if (pt.isHidden()) {
        continue;
      }

      pt1.setX(pt.lon);
      pt1.setY(pt.lat);
      pt1 *= DEG_TO_RAD;
      lineSimple << pt1;
    }
  } else {
    // in full mode the complete track including points marked as deleted
    // is drawn as gray line first. Then the track without points marked as
    // deleted is drawn with it's configured color
    for (const CTrackData::trkpt_t& pt : trk) {
      pt1.setX(pt.lon);
      pt1.setY(pt.lat);
      pt1 *= DEG_TO_RAD;

      lineFull << pt1;

      if (pt.isHidden()) {
        continue;
      }

      lineSimple << pt1;
    }
  }
  gis->convertRad2Px(lineSimple);
  gis->convertRad2Px(lineFull);

  // draw the full line first
  if (mode == eModeRange) {
    QList<QPolygonF> lines;
    splitLineToViewport(lineFull, extViewport, lines);

    p.setPen(QPen(Qt::lightGray, penWidthBg, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    for (const QPolygonF& l : std::as_const(lines)) {
      p.drawPolyline(l);
    }

    QPixmap bullet("://icons/8x8/bullet_dark_gray.png");
    for (const QPolygonF& l : std::as_const(lines)) {
      for (const QPointF& pt : l) {
        p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
      }
    }
  }
  // -------------------------

  // draw the reduced track line
  QList<QPolygonF> lines;
  splitLineToViewport(lineSimple, extViewport, lines);

  const CMainWindow& w = CMainWindow::self();
  if (key == keyUserFocus && w.isShowTrackHighlight()) {
    p.setPen(QPen(Qt::red, penWidthHi, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for (const QPolygonF& l : std::as_const(lines)) {
      p.drawPolyline(l);
    }
  }

  p.setBrush(color);
  p.setPen(penBackground);
  for (const QPolygonF& l : std::as_const(lines)) {
    p.drawPolyline(l);
    const QRectF& bounding = l.boundingRect();
    auto area = bounding.width() * bounding.height();
    if (!isNogo() && showArrows.val().toBool() && (area > (100 * 100))) {
      CDraw::arrows(l, extViewport, p, 2, 120, lineScale.val().toDouble());
    }
  }

  if (getColorizeSource().isEmpty()) {
    // use the track's ordinary color
    penForeground.setColor(color);
    p.setPen(penForeground);
    for (const QPolygonF& l : std::as_const(lines)) {
      p.drawPolyline(l);
    }
  } else if (getColorizeSource() == "activity") {
    drawColorizedByActivity(p);
  } else {
    drawColorized(p);
  }

  if (isNogo()) {
    for (const QPolygonF& l : std::as_const(lines)) {
      CDraw::nogos(l, extViewport, p, 80);
    }
  }

  // -------------------------
  bool skipDecorations = (!keyUserFocus.item.isEmpty() && (key != keyUserFocus));
  // draw min/max labels
  if (w.isShowMinMaxTrackLabels() && !skipDecorations) {
    const QStringList& keys = extrema.keys();
    for (const QString& key : keys) {
      if (key == CKnownExtension::internalProgress) {
        continue;
      }

      const limits_t& limit = extrema[key];
      QPointF posMin = limit.posMin * DEG_TO_RAD;
      QPointF posMax = limit.posMax * DEG_TO_RAD;

      gis->convertRad2Px(posMin);
      gis->convertRad2Px(posMax);

      p.setPen(Qt::white);
      p.setBrush(Qt::darkGreen);
      p.drawEllipse(posMin, 5, 5);
      p.setBrush(Qt::darkRed);
      p.drawEllipse(posMax, 5, 5);
    }
  }

  // draw info points
  if (w.isShowTrackInfoPoints() && !skipDecorations) {
    const QFont& f = w.getMapFont();
    const int pointSize = f.pointSize();
    const int size = (pointSize + (f.bold() ? 3 : 2)) * 2;
    p.setFont(f);

    quint32 cnt = 1;
    QList<cluster> clusters;
    for (const CTrackData::trkpt_t& trkpt : trk) {
      if (trkpt.desc.isEmpty() || trkpt.isHidden()) {
        continue;
      }

      QPointF pos(trkpt.lon, trkpt.lat);
      pos *= DEG_TO_RAD;
      gis->convertRad2Px(pos);

      QRect r(0, 0, size, size);
      r.moveCenter(pos.toPoint());
      cluster::addToCluster(r, clusters, cnt++);
    }

    cluster::draw(clusters, p, size);
  }
}

void CGisItemTrk::drawLimitLabels(limit_type_e type, const QString& label, const QPointF& pos, QPainter& p,
                                  const QFontMetricsF& fm, QList<QRectF>& blockedAreas) {
  const QString& fullLabel = (type == eLimitTypeMin ? tr("min.") : tr("max.")) + " " + label;
  QRectF rect = fm.boundingRect(fullLabel);
  rect.moveBottomLeft(pos.toPoint() + QPoint(10, -10));
  rect.adjust(-4, -2, 4, 2);

  qint32 baseWidth = 10;
  qint32 basePos = 10;

  if (CDraw::doesOverlap(blockedAreas, rect)) {
    rect.moveBottomRight(pos.toPoint() + QPoint(-10, -10));
    basePos = rect.width() - 10;

    if (CDraw::doesOverlap(blockedAreas, rect)) {
      rect.moveTopLeft(pos.toPoint() + QPoint(10, 10));
      basePos = 10;

      if (CDraw::doesOverlap(blockedAreas, rect)) {
        rect.moveTopRight(pos.toPoint() + QPoint(-10, 10));
        basePos = rect.width() - 10;

        if (CDraw::doesOverlap(blockedAreas, rect)) {
          return;
        }
      }
    }
  }

  CDraw::bubble(p, rect.toRect(), pos.toPoint(), Qt::white, baseWidth, basePos,
                (key == keyUserFocus) ? CDraw::penBorderRed : CDraw::penBorderGray);
  CDraw::text(fullLabel, p, rect.toRect(), type == eLimitTypeMin ? Qt::darkGreen : Qt::darkRed);
  blockedAreas << rect;
}

void CGisItemTrk::setPen(QPainter& p, QPen& pen, trkact_t act) const {
  pen.setColor((act == CTrackData::trkpt_t::eAct20None) ? color : CActivityTrk::getDescriptor(act).color);
  p.setPen(pen);
}

void CGisItemTrk::drawColorizedByActivity(QPainter& p) const {
  QPen pen;
  pen.setWidth(penWidthFg);
  pen.setCapStyle(Qt::RoundCap);

  for (const CTrackData::trkseg_t& segment : trk.segs) {
    const CTrackData::trkpt_t* ptPrev = nullptr;

    for (const CTrackData::trkpt_t& pt : segment.pts) {
      if (pt.isHidden()) {
        continue;
      }
      if (nullptr == ptPrev) {
        setPen(p, pen, pt.getAct());
        ptPrev = &pt;
        continue;
      }

      p.drawLine(lineSimple[ptPrev->idxVisible], lineSimple[pt.idxVisible]);

      if (ptPrev->getAct() != pt.getAct()) {
        setPen(p, pen, pt.getAct());
      }

      ptPrev = &pt;
    }
  }
}

void CGisItemTrk::drawColorized(QPainter& p) const {
  auto valueFunc = CKnownExtension::get(getColorizeSource()).valueFunc;

  QImage colors(1, 256, QImage::Format_RGB888);
  QPainter colorsPainter(&colors);

  QLinearGradient colorsGradient(colors.rect().topLeft(), colors.rect().bottomLeft());
  colorsGradient.setColorAt(1.0, QColor(0, 0, 255));    // blue
  colorsGradient.setColorAt(0.6, QColor(0, 255, 0));    // green
  colorsGradient.setColorAt(0.4, QColor(255, 255, 0));  // yellow
  colorsGradient.setColorAt(0.0, QColor(255, 0, 0));    // red
  colorsPainter.fillRect(colors.rect(), colorsGradient);

  const qreal factor = CKnownExtension::get(getColorizeSource()).factor;

  for (const CTrackData::trkseg_t& segment : trk.segs) {
    const CTrackData::trkpt_t* ptPrev = nullptr;
    QColor colorStart;

    for (const CTrackData::trkpt_t& pt : segment.pts) {
      if (pt.isHidden()) {
        continue;
      }
      if (nullptr == ptPrev) {
        ptPrev = &pt;
        continue;
      }

      float colorAt =
          (factor * valueFunc(pt) - getColorizeLimitLow()) / (getColorizeLimitHigh() - getColorizeLimitLow());
      colorAt = qMin(qMax(colorAt, 0.f), 1.f);

      const QColor& colorEnd = colors.pixel(0, ((1.f - colorAt) * 255.f));
      if (!colorStart.isValid()) {
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

qreal CGisItemTrk::getMin(const QString& source) const {
  return extrema.value(source).min * CKnownExtension::get(source).factor;
}

qreal CGisItemTrk::getMax(const QString& source) const {
  return extrema.value(source).max * CKnownExtension::get(source).factor;
}

qreal CGisItemTrk::getMinProp(const QString& source) const {
  return propHandler == nullptr ? NOFLOAT : propHandler->propBySource(source).min;
}

qreal CGisItemTrk::getMaxProp(const QString& source) const {
  return propHandler == nullptr ? NOFLOAT : propHandler->propBySource(source).max;
}

QString CGisItemTrk::getUnitProp(const QString& source) const {
  return propHandler == nullptr ? QString() : propHandler->propBySource(source).unit;
}

QStringList CGisItemTrk::getExistingDataSources() const {
  QStringList known;
  QStringList unknown;

  for (const QString& key : existingExtensions) {
    if (CKnownExtension::isKnown(key)) {
      known << key;
    } else {
      unknown << key;
    }
  }

  auto stringSort = [](const QString& s1, const QString& s2) { return s1.toLower() < s2.toLower(); };

  std::sort(known.begin(), known.end(), stringSort);
  std::sort(unknown.begin(), unknown.end(), stringSort);

  return known + unknown;
}

void CGisItemTrk::setColorizeSource(QString src) {
  if (src != getColorizeSource()) {
    colorSourceLimit.setSource(src);
    updateHistory(eVisualColorLegend | eVisualDetails);
  }
}

void CGisItemTrk::setColorizeLimitLow(qreal limit) {
  colorSourceLimit.setMin(limit);
  updateHistory(eVisualColorLegend | eVisualDetails);
}

void CGisItemTrk::setColorizeLimitHigh(qreal limit) {
  colorSourceLimit.setMax(limit);
  updateHistory(eVisualColorLegend | eVisualDetails);
}

QString CGisItemTrk::getColorizeUnit() const { return CKnownExtension::get(getColorizeSource()).unit; }

void CGisItemTrk::drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis) {
  QMutexLocker lock(&mutexItems);

  if (trk.segs.isEmpty()) {
    return;
  }

  if (hasUserFocus() && mouseMoveFocus && (mode != eModeRange)) {
    // derive anchor
    QPointF anchor(mouseMoveFocus->lon, mouseMoveFocus->lat);
    anchor *= DEG_TO_RAD;
    gis->convertRad2Px(anchor);

    // create trackpoint info text
    QString str = getInfoTrkPt(*mouseMoveFocus);

    // search for track point information in the neighboring points
    const int idxMin = qMax(mouseMoveFocus->idxTotal - 2, 0);
    const int idxMax = qMin(mouseMoveFocus->idxTotal + 3, cntTotalPoints);
    for (int idx = idxMin; idx < idxMax; idx++) {
      const QString& desc = trk.getTrkPtByTotalIndex(idx)->desc;
      if (!desc.isEmpty()) {
        str += "\nInfo: " + desc;
        break;
      }
    }

    // calculate bounding box of text
    QFont f = CMainWindow::self().getMapFont();
    QFontMetrics fm(f);
    QRect rectText = fm.boundingRect(QRect(0, 0, 500, 0), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, str);

    // The initial minimum size of the box will be MIN_WIDTH_INFO_BOX.
    // If a larger box is needed the minimum grows. By that the width
    // of the box will only grow but not jump between sizes
    if (rectText.width() < widthInfoBox) {
      rectText.setWidth(widthInfoBox);
    } else {
      widthInfoBox = rectText.width();
    }

    // create info box
    int w = rectText.width() + 5 + 5;
    int h = rectText.height() + 5 + (fm.height() + 8);
    if (totalElapsedSeconds != 0) {
      h += 5 + fm.height() + 8;
    }

    p.setFont(f);

    // draw the bubble
    QWidget widget;
    const QPalette& pal = widget.palette();
    const QColor& colorBg = pal.color(QPalette::Window);
    const QColor& colorFg = pal.color(QPalette::WindowText);

    QRect box(0, 0, w, h);
    box.moveBottomLeft(anchor.toPoint() + QPoint(-50, -50));
    CDraw::bubble(p, box, anchor.toPoint(), colorBg, 18 /* px */, 21 /* px */);

    p.save();
    p.translate(box.topLeft());

    QColor pbarBlue(150, 150, 255);
    QColor pbarGreen(150, 255, 150);

    // draw progress bar distance
    p.translate(5, 5);
    QRect rectBar1(0, 0, rectText.width(), fm.height());
    p.setPen(pbarBlue);
    p.setBrush(pbarBlue);
    p.drawRect(rectBar1);
    qreal d = mouseMoveFocus->distance * rectBar1.width() / totalDistance;
    p.setPen(pbarGreen);
    p.setBrush(pbarGreen);
    p.drawRect(0, 0, d, fm.height());

    QString val1, unit1, val2, unit2;
    IUnit::self().meter2distance(mouseMoveFocus->distance, val1, unit1);
    IUnit::self().meter2distance(totalDistance - mouseMoveFocus->distance, val2, unit2);
    p.setPen(Qt::darkBlue);
    p.drawText(QRect(0, 1, rectBar1.width(), fm.height()), Qt::AlignVCenter | Qt::AlignLeft,
               QString("%1%2").arg(val1, unit1));
    p.drawText(QRect(0, 1, rectBar1.width(), fm.height()), Qt::AlignCenter,
               QString("%1%").arg(mouseMoveFocus->distance * 100 / totalDistance, 0, 'f', 0));
    p.drawText(QRect(0, 1, rectBar1.width(), fm.height()), Qt::AlignVCenter | Qt::AlignRight,
               QString("%1%2").arg(val2, unit2));

    // draw progress bar time
    if (totalElapsedSeconds != 0) {
      p.translate(0, fm.height() + 5);
      QRect rectBar2(0, 0, rectText.width(), fm.height());
      p.setPen(pbarBlue);
      p.setBrush(pbarBlue);
      p.drawRect(rectBar2);
      qreal t = mouseMoveFocus->elapsedSecondsMoving * rectBar2.width() / totalElapsedSecondsMoving;
      p.setPen(pbarGreen);
      p.setBrush(pbarGreen);
      p.drawRect(0, 0, t, fm.height());

      IUnit::self().seconds2time(mouseMoveFocus->elapsedSecondsMoving, val1, unit1);
      IUnit::self().seconds2time(totalElapsedSecondsMoving - mouseMoveFocus->elapsedSecondsMoving, val2, unit2);
      p.setPen(Qt::darkBlue);
      p.drawText(QRect(0, 1, rectBar1.width(), fm.height()), Qt::AlignVCenter | Qt::AlignLeft,
                 QString("%1%2").arg(val1, unit1));
      p.drawText(QRect(0, 1, rectBar1.width(), fm.height()), Qt::AlignCenter,
                 QString("%1%").arg(mouseMoveFocus->elapsedSecondsMoving * 100 / totalElapsedSecondsMoving, 0, 'f', 0));
      p.drawText(QRect(0, 1, rectBar1.width(), fm.height()), Qt::AlignVCenter | Qt::AlignRight,
                 QString("%1%2").arg(val2, unit2));
    }

    // draw text
    p.translate(0, fm.height() + 8);
    p.setPen(colorFg);
    p.drawText(rectText, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, str);

    p.restore();
  }

  if (!scrOpt.isNull() && mouseClickFocus) {
    QPointF anchor(mouseClickFocus->lon, mouseClickFocus->lat);
    anchor *= DEG_TO_RAD;
    gis->convertRad2Px(anchor);

    p.drawPixmap(anchor - QPointF(4, 4), QPixmap(IGisItem::colorMap[colorIdx].bullet));
  }

  drawRange(p, gis);
}

void CGisItemTrk::drawLabel(QPainter& p, const QPolygonF&, QList<QRectF>& blockedAreas, const QFontMetricsF& fm,
                            CGisDraw* gis) {
  if (!keyUserFocus.item.isEmpty() && (key != keyUserFocus)) {
    return;
  }

  const CMainWindow& w = CMainWindow::self();
  if (w.isShowMinMaxTrackLabels()) {
    const QStringList& keys = extrema.keys();
    for (const QString& key : keys) {
      if (key == CKnownExtension::internalSpeedTime) {
        // already handled by CKnownExtension::internalSpeedDist
        continue;
      }

      const CKnownExtension& ext = CKnownExtension::get(key);
      const limits_t& limit = extrema[key];
      QPointF posMin = limit.posMin * DEG_TO_RAD;
      QPointF posMax = limit.posMax * DEG_TO_RAD;

      gis->convertRad2Px(posMin);
      gis->convertRad2Px(posMax);

      QString labelMin = ext.toString(limit.min, true, key);
      QString labelMax = ext.toString(limit.max, true, key);

      if (!labelMin.isEmpty()) {
        drawLimitLabels(eLimitTypeMin, labelMin, posMin, p, fm, blockedAreas);
      }

      if (!labelMax.isEmpty()) {
        drawLimitLabels(eLimitTypeMax, labelMax, posMax, p, fm, blockedAreas);
      }
    }
  }
}

void CGisItemTrk::drawHighlight(QPainter& p) {
  QMutexLocker lock(&mutexItems);

  if (lineSimple.isEmpty() || hasUserFocus()) {
    return;
  }

  // draw the reduced track line
  QList<QPolygonF> lines;
  splitLineToViewport(lineSimple, p.viewport(), lines);

  p.setPen(QPen(QColor(255, 0, 0, 100), penWidthHi, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  for (const QPolygonF& line : std::as_const(lines)) {
    p.drawPolyline(line);
  }
}

void CGisItemTrk::drawRange(QPainter& p, CGisDraw* gis) {
  QMutexLocker lock(&mutexItems);

  int idx1, idx2;
  getMouseRange(idx1, idx2, mode == eModeRange);

  if (NOIDX == idx1) {
    return;
  }

  const QPolygonF& line = (mode == eModeRange) ? lineFull : lineSimple;

  QPolygonF seg = line.mid(idx1, idx2 - idx1 + 1);

  if (seg.size() == 1) {
    const auto r1 = qRound(penWidthHi / 2.0);
    const auto r2 = qRound(penWidthFg / 2.0);

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::darkGreen);
    p.drawEllipse(seg[0], r1, r1);
    p.setBrush(Qt::green);
    p.drawEllipse(seg[0], r2, r2);
  } else {
    p.setPen(QPen(Qt::darkGreen, penWidthHi, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(seg);

    p.setPen(QPen(Qt::green, penWidthFg, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(seg);
  }

  if (mouseMoveFocus == mouseRange1 || mouseMoveFocus == mouseRange2) {
    QPointF pt(mouseMoveFocus->lon, mouseMoveFocus->lat);
    pt *= DEG_TO_RAD;
    gis->convertRad2Px(pt);

    const auto r1 = qRound(penWidthHi / 2.0) + 1;
    const auto r2 = qRound(penWidthFg / 2.0) + 1;

    p.setPen(Qt::NoPen);
    p.setBrush(Qt::darkGreen);
    p.drawEllipse(pt, r1, r1);
    p.setBrush(Qt::green);
    p.drawEllipse(pt, r2, r2);
  }
}

bool CGisItemTrk::setMode(mode_e m, const QString& owner) {
  if (!mouseFocusOwner.isEmpty() && owner != mouseFocusOwner) {
    return false;
  }

  mode = m;
  // always reset the range statemachine
  resetMouseRange();
  mouseFocusOwner = (mode == eModeRange) ? owner : "";

  CCanvas::triggerCompleteUpdate(CCanvas::eRedrawGis);

  if (!dlgDetails.isNull()) {
    dlgDetails->enableTabFilter();
  }

  return true;
}

void CGisItemTrk::setName(const QString& str) {
  setText(CGisListWks::eColumnName, str);
  trk.name = str;
  changed(tr("Changed name"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setComment(const QString& str) {
  trk.cmt = str;
  changed(tr("Changed comment"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setDescription(const QString& str) {
  trk.desc = str;
  changed(tr("Changed description"), "://icons/48x48/EditText.png");
}

void CGisItemTrk::setLinks(const QList<link_t>& links) {
  trk.links = links;
  changed(tr("Changed links"), "://icons/48x48/Link.png");
}

void CGisItemTrk::setElevation(qint32 idx, qint32 ele) {
  CTrackData::trkpt_t* trkpt = trk.getTrkPtByTotalIndex(idx);
  if ((trkpt != nullptr) && (trkpt->ele != ele)) {
    trkpt->ele = ele;
    deriveSecondaryData();
    changed(tr("Changed elevation of point %1 to %2 %3")
                .arg(idx)
                .arg(ele * IUnit::self().elevationFactor)
                .arg(IUnit::self().elevationUnit),
            "://icons/48x48/SetEle.png");
  }
}

void CGisItemTrk::setColor(int idx) {
  if (idx < IGisItem::getColorMap().count()) {
    setColor(IGisItem::getColorMap()[idx].color);
    updateHistory(eVisualColorLegend | eVisualDetails);
  }
}

void CGisItemTrk::setActivity(trkact_t act) {
  for (CTrackData::trkpt_t& trkpt : trk) {
    trkpt.setAct(act);
  }

  deriveSecondaryData();

  const CActivityTrk::desc_t& desc = CActivityTrk::getDescriptor(act);
  changed(tr("Changed activity to '%1' for complete track.").arg(desc.name), desc.iconLarge);
}

void CGisItemTrk::setActivityRange(trkact_t act) {
  if (!setReadOnlyMode(false)) {
    return;
  }

  const CActivityTrk::desc_t& desc = CActivityTrk::getDescriptor(act);

  // read start/stop indices
  qint32 idx1, idx2;
  getMouseRange(idx1, idx2, true);

  if (NOIDX == idx1) {
    return;
  }

  // iterate over all segments and set activity flag for points between idx1 and idx2
  for (CTrackData::trkpt_t& trkpt : trk) {
    if ((idx1 <= trkpt.idxTotal) && (trkpt.idxTotal < idx2)) {
      trkpt.setAct(act);
    }
  }

  deriveSecondaryData();
  changed(tr("Changed activity to '%1' for range(%2..%3).").arg(desc.name).arg(idx1).arg(idx2), desc.iconLarge);
}

void CGisItemTrk::setColor(const QColor& c) {
  colorIdx = DEFAULT_COLOR;

  const QVector<IGisItem::color_t>& colorMap = IGisItem::getColorMap();
  const int N = colorMap.count();
  for (int n = 0; n < N; n++) {
    if (c == colorMap[n].color) {
      colorIdx = n;
      break;
    }
  }

  color = colorMap[colorIdx].color;
  bullet = QPixmap(colorMap[colorIdx].bullet);

  setIcon(color2str(color));
}

void CGisItemTrk::setIcon(const QString& iconColor) {
  trk.color = iconColor;
  QPixmap icon = QPixmap("://icons/48x48/Track.png");

  QPixmap mask(icon.size());
  mask.fill(str2color(iconColor));
  mask.setMask(icon.createMaskFromColor(Qt::transparent));
  IGisItem::setIcon(mask.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

bool CGisItemTrk::setMouseFocusByDistance(qreal dist, focusmode_e fmode, const QString& owner) {
  const CTrackData::trkpt_t* newPointOfFocus = nullptr;

  if (dist != NOFLOAT) {
    qreal delta = totalDistance;

    /// @todo: optimize search by single out segment and then do a binary search

    for (const CTrackData::trkpt_t& pt : trk) {
      if (pt.isHidden()) {
        continue;
      }

      qreal d = qAbs(pt.distance - dist);
      if (d <= delta) {
        newPointOfFocus = &pt;
        delta = d;
      } else {
        break;
      }
    }
  }

  return publishMouseFocus(newPointOfFocus, fmode, owner);
}

bool CGisItemTrk::setMouseFocusByTime(quint32 time, focusmode_e fmode, const QString& owner) {
  const CTrackData::trkpt_t* newPointOfFocus = nullptr;

  if (time != NOTIME) {
    /// @todo: optimize search by single out segment and then do a binary search

    qreal delta = totalElapsedSeconds;

    for (const CTrackData::trkpt_t& pt : trk) {
      if (pt.isHidden()) {
        continue;
      }

      qreal d = qAbs(qreal(pt.time.toSecsSinceEpoch()) - qreal(time));
      if (d <= delta) {
        newPointOfFocus = &pt;
        delta = d;
      } else {
        break;
      }
    }
  }

  return publishMouseFocus(newPointOfFocus, fmode, owner);
}

QPointF CGisItemTrk::setMouseFocusByPoint(const QPoint& pt, focusmode_e fmode, const QString& owner) {
  QMutexLocker lock(&mutexItems);

  const CTrackData::trkpt_t* newPointOfFocus = nullptr;
  quint32 idx = 0;

  const QPolygonF& line = (mode == eModeRange) ? lineFull : lineSimple;

  if (pt != NOPOINT && GPS_Math_DistPointPolyline(line, pt) < MIN_DIST_FOCUS) {
    /*
        Iterate over the polyline used to draw the track as it contains screen
        coordinates. The polyline is a linear representation of the segments in the
        track. That is why the index into the polyline can't be used directly.
        In a second step we have to iterate over all segments and points of the CTrackData object
        until the index is reached. This is done by either getTrkPtByVisibleIndex(), or
        getTrkPtByTotalIndex(). Depending on the current mode.
     */

    idx = getIdxPointCloseBy(pt, line);
    newPointOfFocus = (mode == eModeRange) ? trk.getTrkPtByTotalIndex(idx) : trk.getTrkPtByVisibleIndex(idx);
  }

  if (!publishMouseFocus(newPointOfFocus, fmode, owner)) {
    newPointOfFocus = nullptr;
  }

  /*
     Test for line size before applying index. This fixes random assertions because
     of an invalid index. The reason for this is unknown.
   */
  return newPointOfFocus ? ((int)idx < line.size() ? line[idx] : NOPOINTF) : NOPOINTF;
}

bool CGisItemTrk::setMouseFocusByTotalIndex(qint32 idx, focusmode_e fmode, const QString& owner) {
  const CTrackData::trkpt_t* newPointOfFocus = trk.getTrkPtByTotalIndex(idx);
  if (nullptr != newPointOfFocus) {
    return publishMouseFocus(newPointOfFocus, fmode, owner);
  }

  return false;
}

bool CGisItemTrk::setMouseRangeByTotalIndex(qint32 idx1, qint32 idx2, const QString& owner) {
  if (mode != eModeRange || mouseFocusOwner != owner) {
    return false;
  }

  mouseRange1 = trk.getTrkPtByTotalIndex(idx1);
  mouseRange2 = trk.getTrkPtByTotalIndex(idx2);
  setMouseFocusVisuals(mouseRange2);
  setMouseRangeFocusVisuals(mouseRange1, mouseRange2);
  return true;
}

bool CGisItemTrk::incMouseRangeBegin(qint32 i, const QString& owner) {
  if (mouseRange1 == nullptr || mouseRange2 == nullptr) {
    return false;
  }
  const CTrackData::trkpt_t*& pt = mouseRange1->idxTotal < mouseRange2->idxTotal ? mouseRange1 : mouseRange2;
  return moveMouseFocus(pt, qMin(cntTotalPoints - 1, pt->idxTotal + i), owner);
}

bool CGisItemTrk::decMouseRangeBegin(qint32 i, const QString& owner) {
  if (mouseRange1 == nullptr || mouseRange2 == nullptr) {
    return false;
  }
  const CTrackData::trkpt_t*& pt = mouseRange1->idxTotal < mouseRange2->idxTotal ? mouseRange1 : mouseRange2;
  return moveMouseFocus(pt, qMax(0, pt->idxTotal - i), owner);
}

bool CGisItemTrk::incMouseRangeEnd(qint32 i, const QString& owner) {
  if (mouseRange1 == nullptr || mouseRange2 == nullptr) {
    return false;
  }

  const CTrackData::trkpt_t*& pt = mouseRange1->idxTotal > mouseRange2->idxTotal ? mouseRange1 : mouseRange2;
  return moveMouseFocus(pt, qMin(cntTotalPoints - 1, pt->idxTotal + i), owner);
}

bool CGisItemTrk::decMouseRangeEnd(qint32 i, const QString& owner) {
  if (mouseRange1 == nullptr || mouseRange2 == nullptr) {
    return false;
  }
  const CTrackData::trkpt_t*& pt = mouseRange1->idxTotal > mouseRange2->idxTotal ? mouseRange1 : mouseRange2;
  return moveMouseFocus(pt, qMax(0, pt->idxTotal - i), owner);
}

bool CGisItemTrk::moveMouseFocus(const CTrackData::trkpt_t*& pt, qint32 idxNext, const QString& owner) {
  if (mode != eModeRange || mouseFocusOwner != owner) {
    return false;
  }

  rangeState = eRangeStateClicked2nd;

  pt = trk.getTrkPtByTotalIndex(idxNext);
  mouseMoveFocus = pt;

  setMouseFocusVisuals(pt);
  setMouseRangeFocusVisuals(mouseRange1, mouseRange2);
  return true;
}

bool CGisItemTrk::newMouseRangeBegin(const QString& owner) {
  if (mode != eModeRange || mouseFocusOwner != owner || mouseRange1 == nullptr || mouseRange2 == nullptr) {
    return false;
  }

  const CTrackData::trkpt_t* pt = mouseRange1->idxTotal < mouseRange2->idxTotal ? mouseRange1 : mouseRange2;
  mouseRange1 = pt;
  mouseRange2 = pt;
  rangeState = eRangeStateClicked2nd;
  setMouseFocusVisuals(pt);
  setMouseRangeFocusVisuals(mouseRange1, mouseRange2);
  return true;
}

bool CGisItemTrk::newMouseRangeEnd(const QString& owner) {
  if (mode != eModeRange || mouseFocusOwner != owner || mouseRange1 == nullptr || mouseRange2 == nullptr) {
    return false;
  }

  const CTrackData::trkpt_t* pt = mouseRange1->idxTotal > mouseRange2->idxTotal ? mouseRange1 : mouseRange2;
  mouseRange1 = pt;
  mouseRange2 = pt;
  rangeState = eRangeStateClicked2nd;
  setMouseFocusVisuals(pt);
  setMouseRangeFocusVisuals(mouseRange1, mouseRange2);
  return true;
}

void CGisItemTrk::resetMouseRange() {
  mouseClickFocus = nullptr;
  mouseMoveFocus = nullptr;
  mouseRange1 = nullptr;
  mouseRange2 = nullptr;
  rangeState = eRangeStateIdle;
  setMouseFocusVisuals(nullptr);
  setMouseRangeFocusVisuals(nullptr, nullptr);
}

bool CGisItemTrk::publishMouseFocus(const CTrackData::trkpt_t* pt, focusmode_e fmode, const QString& owner) {
  if (mode == eModeRange) {
    if (mouseFocusOwner != owner) {
      return false;
    }

    publishMouseFocusRangeMode(pt, fmode);
  } else {
    publishMouseFocusNormalMode(pt, fmode);
  }

  return true;
}

void CGisItemTrk::publishMouseFocusRangeMode(const CTrackData::trkpt_t* pt, focusmode_e fmode) {
  switch (rangeState) {
    case eRangeStateIdle: {
      if (pt == nullptr) {
        break;
      }

      mouseMoveFocus = pt;
      mouseRange1 = pt;
      mouseRange2 = pt;

      if (fmode == eFocusMouseClick) {
        rangeState = eRangeStateClicked1st;
      }
      break;
    }

    case eRangeStateClicked1st: {
      if (pt == nullptr) {
        break;
      }

      mouseMoveFocus = pt;
      mouseRange2 = pt;
      if (fmode == eFocusMouseClick) {
        rangeState = eRangeStateClicked2nd;
      }
      break;
    }

    case eRangeStateClicked2nd: {
      mouseMoveFocus = pt;

      if (fmode == eFocusMouseClick) {
        if (mouseMoveFocus == mouseRange1) {
          rangeState = eRangeStateMove1st;
        } else if (mouseMoveFocus == mouseRange2) {
          rangeState = eRangeStateMove2nd;
        } else {
          resetMouseRange();
        }
      }
      break;
    }

    case eRangeStateMove1st: {
      if (pt == nullptr) {
        break;
      }

      mouseMoveFocus = pt;
      mouseRange1 = pt;
      if (fmode == eFocusMouseClick) {
        rangeState = eRangeStateClicked2nd;
      }

      break;
    }

    case eRangeStateMove2nd: {
      if (pt == nullptr) {
        break;
      }

      mouseMoveFocus = pt;
      mouseRange2 = pt;
      if (fmode == eFocusMouseClick) {
        rangeState = eRangeStateClicked2nd;
      }

      break;
    }
  }

  setMouseFocusVisuals(pt);
  setMouseRangeFocusVisuals(mouseRange1, mouseRange2);
}
void CGisItemTrk::publishMouseFocusNormalMode(const CTrackData::trkpt_t* pt, focusmode_e fmode) {
  switch (fmode) {
    case eFocusMouseMove:
      if (pt != mouseMoveFocus) {
        mouseMoveFocus = pt;
        setMouseFocusVisuals(pt);
        setMouseRangeFocusVisuals(0, 0);
      }
      break;

    case eFocusMouseClick:
      if (pt != mouseClickFocus) {
        mouseClickFocus = pt;
        setMouseClickFocusVisuals(pt);
      }
  }
}

void CGisItemTrk::changed(const QString& what, const QString& icon) {
  IGisItem::changed(what, icon);
  updateVisuals(eVisualAll, "changed()");
}

void CGisItemTrk::updateHistory(quint32 visuals) {
  IGisItem::updateHistory();
  updateVisuals(visuals, "updateHistory()");
}

void CGisItemTrk::updateVisuals(quint32 visuals, const QString& who) {
  qDebug() << "CGisItemTrk::updateVisuals()" << getName() << who;

  if (!dlgDetails.isNull() && (visuals & eVisualDetails)) {
    dlgDetails->updateData();
  }

  for (INotifyTrk* visual : std::as_const(registeredVisuals)) {
    if (visuals & visual->mask) {
      visual->updateData();
    }
  }

  const CMainWindow& main = CMainWindow::self();
  const QList<CCanvas*>& allCanvas = main.getCanvas();
  for (CCanvas* canvas : allCanvas) {
    canvas->slotUpdateTrackInfo(false);
  }
}

void CGisItemTrk::setMouseFocusVisuals(const CTrackData::trkpt_t* pt) {
  if (!dlgDetails.isNull()) {
    dlgDetails->setMouseFocus(pt);
  }

  for (INotifyTrk* visual : std::as_const(registeredVisuals)) {
    visual->setMouseFocus(pt);
  }
}

void CGisItemTrk::setMouseRangeFocusVisuals(const CTrackData::trkpt_t* pt1, const CTrackData::trkpt_t* pt2) {
  if (!dlgDetails.isNull()) {
    dlgDetails->setMouseRangeFocus(pt1, pt2);
  }

  for (INotifyTrk* visual : std::as_const(registeredVisuals)) {
    visual->setMouseRangeFocus(pt1, pt2);
  }
}

void CGisItemTrk::setMouseClickFocusVisuals(const CTrackData::trkpt_t* pt) {
  if (!dlgDetails.isNull()) {
    dlgDetails->setMouseClickFocus(pt);
  }

  for (INotifyTrk* visual : std::as_const(registeredVisuals)) {
    visual->setMouseClickFocus(pt);
  }
}

void CGisItemTrk::setupInterpolation(bool on, qint32 q) {
  interp.valid = on;
  interp.Q = (quality_e)q;

  if (!on) {
    updateVisuals(eVisualPlot, "setupInterpolation()");
    return;
  }

  const qint32 N = getNumberOfVisiblePoints();
  alglib::real_1d_array x, y;
  x.setlength(N);
  y.setlength(N);

  for (const CTrackData::trkpt_t& trkpt : trk) {
    if (trkpt.isHidden()) {
      continue;
    }

    if (trkpt.ele == NOINT) {
      continue;
    }

    x[trkpt.idxVisible] = trkpt.distance;
    y[trkpt.idxVisible] = trkpt.ele;
  }

  interp.m = interp.Q * N / 10;

  try {
    alglib::spline1dfit(x, y, interp.m, 0.00001, interp.p, interp.rep);
    interp.valid = true;
  } catch (const alglib::ap_error& e) {
    qWarning() << "Error from alglib: " << e.msg.c_str();
  }

  updateVisuals(eVisualPlot, "setupInterpolation()");
}

qreal CGisItemTrk::getElevationInterpolated(qreal d) {
  try {
    return alglib::spline1dcalc(interp.p, d);
  } catch (const alglib::ap_error& e) {
    qWarning() << "Error from alglib: " << e.msg.c_str();
    interp.valid = false;
  }

  return NOFLOAT;
}

void CGisItemTrk::getMouseRange(int& idx1, int& idx2, bool total) const {
  if (nullptr == mouseRange1 || nullptr == mouseRange2) {
    idx1 = NOIDX;
    idx2 = NOIDX;
  } else {
    idx1 = total ? mouseRange1->idxTotal : mouseRange1->idxVisible;
    idx2 = total ? mouseRange2->idxTotal : mouseRange2->idxVisible;

    if (idx1 > idx2) {
      qSwap(idx1, idx2);
    }
  }
}

void CGisItemTrk::getMouseRange(const CTrackData::trkpt_t*& mr1, const CTrackData::trkpt_t*& mr2) const {
  mr1 = mouseRange1;
  mr2 = mouseRange2;
}

const searchValue_t CGisItemTrk::getValueByKeyword(searchProperty_e keyword) {
  if (keywordLambdaMap.contains(keyword)) {
    return keywordLambdaMap.value(keyword)(this);
  }
  return searchValue_t();
}

bool CGisItemTrk::findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32& threshold, QPolygonF& polyline) {
  qreal dist1 = GPS_Math_DistPointPolyline(lineSimple, pt1, threshold);
  qreal dist2 = GPS_Math_DistPointPolyline(lineSimple, pt2, threshold);

  if (dist1 < threshold && dist2 < threshold) {
    trk.getPolyline(polyline);
    threshold = qMin(dist1, dist2);
  }

  return !polyline.isEmpty();
}

void CGisItemTrk::checkForInvalidPoints() {
  IGisProject* project = getParentProject();
  if (project && project->getInvalidDataOk()) {
    return;
  }

  if ((cntInvalidPoints != 0) && (cntInvalidPoints < cntVisiblePoints) && !isOnDevice()) {
    CInvalidTrk dlg(*this, CMainWindow::self().getBestWidgetForParent());
    dlg.exec();
  }
}

QMap<searchProperty_e, CGisItemTrk::fSearch> CGisItemTrk::keywordLambdaMap = CGisItemTrk::initKeywordLambdaMap();
QMap<searchProperty_e, CGisItemTrk::fSearch> CGisItemTrk::initKeywordLambdaMap() {
  QMap<searchProperty_e, CGisItemTrk::fSearch> map;

  // General keywords
  map.insert(eSearchPropertyGeneralName, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->trk.name;
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralFullText, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getInfo(eFeatureShowFullText | eFeatureShowName);
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralElevation, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getMin(CKnownExtension::internalEle);
    searchValue.str1 = CKnownExtension::get(CKnownExtension::internalEle).unit;
    searchValue.value2 = item->getMax(CKnownExtension::internalEle);
    searchValue.str2 = CKnownExtension::get(CKnownExtension::internalEle).unit;
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralDate, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    if (item->timeStart.isValid()) {
      searchValue.value1 = item->timeStart.toSecsSinceEpoch();
      searchValue.str1 = "SsE";
    }
    if (item->timeEnd.isValid()) {
      searchValue.value2 = item->timeEnd.toSecsSinceEpoch();
      searchValue.str2 = "SsE";
    }
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralComment, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getComment();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralDescription, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.str1 = item->getDescription();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralRating, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getRating();
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralKeywords, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.str1 = QStringList(item->getKeywords().values()).join(", ");
    return searchValue;
  });
  map.insert(eSearchPropertyGeneralType, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.str1 = tr("track");
    return searchValue;
  });
  // Route / track keywords
  map.insert(eSearchPropertyRteTrkDistance, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    IUnit::self().meter2distance(item->totalDistance, searchValue.value1, searchValue.str1);
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkAscent, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    IUnit::self().meter2elevation(item->totalAscent, searchValue.value1, searchValue.str1);
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkDescent, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    IUnit::self().meter2elevation(item->totalDescent, searchValue.value1, searchValue.str1);
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkMinElevation, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getMin(CKnownExtension::internalEle);
    searchValue.str1 = CKnownExtension::get(CKnownExtension::internalEle).unit;
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkMaxElevation, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getMax(CKnownExtension::internalEle);
    searchValue.str1 = CKnownExtension::get(CKnownExtension::internalEle).unit;
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkMaxSpeed, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getMax(CKnownExtension::internalSpeedDist);
    searchValue.str1 = CKnownExtension::get(CKnownExtension::internalSpeedDist).unit;
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkMinSpeed, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->getMin(CKnownExtension::internalSpeedDist);
    searchValue.str1 = CKnownExtension::get(CKnownExtension::internalSpeedDist).unit;
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkAvgSpeed, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    IUnit::self().meter2speed(item->totalDistance / item->totalElapsedSecondsMoving, searchValue.value1,
                              searchValue.str1);
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkActivity, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    QStringList strL;
    item->activities.getActivityNames(strL);
    searchValue.str1 = strL.join(", ");
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkTotalTime, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->totalElapsedSeconds;
    searchValue.str1 = "S";
    return searchValue;
  });
  map.insert(eSearchPropertyRteTrkTimeMoving, [](CGisItemTrk* item) {
    searchValue_t searchValue;
    searchValue.value1 = item->totalElapsedSecondsMoving;
    searchValue.str1 = "S";
    return searchValue;
  });

  return map;
}

void CGisItemTrk::toRoute() {
  qint32 idx1, idx2;
  QString routeName;
  bool saveSubPts;
  IGisProject* project = getParentProject();

  if (nullptr == project) {
    qCritical("CGisItemTrk::toRoute no project");
    return;
  }
  getMouseRange(idx1, idx2, true);

  if (NOIDX == idx1) {
    routeName = getName();
  } else {
    routeName = getName() + QString(" (%1-%2)").arg(idx1).arg(idx2);
  }

  CTrkToRteDialog dlg(project, routeName, saveSubPts);
  if (dlg.exec() == QDialog::Rejected) {
    return;
  }

  SGisLine points;
  getPolylineRangeFromData(points, idx1, idx2, saveSubPts);

  new CGisItemRte(points, routeName, project, NOIDX);
}
