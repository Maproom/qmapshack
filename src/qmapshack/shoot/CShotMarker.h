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

#ifndef CSHOTMARKER_H
#define CSHOTMARKER_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QPointer>
#include <QStringList>
#include <functional>

#include "shoot/CShotMarks.h"

class QTimer;
class QWidget;

/**
   @brief Placing marks by pointing, in the state process, and showing the selected image's marks on screen.

   Ctrl+Shift+F10 puts the picked marker on what the pointer is on. When that is unclear, the key steps through the
   candidates and a click or Return takes one; an arrow then takes a second click for its tail. Escape cancels. Every
   key and click of that is swallowed, so an open menu stays open and nothing under the pointer is triggered.
 */
class CShotMarker : public QObject {
  Q_OBJECT
 public:
  /**
     @param send  a line to the launcher
     @param report  the launcher's status line
   */
  CShotMarker(std::function<void(const QString&)> send, std::function<void(const QString&)> report, QObject* parent);
  ~CShotMarker() override;

  /** @brief The marker the panel has picked: frame, disc, arrow, pointer, hand or resize; empty for none. */
  void setMarker(const QString& kind);

  /** @brief The selected image's marks, shown on screen and reported with their status. */
  void setMarks(const QJsonArray& marks);

  /** @brief The selected image's shot, for what its image shows; empty when it has none. */
  void setShot(const QJsonObject& shot) { this->shot = shot; }

  /** @return the selected image's marks as last set */
  const QJsonArray& currentMarks() const { return marks; }

  /** @brief "Show marks" in the panel. */
  void setShown(bool on);

  /** @brief Hidden while an image is taken, whatever "Show marks" says. */
  void setHidden(bool on);

  /** @brief Ctrl+Shift+F10: start placing the picked marker for @p row, or step to the next candidate. */
  void markAtPointer(const QString& row);

  /** @brief Return in the panel while choosing: take the candidate shown. */
  void confirm();

  /** @brief Say @p why nothing is marked, in the status line and at the pointer. */
  void refuse(const QString& why);

  /** @brief Show @p text at the pointer, wrapped at kHintColumns, for a few seconds. */
  static void hint(const QString& text);

  /** A hint's line length at the pointer [characters]. */
  static constexpr qsizetype kHintColumns = 40;

  /** @brief Drop a half-placed mark, e.g. when the application starts again. */
  void cancel(const QString& why = QString());

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  struct candidate_t {
    QJsonObject mark;
    QString name;
    QRect area; /**< global coordinates */
  };
  enum mode_e { eIdle, eChoosing, eTail };

  /** @return what can be marked at @p global, the most specific first; each one resolves */
  QList<candidate_t> candidatesAt(const QPoint& global) const;
  /** @return @p mark with the picked marker's shape */
  QJsonObject shaped(const QJsonObject& mark) const;
  void step();
  void accept();
  void finish(const QJsonObject& mark);
  void leaveMode();
  /**
     @return what a `window` mark is placed on: the open dialog, the window pointed at while placing, the selected
             image's window, the active one
   */
  QWidget* windowShot() const;
  /** @return false when @p placed lies outside what the selected image shows */
  bool inImage(const CShotMarks::placed_t& placed) const;
  /** @brief Place the marks again, repaint them and report their status when it changed. */
  void refresh();

  std::function<void(const QString&)> send;
  std::function<void(const QString&)> report;

  QString marker;
  QJsonArray marks;
  QList<CShotMarks::mark_t> selectedMarks; /**< marks as read, up to the first that cannot be */
  QJsonObject shot;
  QStringList statuses;
  bool shown = true;
  bool hidden = false;

  mode_e mode = eIdle;
  QString row;
  QList<candidate_t> candidates;
  qint32 current = 0;
  QJsonObject head;

  QPointer<QWidget> overlay;
  /** The window Ctrl+Shift+F10 was pressed over, while a mark is placed; cleared when it is done. */
  QPointer<QWidget> pointedWindow;
  QTimer* timer = nullptr;
};

#endif  // CSHOTMARKER_H
