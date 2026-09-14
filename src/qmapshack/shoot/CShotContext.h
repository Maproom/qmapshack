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

#ifndef CSHOTCONTEXT_H
#define CSHOTCONTEXT_H

#include <QRect>
#include <QSize>
#include <QString>

class CShotWriter;
class QWidget;

/**
   @brief What a shot emits its pictures through.
 */
class CShotContext {
 public:
  explicit CShotContext(const CShotWriter& writer);

  /**
     @brief Start shot @p id.

     @param rect  the crop of every picture, invalid for none
   */
  void begin(const QString& id, const QRect& rect = QRect());

  /**
     @param size  a window's size, invalid for its sizeHint; ignored for a non-window widget
     @return false when no picture was written
   */
  bool shot(QWidget* w, const QSize& size = QSize());

 private:
  /** @return false when the picture is refused and nothing was written */
  bool emitPicture(QWidget* w, const QSize& size, const QString& stem) const;

  const CShotWriter& writer;
  QString id;
  QRect rect;
};

#endif  // CSHOTCONTEXT_H
