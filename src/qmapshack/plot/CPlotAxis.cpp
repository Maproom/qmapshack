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

#include "plot/CPlotAxis.h"

#include <QtWidgets>

inline qreal qLog10(qreal x) { return qLn(x) / qLn(10); }

void CPlotAxis::setLimits(qreal min, qreal max) {
  limitMin = min;
  limitMax = max;
}

void CPlotAxis::setMinMax(qreal givenMin, qreal givenMax) {
  if (givenMin == givenMax) {
    if (0.0 != givenMin) {
      givenMin -= givenMin / 10.0;
      givenMax += givenMax / 10.0;
    } else {
      givenMin -= 0.1;
      givenMax += 0.1;
    }
  }

  if (givenMin > givenMax) {
    qSwap(givenMin, givenMax);
  }

  usedMin = givenMin;
  usedMax = givenMax;

  calc();

  initialized = true;
}

void CPlotAxis::calc() {
  qreal tmpAbs = qFabs(usedMax - usedMin) * ticScale;
  qreal tmp = qLog10(tmpAbs / 10.0);

  qreal exponent = (int)tmp;
  qreal residue = tmp - exponent;

  qreal resSteps[] = {qLog10(0.1), qLog10(0.2), qLog10(0.5), qLog10(1.0), qLog10(2.0), qLog10(5.0), qLog10(10.0)};
  for (const qreal& step : resSteps) {
    if (residue <= step) {
      residue = step;
      break;
    }
  }

  interval = exponent + residue;
  interval = qPow(10, interval) / ticScale;

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

const QString CPlotAxis::fmtsgl(qreal val) {
  QString f;
  int exponent = (0. == val) ? 0 : (int)qLog10(qFabs(val));

  // val *= ticScale;
  if (abs(exponent) > 5) {
    f = "%1.2e";
  } else if (exponent >= 0) {
    f = "%" + QString("%1").arg(exponent + 1) + ((0 == exponent) ? ".1f" : ".0f");
  } else {
    f = "%1." + QString("%1").arg(-exponent + 1) + "f";
  }

  return f;
}

/**
   Generates a asprintf style format string for a given value.
   <pre>
   0.001   -> "%1.4f"
   0.01    -> "%1.3f"
   0.1     -> "%1.2f"
   1       -> "%1.1f"
   10      -> "%2.1f"
   >10000 scientific notation "%1.3e"
   </pre>

   @param val value to calculate the string on

   @return a zero terminated format string

 */
const QString CPlotAxis::fmtdbl(qreal val) {
  int exponent = 0;

  qreal residue = 0;
  val *= ticScale;

  if (val != 0) {
    qreal tmp = qLog10(qFabs(val));
    exponent = (int)tmp;
    residue = tmp - exponent;
  }

  QString f;
  if (abs(exponent) > 5) {
    f = "%1.3e";
  } else if (exponent >= 0) {
    f = "%" + QString("%1").arg(exponent + 1) + (((0. == exponent) && (0 > residue)) ? ".2f" : ".1f");
  } else {
    f = "%1." + QString("%1").arg(-exponent + 2) + "f";
  }
  return f;
}

int CPlotAxis::getScaleWidth(const QFontMetrics& fm) {
  if (!valid) {
    return 0;
  }

  if (scaleWidth > 0) {
    return scaleWidth * fm.horizontalAdvance("X");
  }

  int width = 6 * fm.horizontalAdvance("X");
  QString format_single_prec = ((interval * ticScale) < 1) ? fmtdbl(interval) : fmtsgl(interval);

  const tic_t* t = ticmark();
  while (nullptr != t) {
    int tmp = fm.horizontalAdvance(QString::asprintf(format_single_prec.toLatin1().data(), t->val * ticScale));
    width = qMax(width, tmp);

    t = ticmark(t);
  }
  return width;
}

void CPlotAxis::getLimits(qreal& limMin, qreal& limMax, qreal& useMin, qreal& useMax) {
  limMin = limitMin;
  limMax = limitMax;
  useMin = usedMin;
  useMax = usedMax;
}

const CPlotAxis::tic_t* CPlotAxis::ticmark(const tic_t* t) {
  QString format_single_prec = ((interval * ticScale) < 1) ? fmtdbl(interval) : fmtsgl(interval);

  switch (ticType) {
    case eNoTic:
      return nullptr;
      break;

    case eTicMinMax:
      if (nullptr == t) {
        tic.val = usedMin;
        firstTic = true;
      } else if (firstTic) {
        tic.val = usedMax;
        firstTic = false;
      } else {
        return nullptr;
      }
      break;

    case eTicNorm:
      if (0. == interval) {
        // qWarning() << "CPlotAxis::ticmark() mode 'norm': interval == 0";
        return nullptr;
      }
      if (nullptr == t) {
        tic.val = ticStart;
      } else {
        tic.val += interval;
        if (tic.val > usedMax) {
          return nullptr;
        }
      }
      break;

    case eTicFull:
      if (nullptr == t) {
        tic.val = usedMin;
        firstTic = true;
      } else if (firstTic) {
        tic.val = ticStart;
        firstTic = false;
      } else if (lastTic) {
        lastTic = false;
        return nullptr;
      } else {
        tic.val += interval;
        if (tic.val > usedMax) {
          tic.val = usedMax;
          lastTic = true;
        }
      }
      break;
  }

  tic.lbl = QString::asprintf(format_single_prec.toLatin1(), tic.val * ticScale);

  return &tic;
}

void CPlotAxis::setScale(const unsigned int pts) {
  // if ( !initialized )
  // qWarning( "you try to set the scale before defining the min & max value. not very sensible." );
  points = pts;
  scale = pts / (usedMax - usedMin);
}

void CPlotAxis::resetZoom() { setMinMax(limitMin, limitMax); }

void CPlotAxis::zoom(bool in, int point) {
  qreal factor = in ? (1 / 1.1) : 1.1;

  qreal p = pt2val(point);
  qreal min = (p - usedMin) * (1 - factor) + usedMin;
  qreal d = min - usedMin * factor;
  qreal max = usedMax * factor + d;

  if (qRound(max - min) <= qRound(limitMax - limitMin)) {
    setMinMax(min, max);
    move(0);
  }
}

void CPlotAxis::move(int delta_pt) {
  qreal delta_val = pt2val(delta_pt) - pt2val(0);
  bool f = !(usedMax - usedMin < limitMax - limitMin);
  if (f ^ (usedMin + delta_val < limitMin)) {
    delta_val = (limitMin - usedMin);
  }
  if (f ^ (usedMax + delta_val > limitMax)) {
    delta_val = (limitMax - usedMax);
  }
  setMinMax(usedMin + delta_val, usedMax + delta_val);
}
