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

#include "shoot/CShotPage.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMenu>
#include <QPointer>
#include <QRect>
#include <QRegularExpression>
#include <QSet>
#include <QSize>
#include <QSplitter>
#include <QTabWidget>
#include <QVariant>
#include <QWidget>
#include <cmath>
#include <memory>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/proj_x.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotRegistry.h"
#include "shoot/CShotReplay.h"
#include "shoot/CShotWriter.h"

namespace {
const QSet<QString> kShotKeys = {"active",   "exposure", "id",   "note",   "open",  "rect",
                                 "scenario", "set",      "size", "widget", "window"};

/** Coordinates of two views closer than this are the same place [°]. */
constexpr qreal kViewEpsilon = 1e-9;

/** @return the central tab widget holding the canvases, nullptr when there is no canvas */
QTabWidget* centralTabs(const CMainWindow& main) {
  const QList<CCanvas*>& canvases = main.getCanvas();
  for (QWidget* w = canvases.isEmpty() ? nullptr : canvases.first()->parentWidget(); nullptr != w;
       w = w->parentWidget()) {
    if (QTabWidget* tabs = qobject_cast<QTabWidget*>(w); nullptr != tabs) {
      return tabs;
    }
  }
  return nullptr;
}

/** @return the active popup, else the active modal dialog, else @p main */
QWidget* topmost(QWidget* main) {
  QWidget* top = CShotReplay::blocker();
  return (nullptr != top) ? top : main;
}

/** @return false when the shot has a `size` that is not [width, height] */
bool readSize(const QJsonObject& shot, QSize& size) {
  size = QSize();
  if (!shot.contains("size")) {
    return true;
  }
  const QJsonArray& value = shot["size"].toArray();
  size = QSize(value.at(0).toInt(), value.at(1).toInt());
  return 2 == value.size() && !size.isEmpty();
}

/** @return false when the shot has a `rect` that is not [x, y, width, height] */
bool readRect(const QJsonObject& shot, QRect& rect) {
  rect = QRect();
  if (!shot.contains("rect")) {
    return true;
  }
  const QJsonArray& value = shot["rect"].toArray();
  rect = QRect(value.at(0).toInt(), value.at(1).toInt(), value.at(2).toInt(), value.at(3).toInt());
  return 4 == value.size() && !rect.isEmpty();
}

/** @return the shot's key and shape errors, each warned about; a misspelt key would silently be ignored */
qint32 shotProblems(const QJsonObject& shot, const QString& id, const QJsonObject& scenarios) {
  qint32 failures = 0;
  const QStringList& keys = shot.keys();
  for (const QString& key : keys) {
    if (!kShotKeys.contains(key)) {
      qWarning() << "shoot:" << id << "has an unknown key" << key;
      failures++;
    }
  }

  QSize size;
  if (!readSize(shot, size)) {
    qWarning() << "shoot:" << id << "has a size that is not [width, height]:" << shot["size"];
    failures++;
  }
  QRect rect;
  if (!readRect(shot, rect)) {
    qWarning() << "shoot:" << id << "has a rect that is not [x, y, width, height]:" << shot["rect"];
    failures++;
  }
  if (shot.contains("set") && !shot["set"].isObject()) {
    qWarning() << "shoot:" << id << "has a set that is not an object:" << shot["set"];
    failures++;
  }
  const QString& scenario = shot["scenario"].toString();
  if (shot.contains("scenario") && !scenarios[scenario].isArray()) {
    qWarning() << "shoot:" << id << "wants the scenario" << scenario << "- the ones there are:" << scenarios.keys();
    failures++;
  }
  if ((shot.contains("scenario") || shot.contains("open")) && !size.isValid()) {
    qWarning() << "shoot:" << id << "runs steps and has no size";
    failures++;
  }
  if (shot.contains("open") && !shot["open"].isObject()) {
    qWarning() << "shoot:" << id << "has an open that is not a step:" << shot["open"];
    failures++;
  }
  if (shot.contains("exposure") && shot.contains("widget")) {
    qWarning() << "shoot:" << id << "has both the exposure" << shot["exposure"].toString() << "and a widget";
    failures++;
  }
  if (shot.contains("exposure") && !CShotRegistry::self().exposureNames().contains(shot["exposure"].toString())) {
    qWarning() << "shoot:" << id << "wants the exposure" << shot["exposure"].toString()
               << "- the ones there are:" << CShotRegistry::self().exposureNames();
    failures++;
  }
  return failures;
}

/** @return false, warned about, when @p main does not take @p size; a window's minimum silently wins */
bool sizeMainWindow(QWidget* main, const QSize& size, const QString& id) {
  main->resize(size);
  CShotWriter::settle(main);
  if (main->size() == size) {
    return true;
  }
  qWarning().noquote() << QString(
                              "shoot: %1: the application window could not be made %2 x %3 on this computer, it is "
                              "%4 x %5.")
                              .arg(id)
                              .arg(size.width())
                              .arg(size.height())
                              .arg(main->width())
                              .arg(main->height());
  return false;
}

/** @return what the shot photographs, nullptr when it is not there; a built exposure is handed to @p exposure */
QWidget* pictureTarget(const QJsonObject& shot, const QString& id, CShotContext& ctx, QWidget* main,
                       std::unique_ptr<QWidget>& exposure) {
  if (shot.contains("exposure")) {
    const QString& name = shot["exposure"].toString();
    exposure.reset(CShotRegistry::self().buildExposure(name, ctx, CMainWindow::getBestWidgetForParent()));
    if (nullptr == exposure) {
      qWarning() << "shoot:" << id << "cannot build the exposure" << name;
    }
    return exposure.get();
  }
  const QString& address = shot["widget"].toString();
  QWidget* widget = address.isEmpty() ? topmost(main) : CShotAddress::resolve(main, address);
  if (nullptr == widget) {
    qWarning().noquote() << "shoot:" << id
                         << "- what it photographs is not on screen. Take the picture again "
                            "in the panel.";
    qDebug().noquote() << "shoot:" << id << "photographs the widget at" << address;
  }
  return widget;
}

/** @return false when @p widget is no menu with the entry @p active; a context menu's actions are its builder's */
bool highlightEntry(QWidget* widget, const QString& active, const QString& id) {
  QMenu* menu = qobject_cast<QMenu*>(widget);
  QAction* action = nullptr;
  const QList<QAction*>& actions = (nullptr == menu) ? QList<QAction*>() : menu->actions();
  for (QAction* entry : actions) {
    if (entry->objectName() == active) {
      action = entry;
    }
  }
  if (nullptr == action) {
    qWarning() << "shoot:" << id << "has no menu entry" << active << "to highlight";
    return false;
  }
  menu->setActiveAction(action);
  return true;
}

/** A value a shot's `set` replaced, put back after the picture so shots stay independent. */
struct restore_t {
  QPointer<QObject> target;
  QString property;
  QVariant value;
};

/** @return the `set` entries that failed; what each one replaced is prepended to @p restore, newest first */
qint32 driveSet(QWidget* widget, const QJsonObject& set, const QString& id, QList<restore_t>& restore) {
  qint32 failed = 0;
  for (auto it = set.constBegin(); it != set.constEnd(); ++it) {
    // `child.property` addresses below the photographed widget; a bare property is its own.
    const qsizetype dot = it.key().lastIndexOf('.');
    const QString& target = (dot < 0) ? QString() : it.key().left(dot);
    const QString& property = it.key().mid(dot + 1);
    const QByteArray& name = property.toLatin1();
    QObject* driven = CShotAddress::resolve(widget, target);
    if (nullptr == driven) {
      qWarning() << "shoot:" << id << "has no" << target << "to set" << property << "on";
      failed++;
    } else if (driven->metaObject()->indexOfProperty(name.constData()) < 0) {
      qWarning() << "shoot:" << id << "has no property" << property << "on" << driven->metaObject()->className();
      failed++;
    } else {
      // Read before driving: a failed set may still have changed the value.
      restore.prepend({driven, property, driven->property(name.constData())});
      if (!CShotPage::driveProperty(driven, property, it.value().toVariant())) {
        qWarning() << "shoot:" << id << "cannot set" << it.key() << "to" << it.value().toVariant() << "- it is"
                   << driven->property(name.constData());
        failed++;
      }
    }
  }
  return failed;
}

/** @return the values that could not be put back */
qint32 restoreSet(const QList<restore_t>& restore, const QString& id) {
  qint32 failed = 0;
  for (const restore_t& before : restore) {
    if (!CShotPage::driveProperty(before.target, before.property, before.value)) {
      qWarning() << "shoot:" << id << "cannot put" << before.property << "back to" << before.value;
      failed++;
    }
  }
  return failed;
}
}  // namespace

bool CShotPage::driveProperty(QObject* target, const QString& property, const QVariant& value) {
  const QByteArray& name = property.toLatin1();
  // setProperty() on a name the class has not got adds a dynamic property and answers false.
  if (nullptr == target || target->metaObject()->indexOfProperty(name.constData()) < 0 ||
      !target->setProperty(name.constData(), value)) {
    return false;
  }
  // JSON has one number type: 1.0 is the same currentIndex as 1.
  const QVariant& now = target->property(name.constData());
  QVariant wanted = value;
  return wanted.convert(now.metaType()) && wanted == now;
}

QJsonObject CShotPage::viewOf(const CCanvas* canvas) {
  const QPointF& focus = canvas->getPosFocus() * RAD_TO_DEG;
  return QJsonObject{{"do", "view"}, {"lat", focus.y()}, {"lon", focus.x()}, {"zoom", canvas->getZoomIndex()}};
}

QJsonObject CShotPage::layoutOf(const CMainWindow& main) {
  QJsonObject action{{"do", "layout"}, {"state", QString::fromLatin1(main.saveState().toBase64())}};
  if (const QTabWidget* tabs = centralTabs(main); nullptr != tabs) {
    action["tab"] = tabs->currentIndex();
  }

  // saveState() covers dockers and toolbars and nothing inside the central widget.
  QJsonObject splitters;
  const QList<QSplitter*>& all = main.findChildren<QSplitter*>();
  for (const QSplitter* splitter : all) {
    const std::optional<QString>& address = CShotAddress::addressOf(&main, splitter);
    if (address.has_value() && !address->isEmpty()) {
      splitters[*address] = QString::fromLatin1(splitter->saveState().toBase64());
    }
  }
  if (!splitters.isEmpty()) {
    action["splitters"] = splitters;
  }
  return action;
}

bool CShotPage::applyLayout(CMainWindow& main, const QJsonObject& layout) {
  const bool restored = main.restoreState(QByteArray::fromBase64(layout["state"].toString().toLatin1()));
  CShotWriter::settle(&main);
  return restored;
}

qint32 CShotPage::applyArrangement(CMainWindow& main, const QJsonObject& layout) {
  qint32 failures = 0;
  if (layout.contains("tab")) {
    QTabWidget* tabs = centralTabs(main);
    const qint32 tab = layout["tab"].toInt(-1);
    if (nullptr == tabs || tab < 0 || tab >= tabs->count()) {
      // tab is 0-based; the writer counts from 1.
      qWarning().noquote() << QString(
                                  "shoot: the scenario was recorded with tab %1 open and the window has only %2 "
                                  "tab(s). Record the scenario again, starting from what the fixture opens.")
                                  .arg(layout["tab"].toInt(-1) + 1)
                                  .arg(nullptr == tabs ? 0 : tabs->count());
      failures++;
    } else {
      tabs->setCurrentIndex(tab);
    }
  }

  const QJsonObject& splitters = layout["splitters"].toObject();
  for (auto it = splitters.constBegin(); it != splitters.constEnd(); ++it) {
    QSplitter* splitter = qobject_cast<QSplitter*>(CShotAddress::resolve(&main, it.key()));
    if (nullptr == splitter) {
      qWarning().noquote() << "shoot: the scenario was recorded with a divider" << it.key()
                           << "that the window does not have now. Record the scenario again.";
      failures++;
    } else if (!splitter->restoreState(QByteArray::fromBase64(it.value().toString().toLatin1()))) {
      qWarning() << "shoot: the splitter at" << it.key() << "refuses the layout's state";
      failures++;
    }
  }
  CShotWriter::settle(&main);
  return failures;
}

bool CShotPage::applyView(CCanvas* canvas, const QJsonObject& view) {
  if (nullptr == canvas || !view["lat"].isDouble() || !view["lon"].isDouble() || !view["zoom"].isDouble()) {
    return false;
  }

  canvas->zoom(view["zoom"].toInt());
  canvas->moveTo(QPointF(view["lon"].toDouble(), view["lat"].toDouble()) * DEG_TO_RAD);

  const QJsonObject& now = viewOf(canvas);
  return now["zoom"].toInt() == view["zoom"].toInt() &&
         std::abs(now["lat"].toDouble() - view["lat"].toDouble()) < kViewEpsilon &&
         std::abs(now["lon"].toDouble() - view["lon"].toDouble()) < kViewEpsilon;
}

namespace {
/** @return the failures that keep @p shot from being taken at all, each warned about */
qint32 refusal(const QJsonObject& shot, const QJsonObject& scenarios) {
  const QString& id = shot["id"].toString();
  if (id.isEmpty()) {
    qWarning() << "shoot: a shot has no id:" << shot;
    return 1;
  }
  return shotProblems(shot, id, scenarios);
}

/** @return false, warned about, when there is no main window to run steps in or it does not take @p shot's size */
bool sizeForSteps(const QJsonObject& shot, QWidget* main) {
  const QString& id = shot["id"].toString();
  if (nullptr == main) {
    qWarning() << "shoot:" << id << "runs steps and there is no main window";
    return false;
  }
  QSize size;
  readSize(shot, size);
  // Before the steps: restoreState() distributes dock extents in pixels.
  return sizeMainWindow(main, size, id);
}

/**
   @brief Photograph @p shot as the screen is now.

   @param stepped  steps ran for it: the main window is sized already and a window a step opened keeps its own size
   @return the failures
 */
qint32 takePicture(const QJsonObject& shot, CShotContext& ctx, QWidget* main, bool stepped) {
  const QString& id = shot["id"].toString();
  QSize size;
  readSize(shot, size);
  QRect rect;
  readRect(shot, rect);

  // Outlives the restore of `set` values below.
  std::unique_ptr<QWidget> exposure;
  QWidget* widget = pictureTarget(shot, id, ctx, main, exposure);
  if (nullptr == widget) {
    return 1;
  }
  if (const QString& active = shot["active"].toString(); !active.isEmpty() && !highlightEntry(widget, active, id)) {
    return 1;
  }

  // Guards against photographing the window behind a dialog that failed to open.
  const QString& window = shot["window"].toString();
  const QString& className = QString::fromLatin1(widget->metaObject()->className());
  if (!window.isEmpty() && window != className) {
    qWarning().noquote() << QString(
                                "shoot: %1 - the window it photographs did not open: %2 is on top, not %3. If a "
                                "step failed above, that is why; else take the picture again in the panel.")
                                .arg(id, className, window);
    return 1;
  }

  // A window a scenario step opened is rendered at its own size.
  const QSize renderSize = (stepped && widget != main) ? QSize() : size;
  if (!stepped) {
    // Main window layout differs per platform, so such a shot must fix the window size.
    const bool sizedByWindow = (widget == main) || (!widget->isWindow() && widget->window() == main);
    if (sizedByWindow && !size.isValid()) {
      qWarning() << "shoot:" << id << "is sized by the main window and has no size";
      return 1;
    }
    // Before any `set`: the state depends on the window size.
    if (sizedByWindow && !sizeMainWindow(main, size, id)) {
      return 1;
    }
    if (!sizedByWindow && !widget->isWindow() && size.isValid()) {
      // A size applies to a window only.
      qWarning() << "shoot:" << id << "has a size and is laid out by" << widget->window()->metaObject()->className();
      return 1;
    }
  }

  QList<restore_t> restore;
  qint32 failed = driveSet(widget, shot["set"].toObject(), id, restore);
  if (0 == failed) {
    CShotWriter::settle(widget);
    ctx.begin(id, rect);
    if (!ctx.shot(widget, widget->isWindow() ? renderSize : QSize())) {
      failed++;
    }
  }
  return failed + restoreSet(restore, id);
}

/** @return the failures of @p shot's picture, inside the exec() of its context menu when it has an `open` step */
qint32 takeOpened(const QJsonObject& shot, CShotContext& ctx, QWidget* main) {
  if (!shot.contains("open")) {
    return takePicture(shot, ctx, main, true);
  }
  // Closes only the menu: what the scenario opened stays up for the next shot.
  return CShotReplay::perform(main, {shot["open"].toObject()}, [&]() { return takePicture(shot, ctx, main, true); });
}

/**
   @brief Take @p shots, all of @p scenario, in one replay of it: a scenario performed on top of itself is another
   state.

   @return the failures
 */
qint32 shootScenario(const QList<QJsonObject>& shots, CShotContext& ctx, const QJsonObject& scenarios,
                     const QString& scenario) {
  qint32 failures = 0;
  QList<QJsonObject> together;
  QList<QJsonObject> apart;
  for (const QJsonObject& shot : shots) {
    if (const qint32 refused = refusal(shot, scenarios); 0 != refused) {
      failures += refused;
    } else if (together.isEmpty() || shot["size"] == together.first()["size"]) {
      together << shot;
    } else {
      // The window is sized before the steps, so another size needs a replay of its own.
      apart << shot;
    }
  }
  for (const QJsonObject& shot : std::as_const(apart)) {
    failures += CShotPage::shootOne(shot, ctx, scenarios);
  }
  if (together.isEmpty()) {
    return failures;
  }

  QWidget* main = CMainWindow::isNull() ? nullptr : &CMainWindow::self();
  if (!sizeForSteps(together.first(), main)) {
    return failures + together.size();
  }
  const QJsonArray& steps = scenarios[scenario].toArray();
  failures += CShotReplay::replay(steps, ctx, [&]() {
    qint32 failed = 0;
    for (const QJsonObject& shot : std::as_const(together)) {
      // Each picture may wait as long as one alone.
      CShotReplay::keepAlive();
      failed += takeOpened(shot, ctx, main);
    }
    return failed;
  });
  CShotReplay::clear(steps, ctx);
  return failures;
}
}  // namespace

qint32 CShotPage::shootOne(const QJsonObject& shot, CShotContext& ctx, const QJsonObject& scenarios) {
  if (const qint32 failures = refusal(shot, scenarios); 0 != failures) {
    return failures;
  }
  const bool inScenario = shot.contains("scenario");
  const QString& scenario = shot["scenario"].toString();
  // Steps run for the picture, measured against the main window's size like a scenario's.
  const bool stepped = inScenario || shot.contains("open");

  QWidget* main = CMainWindow::isNull() ? nullptr : &CMainWindow::self();
  if (stepped && !sizeForSteps(shot, main)) {
    return 1;
  }

  // One path with or without a scenario: no steps take the picture at once. A live scenario is on screen already.
  const bool perform = inScenario && ctx.liveScenario() != scenario;
  QJsonArray steps = perform ? scenarios[scenario].toArray() : QJsonArray();
  // A context menu is taken inside the exec() its step runs.
  if (shot.contains("open")) {
    steps.append(shot["open"]);
  }
  // Taken as the scenario's last step: a dialog a step opened is gone once replay() returns.
  qint32 failures = CShotReplay::replay(steps, ctx, [&]() { return takePicture(shot, ctx, main, stepped); });
  // The next shot of this run must not start from what the scenario left.
  if (!steps.isEmpty()) {
    CShotReplay::clear(steps, ctx);
  }
  return failures;
}

qint32 CShotPage::run(const QString& file, CShotContext& ctx, const QString& only, const QString& scenario) {
  QFile in(file);
  if (!in.open(QIODevice::ReadOnly)) {
    qWarning() << "shoot: cannot read" << file;
    return 1;
  }
  QJsonParseError error;
  const QJsonDocument& document = QJsonDocument::fromJson(in.readAll(), &error);
  if (QJsonParseError::NoError != error.error || !document.object()["shots"].isArray()) {
    qWarning() << "shoot:" << file << "is not a shot file:" << error.errorString() << "at offset" << error.offset;
    return 1;
  }

  // Not a path glob: `*` crosses the `/` between page and name.
  const QRegularExpression pattern(QRegularExpression::wildcardToRegularExpression(
      only.isEmpty() ? QString("*") : only, QRegularExpression::NonPathWildcardConversion));

  qint32 failures = 0;
  const QStringList& duplicates = CShotRegistry::self().duplicates();
  for (const QString& name : duplicates) {
    qWarning() << "shoot: the exposure" << name << "is registered more than once";
    failures++;
  }

  if (document.object().contains("scenarios") && !document.object()["scenarios"].isObject()) {
    qWarning() << "shoot:" << file << "has scenarios that are not an object";
    return failures + 1;
  }
  const QJsonObject& scenarios = document.object()["scenarios"].toObject();

  // A scenario starts from the process's own configuration; a second one in the same process starts from the first.
  const QJsonArray& shots = document.object()["shots"].toArray();
  if (scenario.isEmpty()) {
    for (const QJsonValue& value : shots) {
      const QJsonObject& shot = value.toObject();
      if (shot.contains("scenario") && pattern.match(shot["id"].toString()).hasMatch()) {
        qWarning() << "shoot:" << file << "has shots in scenarios: take them with --shoot-scenario, one per process";
        return failures + 1;
      }
    }
  }

  qint32 taken = 0;
  QSet<QString> ids;
  QList<QJsonObject> toTake;
  for (const QJsonValue& value : shots) {
    const QJsonObject& shot = value.toObject();
    const QString& id = shot["id"].toString();

    // Both would write the same file.
    if (!id.isEmpty() && ids.contains(id)) {
      qWarning() << "shoot:" << id << "is in" << file << "more than once";
      failures++;
      continue;
    }
    ids << id;

    if (!pattern.match(id).hasMatch()) {
      continue;
    }
    if (!scenario.isEmpty()) {
      const QString& own = shot["scenario"].toString();
      if ((kBaseScenario == scenario) ? !own.isEmpty() : (own != scenario)) {
        continue;
      }
    }

    taken++;
    toTake << shot;
  }

  // In a process of its own, a scenario is replayed once for all its shots.
  const bool together = !scenario.isEmpty() && kBaseScenario != scenario && ctx.liveScenario() != scenario;
  if (together) {
    failures += shootScenario(toTake, ctx, scenarios, scenario);
  } else {
    for (const QJsonObject& shot : std::as_const(toTake)) {
      failures += shootOne(shot, ctx, scenarios);
    }
  }

  if (0 == taken) {
    qWarning() << "shoot: no shot in" << file << "matches" << only << scenario;
    failures++;
  }
  qDebug() << "shoot:" << taken << "shots from" << file << "," << failures << "failures";
  return failures;
}
