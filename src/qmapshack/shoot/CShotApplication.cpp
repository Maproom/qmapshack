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

#include "shoot/CShotApplication.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QThread>
#include <QWidget>
#include <algorithm>
#include <vector>

namespace {
std::vector<SShotFrame> frames;
quint64 counter = 0;
bool trace = false;
std::function<void(const SShotFrame&, bool)> listener;

/**
   @brief Is @p type, delivered @p spontaneous, one user interaction?

   Shortcut and context menu events are Qt's own and open a frame regardless. A close opens none: Qt 6 closes through
   the platform, so the application's close and the title bar's are the same event.
 */
bool opensFrame(QEvent::Type type, bool spontaneous) {
  switch (type) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::Wheel:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      return spontaneous;
    case QEvent::Shortcut:
    case QEvent::ContextMenu:
      return true;
    default:
      return false;
  }
}

SShotFrame frameOf(QObject* receiver, const QEvent* event) {
  SShotFrame frame;
  frame.number = ++counter;
  frame.depth = qint32(frames.size());
  frame.receiver = receiver;
  frame.type = event->type();
  if (const QInputEvent* input = dynamic_cast<const QInputEvent*>(event); nullptr != input) {
    frame.mods = input->modifiers();
  }
  if (const QMouseEvent* mouse = dynamic_cast<const QMouseEvent*>(event); nullptr != mouse) {
    frame.button = mouse->button();
    frame.pos = mouse->position().toPoint();
  } else if (const QContextMenuEvent* menu = dynamic_cast<const QContextMenuEvent*>(event); nullptr != menu) {
    frame.pos = menu->pos();
  } else if (const QKeyEvent* pressed = dynamic_cast<const QKeyEvent*>(event); nullptr != pressed) {
    frame.key = pressed->key();
    frame.text = pressed->text();
  }
  return frame;
}

bool isInput(QEvent::Type type) {
  switch (type) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::Wheel:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      return true;
    default:
      return false;
  }
}

QString nameOf(const QObject* object) {
  if (nullptr == object) {
    return "(none)";
  }
  return QString("%1(%2)").arg(QString::fromLatin1(object->metaObject()->className()), object->objectName());
}
}  // namespace

CShotApplication::CShotApplication(int& argc, char** argv) : QApplication(argc, argv) {}

SShotFrame CShotApplication::frame() { return frames.empty() ? SShotFrame() : frames.back(); }

SShotFrame CShotApplication::inputFrame() {
  for (auto it = frames.rbegin(); it != frames.rend(); ++it) {
    if (isInput(it->type)) {
      return *it;
    }
  }
  return SShotFrame();
}

quint64 CShotApplication::lastNumber() { return counter; }

bool CShotApplication::isOpen(quint64 number) {
  return std::any_of(frames.begin(), frames.end(),
                     [number](const SShotFrame& frame) { return frame.number == number; });
}

bool CShotApplication::hasListener() { return bool(listener); }

void CShotApplication::setListener(const std::function<void(const SShotFrame&, bool)>& fn) { listener = fn; }

bool CShotApplication::frameWithin(const QObject* object) {
  if (frames.empty() || nullptr == object || frames.back().receiver.isNull()) {
    return false;
  }
  const QObject* receiver = frames.back().receiver.data();
  // Widgets walk parentWidget(), so a viewport, a popup or a dialog reaches the widget it belongs to.
  if (const QWidget* widget = qobject_cast<const QWidget*>(receiver); nullptr != widget) {
    for (const QWidget* w = widget; nullptr != w; w = w->parentWidget()) {
      if (w == object) {
        return true;
      }
    }
    return false;
  }
  for (const QObject* o = receiver; nullptr != o; o = o->parent()) {
    if (o == object) {
      return true;
    }
  }
  return false;
}

void CShotApplication::setTrace(bool on) { trace = on; }

bool CShotApplication::notify(QObject* receiver, QEvent* event) {
  if (!opensFrame(event->type(), event->spontaneous()) || QThread::currentThread() != thread()) {
    return QApplication::notify(receiver, event);
  }

  frames.push_back(frameOf(receiver, event));
  // Copied: nested frames can reallocate the vector.
  const SShotFrame opened = frames.back();
  if (trace) {
    qDebug().noquote() << "shoot: frame" << opened.number << "open" << event->type() << "to" << nameOf(receiver);
  }
  if (listener) {
    listener(opened, true);
  }

  const bool answered = QApplication::notify(receiver, event);
  frames.pop_back();

  if (trace) {
    qDebug().noquote() << "shoot: frame" << opened.number << "closed";
  }
  if (listener) {
    listener(opened, false);
  }
  return answered;
}
