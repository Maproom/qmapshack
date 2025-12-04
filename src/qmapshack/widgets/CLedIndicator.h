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

#ifndef CLEDINDICATOR_H
#define CLEDINDICATOR_H

#include <QColor>
#include <QWidget>

class QPropertyAnimation;

class CLedIndicator final : public QWidget {
  Q_OBJECT

  // Animatable color property
  Q_PROPERTY(QColor animatedColor READ animatedColor WRITE setAnimatedColor NOTIFY animatedColorChanged)

  // Animatable opacity property for show/hide animation
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

 public:
  explicit CLedIndicator(QWidget* parent = nullptr);

  QColor color() const;
  void setColor(const QColor& c);

  // hide/show animations
  void animateHide();
  void animateShow();
  void animateFlash(bool on);

  qreal opacity() const;
  void setOpacity(qreal o);

 protected:
  void paintEvent(QPaintEvent* e) override;

 private:
  // property accessors for color animation
  QColor animatedColor() const;
  void setAnimatedColor(const QColor& c);

 signals:
  void animatedColorChanged();
  void opacityChanged();

 private:
  QColor m_displayColor;  // animated color currently shown
  QColor m_targetColor;   // target (requested) color

  qreal m_opacity = 1.0;  // animated opacity

  QPropertyAnimation* m_colorAnim;
  QPropertyAnimation* m_opacityAnim;
};

#endif  // CLEDINDICATOR_H
