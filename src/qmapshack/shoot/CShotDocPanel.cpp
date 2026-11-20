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

#include <QAction>
#include <QCloseEvent>
#include <QComboBox>
#include <QFrame>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QScreen>
#include <QSettings>
#include <QSignalBlocker>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWindow>
#include <algorithm>
#include <utility>

#include "shoot/CShotMarks.h"
#include "theme/CUiTheme.h"

namespace {
enum column_e { eColumnId, eColumnState, eColumnScenario };

const QSize kDefaultSize(460, 760);
/** The buttons' labels, relative to the panel's font. */
constexpr qreal kLabelScale = 0.85;
const QString kSizeKey = "panel/size";
const QString kPosKey = "panel/pos";
const QString kRecordIcon = ":/icons/DocRecord.svgt";
const QString kRecordText = "Record";
const QString kStopIcon = ":/icons/DocStop.svgt";
const QString kStopText = "Stop";
/**
   @return @p text as a tooltip, @p title bold in front; rich text, which Qt wraps, while plain text stays one line
   @param text  plain text; `<br>` starts a new line
 */
QString tipOf(const QString& title, const QString& text) {
  QString body = text.toHtmlEscaped();
  body.replace("&lt;br&gt;", "<br>");
  return "<qt>" + (title.isEmpty() ? body : QString("<b>%1</b>: %2").arg(title.toHtmlEscaped(), body)) + "</qt>";
}
const QString kRecordHint = "Name the scenario, start from the base, do what the image needs, then press Stop.";
const QString kStopHint = "Stop the recording and store it under the name it was started with.";
/** Distance of the panel from the screen's top right corner [px]. */
constexpr qint32 kScreenMargin = 20;
/** A second centring of the wait box, once the window manager has placed the panel [ms]. */
constexpr qint32 kRecentreMs = 250;
}  // namespace

CShotDocPanel::CShotDocPanel(const QString& page, const QString& sizeFile, QWidget* parent)
    : QDialog(parent, Qt::Window), sizeFile(sizeFile) {
  setWindowTitle("Documentation mode");
  // The writer works in QMapShack, another process: the panel is rarely the active window.
  setAttribute(Qt::WA_AlwaysShowToolTips);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel(QString("Page <b>%1</b>").arg(page), this));

  this->page = new QLabel(this);
  this->page->setWordWrap(true);
  layout->addWidget(this->page);
  fixture = new QLabel(this);
  fixture->setWordWrap(true);
  layout->addWidget(fixture);

  // --- scenarios ---
  QGroupBox* scenarioBox = new QGroupBox("Scenarios - the states your images are taken in", this);
  QVBoxLayout* scenarioLayout = new QVBoxLayout(scenarioBox);
  layout->addWidget(scenarioBox);

  scenarios = new QListWidget(scenarioBox);
  scenarios->setAlternatingRowColors(true);
  // Five rows and the list's frame.
  scenarios->setMaximumHeight(5 * scenarios->fontMetrics().height() + 8);
  // A click, not a current row change: the arrow keys must not start a state each.
  connect(scenarios, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
    if (nullptr == item) {
      return;
    }
    const QString& name = item->data(Qt::UserRole).toString();
    // Queued: the answer rebuilds the list whose click is being delivered.
    QTimer::singleShot(0, this, [this, name]() { emit scenarioPicked(name); });
  });
  connect(scenarios, &QListWidget::currentItemChanged, this, [this]() { updateScenarioActions(); });
  scenarioLayout->addWidget(scenarios);

  const qint32 side = fontMetrics().height() + 2;
  QFont label = font();
  label.setPointSizeF(label.pointSizeF() * kLabelScale);
  const auto newButton = [this, side, label](const QString& icon, const QString& text, const QString& hint) {
    QToolButton* button = new QToolButton(this);
    button->setIcon(QIcon(icon));
    button->setIconSize(QSize(3 * side / 2, 3 * side / 2));
    button->setText(text);
    button->setFont(label);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setToolTip(tipOf(text, hint));
    buttons << button;
    return button;
  };
  const auto addButton = [this, &newButton](QBoxLayout* row, const QString& icon, const QString& text,
                                            const QString& hint, void (CShotDocPanel::*clicked)()) {
    QToolButton* button = newButton(icon, text, hint);
    connect(button, &QToolButton::clicked, this, clicked);
    row->addWidget(button);
    return button;
  };

  // Between a row's groups of buttons.
  const auto addSeparator = [](QBoxLayout* row, QWidget* parent) {
    QFrame* separator = new QFrame(parent);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    row->addWidget(separator);
  };

  QHBoxLayout* scenarioButtons = new QHBoxLayout;
  recordButton = addButton(scenarioButtons, kRecordIcon, kRecordText, kRecordHint, &CShotDocPanel::recordClicked);
  renameButton = addButton(scenarioButtons, ":/icons/DocRename.svgt", "Rename",
                           "Give the selected scenario another name.", &CShotDocPanel::renameClicked);
  deleteButton = addButton(scenarioButtons, ":/icons/DocDelete.svgt", "Delete",
                           "Delete the selected scenario; every image taken in it has to be taken again.",
                           &CShotDocPanel::deleteScenarioClicked);
  baseButton = newButton(":/icons/DocBase.svgt", "Base",
                         "This page's (base): save what is on screen as it, or copy another page's or the default "
                         "fixture's. A scenario keeps what it was recorded with.");
  QMenu* baseMenu = new QMenu(baseButton);
  connect(baseMenu->addAction("Save what is on screen as this page's base..."), &QAction::triggered, this,
          &CShotDocPanel::storeConfigClicked);
  connect(baseMenu->addAction("Copy the base of another page..."), &QAction::triggered, this,
          &CShotDocPanel::copyBaseClicked);
  baseButton->setMenu(baseMenu);
  baseButton->setPopupMode(QToolButton::InstantPopup);
  scenarioButtons->addWidget(baseButton);
  scenarioButtons->addStretch();
  scenarioLayout->addLayout(scenarioButtons);

  // --- images ---
  QGroupBox* shotBox = new QGroupBox("Images", this);
  QVBoxLayout* shotLayout = new QVBoxLayout(shotBox);
  layout->addWidget(shotBox);
  QLabel* shotTitle =
      new QLabel("Select one, point in QMapShack at what it shows and press <b>Ctrl+Shift+F9</b>:", shotBox);
  shotTitle->setWordWrap(true);
  shotLayout->addWidget(shotTitle);

  shots = new QTreeWidget(shotBox);
  shots->setRootIsDecorated(false);
  shots->setAlternatingRowColors(true);
  shots->setColumnCount(3);
  shots->setHeaderLabels({"Image", "State", "Taken in"});
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
  shotLayout->addWidget(shots);

  QHBoxLayout* shotActions = new QHBoxLayout;
  const auto addShotAction = [&](const QString& icon, const QString& text, const QString& hint,
                                 void (CShotDocPanel::*clicked)(const QString&)) {
    QToolButton* button = newButton(icon, text, hint);
    button->setEnabled(false);
    connect(button, &QToolButton::clicked, this, [this, clicked]() {
      if (const QString& id = currentId(); !id.isEmpty()) {
        emit(this->*clicked)(id);
      }
    });
    shotActions->addWidget(button);
    return button;
  };
  againButton = addShotAction(
      ":/icons/DocRetake.svgt", "Retake",
      "Take this image again into doc/<lang>/images/_work, as a build would; the application on screen is not touched.",
      &CShotDocPanel::retakeShotClicked);
  regionButton = addShotAction(":/icons/DocRegion.svgt", "Region",
                               "Drag a rectangle for this image instead of pointing at one widget.",
                               &CShotDocPanel::takeRegionClicked);
  revertButton =
      addShotAction(":/icons/DocRevert.svgt", "Revert",
                    "Throw the image just taken away and put the shot back as it was; the published one stays.",
                    &CShotDocPanel::revertShotClicked);
  publishButton =
      addShotAction(":/icons/DocPublish.svgt", "Publish", "Copy this image taken again into doc/<lang>/images.",
                    &CShotDocPanel::publishShotClicked);

  // The page's own actions, apart from the selected image's.
  addSeparator(shotActions, shotBox);
  whileIdle << addButton(shotActions, ":/icons/DocRetakeAll.svgt", "All",
                         "Take every image of this page again into doc/<lang>/images/_work, to compare and publish.",
                         &CShotDocPanel::retakePageClicked);
  reapButton = addButton(shotActions, ":/icons/DocClean.svgt", "Clean",
                         "Delete the shots and images of this page no page line uses.", &CShotDocPanel::reapClicked);
  whileIdle << reapButton;
  publishAllButton =
      addButton(shotActions, ":/icons/DocPublishAll.svgt", "Publish all",
                "Copy every image of this page taken again into doc/<lang>/images.", &CShotDocPanel::publishClicked);
  // Short on the button, where the longest label sets the panel's width; the tooltip keeps the name.
  publishAllButton->setText("Publ. All");
  whileIdle << publishAllButton;
  shotActions->addStretch();
  shotLayout->addLayout(shotActions);

  // --- the selected image: its marks and its preview ---
  imageBox = new QGroupBox(this);
  QVBoxLayout* imageLayout = new QVBoxLayout(imageBox);
  layout->addWidget(imageBox, 1);
  QLabel* markTitle =
      new QLabel("<b>Marks</b> - pick one, point in QMapShack at a control and press <b>Ctrl+Shift+F10</b>:", imageBox);
  markTitle->setWordWrap(true);
  imageLayout->addWidget(markTitle);
  QHBoxLayout* markers = new QHBoxLayout;
  struct marker_t {
    QString kind;
    QString text;
    QString hint;
  };
  const QList<marker_t> kinds = {
      {"frame", "Frame", "a red rectangle around a button, a list, a field, a docker or a tab."},
      {"disc", "Disc", "a red spot on something small: a slider's knob, the arrow that opens a list entry."},
      {"arrow", "Arrow", "a red arrow pointing at anything; then click where it starts. It runs in steps of 45°."},
      {"pointer", "Pointer", "a red mouse pointer where to click, also on a place on the map."},
      {"hand", "Hand", "a red pointing hand on what to drag."},
      {"resize", "Resize", "a red double arrow on the separator between two dockers, which can be dragged."},
  };
  for (const auto& [kind, text, hint] : kinds) {
    QToolButton* button =
        newButton(QString(), text, hint + "<br>Ctrl+Shift+F10 places it; click the button again to put it away.");
    // Drawn at the size shown: the marks' line widths are fixed, scaled down they vanish.
    button->setIcon(QIcon(QPixmap::fromImage(CShotMarks::icon(kind, button->iconSize().width()))));
    button->setCheckable(true);
    markerButtons.insert(kind, button);
    connect(button, &QToolButton::toggled, this, [this, button, kind](bool on) {
      // One at a time, and none when the picked one is clicked again.
      if (on) {
        for (QToolButton* other : std::as_const(markerButtons)) {
          if (other != button && other->isChecked()) {
            QSignalBlocker block(other);
            other->setChecked(false);
          }
        }
      }
      emit markerPicked(on ? kind : QString());
    });
    markers->addWidget(button);
  }
  // What is done with the marks, apart from the markers.
  addSeparator(markers, imageBox);
  showMarksButton = newButton(":/icons/ShowAll.svgt", "Show",
                              "show the selected image's marks in QMapShack; they go into the image "
                              "either way.");
  showMarksButton->setCheckable(true);
  showMarksButton->setChecked(true);
  connect(showMarksButton, &QToolButton::toggled, this, &CShotDocPanel::showMarksToggled);
  markers->addWidget(showMarksButton);
  removeMarkButton = newButton(":/icons/DocDelete.svgt", "Remove", "take the selected mark off this image.");
  removeMarkButton->setEnabled(false);
  markers->addWidget(removeMarkButton);
  markers->addStretch();
  imageLayout->addLayout(markers);

  marks = new QTreeWidget(imageBox);
  marks->setRootIsDecorated(false);
  marks->setAlternatingRowColors(true);
  marks->setColumnCount(2);
  marks->setHeaderLabels({"Mark", "Status"});
  marks->header()->setSectionResizeMode(0, QHeaderView::Stretch);
  marks->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  marks->header()->setStretchLastSection(false);
  // The header, four rows and the list's frame.
  marks->setMaximumHeight(marks->header()->sizeHint().height() + 4 * marks->fontMetrics().height() + 12);
  imageLayout->addWidget(marks);
  connect(marks, &QTreeWidget::currentItemChanged, this,
          [this]() { removeMarkButton->setEnabled(nullptr != marks->currentItem()); });
  connect(removeMarkButton, &QToolButton::clicked, this, [this]() {
    if (const qint32 index = marks->indexOfTopLevelItem(marks->currentItem()); index >= 0) {
      emit removeMarkClicked(index);
    }
  });

  preview = new QLabel(imageBox);
  preview->setMinimumHeight(200);
  preview->setAlignment(Qt::AlignCenter);
  preview->setFrameShape(QFrame::StyledPanel);
  imageLayout->addWidget(preview, 1);

  status = new QLabel(this);
  status->setWordWrap(true);
  layout->addWidget(status);

  // One height for all, each as wide as its label and never narrower than tall; Record turns into Stop.
  qint32 height = 0;
  for (const QToolButton* button : std::as_const(buttons)) {
    height = qMax(height, button->sizeHint().height());
  }
  for (QToolButton* button : std::as_const(buttons)) {
    const qint32 width =
        (button == recordButton)
            ? qMax(button->sizeHint().width(), button->fontMetrics().horizontalAdvance(kStopText) + height / 2)
            : button->sizeHint().width();
    button->setFixedSize(qMax(width, height), height);
  }

  updateShotActions();
  resize(kDefaultSize);
}

void CShotDocPanel::setMarks(const QString& id, const QStringList& names) {
  if (id == markedImage && names == markNames) {
    return;
  }
  if (id != markedImage) {
    // Another image's list: nothing of the old one is picked.
    marks->setCurrentItem(nullptr);
  }
  markedImage = id;
  markNames = names;
  showMarks(QStringList());
}

QString CShotDocPanel::pickedMarker() const {
  for (auto it = markerButtons.constBegin(); it != markerButtons.constEnd(); ++it) {
    if (it.value()->isChecked()) {
      return it.key();
    }
  }
  return QString();
}

bool CShotDocPanel::marksShown() const { return showMarksButton->isChecked(); }

void CShotDocPanel::setMarkStatuses(const QStringList& statuses) { showMarks(statuses); }

void CShotDocPanel::showMarks(const QStringList& statuses) {
  const qint32 row = marks->indexOfTopLevelItem(marks->currentItem());
  marks->clear();
  for (qsizetype i = 0; i < markNames.size(); i++) {
    new QTreeWidgetItem(marks, {markNames.at(i), statuses.value(i)});
  }
  marks->setCurrentItem(marks->topLevelItem(qMin(row, qint32(markNames.size()) - 1)));
  removeMarkButton->setEnabled(nullptr != marks->currentItem());
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
      break;
    }
  }
  updateScenarioActions();
}

void CShotDocPanel::updateScenarioActions() {
  // (base) is no scenario: it has no name to change and cannot go, and it is the only state its config is stored from.
  const QListWidgetItem* item = scenarios->currentItem();
  const bool idle = !recording && nullptr != item;
  const bool base = nullptr != item && item->data(Qt::UserRole).toString().isEmpty();
  renameButton->setEnabled(idle && !base);
  deleteButton->setEnabled(idle && !base);
  baseButton->setEnabled(idle && base);
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
    // Changed marks win: the image no longer shows them, whatever else it is.
    row->setText(eColumnState, entry.marksChanged ? QString("marks changed, take it again")
                                                  : (entry.changed ? text + ", taken again" : text));
    row->setToolTip(eColumnId, tipOf(QString(), entry.note.isEmpty() ? entry.id : entry.note));
    if (entry.marksChanged || entry.changed || CShotFiles::eMissing == entry.state ||
        CShotFiles::eNoImage == entry.state) {
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
  const bool waiting =
      std::any_of(entries.cbegin(), entries.cend(), [](const CShotFiles::row_t& e) { return e.changed; });
  publishAllButton->setEnabled(waiting && shots->isEnabled());
  reapButton->setToolTip(
      tipOf("Clean", QString("delete the shots and images of this page no page line uses: %1.").arg(unused)));

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
  againButton->setEnabled(selected && entry.takeable);
  regionButton->setEnabled(selected);
  revertButton->setEnabled(selected && entry.revertable);
  publishButton->setEnabled(selected && entry.changed);

  // Marks and preview belong to an image: without one, the group is off and no marker stays picked.
  imageBox->setTitle(entry.id.isEmpty() ? QString("No image selected") : "Selected image: " + entry.id);
  imageBox->setEnabled(selected);
  if (!selected) {
    for (QToolButton* button : std::as_const(markerButtons)) {
      button->setChecked(false);
    }
  }
}

void CShotDocPanel::showPreview() {
  updateShotActions();

  const CShotFiles::row_t& entry = entries.value(shots->indexOfTopLevelItem(shots->currentItem()));
  if (entry.imagePath.isEmpty()) {
    preview->setPixmap(QPixmap());
    preview->setText(entry.id.isEmpty() ? "Select an image." : "No image yet.");
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

void CShotDocPanel::setFixture(const QStringList& own, const QStringList& empty, const QString& dir) {
  QStringList filled;
  for (const QString& part : own) {
    if (!empty.contains(part)) {
      filled << part;
    }
  }
  QStringList says;
  if (!empty.isEmpty()) {
    says << QString("%1 left out").arg(empty.join(", "));
  }
  if (!filled.isEmpty()) {
    says << QString("%1 from this page").arg(filled.join(", "));
  }
  fixture->setText(says.isEmpty() ? QString("Fixture: the default")
                                  : QString("Fixture: %1; the rest from the default").arg(says.join("; ")));
  fixture->setToolTip(
      tipOf(QString(), QString("A part holding something in %1 replaces the default's; an empty one is the "
                               "default's. A file EMPTY in a part leaves the page without it.")
                           .arg(dir)));
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
  recordButton->setText(on ? kStopText : kRecordText);
  recordButton->setIcon(QIcon(on ? kStopIcon : kRecordIcon));
  recordButton->setToolTip(tipOf(on ? kStopText : kRecordText, on ? kStopHint : kRecordHint));
  for (QToolButton* button : std::as_const(whileIdle)) {
    button->setEnabled(!on);
  }
  scenarios->setEnabled(!on);
  updateScenarioActions();
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

QString CShotDocPanel::currentScenario() const {
  const QListWidgetItem* item = scenarios->currentItem();
  return (nullptr == item) ? QString() : item->data(Qt::UserRole).toString();
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
  shots->setCurrentItem(nullptr);
}
