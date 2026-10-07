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

#ifndef CSHOTSYNTH_H
#define CSHOTSYNTH_H

#include <QPoint>
#include <QString>
#include <QtTest/QTest>

class QWidget;

/**
   @brief Input through the widget's window and QWindowSystemInterface, as a user's own.

   QTest's `QWidget` overloads hand the event to the widget directly and skip hit testing, grabs, popups, double-click
   synthesis, focus and modifier state, so replay and CShotSelfTest use only these. The widget must be shown; positions
   are in its own coordinates.
 */
namespace CShotSynth {
/**
   @brief What input at @p pos of @p widget would reach: another widget when it is covered, scrolled away or clipped.

   @return empty when it reaches @p widget or a widget inside it
 */
QString missed(QWidget* widget, const QPoint& pos);

/**
   @brief A press, a release, a click or a double click at @p pos of @p widget.

   @param delay  ms since the previous input; -1 is QTest's own, which keeps two clicks from making a double click
 */
void mouse(QTest::MouseAction action, QWidget* widget, Qt::MouseButton button, Qt::KeyboardModifiers mods,
           const QPoint& pos, int delay = -1);

/** @brief The pointer to @p pos of @p widget, with whatever buttons QTest holds down and @p mods held */
void move(QWidget* widget, const QPoint& pos, Qt::KeyboardModifiers mods = Qt::NoModifier);

/**
   @brief The pointer onto @p pos of @p widget in several moves from beside it.

   A mouse context menu goes to the widget the pointer last entered (qt_last_mouse_receiver), a move to the current
   position is dropped, and a menu closes on a press unless the pointer made more than 6 moves over it
   (QMenuPrivate::hasMouseMoved()).
 */
void arrive(QWidget* widget, const QPoint& pos);

/** @brief The wheel turned by @p angle at @p pos of @p widget, with @p mods held */
void wheel(QWidget* widget, const QPoint& pos, const QPoint& angle, Qt::KeyboardModifiers mods);

/** @brief Give @p widget the keyboard: its window active, and the focus in it */
void focus(QWidget* widget);

/**
   @brief @p key pressed and released in @p widget, which gets the focus first.

   @param text  what the key types; empty for what QTest makes of the key, which is lower case
 */
void key(QWidget* widget, Qt::Key key, Qt::KeyboardModifiers mods = Qt::NoModifier, const QString& text = QString());

/** @brief @p text typed into @p widget one character at a time, as a keyboard would */
void type(QWidget* widget, const QString& text);
}  // namespace CShotSynth

#endif  // CSHOTSYNTH_H
