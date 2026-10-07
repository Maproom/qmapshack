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

#include "shoot/CShotRecorder.h"

#include <QAction>
#include <QActionEvent>
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QToolButton>
#include <QWidget>
#include <algorithm>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotApplication.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotHandlers.h"
#include "shoot/CShotPage.h"
#include "shoot/IShotHandler.h"

namespace {
QString classOf(const QObject* object) { return QString::fromLatin1(object->metaObject()->className()); }

/** @return true when no widget between @p receiver and its ancestor @p widget has a handler of its own */
bool passedUpUnhandled(const QObject* receiver, const QWidget* widget) {
  const QWidget* w = qobject_cast<const QWidget*>(receiver);
  for (; nullptr != w && w != widget; w = w->parentWidget()) {
    if (nullptr != CShotHandlers::of(w)) {
      return false;
    }
  }
  return w == widget;
}
}  // namespace

CShotRecorder::CShotRecorder(const CShotContext& ctx, QObject* parent) : QObject(parent), ctx(ctx) {}

CShotRecorder::~CShotRecorder() {
  if (recording) {
    qApp->removeEventFilter(this);
    // The listener calls back into this recorder.
    CShotApplication::setListener({});
  }
}

void CShotRecorder::start() {
  entries.clear();
  gestures.clear();
  reported.clear();
  touched.clear();
  keyPresses.clear();
  pendingKey.reset();
  menuFrame = 0;

  // Never at stop(): a change made while recording is a step and would be applied twice.
  startLayout = QJsonObject();
  startView = QJsonObject();
  if (const CMainWindow* main = ctx.mainWindow(); nullptr != main) {
    startLayout = CShotPage::layoutOf(*main);
  }
  if (const CCanvas* canvas = ctx.canvas(); nullptr != canvas) {
    startView = CShotPage::viewOf(canvas);
  } else {
    qWarning() << "shoot: there is no map in front, so the recording has no view";
  }

  recording = true;
  qApp->installEventFilter(this);
  CShotApplication::setListener([this](const SShotFrame& frame, bool opened) { onFrame(frame, opened); });
  watchAll();
}

QJsonArray CShotRecorder::stop() {
  if (!recording) {
    return QJsonArray();
  }
  qApp->removeEventFilter(this);
  CShotApplication::setListener({});
  commitPendingKey();
  keyPresses.clear();
  recording = false;
  closeAll();
  if (!gestures.isEmpty()) {
    // A press replayed alone leaves the button down: only a gesture whose button is up is a step, its release having
    // gone to a menu the press opened.
    for (auto it = gestures.constBegin(); it != gestures.constEnd(); ++it) {
      const QString& name = it->step["button"].toString();
      const Qt::MouseButton button =
          ("right" == name) ? Qt::RightButton : (("middle" == name) ? Qt::MiddleButton : Qt::LeftButton);
      if (QGuiApplication::mouseButtons() & button) {
        qWarning() << "shoot: a button was still held when the recording stopped; what it was doing is not recorded";
        const QObject* surface = it.key();
        const quint64 pressed = it->frame;
        entries.removeIf([surface, pressed](const entry_t& entry) {
          return entry.source == surface && entry.frame >= pressed && ("press" == entry.verb || "move" == entry.verb);
        });
      } else if (it->split) {
        entries.append(entry_t{CShotApplication::lastNumber(), it.key(), "release", releaseOf(*it), false});
      } else {
        entries.append(entry_t{it->frame, it.key(), it->step["do"].toString(), it->step, false});
      }
    }
    gestures.clear();
  }

  // By frame, not recording order: a modal dialog's steps are recorded before the step whose slot opened it.
  std::stable_sort(entries.begin(), entries.end(),
                   [](const entry_t& one, const entry_t& other) { return one.frame < other.frame; });

  QJsonArray actions;
  if (!startLayout.isEmpty()) {
    actions.append(startLayout);
  }
  if (!startView.isEmpty()) {
    actions.append(startView);
  }
  for (const entry_t& entry : std::as_const(entries)) {
    actions.append(entry.step);
  }
  return actions;
}

bool CShotRecorder::inputReached(const QObject* source) const {
  const SShotFrame& frame = CShotApplication::frame();
  if (0 == frame.number || nullptr == source) {
    return false;
  }
  if (frame.receiver == source || CShotApplication::frameWithin(source)) {
    return true;
  }
  // A shortcut is delivered to the action itself; a menu entry, a tool button and a toolbar to a widget it sits in.
  if (const QAction* action = qobject_cast<const QAction*>(source); nullptr != action) {
    const QList<QObject*>& owners = action->associatedObjects();
    return std::any_of(owners.begin(), owners.end(),
                       [](const QObject* owner) { return CShotApplication::frameWithin(owner); });
  }
  return false;
}

void CShotRecorder::record(QObject* source, const QJsonObject& step) {
  if (!recording || step.isEmpty()) {
    return;
  }
  const QString& verb = step["do"].toString();
  if (!inputReached(source)) {
    qDebug().noquote() << "shoot:" << classOf(source) << "says" << verb
                       << "while the input went elsewhere - the application's answer, not a step";
    return;
  }

  const quint64 frame = CShotApplication::frame().number;
  splitGestures();
  touch();
  closeAll();
  for (entry_t& entry : entries) {
    if (entry.frame == frame && entry.source == source && entry.verb == verb) {
      entry.step = step;
      return;
    }
  }
  entries.append(entry_t{frame, source, verb, step, false});
}

void CShotRecorder::recordOutcome(QObject* source, const QJsonObject& step) {
  if (!recording || step.isEmpty() || 0 == CShotApplication::frame().number) {
    return;
  }
  splitGestures();
  touch();
  closeAll();
  entries.append(entry_t{CShotApplication::frame().number, source, step["do"].toString(), step, false});
}

void CShotRecorder::amend(QObject* source, const QJsonObject& step, const QObject* alsoThrough) {
  if (!recording || step.isEmpty() ||
      !(inputReached(source) || (nullptr != alsoThrough && CShotApplication::frameWithin(alsoThrough)))) {
    return;
  }

  const QString& verb = step["do"].toString();
  splitGestures();
  touch();
  for (entry_t& entry : entries) {
    if (entry.open && entry.source == source && entry.verb == verb) {
      entry.step = step;
      return;
    }
  }
  closeAll();
  entries.append(entry_t{CShotApplication::frame().number, source, verb, step, true});
}

QJsonObject* CShotRecorder::openStep(QObject* source, const QString& verb) {
  for (entry_t& entry : entries) {
    if (entry.open && entry.source == source && entry.verb == verb) {
      touch();
      return &entry.step;
    }
  }
  return nullptr;
}

void CShotRecorder::close(QObject* source) {
  for (entry_t& entry : entries) {
    if (entry.source == source) {
      entry.open = false;
    }
  }
}

void CShotRecorder::closeAll() {
  for (entry_t& entry : entries) {
    entry.open = false;
  }
}

bool CShotRecorder::retractLast(QObject* source,
                                const std::function<bool(const QJsonObject& step, bool sameFrame)>& matches,
                                const std::function<bool(const QJsonObject& step)>& passes) {
  const quint64 frame = CShotApplication::frame().number;
  for (qsizetype i = entries.size() - 1; i >= 0; --i) {
    if (entries[i].source != source) {
      continue;
    }
    if (matches(entries[i].step, 0 != frame && entries[i].frame == frame)) {
      entries.removeAt(i);
      touch();
      return true;
    }
    if (!passes || !passes(entries[i].step)) {
      return false;
    }
  }
  return false;
}

qint64 CShotRecorder::now() {
  static QElapsedTimer clock;
  if (!clock.isValid()) {
    clock.start();
  }
  return clock.elapsed();
}

void CShotRecorder::beginGesture(QObject* source, const QJsonObject& step, const QPoint& pixel) {
  if (!recording || step.isEmpty() || !inputReached(source)) {
    return;
  }
  touch();
  closeAll();
  gestures.insert(source, gesture_t{CShotApplication::frame().number, step, pixel, now()});
}

CShotRecorder::gesture_t* CShotRecorder::gesture(QObject* source) {
  const auto it = gestures.find(source);
  return (it == gestures.end()) ? nullptr : &it.value();
}

void CShotRecorder::endGesture(QObject* source) {
  const auto it = gestures.find(source);
  if (it == gestures.end()) {
    return;
  }
  const gesture_t done = it.value();
  gestures.erase(it);
  touch();
  closeAll();
  if (done.split) {
    const quint64 frame = CShotApplication::frame().number;
    entries.append(
        entry_t{(0 != frame) ? frame : CShotApplication::lastNumber(), source, "release", releaseOf(done), false});
    return;
  }
  entries.append(entry_t{done.frame, source, done.step["do"].toString(), done.step, false});
}

void CShotRecorder::splitGestures() {
  for (auto it = gestures.begin(); it != gestures.end(); ++it) {
    gesture_t& held = it.value();
    // Not during its own press, whose effects come first; a double click stays one step.
    if (held.split || CShotApplication::isOpen(held.frame) || "dclick" == held.step["do"].toString()) {
      continue;
    }
    held.split = true;
    QJsonObject press = held.step;
    press["do"] = "press";
    press.remove("dx");
    press.remove("dy");
    press.remove("held");
    entries.append(entry_t{held.frame, it.key(), "press", press, false});
    if (!held.moved.isEmpty()) {
      entries.append(entry_t{held.frame, it.key(), "move", held.moved, false});
    }
  }
}

QJsonObject CShotRecorder::releaseOf(const gesture_t& gesture) {
  QJsonObject release{{"do", "release"},
                      {"widget", gesture.step["widget"]},
                      {"button", gesture.step["button"]},
                      {"dx", gesture.step["dx"].toInt()},
                      {"dy", gesture.step["dy"].toInt()}};
  if (gesture.step.contains("mods")) {
    release["mods"] = gesture.step["mods"];
  }
  return release;
}

std::optional<QString> CShotRecorder::address(const QWidget* widget) const {
  const std::optional<QString>& address = CShotAddress::addressOf(ctx.mainWindow(), widget);
  if (!address.has_value()) {
    const QString& className = classOf(widget);
    if (!reported.contains(className)) {
      reported.insert(className);
      qWarning() << "shoot: a" << className << "cannot be named, so what is done to it is not recorded";
    }
    return std::nullopt;
  }
  return address;
}

bool CShotRecorder::eventFilter(QObject* watchedObject, QEvent* event) {
  if (!recording) {
    return QObject::eventFilter(watchedObject, event);
  }

  switch (event->type()) {
    case QEvent::Polish:
      watch(watchedObject);
      break;
    case QEvent::ActionAdded:
      watch(static_cast<QActionEvent*>(event)->action());
      break;
    case QEvent::Show:
      // A context menu request is a step only once a menu has answered it; one that opens nothing is none.
      if (QMenu* menu = qobject_cast<QMenu*>(watchedObject); nullptr != menu) {
        recordContextMenu();
        recordMenuOpened(menu);
      }
      break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::Wheel:
    case QEvent::KeyPress: {
      if (!event->spontaneous() || !watchedObject->isWidgetType()) {
        break;
      }
      // What Qt makes of touch input a surface did not take: a pinch would be recorded as a drag.
      if (const QMouseEvent* mouse = dynamic_cast<const QMouseEvent*>(event);
          nullptr != mouse && Qt::MouseEventNotSynthesized != mouse->source()) {
        break;
      }
      // An ignored input propagates to parents; a surface records it only if nothing below has a handler.
      QWidget* widget = static_cast<QWidget*>(watchedObject);
      const QObject* receiver = CShotApplication::frame().receiver;
      const IShotHandler* handler = CShotHandlers::of(widget);
      if (nullptr != handler) {
        if (receiver == widget || passedUpUnhandled(receiver, widget)) {
          handler->input(widget, event, *this);
        }
      } else if (QEvent::MouseButtonPress == event->type() && receiver == widget) {
        reportUnhandled(widget);
      }
      break;
    }
    case QEvent::TouchBegin:
    case QEvent::NativeGesture:
      // A pinch on the map arrives as either; no step holds it.
      if (event->spontaneous() && !reported.contains("touch")) {
        reported.insert("touch");
        qWarning() << "shoot: touch and touchpad gestures are not recorded";
      }
      break;
    default:
      break;
  }
  return QObject::eventFilter(watchedObject, event);
}

void CShotRecorder::watch(QObject* object) {
  if (nullptr == object || watched.contains(object)) {
    return;
  }
  const IShotHandler* handler = CShotHandlers::of(object);
  if (nullptr == handler) {
    return;
  }

  watched.insert(object);
  connect(object, &QObject::destroyed, this, [this](QObject* gone) {
    // A freed address is reused: the next object must not inherit this one's watch or open step.
    watched.remove(gone);
    gestures.remove(gone);
    for (entry_t& entry : entries) {
      if (entry.source == gone) {
        entry.open = false;
        entry.source = nullptr;
      }
    }
  });
  handler->watch(object, *this);
}

void CShotRecorder::watchAll() {
  const QList<QWidget*>& windows = QApplication::topLevelWidgets();
  for (QWidget* window : windows) {
    watch(window);
    const QList<QObject*>& children = window->findChildren<QObject*>();
    for (QObject* child : children) {
      watch(child);
    }
    // An action needs no parent inside the window to be in it.
    QList<QWidget*> widgets = window->findChildren<QWidget*>();
    widgets << window;
    for (const QWidget* widget : std::as_const(widgets)) {
      const QList<QAction*>& actions = widget->actions();
      for (QAction* action : actions) {
        watch(action);
      }
    }
  }
}

QJsonObject CShotRecorder::contextMenuStep(QWidget** owner) const {
  const SShotFrame& frame = CShotApplication::frame();
  QWidget* receiver = qobject_cast<QWidget*>(frame.receiver);
  // A menu bar's menu shows inside a press, and its pick is the entry's own `trigger`.
  if (QEvent::ContextMenu != frame.type || nullptr == receiver) {
    return QJsonObject();
  }

  // The request goes to a viewport; the address and row belong to the owning view. Without a handler, a position.
  QWidget* opener = CShotHandlers::ownerOf(receiver);
  if (nullptr != owner) {
    *owner = opener;
  }
  const QPoint& pos = opener->mapFromGlobal(receiver->mapToGlobal(frame.pos));
  const IShotHandler* handler = CShotHandlers::of(opener);
  return (nullptr == handler) ? IShotHandler::menuAt(opener, pos, *this) : handler->contextMenu(opener, pos, *this);
}

void CShotRecorder::recordContextMenu() {
  const quint64 number = CShotApplication::frame().number;
  if (menuFrame == number) {
    return;
  }
  QWidget* owner = nullptr;
  const QJsonObject& step = contextMenuStep(&owner);
  if (nullptr == owner) {
    return;
  }
  menuFrame = number;
  record(owner, step);
}

void CShotRecorder::recordMenuOpened(QMenu* menu) {
  const SShotFrame& frame = CShotApplication::frame();
  if (QEvent::ContextMenu == frame.type) {
    return;
  }
  // A menu the application exec()s from a recorded click has no opener; replaying the click opens it again.
  QList<QWidget*> openers;
  const QList<QWidget*>& widgets = QApplication::allWidgets();
  for (QWidget* widget : widgets) {
    if (widget == menu || !widget->isVisible()) {
      continue;
    }
    const QToolButton* tool = qobject_cast<const QToolButton*>(widget);
    const QMenuBar* bar = qobject_cast<const QMenuBar*>(widget);
    const QMenu* parent = qobject_cast<const QMenu*>(widget);
    const QAction* active =
        (nullptr != bar) ? bar->activeAction() : ((nullptr != parent) ? parent->activeAction() : nullptr);
    if ((nullptr != tool &&
         (tool->menu() == menu || (nullptr != tool->defaultAction() && tool->defaultAction()->menu() == menu))) ||
        (nullptr != active && active->menu() == menu)) {
      openers << widget;
    }
  }
  // A tool button that pops its menu up after a delay, and a submenu after hovering, open it outside any input.
  if (0 != frame.number && openers.size() > 1) {
    openers.removeIf([](const QWidget* opener) { return !CShotApplication::frameWithin(opener); });
  }
  if (openers.isEmpty()) {
    return;
  }
  if (openers.size() > 1) {
    qWarning() << "shoot: a menu more than one widget opens is not recorded";
    return;
  }

  QWidget* opener = openers.first();
  QJsonObject step{{"do", "openmenu"}};
  if (const QToolButton* tool = qobject_cast<const QToolButton*>(opener); nullptr != tool) {
    const QAction* action = tool->defaultAction();
    const QWidget* main = ctx.mainWindow();
    if (nullptr != action && !action->objectName().isEmpty() && nullptr != main &&
        main->findChild<QAction*>(action->objectName()) == action) {
      step["action"] = action->objectName();
    } else if (const std::optional<QString>& address = this->address(opener); address.has_value()) {
      step["widget"] = *address;
    } else {
      return;
    }
  } else {
    // A menu's entry has a translated text; the menu's own name is the address.
    if (menu->objectName().isEmpty()) {
      qWarning() << "shoot: the menu" << menu->title() << "has no objectName, so opening it is not recorded";
      return;
    }
    step["menu"] = menu->objectName();
    if (nullptr != qobject_cast<const QMenuBar*>(opener)) {
      const std::optional<QString>& address = this->address(opener);
      if (!address.has_value()) {
        return;
      }
      step["widget"] = *address;
    }
  }
  splitGestures();
  touch();
  closeAll();
  const quint64 number = (0 != frame.number) ? frame.number : CShotApplication::lastNumber();
  entries.append(entry_t{number, menu, "openmenu", step, false});
}

void CShotRecorder::touch() {
  touched.insert(CShotApplication::frame().number);
  // A context menu or a shortcut Qt delivers while handling a key press is that key press's doing.
  touched.insert(CShotApplication::inputFrame().number);
}

void CShotRecorder::commitPendingKey() {
  if (pendingKey.has_value()) {
    splitGestures();
    closeAll();
    entries.append(entry_t{pendingKey->frame, nullptr, "keypress", pendingKey->step, false});
    pendingKey.reset();
  }
}

void CShotRecorder::onFrame(const SShotFrame& frame, bool opened) {
  if (opened) {
    const bool releasesPending =
        QEvent::KeyRelease == frame.type && pendingKey.has_value() && pendingKey->key == frame.key;
    // The pointer moving while the key is held decides nothing about it.
    if (QEvent::Shortcut != frame.type && QEvent::MouseMove != frame.type && !releasesPending) {
      commitPendingKey();
    }
    // Read now: the key can close the window its widget is in.
    QWidget* receiver = qobject_cast<QWidget*>(frame.receiver.data());
    // The platform sends the menu key as a context menu request before the key itself, and that is the `menu` step.
    if (QEvent::KeyPress != frame.type || nullptr == receiver || Qt::Key_Menu == frame.key) {
      return;
    }
    // A completer's popup has no parent and takes the keys while the input it completes keeps the focus.
    if (nullptr == receiver->parentWidget() && Qt::Popup == receiver->windowType() &&
        nullptr == qobject_cast<QMenu*>(receiver) && nullptr != QApplication::focusWidget()) {
      receiver = QApplication::focusWidget();
    }
    const QString& name = CShotHandlers::keyName(frame.key);
    if (name.isEmpty()) {
      return;
    }
    const std::optional<QString>& address = this->address(receiver);
    if (!address.has_value()) {
      return;
    }
    QJsonObject step{{"do", "keypress"}, {"widget", *address}, {"key", name}};
    if (!frame.text.isEmpty() && frame.text.at(0).isPrint()) {
      step["text"] = frame.text;
    }
    CShotHandlers::putMods(step, frame.mods);
    keyPresses.insert(frame.number, step);
    return;
  }

  if (QEvent::Shortcut == frame.type) {
    // The key press before it is this shortcut's, when the shortcut made a step; otherwise it is a step itself.
    if (touched.contains(frame.number)) {
      pendingKey.reset();
    } else {
      commitPendingKey();
    }
    return;
  }
  if (QEvent::KeyRelease == frame.type && pendingKey.has_value() && pendingKey->key == frame.key) {
    // A button clicks at the release of Space: then the release made the step.
    if (touched.contains(frame.number)) {
      pendingKey.reset();
    } else {
      commitPendingKey();
    }
    return;
  }
  const auto it = keyPresses.find(frame.number);
  if (it == keyPresses.end()) {
    return;
  }
  const QJsonObject step = it.value();
  keyPresses.erase(it);
  // A key pressed while a button is down comes between the press and the release, whether or not it is a step itself.
  splitGestures();
  if (!touched.contains(frame.number)) {
    // A key no handler made a step of, held back for a Shortcut or release that may make one.
    commitPendingKey();
    pendingKey = pendingKey_t{frame.number, frame.key, step};
  }
}

void CShotRecorder::reportUnhandled(const QWidget* widget) {
  // A widget inside one that is recorded - a view's viewport, a splitter's handle - is that one's business.
  for (const QWidget* w = widget; nullptr != w; w = w->parentWidget()) {
    if (nullptr != CShotHandlers::of(w)) {
      return;
    }
  }
  const QString& className = classOf(widget);
  if (!reported.contains(className)) {
    reported.insert(className);
    qWarning() << "shoot: a click on" << className << "is not recorded: no handler covers that class";
  }
}
