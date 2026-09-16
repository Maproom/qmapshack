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

#include "shoot/CShotWarnings.h"

#include <QMutex>
#include <QMutexLocker>
#include <algorithm>
#include <utility>

namespace {
const QString kShootTag = QStringLiteral("shoot:");

QMutex& mutex() {
  static QMutex m;
  return m;
}
}  // namespace

CShotWarnings::CShotWarnings() {
  const QMutexLocker lock(&mutex());
  outer = std::exchange(current, this);
  previous = qInstallMessageHandler(collect);
}

CShotWarnings::~CShotWarnings() {
  const QMutexLocker lock(&mutex());
  qInstallMessageHandler(previous);
  current = outer;
}

void CShotWarnings::collect(QtMsgType type, const QMessageLogContext& context, const QString& message) {
  QtMessageHandler next = nullptr;
  {
    const QMutexLocker lock(&mutex());
    // Every instance hears it; only the oldest one's handler is not this function.
    for (CShotWarnings* warnings = current; nullptr != warnings; warnings = warnings->outer) {
      if (QtWarningMsg == type) {
        warnings->given << message;
      }
      next = warnings->previous;
    }
  }
  if (nullptr != next) {
    next(type, context, message);
  }
}

bool CShotWarnings::contains(const QString& text) const {
  const QMutexLocker lock(&mutex());
  return std::any_of(given.begin(), given.end(), [&text](const QString& message) { return message.contains(text); });
}

qsizetype CShotWarnings::count() const {
  const QMutexLocker lock(&mutex());
  return given.size();
}

QString CShotWarnings::first() const {
  const QMutexLocker lock(&mutex());
  for (const QString& message : given) {
    if (const qsizetype at = message.indexOf(kShootTag); at >= 0) {
      const QString& text = message.mid(at + kShootTag.size()).trimmed();
      return text.endsWith('.') ? text : text + '.';
    }
  }
  return QString();
}
