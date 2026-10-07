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

#include "shoot/CShotSynth.h"

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QWindow>

namespace {
/** @return @p widget's window, with @p pos in its coordinates in @p inWindow */
QWindow* windowOf(QWidget* widget, const QPoint& pos, QPoint& inWindow) {
  QWidget* top = widget->window();
  inWindow = widget->mapTo(top, pos);
  return top->windowHandle();
}

}  // namespace

QString CShotSynth::missed(QWidget* widget, const QPoint& pos) {
  QWidget* top = widget->window();
  QWidget* under = top->childAt(widget->mapTo(top, pos));
  if (nullptr == under) {
    under = top;
  }
  for (const QWidget* w = under; nullptr != w; w = w->parentWidget()) {
    if (w == widget) {
      return QString();
    }
  }
  return QString("%1(%2)").arg(QString::fromLatin1(under->metaObject()->className()), under->objectName());
}

void CShotSynth::focus(QWidget* widget) {
  QWidget* top = widget->window();
  if (!top->isActiveWindow()) {
    top->activateWindow();
    if (!QTest::qWaitForWindowActive(top)) {
      qWarning() << "shoot: the window of" << widget->metaObject()->className() << "does not become active";
    }
  }
  widget->setFocus(Qt::OtherFocusReason);
}

void CShotSynth::mouse(QTest::MouseAction action, QWidget* widget, Qt::MouseButton button, Qt::KeyboardModifiers mods,
                       const QPoint& pos, int delay) {
  QPoint inWindow;
  QWindow* window = windowOf(widget, pos, inWindow);
  // A double click needs its two presses close in time; QTest's own delay after a release keeps them apart.
  if (QTest::MouseDClick == action && delay < 0) {
    delay = 10;
  }
  QTest::mouseEvent(action, window, button, mods, inWindow, delay);
}

void CShotSynth::move(QWidget* widget, const QPoint& pos, Qt::KeyboardModifiers mods) {
  QPoint inWindow;
  QWindow* window = windowOf(widget, pos, inWindow);
  QTest::mouseEvent(QTest::MouseMove, window, Qt::NoButton, mods, inWindow);
}

void CShotSynth::arrive(QWidget* widget, const QPoint& pos) {
  // More than 6 distinct moves, so an open menu takes the press.
  const qint32 step = (pos.x() >= 8) ? -1 : 1;
  for (qint32 i = 8; i > 0; --i) {
    move(widget, pos + QPoint(step * i, 0));
  }
  move(widget, pos);
}

void CShotSynth::wheel(QWidget* widget, const QPoint& pos, const QPoint& angle, Qt::KeyboardModifiers mods) {
  QPoint inWindow;
  QWindow* window = windowOf(widget, pos, inWindow);
  // Through the window so QApplication::keyboardModifiers() holds @p mods: IPlot reads that, not the event's.
  QTest::wheelEvent(window, inWindow, angle, QPoint(), mods);
}

void CShotSynth::key(QWidget* widget, Qt::Key key, Qt::KeyboardModifiers mods, const QString& text) {
  focus(widget);
  QWindow* window = widget->window()->windowHandle();
  if (text.isEmpty()) {
    QTest::keyClick(window, key, mods);
  } else {
    QTest::sendKeyEvent(QTest::Click, window, key, text, mods);
  }
}

void CShotSynth::type(QWidget* widget, const QString& text) {
  focus(widget);
  QWindow* window = widget->window()->windowHandle();
  for (const QChar& ch : text) {
    const char latin = ch.toLatin1();
    const Qt::Key code = (0 != latin) ? QTest::asciiToKey(latin) : Qt::Key_unknown;
    QTest::sendKeyEvent(QTest::Click, window, code, QString(ch), Qt::NoModifier);
  }
}
