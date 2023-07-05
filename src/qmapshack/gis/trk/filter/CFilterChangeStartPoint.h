/**********************************************************************************************

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

#ifndef CFILTERCHANGESTARTPOINT_H
#define CFILTERCHANGESTARTPOINT_H

#include <QWidget>

#include "ui_IFilterChangeStartPoint.h"

class CGisItemTrk;

class CFilterChangeStartPoint : public QWidget, private Ui::IFilterChangeStartPoint {
  Q_OBJECT

 public:
  CFilterChangeStartPoint(CGisItemTrk& trk, QWidget* parent = 0);
  virtual ~CFilterChangeStartPoint() = default;

  void updateUi();

 private slots:
  void slotApply();

 private:
  CGisItemTrk& trk;
};

#endif  // CFILTERCHANGESTARTPOINT_H
