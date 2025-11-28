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

#ifndef CFADINGLABEL_H
#define CFADINGLABEL_H

#include <QLabel>

class QPropertyAnimation;
class QGraphicsOpacityEffect;

class CFadingLabel : public QLabel {
  Q_OBJECT
 public:
  explicit CFadingLabel(QWidget* parent = nullptr);

  void fadeOut(int duration = 300);
  void fadeIn(int duration = 300);

 private:
  enum class eState {
    Unknown,
    In,
    Out,
  };
  eState state = eState::Unknown;
  QPropertyAnimation* fadeAnim;
  QGraphicsOpacityEffect* effect;
};

#endif  // CFADINGLABEL_H
