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

#ifndef CSHOTDOCMODE_H
#define CSHOTDOCMODE_H

#include <QDir>
#include <QObject>
#include <QString>
#include <memory>

class CShotContext;
class CShotWriter;
class QLocalSocket;

/**
   @brief The state process: one scenario, put up once and thrown away with the process.

   It loads the fixture, replays its scenario and reports `ready` to the launcher. It ends when its window is closed or
   the channel drops.
 */
class CShotDocMode : public QObject {
  Q_OBJECT
 public:
  /** @param scenario  CShotPage::kBaseScenario for `(base)` */
  CShotDocMode(const QDir& repo, const QString& page, const QString& scenario, QObject* parent);
  virtual ~CShotDocMode();

  /** @brief Connect the channel and set the scenario up once the main window has settled. */
  void start();

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private slots:
  void slotSetUp();

 private:
  /** @brief End the process once: close the window, exit, and exit hard if the event loop does not return. */
  void leave(const QString& why);
  /** @brief One command from the launcher. */
  void obey(const QString& line);
  /** @brief One line to the launcher; logged when no channel is connected. */
  void send(const QString& line);

  QDir repo;
  QString page;
  /** Empty is `(base)`. */
  QString scenario;

  std::unique_ptr<CShotWriter> writer;
  std::unique_ptr<CShotContext> ctx;
  QLocalSocket* channel = nullptr;
  bool leaving = false;
};

#endif  // CSHOTDOCMODE_H
