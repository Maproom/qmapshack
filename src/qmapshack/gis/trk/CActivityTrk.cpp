/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/trk/CActivityTrk.h"

#include <functional>

#include "gis/CGisWorkspace.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CSettings.h"
#include "units/IUnit.h"

QVector<CActivityTrk::desc_t> CActivityTrk::actDescriptor;

#define DESCRIPTOR_ENTRY(name, icon, enumAct, colorIdx)                                 \
  {                                                                                     \
    CTrackData::trkpt_t::enumAct, name, "://icons/48x48/" icon, "://icons/16x16/" icon, \
        IGisItem::getColorMap()[qMin(colorIdx, IGisItem::eColorTransparent)].color,     \
        IGisItem::getColorMap()[qMin(colorIdx, IGisItem::eColorTransparent)].line       \
  }

CActivityTrk::desc_t CActivityTrk::dummyDesc;

CActivityTrk::CActivityTrk(CGisItemTrk* trk) : trk(trk) {}

void CActivityTrk::init() {
  dummyDesc = DESCRIPTOR_ENTRY("-", "ActNone.png", eAct20None, IGisItem::eColorTransparent);

  actDescriptor = {DESCRIPTOR_ENTRY(tr("Foot"), "ActFoot.png", eAct20Foot, IGisItem::eColorBlack),
                   DESCRIPTOR_ENTRY(tr("Bicycle"), "ActCycle.png", eAct20Cycle, IGisItem::eColorDarkRed),
                   DESCRIPTOR_ENTRY(tr("Motor Bike"), "ActBike.png", eAct20Bike, IGisItem::eColorDarkGreen),
                   DESCRIPTOR_ENTRY(tr("Car"), "ActCar.png", eAct20Car, IGisItem::eColorDarkYellow),
                   DESCRIPTOR_ENTRY(tr("Cable Car"), "ActCable.png", eAct20Cable, IGisItem::eColorDarkBlue),
                   DESCRIPTOR_ENTRY(tr("Swim"), "ActSwim.png", eAct20Swim, IGisItem::eColorDarkMagenta),
                   DESCRIPTOR_ENTRY(tr("Ship"), "ActShip.png", eAct20Ship, IGisItem::eColorDarkCyan),
                   DESCRIPTOR_ENTRY(tr("Aeronautic"), "ActAero.png", eAct20Aero, IGisItem::eColorLightGray),
                   DESCRIPTOR_ENTRY(tr("Ski/Winter"), "ActSki.png", eAct20Ski, IGisItem::eColorDarkGray),
                   DESCRIPTOR_ENTRY(tr("Public Transport"), "ActTrain.png", eAct20Train, IGisItem::eColorRed)};

  SETTINGS;
  cfg.beginGroup("Activities");
  int i = 0;
  for (desc_t& desc : actDescriptor) {
    desc.color = QColor(cfg.value(QString("color%1").arg(i), desc.color.name()).toString());
    desc.line = cfg.value(QString("line%1").arg(i), desc.line).toString();
    ++i;
  }
  cfg.endGroup();  // Activities
}

void CActivityTrk::release() {
  SETTINGS;
  cfg.beginGroup("Activities");
  int i = 0;
  for (desc_t& desc : actDescriptor) {
    cfg.setValue(QString("color%1").arg(i), desc.color.name());
    cfg.setValue(QString("line%1").arg(i), desc.line);
    ++i;
  }
  cfg.endGroup();  // Activities
}

QMenu* CActivityTrk::getMenu(const IGisItem::key_t& key, QWidget* parent, bool execute) {
  QList<IGisItem::key_t> keys;
  keys << key;
  return getMenu(keys, parent, execute);
}

QMenu* CActivityTrk::getMenu(const QList<IGisItem::key_t>& keys, QWidget* parent, bool execute) {
  QMenu* menu = new QMenu(tr("Set Track Activity"), parent);
  menu->setIcon(QIcon("://icons/32x32/Activity.png"));
  QAction* act;

  act = menu->addAction(QIcon("://icons/32x32/ActNone.png"), tr("No Activity"));
  auto func = [keys]() { CGisWorkspace::self().slotActivityTrkByKey(keys, CTrackData::trkpt_t::eAct20None); };
  QAction::connect(act, &QAction::triggered, &CGisWorkspace::self(), func);

  for (const desc_t& desc : std::as_const(actDescriptor)) {
    act = menu->addAction(QIcon(desc.iconLarge), desc.name);
    auto func = [keys, desc]() { CGisWorkspace::self().slotActivityTrkByKey(keys, desc.activity); };
    QAction::connect(act, &QAction::triggered, &CGisWorkspace::self(), func);
  }

  if (execute) {
    menu->exec(QCursor::pos());
    delete menu;
    return nullptr;
  }

  return menu;
}

void CActivityTrk::updateFlags() {
  const CTrackData& data = trk->getTrackData();
  trkact_t lastAct = CTrackData::trkpt_t::eAct20Bad;

  for (const CTrackData::trkpt_t& pt : data) {
    pt.unsetFlag(CTrackData::trkpt_t::eFlagActivity);
    if (lastAct != pt.getAct()) {
      pt.setActivityFlag();
      lastAct = pt.getAct();
    }
  }

  const CTrackData::trkpt_t* last = data.last();
  if (last != nullptr) {
    last->setActivityFlag();
  }
}

void CActivityTrk::update() {
  allActivities.clear();
  activityRanges.clear();
  activitySummary.clear();

  const CTrackData& data = trk->getTrackData();
  const CTrackData::trkpt_t* lastTrkpt = nullptr;
  const CTrackData::trkpt_t* startTrkpt = nullptr;

  trkact_t lastAct = CTrackData::trkpt_t::eAct20Bad;
  for (const CTrackData::trkpt_t& pt : data) {
    allActivities << pt.getAct();

    lastTrkpt = &pt;
    if (pt.getAct() != lastAct) {
      if (startTrkpt != nullptr) {
        summary_t& summary = activitySummary[lastAct];
        summary.distance += lastTrkpt->distance - startTrkpt->distance;
        summary.ascent += lastTrkpt->ascent - startTrkpt->ascent;
        summary.descent += lastTrkpt->descent - startTrkpt->descent;
        summary.ellapsedSeconds += lastTrkpt->elapsedSeconds - startTrkpt->elapsedSeconds;
        summary.ellapsedSecondsMoving += lastTrkpt->elapsedSecondsMoving - startTrkpt->elapsedSecondsMoving;

        activityRanges << range_t();
        range_t& activity = activityRanges.last();

        activity.idxTotalBeg = startTrkpt->idxTotal;
        activity.idxTotalEnd = lastTrkpt->idxTotal;
        activity.activity = lastAct;
      }

      startTrkpt = &pt;
      lastAct = pt.getAct();
    }
  }

  if (lastTrkpt == nullptr || startTrkpt == nullptr) {
    return;
  }

  summary_t& summary = activitySummary[lastAct];
  summary.distance += lastTrkpt->distance - startTrkpt->distance;
  summary.ascent += lastTrkpt->ascent - startTrkpt->ascent;
  summary.descent += lastTrkpt->descent - startTrkpt->descent;
  summary.ellapsedSeconds += lastTrkpt->elapsedSeconds - startTrkpt->elapsedSeconds;
  summary.ellapsedSecondsMoving += lastTrkpt->elapsedSecondsMoving - startTrkpt->elapsedSecondsMoving;

  activityRanges << range_t();
  range_t& activity = activityRanges.last();

  activity.idxTotalBeg = startTrkpt->idxTotal;
  activity.idxTotalEnd = lastTrkpt->idxTotal;
  activity.activity = lastAct;

  //    for(int i = 0; i < 9; i++)
  //    {
  //        summary_t& stat   = summaries[i];
  //        qDebug() << "--------------" << i << "--------------";
  //        qDebug() << "stat.distance" << stat.distance;
  //        qDebug() << "stat.ascent" << stat.ascent;
  //        qDebug() << "stat.descent" << stat.descent;
  //        qDebug() << "stat.timeMoving" << stat.ellapsedSecondsMoving;
  //        qDebug() << "stat.timeTotal" << stat.ellapsedSeconds;
  //    }
}

void CActivityTrk::printSummary(QString& str) const { printSummary(activitySummary, allActivities, str); }

void CActivityTrk::printSummary(const QMap<trkact_t, summary_t>& summary, const QSet<trkact_t>& acts, QString& str) {
  QString val, unit;
  qreal total;
  qreal distance;
  bool printTotal = false;
  bool printNoAct = false;

  str += "<table>";

  // gather any used activities
  QVector<const desc_t*> descs;
  for (const desc_t& desc : std::as_const(actDescriptor)) {
    if (acts.contains(desc.activity)) {
      descs << &desc;
    }
  }

  const summary_t& sumActNone = summary[CTrackData::trkpt_t::eAct20None];

  if (sumActNone.distance != 0) {
    printNoAct = true;
  }
  if (descs.size() > 1 || (descs.size() == 1 && printNoAct)) {
    printTotal = true;
  }

  // ############### build header ###############
  str += "<tr>";
  str += "<th></th>";
  for (const desc_t* desc : std::as_const(descs)) {
    str += QString(
               "<th align='right'>"
               "<img src='%1'/><br/>"
               "<img src='%2'/>"
               "</th>")
               .arg(desc->iconSmall, desc->line);
  }
  if (printNoAct) {
    str += QString("<th align='right'><img src='://icons/16x16/ActNone.png'/></th>");
  }
  if (printTotal) {
    str += "<th align='right'>" + tr("Total") + "</th>";
  }
  str += "</tr>";

  // ############### build Distance row ###############
  str += "<tr>";
  str += "<td>" + tr("Distance:") + "</td>";
  distance = 0;
  for (const desc_t* desc : std::as_const(descs)) {
    const summary_t& s = summary[desc->activity];
    IUnit::self().meter2distance(s.distance, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val, unit);
    distance += s.distance;
  }
  if (printNoAct) {
    IUnit::self().meter2distance(sumActNone.distance, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val, unit);
    distance += sumActNone.distance;
  }
  if (printTotal) {
    IUnit::self().meter2distance(distance, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1 %2</td>").arg(val, unit);
  }
  str += "</tr>";

  // ############### build Ascent row ###############
  str += "<tr>";
  str += "<td>" + tr("Ascent:") + "</td>";
  total = 0;
  for (const desc_t* desc : std::as_const(descs)) {
    const summary_t& s = summary[desc->activity];
    IUnit::self().meter2elevation(qMin(s.ascent, NOFLOAT), val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += s.ascent;
  }
  if (printNoAct) {
    IUnit::self().meter2elevation(qMin(sumActNone.ascent, NOFLOAT), val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += sumActNone.ascent;
  }
  if (printTotal) {
    IUnit::self().meter2elevation(qMin(total, NOFLOAT), val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
  }
  str += "</tr>";

  // ############### build Descent row ###############
  str += "<tr>";
  str += "<td>" + tr("Descent:") + "</td>";
  total = 0;
  for (const desc_t* desc : std::as_const(descs)) {
    const summary_t& s = summary[desc->activity];
    IUnit::self().meter2elevation(qMin(s.descent, NOFLOAT), val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += s.descent;
  }
  if (printNoAct) {
    IUnit::self().meter2elevation(qMin(sumActNone.descent, NOFLOAT), val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += sumActNone.descent;
  }
  if (printTotal) {
    IUnit::self().meter2elevation(qMin(total, NOFLOAT), val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
  }
  str += "</tr>";

  // ############### build Speed Moving row ###############
  str += "<tr>";
  str += "<td>" + tr("Speed Moving:") + "</td>";
  total = 0;
  for (const desc_t* desc : std::as_const(descs)) {
    const summary_t& s = summary[desc->activity];
    IUnit::self().meter2speed(s.distance / s.ellapsedSecondsMoving, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += s.ellapsedSecondsMoving;
  }
  if (printNoAct) {
    IUnit::self().meter2speed(sumActNone.distance / sumActNone.ellapsedSecondsMoving, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += sumActNone.ellapsedSecondsMoving;
  }
  if (printTotal) {
    IUnit::self().meter2speed(distance / total, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
  }
  str += "</tr>";

  // ############### build Speed row ###############
  str += "<tr>";
  str += "<td>" + tr("Speed Total:") + "</td>";
  total = 0;
  for (const desc_t* desc : std::as_const(descs)) {
    const summary_t& s = summary[desc->activity];
    IUnit::self().meter2speed(s.distance / s.ellapsedSeconds, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += s.ellapsedSeconds;
  }
  if (printNoAct) {
    IUnit::self().meter2speed(sumActNone.distance / sumActNone.ellapsedSeconds, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += sumActNone.ellapsedSeconds;
  }
  if (printTotal) {
    IUnit::self().meter2speed(distance / total, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
  }
  str += "</tr>";

  // ############### build Time Moving row ###############
  str += "<tr>";
  str += "<td>" + tr("Time Moving:") + "</td>";
  total = 0;
  for (const desc_t* desc : std::as_const(descs)) {
    const summary_t& s = summary[desc->activity];
    IUnit::self().seconds2time(s.ellapsedSecondsMoving, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += s.ellapsedSecondsMoving;
  }
  if (printNoAct) {
    IUnit::self().seconds2time(sumActNone.ellapsedSecondsMoving, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += sumActNone.ellapsedSecondsMoving;
  }
  if (printTotal) {
    IUnit::self().seconds2time(total, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
  }
  str += "</tr>";

  // ############### build Time Moving row ###############
  str += "<tr>";
  str += "<td>" + tr("Time Total:") + "</td>";
  total = 0;
  for (const desc_t* desc : std::as_const(descs)) {
    const summary_t& s = summary[desc->activity];
    IUnit::self().seconds2time(s.ellapsedSeconds, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += s.ellapsedSeconds;
  }
  if (printNoAct) {
    IUnit::self().seconds2time(sumActNone.ellapsedSeconds, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
    total += sumActNone.ellapsedSeconds;
  }
  if (printTotal) {
    IUnit::self().seconds2time(total, val, unit);
    str += QString("<td align='right'>&nbsp;&nbsp;%1%2</td>").arg(val, unit);
  }
  str += "</tr>";

  str += "</table>";
}

void CActivityTrk::sumUp(QMap<trkact_t, summary_t>& summary) const {
  const QList<trkact_t>& acts = activitySummary.keys();
  for (const trkact_t& act : acts) {
    const summary_t& src = activitySummary[act];
    summary_t& dst = summary[act];

    dst.distance += src.distance;
    dst.ascent += src.ascent;
    dst.descent += src.descent;
    dst.ellapsedSeconds += src.ellapsedSeconds;
    dst.ellapsedSecondsMoving += src.ellapsedSecondsMoving;
  }
}

const CActivityTrk::desc_t& CActivityTrk::getDescriptor(trkact_t act) {
  for (const desc_t& desc : std::as_const(actDescriptor)) {
    if (desc.activity == act) {
      return desc;
    }
  }

  return dummyDesc;
}

void CActivityTrk::setColor(trkact_t act, const IGisItem::color_t& color) {
  for (desc_t& desc : actDescriptor) {
    if (desc.activity == act) {
      desc.color = color.color;
      desc.line = color.line;
      return;
    }
  }
}

void CActivityTrk::getActivityNames(QStringList& names) const {
  for (const desc_t& desc : std::as_const(actDescriptor)) {
    if (allActivities.contains(desc.activity)) {
      names << desc.name;
    }
  }
}
