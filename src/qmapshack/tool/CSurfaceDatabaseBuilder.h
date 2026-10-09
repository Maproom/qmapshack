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

#ifndef CSURFACEDATABASEBUILDER_H
#define CSURFACEDATABASEBUILDER_H

#include <QFutureWatcher>
#include <QWidget>
#include <atomic>

class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

/**
   @brief Tool tab to build surface databases from OpenStreetMap extracts and to choose the
          active ones

   The build runs in a worker thread (CSurfaceDbBuilder). Closing the tab cancels it.
 */
class CSurfaceDatabaseBuilder : public QWidget {
  Q_OBJECT
 public:
  explicit CSurfaceDatabaseBuilder(QWidget* parent);
  ~CSurfaceDatabaseBuilder() override;

 private slots:
  void slotSelectSource();
  void slotSelectPath();
  void slotStart();
  void slotCancel();
  void slotFinished();
  void slotItemChanged(QTreeWidgetItem* item, int column);
  void slotDatabasesChanged();
  void updateButtons();

 private:
  QString targetFile() const;

  QLineEdit* lineSource;
  QLineEdit* lineName;
  QLabel* labelPath;
  QLabel* labelTarget;
  QPushButton* pushStart;
  QPushButton* pushCancel;
  QProgressBar* progress;
  QLabel* labelStatus;
  QTreeWidget* treeDatabases;

  QFutureWatcher<QString> watcher;
  std::atomic_bool cancel = false;
};

#endif  // CSURFACEDATABASEBUILDER_H
