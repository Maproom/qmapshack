/**********************************************************************************************
    Copyright (C) 2026 Gert Pellin <gert@pellin.be>

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

#ifndef CSURFACEWIDGET_H
#define CSURFACEWIDGET_H

#include <QWidget>

#include "gis/trk/surface/CSurfaceTrk.h"

class CSurfaceBar;
class QLabel;
class QToolButton;

/**
   @brief Surfaces and way types of a track, for the track's Info tab

   Two stacked bars with a legend (km and %) and a collapsible list of the detailed surfaces.
   Without a surface database for the track it shows how to build one.
 */
class CSurfaceWidget : public QWidget {
  Q_OBJECT
 public:
  explicit CSurfaceWidget(QWidget* parent);
  ~CSurfaceWidget() override = default;

  void setData(const CSurfaceTrk& surface);

 signals:
  /// the user wants to build a surface database
  void sigSetup();

 private slots:
  void slotDetails(bool show);

 private:
  QLabel* labelHint;
  QLabel* labelClasses;
  CSurfaceBar* barClasses;
  QLabel* legendClasses;
  QLabel* labelWays;
  CSurfaceBar* barWays;
  QLabel* legendWays;
  QToolButton* toolDetails;
  QLabel* labelDetails;
};

#endif  // CSURFACEWIDGET_H
