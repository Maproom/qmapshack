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

#include "shoot/CShotDocPanel.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWindow>
#include <utility>

#include "theme/CUiTheme.h"

namespace {
enum column_e { eColumnId, eColumnState, eColumnScenario };

const QSize kDefaultSize(460, 760);
const QString kSizeKey = "panel/size";
const QString kPosKey = "panel/pos";
/** Distance of the panel from the screen's top right corner [px]. */
constexpr qint32 kScreenMargin = 20;
/** A second centring of the wait box, once the window manager has placed the panel [ms]. */
constexpr qint32 kRecentreMs = 250;
}  // namespace

CShotDocPanel::CShotDocPanel(const QString& page, const QString& sizeFile, QWidget* parent)
    : QDialog(parent, Qt::Window), sizeFile(sizeFile) {
  setWindowTitle("Documentation mode");

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel(QString("Page <b>%1</b>").arg(page), this));

  this->page = new QLabel(this);
  this->page->setWordWrap(true);
  layout->addWidget(this->page);

  // --- scenarios ---
  QLabel* scenarioTitle = new QLabel("<b>Scenarios</b> - the states your pictures are taken in:", this);
  scenarioTitle->setWordWrap(true);
  layout->addWidget(scenarioTitle);

  scenarios = new QListWidget(this);
  scenarios->setAlternatingRowColors(true);
  scenarios->setMaximumHeight(120);
  // A click, not a current row change: the arrow keys must not start a state each.
  connect(scenarios, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
    if (nullptr == item) {
      return;
    }
    const QString& name = item->data(Qt::UserRole).toString();
    // Queued: the answer rebuilds the list whose click is being delivered.
    QTimer::singleShot(0, this, [this, name]() { emit scenarioPicked(name); });
  });
  layout->addWidget(scenarios);

  const auto addButton = [this](QBoxLayout* row, const QString& text, const QString& hint,
                                void (CShotDocPanel::*clicked)()) {
    QPushButton* button = new QPushButton(text, this);
    button->setToolTip(hint);
    connect(button, &QPushButton::clicked, this, clicked);
    row->addWidget(button);
    return button;
  };

  QHBoxLayout* scenarioButtons = new QHBoxLayout;
  recordButton = addButton(scenarioButtons, "Record...",
                           "Start from the base, do what the picture needs, then press Stop recording.",
                           &CShotDocPanel::recordClicked);
  whileIdle << addButton(scenarioButtons, "Rename...", "Give the selected scenario another name.",
                         &CShotDocPanel::renameClicked);
  whileIdle << addButton(scenarioButtons, "Delete",
                         "Delete the selected scenario; every picture taken in it has to be taken again.",
                         &CShotDocPanel::deleteScenarioClicked);
  whileIdle << addButton(scenarioButtons, "Save config",
                         "Store the arrangement, size, map and settings on screen as the running scenario's "
                         "configuration; in (base) it asks first.",
                         &CShotDocPanel::storeConfigClicked);
  layout->addLayout(scenarioButtons);

  // --- pictures ---
  QLabel* shotTitle = new QLabel(
      "<b>Pictures</b> - select one, point in QMapShack at what it shows and press <b>Ctrl+Shift+F9</b>:", this);
  shotTitle->setWordWrap(true);
  layout->addWidget(shotTitle);

  shots = new QTreeWidget(this);
  shots->setRootIsDecorated(false);
  shots->setAlternatingRowColors(true);
  shots->setColumnCount(3);
  shots->setHeaderLabels({"Picture", "State", "Taken in"});
  shots->header()->setSectionResizeMode(eColumnId, QHeaderView::Stretch);
  shots->header()->setSectionResizeMode(eColumnState, QHeaderView::ResizeToContents);
  shots->header()->setSectionResizeMode(eColumnScenario, QHeaderView::ResizeToContents);
  connect(shots, &QTreeWidget::currentItemChanged, this, [this]() { showPreview(); });
  connect(shots, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item) {
    const QString& id = (nullptr == item) ? QString() : item->text(eColumnId);
    if (id.isEmpty()) {
      return;
    }
    QTimer::singleShot(0, this, [this, id]() { emit shotPicked(id); });
  });
  layout->addWidget(shots);

  QHBoxLayout* shotActions = new QHBoxLayout;
  const qint32 side = fontMetrics().height() + 2;
  const auto addShotAction = [&](const QString& icon, const QString& text, const QString& hint,
                                 void (CShotDocPanel::*clicked)(const QString&)) {
    QToolButton* button = new QToolButton(this);
    button->setIcon(QIcon(icon));
    button->setIconSize(QSize(side, side));
    button->setText(text);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setToolTip(hint);
    button->setEnabled(false);
    connect(button, &QToolButton::clicked, this, [this, clicked]() {
      if (const QString& id = currentId(); !id.isEmpty()) {
        emit(this->*clicked)(id);
      }
    });
    shotActions->addWidget(button);
    return button;
  };
  againButton = addShotAction(":/icons/Screenshot.svgt", "Take again", "Take this picture again in its own scenario.",
                              &CShotDocPanel::retakeShotClicked);
  regionButton = addShotAction(":/icons/SelectArea.svgt", "Region",
                               "Drag a rectangle for this picture instead of pointing at one widget.",
                               &CShotDocPanel::takeRegionClicked);
  revertButton =
      addShotAction(":/icons/Reset.svgt", "Revert", "Throw the picture just taken away; the published one stays.",
                    &CShotDocPanel::revertShotClicked);
  shotActions->addStretch();
  layout->addLayout(shotActions);

  preview = new QLabel(this);
  preview->setMinimumHeight(200);
  preview->setAlignment(Qt::AlignCenter);
  preview->setFrameShape(QFrame::StyledPanel);
  layout->addWidget(preview);

  QHBoxLayout* pageButtons = new QHBoxLayout;
  whileIdle << addButton(pageButtons, "Take all again",
                         "Replay every picture of this page into doc/images/_check; nothing here changes.",
                         &CShotDocPanel::retakePageClicked);
  reapButton = addButton(pageButtons, "Remove unused", "Delete the shots and pictures of this page no page line uses.",
                         &CShotDocPanel::reapClicked);
  whileIdle << reapButton;
  whileIdle << addButton(pageButtons, "Reload page", "Read the page's image lines again.",
                         &CShotDocPanel::reloadClicked);
  whileIdle << addButton(pageButtons, "Publish", "Copy the pictures taken again into doc/images.",
                         &CShotDocPanel::publishClicked);
  layout->addLayout(pageButtons);

  status = new QLabel(this);
  status->setWordWrap(true);
  layout->addWidget(status);

  resize(kDefaultSize);
}

void CShotDocPanel::showEvent(QShowEvent* event) {
  QDialog::showEvent(event);
  if (placed) {
    return;
  }
  placed = true;

  // A decorated window's geometry belongs to the window manager until it is mapped.
  QTimer::singleShot(0, this, [this]() {
    const QSettings stored(sizeFile, QSettings::IniFormat);
    const QSize& size = stored.value(kSizeKey).toSize();
    resize(size.isValid() ? size : kDefaultSize);
    // Where the writer left it, unless that screen is gone.
    const QVariant& pos = stored.value(kPosKey);
    if (const std::optional<QPoint>& at =
            pos.isValid() ? CShotFiles::onScreen(pos.toPoint(), frameGeometry().size()) : std::nullopt;
        at.has_value()) {
      move(*at);
      return;
    }
    const QScreen* screen = (nullptr != windowHandle()) ? windowHandle()->screen() : QGuiApplication::primaryScreen();
    if (nullptr != screen) {
      const QRect& available = screen->availableGeometry();
      move(available.right() - frameGeometry().width() - kScreenMargin, available.top() + kScreenMargin);
    }
  });
}

void CShotDocPanel::closeEvent(QCloseEvent* event) {
  if (closeRequest && !closeRequest()) {
    event->ignore();
    return;
  }
  event->accept();
  QSettings placement(sizeFile, QSettings::IniFormat);
  placement.setValue(kSizeKey, size());
  placement.setValue(kPosKey, pos());
  emit closed();
}

void CShotDocPanel::moveEvent(QMoveEvent* event) {
  QDialog::moveEvent(event);
  centreWaiting();
}

void CShotDocPanel::resizeEvent(QResizeEvent* event) {
  QDialog::resizeEvent(event);
  centreWaiting();
  // The thumbnail is scaled to the preview's size.
  showPreview();
}

void CShotDocPanel::reject() {}

void CShotDocPanel::setScenarios(const QStringList& names, const QString& current) {
  scenarioNames = names;
  scenarios->clear();

  QListWidgetItem* base = new QListWidgetItem(CShotFiles::kBaseLabel, scenarios);
  base->setData(Qt::UserRole, QString());
  for (const QString& name : names) {
    (new QListWidgetItem(name, scenarios))->setData(Qt::UserRole, name);
  }

  for (qint32 row = 0; row < scenarios->count(); row++) {
    if (scenarios->item(row)->data(Qt::UserRole).toString() == current) {
      scenarios->setCurrentRow(row);
      return;
    }
  }
}

void CShotDocPanel::buildScenarioCell(QTreeWidgetItem* row, const CShotFiles::row_t& entry) {
  QComboBox* combo = new QComboBox(shots);
  combo->addItem(CShotFiles::kBaseLabel);
  combo->addItems(scenarioNames);
  // A shot naming an unknown scenario is shown as it is, never silently rebound.
  if (!entry.scenario.isEmpty() && !scenarioNames.contains(entry.scenario)) {
    combo->addItem(entry.scenario);
  }
  combo->setCurrentText(entry.scenario.isEmpty() ? CShotFiles::kBaseLabel : entry.scenario);

  const QString& id = entry.id;
  connect(combo, &QComboBox::currentTextChanged, this, [this, id](const QString& chosen) {
    if (populating) {
      return;
    }
    const QString& scenario = (CShotFiles::kBaseLabel == chosen) ? QString() : chosen;
    // Queued: the answer rebuilds the rows and deletes this combo box.
    QTimer::singleShot(0, this, [this, id, scenario]() { emit rebindRequested(id, scenario); });
  });
  shots->setItemWidget(row, eColumnScenario, combo);
}

void CShotDocPanel::setShots(const QList<CShotFiles::row_t>& shots_) {
  const QString& current = currentId();

  populating = true;
  entries = shots_;
  shots->clear();
  for (const CShotFiles::row_t& entry : std::as_const(entries)) {
    const QString& text = label(entry.state);
    QTreeWidgetItem* row = new QTreeWidgetItem(shots);
    row->setText(eColumnId, entry.id);
    row->setText(eColumnState, entry.changed ? text + ", changed by the retake" : text);
    row->setToolTip(eColumnId, entry.note.isEmpty() ? entry.id : entry.note);
    if (entry.changed || CShotFiles::eMissing == entry.state || CShotFiles::eNoImage == entry.state) {
      row->setForeground(eColumnState, CUiTheme::foreground(CUiTheme::Role::eWarn));
    }
    buildScenarioCell(row, entry);
  }
  populating = false;

  qint32 unused = 0;
  for (const CShotFiles::row_t& entry : std::as_const(entries)) {
    unused += (CShotFiles::eNotUsed == entry.state) ? 1 : 0;
  }
  reapButton->setEnabled(unused > 0 && shots->isEnabled());
  reapButton->setText(unused > 0 ? QString("Remove %1 unused").arg(unused) : QString("Remove unused"));

  setCurrentShot(current);
  showPreview();
}

void CShotDocPanel::centreWaiting() {
  if (nullptr == waiting || !waiting->isVisible()) {
    return;
  }
  waiting->adjustSize();
  const QPoint& centre = frameGeometry().center();
  waiting->move(centre.x() - waiting->width() / 2, centre.y() - waiting->height() / 2);
  waiting->raise();
}

void CShotDocPanel::updateShotActions() {
  const CShotFiles::row_t& entry = entries.value(shots->indexOfTopLevelItem(shots->currentItem()));
  const bool selected = !entry.id.isEmpty() && shots->isEnabled();
  // A picture nothing ever took has no shot to take again.
  againButton->setEnabled(selected && (CShotFiles::eTaken == entry.state || CShotFiles::eNoImage == entry.state));
  regionButton->setEnabled(selected);
  revertButton->setEnabled(selected && entry.changed);
}

void CShotDocPanel::showPreview() {
  updateShotActions();

  const CShotFiles::row_t& entry = entries.value(shots->indexOfTopLevelItem(shots->currentItem()));
  if (entry.imagePath.isEmpty()) {
    preview->setPixmap(QPixmap());
    preview->setText(entry.id.isEmpty() ? "Select a picture." : "No image yet.");
    return;
  }
  preview->setText(QString());

  if (!entry.changed || entry.publishedPath.isEmpty()) {
    preview->setPixmap(
        QPixmap(entry.imagePath)
            .scaled(preview->width() - 8, preview->height() - 8, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    return;
  }
  preview->setPixmap(comparison(entry.publishedPath, entry.imagePath));
}

QPixmap CShotDocPanel::comparison(const QString& before, const QString& after) const {
  const qint32 gap = 8;
  const qint32 caption = fontMetrics().height() + 2;
  const qint32 half = (preview->width() - 3 * gap) / 2;
  const qint32 tall = preview->height() - 2 * gap - caption;
  if (half < 16 || tall < 16) {
    return QPixmap();
  }

  const QPixmap& left = QPixmap(before).scaled(half, tall, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  const QPixmap& right = QPixmap(after).scaled(half, tall, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  QPixmap sheet(preview->width() - 2, preview->height() - 2);
  sheet.fill(Qt::transparent);
  QPainter paint(&sheet);
  paint.setPen(palette().color(QPalette::WindowText));
  paint.drawText(QRect(gap, gap, half, caption), Qt::AlignHCenter, "published");
  paint.drawText(QRect(2 * gap + half, gap, half, caption), Qt::AlignHCenter, "just taken");
  paint.drawPixmap(gap + (half - left.width()) / 2, gap + caption, left);
  paint.drawPixmap(2 * gap + half + (half - right.width()) / 2, gap + caption, right);
  return sheet;
}

void CShotDocPanel::setStatus(const QString& text) { status->setText(text); }

void CShotDocPanel::setPage(const QString& path, bool exists) {
  page->setText(exists ? QString("Page: %1").arg(path)
                       : CUiTheme::span(CUiTheme::Role::eWarn, QString("There is no page %1 yet.").arg(path)));
}

void CShotDocPanel::setBusy(bool on, const QString& what) {
  if (!on) {
    if (nullptr != waiting) {
      waiting->hide();
      waiting->deleteLater();
      waiting = nullptr;
    }
    return;
  }

  if (nullptr == waiting) {
    waiting = new QMessageBox(QMessageBox::Information, "Documentation mode", what, QMessageBox::NoButton, this);
    waiting->setWindowModality(Qt::WindowModal);
    // Before the first show(): shown without buttons QMessageBox adds an OK button and makes it the escape button,
    // and removing it afterwards leaves Escape pointing at a deleted button (measured: SIGSEGV). Set explicitly, it
    // adds none, and Escape and a close request are ignored.
    waiting->setStandardButtons(QMessageBox::NoButton);
  }
  waiting->setText(what);
  waiting->show();
  centreWaiting();
  QTimer::singleShot(0, waiting, [this]() { centreWaiting(); });
  QTimer::singleShot(kRecentreMs, waiting, [this]() { centreWaiting(); });
}

void CShotDocPanel::setRecording(bool on) {
  // Called on every change of what runs; leaving recording rebuilds the rows.
  if (on == recording) {
    return;
  }
  recording = on;
  recordButton->setText(on ? "Stop recording" : "Record...");
  for (QPushButton* button : std::as_const(whileIdle)) {
    button->setEnabled(!on);
  }
  scenarios->setEnabled(!on);
  shots->setEnabled(!on);
  updateShotActions();
  if (!on) {
    // Its enabled state follows the unused count.
    setShots(entries);
  }
}

QString CShotDocPanel::label(CShotFiles::state_e state) {
  switch (state) {
    case CShotFiles::eNoImage:
      return "no image";
    case CShotFiles::eNotUsed:
      return "not used";
    case CShotFiles::eMissing:
      return "not taken";
    case CShotFiles::eUnregistered:
      return "not registered";
    case CShotFiles::eTaken:
      break;
  }
  return "taken";
}

QString CShotDocPanel::currentId() const {
  const QTreeWidgetItem* item = shots->currentItem();
  return (nullptr == item) ? QString() : item->text(eColumnId);
}

void CShotDocPanel::setCurrentShot(const QString& id) {
  for (qsizetype row = 0; row < entries.size(); row++) {
    if (entries.at(row).id == id) {
      shots->setCurrentItem(shots->topLevelItem(row));
      return;
    }
  }
}
