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

#ifndef CSHOTPRIVACY_H
#define CSHOTPRIVACY_H

#include <QList>
#include <QRegularExpression>
#include <QString>
#include <functional>

class QTextDocument;
class QWidget;

/**
   @brief Keeps what is private to the writer out of a picture while it lives.

   Paths in the text of the standard widgets below the given ones read as `/home/user/...`; the texts are put back when
   it ends. Hold it only around the render call: nothing may run the event loop while it lives, since it keeps
   pointers to items and edits a document in place.
 */
class CShotPrivacy {
 public:
  explicit CShotPrivacy(const QList<QWidget*>& roots);
  ~CShotPrivacy();

  CShotPrivacy(const CShotPrivacy&) = delete;
  CShotPrivacy& operator=(const CShotPrivacy&) = delete;

  /** @return @p text with every path private to the writer as `/home/user/...` */
  static QString neutral(const QString& text);

 private:
  struct rule_t {
    QRegularExpression pattern;
    QString replacement;
  };

  static QList<rule_t> rules();
  static QString apply(const QList<rule_t>& list, const QString& text);
  void rewriteDocument(QWidget* edit, QTextDocument* doc, const QList<rule_t>& list);

  QList<std::function<void()>> undo;
};

#endif  // CSHOTPRIVACY_H
