/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "widgets/CScaleWidget.h"

#include <QtWidgets>

#include "helpers/CDraw.h"
#include "units/IUnit.h"

constexpr int kBarHeight = 6;
constexpr int kHorMargin = 3;
constexpr int kIndicatorWidth = 5;

CScaleWidget::CScaleWidget(QWidget* parent) : QWidget(parent) { /*setScaledContents(true);*/ }

void CScaleWidget::setValue(qreal min, qreal scale, qreal max) {
  minScale = min;
  maxScale = max;
  currentScale = scale;

  update();
}

void CScaleWidget::paintEvent(QPaintEvent* e) {
  int w = width();
  int h = height();

  QPainter p;
  p.begin(this);
  USE_ANTI_ALIASING(p, true);

  p.fillRect(rect(), Qt::transparent);
  // draw bar background
  const int xBar = kHorMargin;
  const int yBar = (h - kBarHeight) / 2;

  QRect bar(xBar, yBar, w - 2 * kHorMargin, kBarHeight);
  p.setPen(Qt::darkBlue);
  p.setBrush(Qt::white);
  p.drawRect(bar);

  // draw current scale range
  if ((minScale != NOFLOAT) || (maxScale != NOFLOAT)) {
    const int x1Range = minScale != NOFLOAT
                            ? qRound(bar.width() * (1 + log10(minScale)) / 5) - qRound(kIndicatorWidth * 0.5)
                            : bar.left();
    const int x2Range = maxScale != NOFLOAT
                            ? qRound(bar.width() * (1 + log10(maxScale)) / 5) + qRound(kIndicatorWidth * 0.5)
                            : bar.right();
    const int yRange = yBar;

    QRect range(x1Range, yRange, x2Range - x1Range, kBarHeight);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::darkGreen);
    p.drawRect(range);
  }

  // draw scale indicator
  const int xInd = qRound(bar.width() * (1 + log10(currentScale)) / 5) - kHorMargin;
  const int yInd = yBar - 1;

  QRect ind(xInd, yInd, kIndicatorWidth, kBarHeight + 2);
  p.setPen(Qt::darkBlue);
  p.setBrush(Qt::NoBrush);
  p.drawRect(ind);

  p.end();
}
