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

#ifndef CSHOTDOCPANEL_H
#define CSHOTDOCPANEL_H

#include <QDialog>
#include <QList>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <functional>

#include "shoot/CShotFiles.h"

class QLabel;
class QListWidget;
class QMessageBox;
class QPushButton;
class QToolButton;
class QTreeWidget;
class QTreeWidgetItem;

/**
   @brief The writer's panel: the page's scenarios above, its pictures below.

   Every button calls a handler the launcher sets; the panel itself touches no file but its own size.
 */
class CShotDocPanel : public QDialog {
  Q_OBJECT
 public:
  /** @param sizeFile  the INI file the panel's size and position are kept in between sessions */
  CShotDocPanel(const QString& page, const QString& sizeFile, QWidget* parent);

  /** @brief Fill the scenario list; call before setShots(), whose combo boxes offer these. */
  void setScenarios(const QStringList& names, const QString& current);

  void setShots(const QList<CShotFiles::row_t>& shots);

  /** @return the selected picture's id, empty when none is selected */
  QString currentId() const;

  /** @brief Select the picture @p id; nothing when there is no such row. */
  void setCurrentShot(const QString& id);

  /** @brief Asked before a close is accepted; false keeps the panel open, and must only follow a question. */
  void setCloseRequestHandler(std::function<bool()> handler) { closeRequest = handler; }

  void setStatus(const QString& text);

  void setPage(const QString& path, bool exists);

  /** @brief While a recording runs, everything but Stop is disabled. */
  void setRecording(bool on);

  /**
     @brief Refuse input while @p on, through a window-modal box.

     Shown, never exec()'d: the launcher keeps answering the state process meanwhile.
   */
  void setBusy(bool on, const QString& what);

 signals:
  /** @brief A picture's row was clicked; emitted queued, after the click is delivered. */
  void shotPicked(const QString& id);
  /** @brief A scenario was clicked; emitted queued: the answer rebuilds the list. */
  void scenarioPicked(const QString& name);
  void recordClicked();
  void renameClicked();
  void deleteScenarioClicked();
  void storeConfigClicked();
  /** @brief A picture's combo box changed to @p scenario, empty for `(base)`; emitted queued: the answer rebuilds the
   * rows. */
  void rebindRequested(const QString& id, const QString& scenario);
  void retakeShotClicked(const QString& id);
  void takeRegionClicked(const QString& id);
  void revertShotClicked(const QString& id);
  void retakePageClicked();
  void reapClicked();
  void reloadClicked();
  /** @brief Publish every picture of the page taken again. */
  void publishClicked();
  /** @brief After a close was accepted. */
  void closed();

 protected:
  /** @brief Escape does not close the panel. */
  void reject() override;
  void showEvent(QShowEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  void moveEvent(QMoveEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

 private:
  void centreWaiting();
  void showPreview();
  /** @return the published and the work picture side by side, captioned */
  QPixmap comparison(const QString& before, const QString& after) const;
  void buildScenarioCell(QTreeWidgetItem* row, const CShotFiles::row_t& entry);
  void updateShotActions();
  static QString label(CShotFiles::state_e state);

  std::function<bool()> closeRequest;

  const QString sizeFile;

  QListWidget* scenarios = nullptr;
  QTreeWidget* shots = nullptr;
  QPushButton* recordButton = nullptr;
  QPushButton* reapButton = nullptr;
  QToolButton* againButton = nullptr;
  QToolButton* regionButton = nullptr;
  QToolButton* revertButton = nullptr;
  /** Disabled while a recording runs. */
  QList<QPushButton*> whileIdle;
  QLabel* page = nullptr;
  QLabel* status = nullptr;
  QLabel* preview = nullptr;

  QStringList scenarioNames;
  QList<CShotFiles::row_t> entries;
  /** Set while the rows are rebuilt: a combo box set then is not the writer's choice. */
  bool populating = false;
  /** The window manager owns a decorated window's geometry until it is mapped; ours is applied once after show. */
  bool placed = false;
  bool recording = false;
  QMessageBox* waiting = nullptr;
};

#endif  // CSHOTDOCPANEL_H
