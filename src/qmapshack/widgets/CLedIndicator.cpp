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

#include "CLedIndicator.h"

#include <QEasingCurve>
#include <QPainter>
#include <QPropertyAnimation>

CLedIndicator::CLedIndicator(QWidget* parent)
    : QWidget(parent), m_displayColor(Qt::lightGray), m_targetColor(Qt::lightGray) {
  // --- Color animation ---
  m_colorAnim = new QPropertyAnimation(this, "animatedColor", this);
  m_colorAnim->setDuration(250);
  m_colorAnim->setEasingCurve(QEasingCurve::InOutQuad);
  connect(m_colorAnim, &QPropertyAnimation::finished, this, [this]() { setAnimatedColor(m_targetColor); });

  // --- Opacity animation ---
  m_opacityAnim = new QPropertyAnimation(this, "opacity", this);
  m_opacityAnim->setDuration(250);
  m_opacityAnim->setEasingCurve(QEasingCurve::InOutQuad);
}

QColor CLedIndicator::color() const { return m_targetColor; }

void CLedIndicator::setColor(const QColor& c) {
  if (c == m_targetColor) return;

  m_targetColor = c;

  m_colorAnim->stop();
  m_colorAnim->setStartValue(m_displayColor);
  m_colorAnim->setEndValue(c);
  m_colorAnim->start();
}

// --- animatedColor property ---
QColor CLedIndicator::animatedColor() const { return m_displayColor; }

void CLedIndicator::setAnimatedColor(const QColor& c) {
  if (c == m_displayColor) return;
  m_displayColor = c;
  update();
  emit animatedColorChanged();
}

// --- opacity property ---
qreal CLedIndicator::opacity() const { return m_opacity; }

void CLedIndicator::setOpacity(qreal o) {
  if (o == m_opacity) return;
  m_opacity = o;
  update();
  emit opacityChanged();
}

// --- hide/show animations ---
void CLedIndicator::animateHide() {
  m_opacityAnim->stop();
  m_opacityAnim->setDuration(250);
  m_opacityAnim->setStartValue(m_opacity);
  m_opacityAnim->setEndValue(0.0);
  m_opacityAnim->setLoopCount(1);
  m_opacityAnim->start();
}

void CLedIndicator::animateShow() {
  m_opacityAnim->stop();
  m_opacityAnim->setDuration(250);
  m_opacityAnim->setStartValue(m_opacity);
  m_opacityAnim->setEndValue(1.0);
  m_opacityAnim->setLoopCount(1);
  m_opacityAnim->start();
}

void CLedIndicator::animateFlash(bool on) {
  m_opacityAnim->stop();
  if (on) {
    m_opacityAnim->setDuration(500);
    m_opacityAnim->setStartValue(1.0);
    m_opacityAnim->setEndValue(0.0);
    m_opacityAnim->setLoopCount(-1);
    m_opacityAnim->setDirection(QAbstractAnimation::Forward);
    m_opacityAnim->start();
  } else {
    animateShow();
  }
}

void CLedIndicator::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  p.setOpacity(m_opacity);
  p.setBrush(m_displayColor);
  p.setPen(Qt::NoPen);

  p.drawRoundedRect(rect(), 4, 4);
}
