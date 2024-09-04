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

#include "plot/CPlotAxisTime.h"

#include <QtWidgets>

void CPlotAxisTime::calc() /* override */
{
  int dSec = usedMax - usedMin;
  ticStart = usedMin;

  // abort if the xRange has a length of 0
  if (0 == dSec) {
    valid = false;
    return;
  }

  if (dSec < 0) {
    qDebug() << "ouch";
    valid = false;
    return;
  } else if (dSec < 20) {
    interval = 1;
    ticStart = usedMin;
  } else if (dSec < 100) {
    interval = 5;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 200) {
    interval = 10;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 600) {
    interval = 30;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 1200) {
    interval = 60;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 6000) {
    interval = 600;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 12000) {
    interval = 600;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 36000) {
    interval = 1800;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 72000) {
    interval = 3600;
    ticStart = qCeil(usedMin / interval) * interval;
  } else if (dSec < 216000) {
    interval = 10800;
    ticStart = qCeil(usedMin / interval) * interval;
  } else {
    qDebug() << "ouch";
    valid = false;
    return;
  }

  if (autoscale) {
    usedMin = qFloor(usedMin / interval) * interval;
    usedMax = qCeil(usedMax / interval) * interval;
  }

  int t1 = (int)(usedMin / interval + 0.5);
  ticStart = interval * t1;
  if (ticStart < usedMin) {
    ticStart += interval;
  }

  valid = true;
}

const CPlotAxis::tic_t* CPlotAxisTime::ticmark(const tic_t* t) /* override */
{
  const tic_t* _tic_ = CPlotAxis::ticmark(t);
  if (_tic_) {
    QDateTime time = QDateTime::fromSecsSinceEpoch(tic.val);
    time.setTimeSpec(Qt::LocalTime);
    tic.lbl = time.toString(strFormat);
  }

  return _tic_;
}
