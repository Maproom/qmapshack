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

#ifndef CSHOTWRITER_H
#define CSHOTWRITER_H

#include <QImage>
#include <QString>

class QWidget;

/** @brief Renders a widget to PNG. */
class CShotWriter {
 public:
  explicit CShotWriter(const QString& outDir);

  /** @brief Polish, end running animations and let the layout settle. */
  static void settle(QWidget* w);

  /**
     @brief Wait until every canvas in @p w is complete and two renders agree.

     @return false when a map is hidden, timed out or drew a hole
   */
  static bool settleStable(QWidget* w);

  /**
     @brief Settle, size and render @p w at dpr 1.

     @param size  a window's size, invalid for its sizeHint; ignored for a non-window widget
     @return a null image when a map is incomplete
   */
  static QImage render(QWidget* w, const QSize& size);

  /** @return the path written, or an empty string on failure */
  QString write(const QImage& image, const QString& id) const;

  void setOutDir(const QString& dir) { outDir = dir; }
  const QString& getOutDir() const { return outDir; }

 private:
  static QImage renderAtDpr1(QWidget* w);

  QString outDir;
};

#endif  // CSHOTWRITER_H
