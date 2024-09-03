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

#include "plot/CPlotData.h"

#include "plot/CPlotAxis.h"
#include "plot/CPlotAxisTime.h"
#include "units/IUnit.h"

CPlotData::CPlotData(axistype_e type, QObject* parent) : QObject(parent), axisType(type) {
  setXAxisType(type);
  yaxis = new CPlotAxis(this);
}

CPlotData::~CPlotData() {}

void CPlotData::setXAxisType(axistype_e type) {
  delete xaxis;

  if (type == eAxisLinear) {
    xaxis = new CPlotAxis(this);
  } else {
    xaxis = new CPlotAxisTime(this);
  }
  xaxis->setAutoscale(false);

  axisType = type;
}

void CPlotData::setLimits() {
  if (lines.size() == 0 || badData) {
    return;
  }

  {
    const QPointF& p = lines.first().points.first();
    xmin = p.x();
    xmax = p.x();
    ymin = p.y();
    ymax = p.y();
  }

  for (const line_t& line : std::as_const(lines)) {
    const QPolygonF& points = line.points;
    for (const QPointF& p : points) {
      if (p.y() != NOFLOAT) {
        xmin = qMin(xmin, p.x());
        xmax = qMax(xmax, p.x());
        ymin = qMin(ymin, p.y());
        ymax = qMax(ymax, p.y());
      }
    }
  }

  if (xmin == xmax) {
    badData = true;
  } else {
    xaxis->setLimits(xmin, xmax);
    yaxis->setLimits(ymin, ymax);
  }
}
