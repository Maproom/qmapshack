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

#ifndef CSHOTWARNINGS_H
#define CSHOTWARNINGS_H

#include <QString>
#include <QStringList>
#include <QtGlobal>

/** @brief Collects the warnings given during its lifetime, on any thread; instances nest. */
class CShotWarnings {
 public:
  CShotWarnings();
  ~CShotWarnings();
  CShotWarnings(const CShotWarnings&) = delete;
  CShotWarnings& operator=(const CShotWarnings&) = delete;

  /** @return true when a warning so far contains @p text */
  bool contains(const QString& text) const;
  /** @return how many warnings were given so far */
  qsizetype count() const;
  /** @return the first `shoot:` warning so far as a sentence, so text appended to it reads as the next one */
  QString first() const;

 private:
  static void collect(QtMsgType type, const QMessageLogContext& context, const QString& message);

  /** The newest instance; each one keeps the one before it in `outer`. */
  static inline CShotWarnings* current = nullptr;
  CShotWarnings* outer = nullptr;
  QtMessageHandler previous = nullptr;
  QStringList given;
};

#endif  // CSHOTWARNINGS_H
