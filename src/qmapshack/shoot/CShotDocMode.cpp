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

#include "shoot/CShotDocMode.h"

#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QDataStream>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QEvent>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QGroupBox>
#include <QHash>
#include <QImage>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QKeyEvent>
#include <QLabel>
#include <QLocalSocket>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRect>
#include <QSaveFile>
#include <QScreen>
#include <QSettings>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <algorithm>
#include <cstdlib>
#include <utility>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "helpers/CSettings.h"
#include "setup/CAppOpts.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotFixture.h"
#include "shoot/CShotPage.h"
#include "shoot/CShotRecorder.h"
#include "shoot/CShotRegistry.h"
#include "shoot/CShotReplay.h"
#include "shoot/CShotWriter.h"

namespace {
/** CMainWindow maximizes a window with no stored geometry after 500 ms. */
constexpr qint32 kStartupMs = 1000;
/** How long leave() waits for the event loop to return before exiting [ms]. */
constexpr qint32 kLeaveTimeoutMs = 2000;
/** The documentation key; a bare function key would be taken from the application, where F1, F8 and F11 are bound. */
constexpr Qt::KeyboardModifiers kTagModifiers = Qt::ControlModifier | Qt::ShiftModifier;
constexpr Qt::Key kTagKey = Qt::Key_F9;
/** A dragged rectangle smaller than this on either side is a click [px]. */
constexpr qint32 kMinRegionSide = 8;
/** Where the application window sat, in CShotFiles::placementFile(). */
const QString kWindowPosKey = "window/pos";
/** How long the window rests before its position is written [ms]. */
constexpr qint32 kPlacementSettleMs = 300;
/** QWidget::saveGeometry()'s magic number and the only major version portableGeometry() knows. */
constexpr quint32 kGeometryMagic = 0x1D9D0CB;
constexpr quint16 kGeometryMajor = 3;
const QString kSetupConfig = QStringLiteral("setup.ini");

/** Collects the `shoot:` warnings given during its lifetime, so a writer without a console reads them. */
class CShootWarnings {
 public:
  CShootWarnings() {
    given.clear();
    previous = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context, const QString& message) {
      if (QtWarningMsg == type && message.contains("shoot:")) {
        given << message.mid(message.indexOf("shoot:") + 6).trimmed();
      }
      previous(type, context, message);
    });
  }
  ~CShootWarnings() { qInstallMessageHandler(previous); }

  static QString first() { return given.isEmpty() ? QString() : given.first(); }

 private:
  static inline QStringList given;
  static inline QtMessageHandler previous = nullptr;
};

/**
   @brief A pane over the main window the writer drags a rectangle on.

   It paints only the dimmed surround and is gone before anything is photographed.
 */
class CShotRegionPicker : public QWidget {
 public:
  explicit CShotRegionPicker(QWidget* parent) : QWidget(parent) {
    setGeometry(parent->rect());
    setAttribute(Qt::WA_NoSystemBackground);
    setCursor(Qt::CrossCursor);
    // Taking the focus would repaint a row whose buttons show only while its view has it; Escape comes via qApp.
    setFocusPolicy(Qt::NoFocus);
    qApp->installEventFilter(this);
    show();
    raise();
  }

  ~CShotRegionPicker() override { qApp->removeEventFilter(this); }

  /** @return the rectangle in the parent's coordinates, empty when the writer cancelled */
  QRect pick() {
    loop.exec();
    return region.normalized();
  }

 protected:
  void paintEvent(QPaintEvent*) override {
    QPainter p(this);
    const QRect& chosen = region.normalized();
    const QColor dim(0, 0, 0, 90);
    p.fillRect(QRect(0, 0, width(), chosen.top()), dim);
    p.fillRect(QRect(0, chosen.bottom() + 1, width(), height() - chosen.bottom() - 1), dim);
    p.fillRect(QRect(0, chosen.top(), chosen.left(), chosen.height()), dim);
    p.fillRect(QRect(chosen.right() + 1, chosen.top(), width() - chosen.right() - 1, chosen.height()), dim);
    if (!chosen.isEmpty()) {
      p.setPen(QPen(Qt::white, 1, Qt::DashLine));
      p.drawRect(chosen.adjusted(0, 0, -1, -1));
    }
  }

  void mousePressEvent(QMouseEvent* e) override {
    pressed = true;
    origin = e->pos();
    region = QRect(origin, QSize());
    update();
  }

  void mouseMoveEvent(QMouseEvent* e) override {
    if (pressed) {
      region = QRect(origin, e->pos());
      update();
    }
  }

  void mouseReleaseEvent(QMouseEvent*) override { loop.quit(); }

  bool eventFilter(QObject* watched, QEvent* event) override {
    if (QEvent::KeyPress == event->type() && Qt::Key_Escape == static_cast<QKeyEvent*>(event)->key()) {
      region = QRect();
      loop.quit();
      return true;
    }
    return QWidget::eventFilter(watched, event);
  }

 private:
  QEventLoop loop;
  bool pressed = false;
  QPoint origin;
  QRect region;
};

/** @return true when @p widget is @p main or below it; unlike isAncestorOf() across a popup's window boundary */
bool isPartOf(const QWidget* main, const QWidget* widget) {
  for (const QWidget* w = widget; nullptr != w; w = w->parentWidget()) {
    if (w == main) {
      return true;
    }
  }
  return false;
}

/** @return `Demo Track` as `demo-track` */
QString slug(const QString& text) {
  QString out;
  for (const QChar c : text) {
    if (c.isLetterOrNumber()) {
      out += c.toLower();
    } else if (!out.isEmpty() && !out.endsWith('-')) {
      out += '-';
    }
  }
  while (out.endsWith('-')) {
    out.chop(1);
  }
  return out;
}

QString label(const QString& scenario) { return scenario.isEmpty() ? CShotFiles::kBaseLabel : scenario; }

QWidget* dialogParent() { return CMainWindow::getBestWidgetForParent(); }

/** @return an INI file's lines as `section/key` to the value as written; QSettings writes one line per key */
QMap<QString, QString> iniLines(const QString& path) {
  QMap<QString, QString> lines;
  QFile in(path);
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return lines;
  }
  QString section;
  while (!in.atEnd()) {
    const QString& line = QString::fromUtf8(in.readLine()).trimmed();
    if (line.startsWith('[') && line.endsWith(']')) {
      section = line.mid(1, line.size() - 2);
    } else if (const qsizetype equals = line.indexOf('='); equals > 0) {
      lines.insert(section + "/" + line.left(equals).trimmed(), line.mid(equals + 1).trimmed());
    }
  }
  return lines;
}
}  // namespace

CShotDocMode::CShotDocMode(const QDir& repo, const QString& page, const QString& scenario, const QString& trial,
                           QObject* parent)
    : QObject(parent),
      repo(repo),
      page(page),
      files(repo, page),
      scenario(CShotPage::kBaseScenario == scenario ? QString() : scenario),
      trial(trial),
      // Never doc/images: only publishing writes there.
      writer(std::make_unique<CShotWriter>(repo.absoluteFilePath("doc/images/_work"), "en")),
      ctx(std::make_unique<CShotContext>(*writer)) {
  recorder = new CShotRecorder(*ctx, this);
  placementTimer = new QTimer(this);
  placementTimer->setSingleShot(true);
  placementTimer->setInterval(kPlacementSettleMs);
  connect(placementTimer, &QTimer::timeout, this, &CShotDocMode::storePlacement);
}

CShotDocMode::~CShotDocMode() = default;

void CShotDocMode::start() {
  if (!qlOpts->doc.docChannel.isEmpty()) {
    channel = new QLocalSocket(this);
    connect(channel, &QLocalSocket::readyRead, this, [this]() {
      while (nullptr != channel && channel->canReadLine()) {
        obey(QString::fromUtf8(channel->readLine()).trimmed());
      }
    });
    // A state without its launcher is an orphan.
    connect(channel, &QLocalSocket::disconnected, this, [this]() { leave("the launcher is gone"); });
    // A connect that fails emits errorOccurred and never disconnected (measured: ServerNotFoundError).
    connect(channel, &QLocalSocket::errorOccurred, this, [this]() {
      if (QLocalSocket::ConnectedState != channel->state()) {
        leave("there is no launcher");
      }
    });
    channel->connectToServer(qlOpts->doc.docChannel);
  }
  // The key press reaches the widget under the focus; only the application sees all of them.
  qApp->installEventFilter(this);
  QTimer::singleShot(0, this, &CShotDocMode::slotSetUp);
}

bool CShotDocMode::eventFilter(QObject* watched, QEvent* event) {
  // quitOnLastWindowClosed does not end the process.
  if (QEvent::Close == event->type() && !CMainWindow::isNull() && watched == &CMainWindow::self()) {
    leave("the application window was closed");
    return QObject::eventFilter(watched, event);
  }

  // Every state process comes up where the writer moved the last one to.
  if (QEvent::Move == event->type() && ready && !CMainWindow::isNull() && watched == &CMainWindow::self()) {
    // A drag moves the window many times; each write is a file sync.
    placementTimer->start();
    return QObject::eventFilter(watched, event);
  }

  // Shown inside the request's frame; a replay of its picture has only the step to open it again.
  if (QEvent::Show == event->type()) {
    if (QMenu* menu = qobject_cast<QMenu*>(watched); nullptr != menu) {
      if (const QJsonObject& step = recorder->contextMenuStep(); !step.isEmpty()) {
        contextMenu = menu;
        contextMenuOpen = step;
      }
    }
    return QObject::eventFilter(watched, event);
  }

  if (QEvent::KeyPress != event->type()) {
    return QObject::eventFilter(watched, event);
  }
  const QKeyEvent* key = static_cast<QKeyEvent*>(event);
  const Qt::KeyboardModifiers modifiers =
      key->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier | Qt::MetaModifier);
  if (kTagKey != key->key() || kTagModifiers != modifiers) {
    return QObject::eventFilter(watched, event);
  }
  if (ready && !busy) {
    busy = true;
    tag();
    busy = false;
  }
  return true;
}

void CShotDocMode::leave(const QString& why) {
  if (leaving) {
    return;
  }
  leaving = true;
  qDebug().noquote() << "doc:" << why << "- the state process ends";
  if (placementTimer->isActive()) {
    placementTimer->stop();
    storePlacement();
  }
  qApp->removeEventFilter(this);
  // Destroying the main window while shown crashes in its docks' visibilityChanged.
  if (!CMainWindow::isNull()) {
    CMainWindow::self().close();
  }
  qApp->exit(0);
  // exit() ends only a running event loop: called before exec() or from a nested loop the process stays (measured).
  QTimer::singleShot(kLeaveTimeoutMs, qApp, []() {
    qWarning() << "doc: the state process did not end by itself";
    std::_Exit(0);
  });
}

void CShotDocMode::slotSetUp() {
  QEventLoop loop;
  QTimer::singleShot(kStartupMs, &loop, &QEventLoop::quit);
  loop.exec(QEventLoop::ExcludeUserInputEvents);
  if (leaving) {
    return;
  }

  const QString& name = trial.isEmpty() ? label(scenario) : trial;
  if (0 != CShotFixture::load(*ctx)) {
    send(trial.isEmpty() ? QString("ready The fixture did not load in %1. The console says why.").arg(name)
                         : QString("trial-failed The fixture did not load, so %1 was not stored.").arg(name));
    if (!trial.isEmpty()) {
      files.dropParked();
    }
    return;
  }
  CMainWindow* main = ctx->mainWindow();

  // Taken before the geometry is applied again, and replayed for the base row after it.
  QJsonArray base{CShotPage::layoutOf(*main)};
  if (nullptr != ctx->canvas()) {
    base.append(CShotPage::viewOf(ctx->canvas()));
  }

  // What CMainWindow restores in its constructor is a size the docks grow past once they are populated.
  {
    SETTINGS;
    const QByteArray& geometry = cfg.value("MainWindow/geometry").toByteArray();
    if (!geometry.isEmpty()) {
      main->restoreGeometry(geometry);
    }
  }

  if (!trial.isEmpty()) {
    runTrial();
    placeWindow();
    return;
  }

  QString problem;
  {
    const CShootWarnings warnings;
    if (scenario.isEmpty()) {
      if (0 != CShotReplay::replay(base, *ctx, {})) {
        problem = QString("The base's arrangement does not come back: %1").arg(CShootWarnings::first());
      }
    } else {
      const QJsonObject& recorded = files.scenarios();
      if (!recorded.contains(scenario)) {
        problem =
            QString("The page has no scenario %1; the application is in %2.").arg(scenario, CShotFiles::kBaseLabel);
        scenario.clear();
      } else if (0 != CShotReplay::replay(recorded.value(scenario).toArray(), *ctx, {})) {
        problem = QString("%1 does not replay: %2").arg(scenario, CShootWarnings::first());
      }
    }
  }
  // What is on screen now, broken or not: a picture of it performs nothing again.
  ctx->setLiveScenario(scenario);
  // Last, so nothing a scenario replays moves the window again.
  placeWindow();
  snapshotSetup();
  ready = true;
  qDebug() << "doc: state" << label(scenario) << "of page" << page;
  send("ready " + (problem.isEmpty() ? QString("The application is in %1. Point at what to photograph and press "
                                               "Ctrl+Shift+F9.")
                                           .arg(label(scenario))
                                     : problem));
}

void CShotDocMode::runTrial() {
  QJsonArray steps;
  QString problem = files.parkedRecording(steps);
  if (problem.isEmpty() && !QFileInfo::exists(files.trialConfig())) {
    problem = QString("The recording's settings %1 are missing.").arg(files.trialConfig());
  }
  if (problem.isEmpty()) {
    const CShootWarnings warnings;
    if (0 != CShotReplay::replay(steps, *ctx, {})) {
      problem = QString("It does not replay: %1 Record it differently.").arg(CShootWarnings::first());
    }
  }
  if (problem.isEmpty()) {
    problem = files.storeScenario(trial, steps, files.trialConfig());
  }
  files.dropParked();

  if (!problem.isEmpty()) {
    qWarning().noquote() << "doc:" << trial << "was not stored:" << problem;
    send(QString("trial-failed %1 was not stored. %2").arg(trial, problem));
    return;
  }

  scenario = std::exchange(trial, QString());
  ctx->setLiveScenario(scenario);
  snapshotSetup();
  ready = true;
  qDebug() << "doc: state" << scenario << "recorded on page" << page;
  send(QString("ready %1 replays and is stored. The application is in it; point at what to photograph and press "
               "Ctrl+Shift+F9.")
           .arg(scenario));
}

void CShotDocMode::storePlacement() {
  if (!CMainWindow::isNull()) {
    QSettings(files.placementFile(), QSettings::IniFormat).setValue(kWindowPosKey, CMainWindow::self().pos());
  }
}

void CShotDocMode::placeWindow() {
  // A stored geometry carries a position on the whole desktop; the window goes where the writer left it last.
  if (CMainWindow::isNull()) {
    return;
  }
  CMainWindow& window = CMainWindow::self();
  const QVariant& pos = QSettings(files.placementFile(), QSettings::IniFormat).value(kWindowPosKey);
  if (pos.isValid() &&
      nullptr != QGuiApplication::screenAt(QRect(pos.toPoint(), window.frameGeometry().size()).center())) {
    // move() places the frame.
    window.move(pos.toPoint());
    qDebug() << "doc: window at" << window.frameGeometry();
  }
}

void CShotDocMode::obey(const QString& line) {
  const QString& verb = line.section(' ', 0, 0);
  const QString& rest = line.section(' ', 1);

  if ("select" == verb) {
    wantedShot = rest;
    return;
  }
  if ("sync" == verb) {
    // Every file is read when it is used.
    return;
  }
  if (!ready) {
    send(QString("status The application is not set up yet; '%1' was not done.").arg(verb));
    return;
  }
  if (busy) {
    send(QString("status Answer the question in the application first; '%1' was not done.").arg(verb));
    return;
  }

  busy = true;
  if ("region" == verb) {
    takeRegion(rest);
  } else if ("update" == verb) {
    updateScenario(rest);
  } else if ("record" == verb) {
    startRecording();
  } else if ("stop" == verb) {
    stopRecording();
  } else if ("name" == verb) {
    parkRecording(rest);
  } else if ("discard" == verb) {
    pendingSteps = QJsonArray();
    // A running recording still needs the settings it started from.
    if (!recorder->isRecording()) {
      files.dropParked();
    }
  } else {
    qWarning() << "doc: the state process does not know" << line;
    send(QString("status The application does not know '%1'.").arg(verb));
  }
  busy = false;
}

void CShotDocMode::send(const QString& line) {
  if (nullptr == channel || QLocalSocket::ConnectedState != channel->state()) {
    qDebug().noquote() << "doc:" << line;
    return;
  }
  channel->write(line.toUtf8() + '\n');
  channel->flush();
}

void CShotDocMode::report(const QString& status) { send("status " + status); }

// --- pictures ------------------------------------------------------------------------------------

QWidget* CShotDocMode::activeTarget() {
  if (QWidget* popup = QApplication::activePopupWidget(); nullptr != popup) {
    return popup;
  }
  if (QWidget* modal = QApplication::activeModalWidget(); nullptr != modal) {
    return modal;
  }
  if (QWidget* active = QApplication::activeWindow(); nullptr != active) {
    return active;
  }
  QWidget* focus = QApplication::focusWidget();
  return nullptr == focus ? nullptr : focus->window();
}

QString CShotDocMode::onScreenName(const QWidget* w) {
  if (nullptr == w) {
    return QString();
  }
  if (const QDockWidget* dock = qobject_cast<const QDockWidget*>(w); nullptr != dock) {
    return dock->windowTitle();
  }
  // A page of a tab widget is named by its tab; its parent is the stack inside the tab widget.
  if (const QWidget* stack = w->parentWidget(); nullptr != stack) {
    if (const QTabWidget* tabs = qobject_cast<const QTabWidget*>(stack->parentWidget()); nullptr != tabs) {
      if (const qint32 index = tabs->indexOf(const_cast<QWidget*>(w)); index >= 0) {
        return tabs->tabText(index);
      }
    }
  }
  if (const QGroupBox* group = qobject_cast<const QGroupBox*>(w); nullptr != group) {
    return group->title();
  }
  return w->isWindow() ? w->windowTitle() : QString();
}

QList<QWidget*> CShotDocMode::livePartsAt(CMainWindow* main, QStringList& labels) const {
  // Where the writer points, not where the keyboard focus is.
  QWidget* start = QApplication::widgetAt(QCursor::pos());
  if (nullptr == start || !isPartOf(main, start)) {
    start = QApplication::focusWidget();
  }

  QList<QWidget*> parts;
  QStringList names;
  QStringList addresses;
  for (QWidget* w = start; nullptr != w && w != main; w = w->parentWidget()) {
    // Only what a shot can find again.
    const std::optional<QString>& address = CShotAddress::addressOf(main, w);
    if (!address.has_value() || address->isEmpty()) {
      continue;
    }
    // Qt's own plumbing is never what the writer pointed at.
    if (w->objectName().startsWith("qt_")) {
      continue;
    }
    const QString& name = onScreenName(w);
    parts << w;
    names << (name.isEmpty() ? QString::fromLatin1(w->metaObject()->className()) : name);
    addresses << *address;
  }
  parts << main;
  names << QString("The whole application");
  addresses << QString();

  // chooseLivePart() finds the part by its label, so two that read the same are told apart by their address.
  labels.clear();
  for (qsizetype i = 0; i < names.size(); i++) {
    const QString& name = names.at(i);
    const bool ambiguous = names.count(name) > 1 && !addresses.at(i).isEmpty();
    labels << (ambiguous ? QString("%1 (%2)").arg(name, addresses.at(i)) : name);
  }
  return parts;
}

QWidget* CShotDocMode::chooseLivePart(CMainWindow* main, const QList<QWidget*>& parts,
                                      const QStringList& labels) const {
  if (parts.size() == 1) {
    return main;
  }
  bool ok = false;
  const QString& chosen = QInputDialog::getItem(dialogParent(), "Take a picture of", "Part:", labels, 0, false, &ok);
  return ok ? parts.value(labels.indexOf(chosen), main) : nullptr;
}

QString CShotDocMode::askForId() const {
  // Only what the page references: a name invented here would be a shot no page uses.
  QStringList wanted;
  QStringList without;
  QStringList with;
  const QList<CShotFiles::row_t>& rows = files.rows();
  for (const CShotFiles::row_t& row : rows) {
    if (CShotFiles::eNotUsed == row.state) {
      continue;
    }
    if (row.id == wantedShot) {
      wanted << row.id;
    } else if (CShotFiles::eMissing == row.state || CShotFiles::eUnregistered == row.state) {
      without << row.id;
    } else {
      with << row.id;
    }
  }
  const QStringList& choices = wanted + without + with;

  if (choices.isEmpty()) {
    QMessageBox::information(dialogParent(), "Take a picture",
                             "The page references no picture. Put an image line where you want one, and its name "
                             "appears here.");
    return QString();
  }
  bool ok = false;
  const QString& id =
      QInputDialog::getItem(dialogParent(), "Take a picture", "Which picture is this?", choices, 0, false, &ok);
  return ok ? id : QString();
}

QString CShotDocMode::scenarioFor(const QString& id) const {
  const QJsonObject& shot = files.shot(id);
  return shot.isEmpty() ? scenario : shot["scenario"].toString();
}

bool CShotDocMode::takenHere(const QString& id) {
  const QString& own = scenarioFor(id);
  if (own == scenario) {
    return true;
  }
  report(QString("%1 is taken in %2 and the application is in %3. Select its row to take it there, or change its "
                 "scenario in the row first.")
             .arg(id, label(own), label(scenario)));
  return false;
}

QByteArray CShotDocMode::previousPicture(const QString& id) const {
  QFile in(files.workImage(id));
  return in.open(QIODevice::ReadOnly) ? in.readAll() : QByteArray();
}

bool CShotDocMode::confirmResult(const QString& id, const QByteArray& previous) const {
  const QImage image(files.workImage(id));

  QDialog preview(dialogParent());
  preview.setWindowTitle("Picture taken");
  QVBoxLayout* layout = new QVBoxLayout(&preview);
  QLabel* picture = new QLabel(&preview);
  picture->setPixmap(QPixmap::fromImage(image.scaled(QSize(720, 540), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  layout->addWidget(picture);
  layout->addWidget(new QLabel("Use it in your page as:", &preview));
  QPlainTextEdit* usage = new QPlainTextEdit(QString("![](images/%1.png)").arg(id), &preview);
  usage->setReadOnly(true);
  usage->setMaximumHeight(40);
  layout->addWidget(usage);
  QDialogButtonBox* buttons = new QDialogButtonBox(&preview);
  buttons->addButton("Keep", QDialogButtonBox::AcceptRole);
  buttons->addButton("Throw away", QDialogButtonBox::RejectRole);
  connect(buttons, &QDialogButtonBox::accepted, &preview, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &preview, &QDialog::reject);
  layout->addWidget(buttons);

  if (QDialog::Accepted == preview.exec()) {
    return true;
  }
  // The work picture a retake replaced stays the writer's.
  if (previous.isEmpty()) {
    QFile::remove(files.workImage(id));
  } else {
    QSaveFile out(files.workImage(id));
    if (!out.open(QIODevice::WriteOnly) || out.write(previous) < 0 || !out.commit()) {
      qWarning() << "doc: the previous work picture of" << id << "cannot be put back";
    }
  }
  return false;
}

void CShotDocMode::reportUnexposed(const QWidget* target) const {
  const QString& className = QString::fromLatin1(target->metaObject()->className());
  const QString& title = target->windowTitle().isEmpty() ? target->objectName() : target->windowTitle();
  // A dialog without Q_OBJECT reports its base class; the object name identifies it then.
  const QString& line =
      QString("SHOT_EXPOSE(\"%1\", \"%2\", %1, [](CShotContext&, QWidget* p) -> QWidget* { return new %1(p); });")
          .arg(className, title);
  qWarning().noquote() << "doc: no exposure for" << className << "object name" << target->objectName() << "-" << line;

  QDialog dialog(dialogParent());
  dialog.setWindowTitle("Cannot photograph this yet");
  QVBoxLayout* layout = new QVBoxLayout(&dialog);
  QLabel* text = new QLabel(QString("\"%1\" cannot be photographed yet. A developer adds this line to "
                                    "src/qmapshack/shoot/CShotExposures.cpp, with the constructor's arguments if it "
                                    "needs more than a parent (object name: %2):")
                                .arg(title, target->objectName()),
                            &dialog);
  text->setWordWrap(true);
  layout->addWidget(text);
  QPlainTextEdit* code = new QPlainTextEdit(line, &dialog);
  code->setReadOnly(true);
  layout->addWidget(code);
  QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
  QPushButton* copy = buttons->addButton("Copy", QDialogButtonBox::ActionRole);
  connect(copy, &QPushButton::clicked, &dialog, [line]() { QApplication::clipboard()->setText(line); });
  connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  layout->addWidget(buttons);
  dialog.exec();
}

void CShotDocMode::tag() {
  if (recorder->isRecording()) {
    report("Stop the recording first: taking a picture drives the application, and that would be recorded.");
    return;
  }
  CMainWindow* main = ctx->mainWindow();
  QWidget* target = activeTarget();
  if (nullptr == main || nullptr == target) {
    return;
  }

  // The entry under the pointer is highlighted and a replay has no pointer; read before a question takes the focus.
  QString activeEntry;
  if (const QMenu* menu = qobject_cast<const QMenu*>(target); nullptr != menu && nullptr != menu->activeAction()) {
    activeEntry = menu->activeAction()->objectName();
  }

  // Rendered before any question: while a question is up the application has no focus, and a row that draws its
  // buttons only with focus would be photographed without them.
  QList<QWidget*> parts;
  QStringList labels;
  QHash<const QWidget*, QImage> pictures;
  if (isPartOf(main, target)) {
    parts = livePartsAt(main, labels);
    pictures = CShotWriter::renderAll(parts);
  }

  // A menu belongs to what opened it and is found again; another window is a dialog.
  const bool ownWindow = target->isWindow() && target != main && nullptr == qobject_cast<QMenu*>(target);
  const bool fromScenario = ownWindow && !scenario.isEmpty();

  QString exposure;
  if (ownWindow && !fromScenario) {
    exposure = CShotRegistry::self().exposureOf(target);
    if (exposure.isEmpty()) {
      reportUnexposed(target);
      return;
    }
  }
  const bool live = !ownWindow && isPartOf(main, target);
  if (!live && !fromScenario && exposure.isEmpty()) {
    reportUnexposed(target);
    return;
  }

  if (live) {
    target = chooseLivePart(main, parts, labels);
    if (nullptr == target) {
      return;
    }
  }

  const QString& id = askForId();
  if (id.isEmpty() || !takenHere(id)) {
    return;
  }

  QJsonObject shot{{"id", id}};
  if (!scenario.isEmpty()) {
    shot["scenario"] = scenario;
  }

  // Which tab is open is part of the picture and nothing else records it.
  QJsonObject set;
  if (const QTabWidget* tabs = qobject_cast<const QTabWidget*>(target); nullptr != tabs) {
    set["currentIndex"] = tabs->currentIndex();
  }
  const QList<QTabWidget*>& children = target->findChildren<QTabWidget*>();
  for (const QTabWidget* tabs : children) {
    const std::optional<QString>& address = CShotAddress::addressOf(target, tabs);
    if (address.has_value() && !address->isEmpty()) {
      set[*address + ".currentIndex"] = tabs->currentIndex();
    }
  }
  if (!set.isEmpty()) {
    shot["set"] = set;
  }

  if (fromScenario) {
    // The scenario opens the window again; the class says which one it has to be.
    shot["widget"] = QString();
    shot["window"] = QString::fromLatin1(target->metaObject()->className());
    shot["size"] = QJsonArray{main->width(), main->height()};
    if (!target->windowTitle().isEmpty()) {
      shot["note"] = target->windowTitle();
    }
  } else if (live) {
    shot["widget"] = CShotAddress::addressOf(main, target).value_or(QString());
    if (target == contextMenu) {
      shot["open"] = contextMenuOpen;
    }
    if (target == contextMenu && !activeEntry.isEmpty()) {
      shot["active"] = activeEntry;
    }
    // A docker is laid out by the window, so its picture depends on the window's size too.
    shot["size"] = QJsonArray{main->width(), main->height()};
    if (!target->windowTitle().isEmpty()) {
      shot["note"] = target->windowTitle();
    }
  } else {
    // A size only when the writer's window differs from what the exposure comes out at by itself.
    const std::unique_ptr<QWidget> probe(CShotRegistry::self().buildExposure(exposure, *ctx, dialogParent()));
    const QSize& defaultSize = (nullptr == probe) ? QSize() : CShotWriter::render(probe.get(), QSize()).size();
    shot["exposure"] = exposure;
    shot["note"] = CShotRegistry::self().exposureDescription(exposure);
    if (defaultSize != target->size()) {
      shot["size"] = QJsonArray{target->width(), target->height()};
    }
  }

  const QByteArray& previous = previousPicture(id);
  bool taken = false;
  // Only a live part is written from what was rendered at the key press; a dialog parented to the main window is in
  // pictures too, and its shot says exposure.
  if (live && pictures.contains(target)) {
    const QImage& picture = pictures.value(target);
    taken = !picture.isNull() && !writer->write(picture, id).isEmpty();
  } else {
    taken = 0 == CShotPage::shootOne(shot, *ctx, files.scenarios());
  }
  if (!taken) {
    report(QString("%1 could not be taken; the console says why. Nothing was changed.").arg(id));
    return;
  }
  if (!confirmResult(id, previous)) {
    report(QString("%1 was thrown away.").arg(id));
    return;
  }
  if (const QString& error = files.storeShot(shot); !error.isEmpty()) {
    report(error);
    return;
  }
  send("tagged " + id);
  report(QString("Took %1.").arg(id) + driftWarning(scenario));
}

void CShotDocMode::takeRegion(const QString& wanted) {
  CMainWindow* main = ctx->mainWindow();
  if (nullptr == main) {
    return;
  }
  const QString& id = wanted.isEmpty() ? askForId() : wanted;
  if (id.isEmpty() || !takenHere(id)) {
    return;
  }

  report("Drag out the part you want. Escape cancels.");
  QRect region;
  {
    // Gone before the window is photographed.
    CShotRegionPicker picker(main);
    region = picker.pick();
  }
  CShotWriter::settle(main);
  if (region.width() < kMinRegionSide || region.height() < kMinRegionSide) {
    report("Nothing taken.");
    return;
  }

  QJsonObject shot{{"id", id}, {"widget", QString()}};
  if (!scenario.isEmpty()) {
    shot["scenario"] = scenario;
  }
  // The rectangle is measured against the window's size.
  shot["size"] = QJsonArray{main->width(), main->height()};
  shot["rect"] = QJsonArray{region.x(), region.y(), region.width(), region.height()};
  shot["note"] =
      scenario.isEmpty() ? QString("A part of the window") : QString("A part of the window in %1").arg(scenario);

  const QByteArray& previous = previousPicture(id);
  if (0 != CShotPage::shootOne(shot, *ctx, files.scenarios())) {
    report(QString("%1 could not be taken; the console says why. Nothing was changed.").arg(id));
    return;
  }
  if (!confirmResult(id, previous)) {
    report(QString("%1 was thrown away.").arg(id));
    return;
  }
  if (const QString& error = files.storeShot(shot); !error.isEmpty()) {
    report(error);
    return;
  }
  send("tagged " + id);
  report(QString("Took %1.").arg(id) + driftWarning(scenario));
}

// --- settings ------------------------------------------------------------------------------------

QByteArray CShotDocMode::portableGeometry(const QByteArray& geometry, bool& known) {
  QDataStream in(geometry);
  in.setVersion(QDataStream::Qt_4_0);
  quint32 magic = 0;
  quint16 major = 0;
  quint16 minor = 0;
  QRect frame;
  QRect normal;
  qint32 screen = 0;
  quint8 maximized = 0;
  quint8 fullScreen = 0;
  qint32 screenWidth = 0;
  QRect rect;
  in >> magic >> major >> minor >> frame >> normal >> screen >> maximized >> fullScreen >> screenWidth >> rect;
  known = QDataStream::Ok == in.status() && kGeometryMagic == magic && kGeometryMajor == major;
  if (!known) {
    return geometry;
  }

  QByteArray out;
  QDataStream stream(&out, QIODevice::WriteOnly);
  stream.setVersion(QDataStream::Qt_4_0);
  stream << magic << major << minor << frame << normal << qint32(0) << maximized << fullScreen << qint32(0) << rect;
  return out;
}

bool CShotDocMode::namesAPlace(const QVariant& value) {
  // A byte array is a blob - geometry, a state - and QDir would read its bytes as a path.
  if (QMetaType::QByteArray == value.typeId()) {
    return false;
  }
  const auto absolute = [](const QString& entry) { return QDir::isAbsolutePath(QDir::fromNativeSeparators(entry)); };
  if (QMetaType::QStringList == value.typeId()) {
    const QStringList& entries = value.toStringList();
    return std::any_of(entries.begin(), entries.end(), absolute);
  }
  return absolute(value.toString());
}

qint32 CShotDocMode::storeSettings(const QString& path, qint32& dropped) const {
  SETTINGS;
  // What the destructor writes, which has not run; then the geometry again, without this machine's screen in it.
  if (CMainWindow* main = ctx->mainWindow(); nullptr != main) {
    bool known = false;
    main->saveConfig();
    cfg.setValue("MainWindow/geometry", portableGeometry(main->saveGeometry(), known));
    if (!known) {
      qWarning() << "doc: QWidget::saveGeometry() wrote a record portableGeometry() does not know; the stored "
                    "geometry carries this machine's screen";
    }
  }
  cfg.sync();

  QSettings out(path, QSettings::IniFormat);
  out.clear();
  qint32 stored = 0;
  dropped = 0;
  const QStringList& keys = cfg.allKeys();
  for (const QString& key : keys) {
    const QVariant& value = cfg.value(key);
    if (namesAPlace(value)) {
      dropped++;
      continue;
    }
    out.setValue(key, value);
    stored++;
  }
  out.sync();
  return (QSettings::NoError == out.status()) ? stored : -1;
}

void CShotDocMode::snapshotSetup() {
  qint32 dropped = 0;
  if (!setUpWith.isValid() || storeSettings(setUpWith.filePath(kSetupConfig), dropped) < 0) {
    qWarning() << "doc: the settings could not be written; a change to them will not be reported";
  }
}

qint32 CShotDocMode::settingsDrift() const {
  const QString& before = setUpWith.filePath(kSetupConfig);
  const QTemporaryDir scratch;
  if (!QFileInfo::exists(before) || !scratch.isValid()) {
    return 0;
  }
  // As text: QSettings reads its own process' writes back typed (a one-entry QStringList) and another's as QString.
  qint32 dropped = 0;
  const QString& current = scratch.filePath("current.ini");
  if (storeSettings(current, dropped) < 0) {
    return 0;
  }
  const QMap<QString, QString>& now = iniLines(current);
  const QMap<QString, QString>& then = iniLines(before);

  QStringList keys = now.keys() + then.keys();
  keys.removeDuplicates();
  qint32 drifted = 0;
  for (const QString& key : std::as_const(keys)) {
    // The arrangement belongs to the scenario's steps.
    if (!key.startsWith("MainWindow/") && now.value(key) != then.value(key)) {
      qDebug() << "doc: the setting" << key << "is" << now.value(key) << "and was" << then.value(key);
      drifted++;
    }
  }
  return drifted;
}

QString CShotDocMode::driftWarning(const QString& name) const {
  const qint32 drifted = settingsDrift();
  if (0 == drifted) {
    return QString();
  }
  return QString(
             " The settings on screen differ in %1 place(s) from the ones %2 was set up with, so a replay will "
             "not produce this picture. %3")
      .arg(drifted)
      .arg(label(name), name.isEmpty() ? QString("Press Save config, or set them back.")
                                       : QString("Set them back, or record the scenario again with them."));
}

void CShotDocMode::updateScenario(const QString& target) {
  const QString& named = (CShotPage::kBaseScenario == target) ? QString() : target;
  if (named != scenario) {
    report(QString("The application is in %1, not %2; nothing was stored.").arg(label(scenario), label(named)));
    return;
  }
  // A configuration stored after a scenario ran holds what its steps did, which a replay would do again on top.
  if (!scenario.isEmpty()) {
    report(QString("%1 keeps the settings it was recorded with. Record it again to change them.").arg(scenario));
    return;
  }

  qint32 dropped = 0;
  const qint32 stored = storeSettings(files.baseFile(), dropped);
  if (stored < 0) {
    report(QString("%1 cannot be written.").arg(repo.relativeFilePath(files.baseFile())));
    return;
  }
  snapshotSetup();
  report(
      QString(
          "%1 settings are this page's base now; %2 naming a place on this machine were left out. Scenarios keep the "
          "settings they were recorded with.")
          .arg(stored)
          .arg(dropped));
}

// --- recording -----------------------------------------------------------------------------------

void CShotDocMode::startRecording() {
  if (recorder->isRecording()) {
    report("A recording is already running; press Stop to end it.");
    return;
  }
  // The launcher starts a recording in a process in the base, so it is a whole state.
  if (!scenario.isEmpty()) {
    report(QString("A recording starts from %1; the application is in %2.").arg(CShotFiles::kBaseLabel, scenario));
    return;
  }
  // The scenario is stored with what its steps were recorded against, never with the settings at Stop.
  qint32 dropped = 0;
  if (storeSettings(files.trialConfig(), dropped) < 0) {
    report(QString("%1 cannot be written; nothing is being recorded.").arg(files.trialConfig()));
    return;
  }

  pendingSteps = QJsonArray();
  recorder->start();
  send("recording");
  report(
      "Recording from the base. Do what the scenario is - load, select, expand, zoom the map, click an item on "
      "it - then press Stop.");
}

void CShotDocMode::stopRecording() {
  if (!recorder->isRecording()) {
    report("There is no recording to stop.");
    return;
  }

  const QJsonArray& steps = recorder->stop();
  // stop() returns the start state even when nothing was done.
  if (0 == recorder->steps()) {
    files.dropParked();
    send("recorded-none");
    report(
        "Nothing was recorded. What is kept is state - a selection, an expanded project, a control you changed, "
        "the map area, a click on an item.");
    return;
  }
  pendingSteps = steps;

  const QString& path =
      (nullptr == ctx->wksList()) ? QString() : CShotAddress::itemPathOf(ctx->wksList()->currentItem());
  const QString& base = slug(path.section(':', -1)).isEmpty() ? QString("scenario") : slug(path.section(':', -1));
  const QStringList& known = files.scenarioNames();
  QString suggestion = base;
  for (qint32 i = 2; known.contains(suggestion); i++) {
    suggestion = QString("%1-%2").arg(base).arg(i);
  }
  send("recorded-pending " + suggestion);
}

void CShotDocMode::parkRecording(const QString& name) {
  const QJsonArray steps = std::exchange(pendingSteps, QJsonArray());
  if (steps.isEmpty()) {
    report("There is no recording to name.");
    return;
  }
  if (const QString& problem = CShotFiles::nameProblem(name); !problem.isEmpty()) {
    files.dropParked();
    report(problem + " The recording was thrown away; record again.");
    return;
  }

  QString error = files.parkRecording(steps);
  // Written when the recording started; the trial replays the steps against it and it becomes the scenario's.
  if (error.isEmpty() && !QFileInfo::exists(files.trialConfig())) {
    error = QString("%1, the settings the recording started from, is gone.").arg(files.trialConfig());
  }
  if (!error.isEmpty()) {
    files.dropParked();
    report(error + " The recording was thrown away.");
    return;
  }
  qDebug().noquote() << "doc: parked" << name << QString::fromUtf8(QJsonDocument(steps).toJson(QJsonDocument::Compact));
  // The launcher answers with a state that replays it and stores it only then.
  send("recorded " + name);
}
