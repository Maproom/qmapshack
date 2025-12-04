/**********************************************************************************************
    Copyright (C) 2025 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "CFadingLabel.h"

#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPropertyAnimation>

CFadingLabel::CFadingLabel(QWidget* parent) : QLabel(parent) {
  effect = new QGraphicsOpacityEffect(this);
  setGraphicsEffect(effect);

  fadeAnim = new QPropertyAnimation(effect, "opacity", this);
  fadeAnim->setEasingCurve(QEasingCurve::InOutQuad);
}

void CFadingLabel::fadeOut(int duration) {
  if (state == eState::Out) {
    return;
  }
  fadeAnim->stop();
  fadeAnim->setDuration(duration);
  fadeAnim->setStartValue(windowOpacity());
  fadeAnim->setEndValue(0.0);
  fadeAnim->start();
  state = eState::Out;
}

void CFadingLabel::fadeIn(int duration) {
  if (state == eState::In) {
    return;
  }
  fadeAnim->stop();
  fadeAnim->setDuration(duration);
  fadeAnim->setStartValue(windowOpacity());
  fadeAnim->setEndValue(1.0);
  fadeAnim->start();
  state = eState::In;
}
