/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CHELPBLOCK_H
#define CHELPBLOCK_H

#include <QFrame>

/**
   @brief A hint card: tinted background, an accent bar and the help icon, with the hint labels laid out inside.

   The left contents margin keeps the labels clear of bar and icon. Colours are read from the palette on each
   paint, so the card follows a change of the colour scheme.
 */
class CHelpBlock : public QFrame {
  Q_OBJECT
 public:
  explicit CHelpBlock(QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* e) override;
};

#endif  // CHELPBLOCK_H
