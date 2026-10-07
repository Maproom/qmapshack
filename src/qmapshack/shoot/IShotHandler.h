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

#ifndef ISHOTHANDLER_H
#define ISHOTHANDLER_H

#include <QJsonObject>
#include <QtGlobal>

class CShotRecorder;
class QEvent;
class QMetaObject;
class QObject;
class QPoint;
class QWidget;

/**
   @brief Records one class's user signals as steps and replays them.

   A class without a handler uses its nearest base's; an uncovered object records only context menus. A replay checks
   the recorded outcome where the step has one and counts a mismatch as a failure.
 */
class IShotHandler {
 public:
  virtual ~IShotHandler() = default;

  /** @return the class handled */
  virtual const QMetaObject* handles() const = 0;

  /**
     @brief Connect @p object's user signals to @p recorder, once per object.

     The recorder drops steps whose input went elsewhere.
   */
  virtual void watch(QObject* object, CShotRecorder& recorder) const = 0;

  /**
     @brief Perform @p step on @p target, as input or as the signal Qt would emit.

     @return the failures
   */
  virtual qint32 replay(const QJsonObject& step, QObject* target) const = 0;

  /**
     @brief A user input event is about to reach @p target, for a surface recorded as raw input.

     Called before delivery, so what is under the pointer can still be read.
   */
  virtual void input(QWidget* target, const QEvent* event, CShotRecorder& recorder) const;

  /** @return the `menu` step for a context menu at @p pos, empty for none */
  virtual QJsonObject contextMenu(QWidget* target, const QPoint& pos, const CShotRecorder& recorder) const;

  /** @return a `menu` step at @p pos as a fraction of @p target, empty when it cannot be addressed */
  static QJsonObject menuAt(QWidget* target, const QPoint& pos, const CShotRecorder& recorder);
};

#endif  // ISHOTHANDLER_H
