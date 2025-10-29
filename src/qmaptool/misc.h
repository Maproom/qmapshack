/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef MISC_H
#define MISC_H

#include <QFile>
#include <QMessageBox>

#include "CMainWindow.h"

inline void openFileCheckSuccess(QIODeviceBase::OpenMode mode, QFile& file) {
  if (!file.open(mode)) {
    QMessageBox::warning(&CMainWindow::self(), CMainWindow::tr("File access failed..."),
                         CMainWindow::tr("Failed to open %1 with error %2").arg(file.fileName(), file.errorString()),
                         QMessageBox::Ok);
  }
}

#endif  // MISC_H
