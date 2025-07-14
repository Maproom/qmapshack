/**********************************************************************************************
    Copyright (C) 2025 Jürgen Beck <>

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

#ifndef CMAPICONSIZESSETUP_H
#define CMAPICONSIZESSETUP_H

#include <QDialog>

#include "ui_IMapIconSizesSetup.h"

class CMapIconSizesSetup : public QDialog, private Ui::IMapIconSizesSetup {
  Q_OBJECT
 public:
  CMapIconSizesSetup(QWidget* parent);
  virtual ~CMapIconSizesSetup();

 public slots:
  void accept() override;

 private slots:
  void slotSetIconSizeWpt(int size);
  void slotSetIconSizePoi(int size);
  void slotResetClicked(bool checked);

 private:
  QImage imageWpt;
  QImage imagePoi;
};

#endif  // CMAPICONSIZESSETUP_H
