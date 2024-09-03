/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "plot/CPlotProfile.h"

#include <interpolation.h>

#include "CMainWindow.h"
#include "gis/proj_x.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "plot/CPlotAxis.h"
#include "units/IUnit.h"

CPlotProfile::CPlotProfile(QWidget* parent) : IPlot(nullptr, CPlotData::eAxisLinear, eModeNormal, parent) { init(); }

CPlotProfile::CPlotProfile(CGisItemTrk* trk, CLimit& lim, mode_e mode, QWidget* parent)
    : IPlot(trk, CPlotData::eAxisLinear, mode, parent), limit(&lim) {
  init();
  connect(limit, &CLimit::sigChanged, this, &CPlotProfile::setLimits);
  setWindowTitle(trk->getNameEx());
  CPlotProfile::updateData();
}

CPlotProfile::~CPlotProfile() {}

void CPlotProfile::init() {
  if (mode != eModeIcon) {
    SETTINGS;
    cfg.beginGroup("Plot");
    showWptLabels = cfg.value("showWptLabels", showWptLabels).toBool();
    cfg.endGroup();

    menu->addSeparator();
    QAction* action = new QAction(tr("Show Labels"), this);
    action->setCheckable(true);
    action->setChecked(showWptLabels);
    menu->addAction(action);
    connect(action, &QAction::toggled, this, &CPlotProfile::slotToggleWptLabels);
  }
}

void CPlotProfile::setTrack(CGisItemTrk* track, CLimit& lim) {
  trk = track;
  trk->registerVisual(this);

  if (limit) {
    disconnect(limit, &CLimit::sigChanged, this, &CPlotProfile::setLimits);
  }
  limit = &lim;
  connect(limit, &CLimit::sigChanged, this, &CPlotProfile::setLimits);

  updateData();
}

#define Q 32
void CPlotProfile::updateData() {
  clear();

  qreal scale;
  QString unit;
  IUnit::self().meter2unit(trk->getTotalDistance(), scale, unit);
  setXTicScale(scale);

  if (mode == eModeIcon) {
    setXLabel(trk->getName());
    setYLabel("");
  } else {
    setXLabel(tr("Distance [%1]").arg(unit));
    setYLabel(tr("Ele. [%1]").arg(IUnit::self().elevationUnit));
  }

  QPolygonF lineEle;
  QPolygonF lineDem;
  QPolygonF coords;

  IGisProject* project = dynamic_cast<IGisProject*>(trk->parent());

  qreal elevationFactor = IUnit::self().elevationFactor;
  const CTrackData& t = trk->getTrackData();
  for (const CTrackData::trkpt_t& trkpt : t) {
    if (trkpt.isHidden()) {
      continue;
    }

    if (trkpt.ele == NOINT) {
      continue;
    }

    lineEle << QPointF(trkpt.distance, trkpt.ele * elevationFactor);
    coords << QPointF(trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD);
    lineDem << QPointF(trkpt.distance, NOFLOAT);

    if (nullptr == project || trkpt.keyWpt.item.isEmpty() || (mode == eModeIcon)) {
      continue;
    }

    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(project->getItemByKey(trkpt.keyWpt));
    if (wpt) {
      CPlotData::point_t tag;
      tag.point = lineEle.last();
      tag.icon = wpt->getIcon();
      tag.label = wpt->getName();
      data->tags << tag;
    }
  }

  if ((mode != eModeIcon) && CMainWindow::self().isShowTrackInfoPoints()) {
    qint32 cnt = 1;
    for (const CTrackData::trkpt_t& trkpt : trk->getTrackData()) {
      if (trkpt.desc.isEmpty()) {
        continue;
      }

      CPlotData::point_t tag;
      tag.point = QPointF(trkpt.distance, trkpt.ele * elevationFactor);
      tag.icon = CDraw::number(cnt++, Qt::black);
      tag.label = trkpt.desc.size() < 20 ? trkpt.desc : trkpt.desc.left(17) + "...";
      data->tags << tag;
    }
  }

  CMainWindow::self().getElevationAt(coords, lineDem);
  for (QPointF& pt : lineDem) {
    pt.ry() *= elevationFactor;
  }

  newLine(lineEle, "GPS");
  if (!lineDem.isEmpty()) {
    addLine(lineDem, "DEM");
  }

  if (trk->isInterpolationEnabled()) {
    QPolygonF spline;
    for (const QPointF& pt : std::as_const(lineEle)) {
      spline << QPointF(pt.x(), trk->getElevationInterpolated(pt.x()) * elevationFactor);
    }

    addLine(spline, "Interp.");
  }

  setLimits();
  resetZoom();
}

void CPlotProfile::setMouseFocus(const CTrackData::trkpt_t* ptMouseMove) {
  if (nullptr == ptMouseMove) {
    if (posMouse1 != NOPOINT) {
      posMouse1 = NOPOINT;
      needsRedraw = true;
    }
  } else {
    if (posMouse1 == NOPOINT) {
      needsRedraw = true;
    }

    posMouse1.rx() = left + data->x().val2pt(ptMouseMove->distance);
    posMouse1.ry() = top + data->y().val2pt(ptMouseMove->ele);
  }
  update();
}

void CPlotProfile::setLimits() {
  IPlot::setLimits();
  data->ymin = limit->getMin() == NOFLOAT ? data->ymin : limit->getMin();
  data->ymax = limit->getMax() == NOFLOAT ? data->ymax : limit->getMax();

  data->y().setLimits(data->ymin, data->ymax);
  resetZoom();
  update();
}

void CPlotProfile::slotToggleWptLabels(bool on) {
  showWptLabels = on;

  if (mode != eModeIcon) {
    SETTINGS;
    cfg.beginGroup("Plot");
    cfg.setValue("showWptLabels", showWptLabels);
    cfg.endGroup();
  }

  update();
}
