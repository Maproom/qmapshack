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

#ifndef CTRKTOAREADIALOG_H
#define CTRKTOAREADIALOG_H

#include "gis/prj/IGisProject.h"
#include "ui_ITrkToAreaDialog.h"

class QTreeWidget;

/*
 * Dialog Class for the track to area conversion.
 */
class CTrkToAreaDialog : public QDialog, private Ui::ITrkToAreaDialog {
  Q_OBJECT
 public:
  /**
     @brief Shows the Dialog and preselects the project and area name
   */
  CTrkToAreaDialog(IGisProject*& project, QString& areaName);
  virtual ~CTrkToAreaDialog();

 private slots:
  void slotProject();
  void slotAreaChanged(const QString& text);

 private:
  void setType(IGisProject::type_e& t);
  void buttonBoxEnabled();

  IGisProject*& project;
  QString& areaName;
};

#endif  // CTRKTOAREADIALOG_H
