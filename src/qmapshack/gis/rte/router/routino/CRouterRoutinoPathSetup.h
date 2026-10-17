/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CROUTERROUTINOPATHSETUP_H
#define CROUTERROUTINOPATHSETUP_H

#include <QDialog>

#include "ui_IRouterRoutinoPathSetup.h"

/**
   @brief Dialog to add or remove the paths of the Routino databases, or of other data kept the same way
 */
class CRouterRoutinoPathSetup : public QDialog, private Ui::IRouterRoutinoPathSetup {
  Q_OBJECT
 public:
  /** @brief the paths of the Routino databases */
  CRouterRoutinoPathSetup(QStringList& paths);
  /**
     @brief the paths of other data, e.g. the surface databases

     @param paths    the paths, changed on accept()
     @param title    the window title, empty for the one of the form
     @param help     the text above the list
     @param caption  the caption of the dialog to select a path
   */
  CRouterRoutinoPathSetup(QStringList& paths, const QString& title, const QString& help, const QString& caption);
  virtual ~CRouterRoutinoPathSetup();

 public slots:
  void accept() override;

 private slots:
  void slotAddPath();
  void slotDelPath();
  void slotItemSelectionChanged();

 private:
  /**
   * @brief Get the paths currently listed in the dialog
   * @return
   */
  QStringList currentPaths() const;

  QStringList& paths;
  const QString caption;
};

#endif  // CROUTERROUTINOPATHSETUP_H
