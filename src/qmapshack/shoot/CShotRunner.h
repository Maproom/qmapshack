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

#ifndef CSHOTRUNNER_H
#define CSHOTRUNNER_H

#include <QObject>
#include <QString>

#include "shoot/CShotOptions.h"

class QWidget;

/** @brief Takes the shots `--shoot` asks for, then quits. */
class CShotRunner : public QObject {
  Q_OBJECT
 public:
  CShotRunner(const CShotOptions::opts_t& opts, QWidget* window);
  virtual ~CShotRunner() = default;

  /** @brief Queue the run: the main window finishes initialising on timers. */
  void start();

  qint32 getFailures() const { return failures; }

 private slots:
  void slotRun();

 private:
  QWidget* window;
  QString outDir;
  QString target;
  QString only;
  QString scenario;
  qint32 failures = 0;
};

#endif  // CSHOTRUNNER_H
