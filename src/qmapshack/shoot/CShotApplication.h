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

#ifndef CSHOTAPPLICATION_H
#define CSHOTAPPLICATION_H

#include <QApplication>
#include <QEvent>
#include <QPoint>
#include <QPointer>
#include <QString>
#include <functional>

/**
   @brief One user input being delivered: what `QApplication::notify()` was called with.
 */
struct SShotFrame {
  quint64 number = 0;                          /**< counted from 1; 0 is no frame at all */
  qint32 depth = 0;                            /**< how many frames enclose it; 0 is input from outside */
  QPointer<QObject> receiver;                  /**< what the event was delivered to; null once that is gone */
  QEvent::Type type = QEvent::None;            /**< the event that opened the frame */
  Qt::MouseButton button = Qt::NoButton;       /**< the button of a mouse event, NoButton for anything else */
  QPoint pos;                                  /**< in the receiver's coordinates, for a mouse or context menu event */
  int key = 0;                                 /**< the key of a key event, 0 for anything else */
  QString text;                                /**< what the key types */
  Qt::KeyboardModifiers mods = Qt::NoModifier; /**< the modifiers of an input event */
};

/**
   @brief The application of a documentation run: it knows when a user input is being delivered.

   `notify()` wraps a whole delivery - filters, handler, slots and any event loop they run - so a signal emitted inside
   a frame was caused by the user, one outside it by the application. Frames nest (a modal dialog's input opens frames
   inside the slot's) and their numbers order a recording. Shortcut and context menu events open frames too.
 */
class CShotApplication : public QApplication {
  Q_OBJECT
 public:
  CShotApplication(int& argc, char** argv);

  /** @return the innermost open frame; its `number` is 0 when no user input is being delivered */
  static SShotFrame frame();

  /** @return the innermost frame of a mouse, wheel or key event; a context menu delivered inside one belongs to it */
  static SShotFrame inputFrame();

  /** @return the number of the frame opened last, open or closed; orders a step made outside any frame */
  static quint64 lastNumber();

  /** @return true while the frame numbered @p number is being delivered */
  static bool isOpen(quint64 number);

  /** @brief Call @p listener with every frame opened and closed; one listener, an empty function removes it */
  static void setListener(const std::function<void(const SShotFrame& frame, bool opened)>& listener);

  /** @return true while a listener is set */
  static bool hasListener();

  /** @return true when the open frame's event went to @p object or to something below it */
  static bool frameWithin(const QObject* object);

  /** @brief Print a line per frame opened and closed */
  static void setTrace(bool on);

 protected:
  bool notify(QObject* receiver, QEvent* event) override;
};

#endif  // CSHOTAPPLICATION_H
