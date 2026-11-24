/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CHELP_H
#define CHELP_H

#include <QDockWidget>
class CHelpBrowser;
class QHelpEngine;
class QMainWindow;
class QSplitter;
class QTabWidget;
class CHelpIndex;
class CHelpSearch;

class CHelp : public QDockWidget {
  Q_OBJECT
 public:
  /** @brief Help from a compressed Qt help collection, with content, index and search panes. */
  CHelp(const QString& helpfile, const QString& homepage, QWidget* parent);
  /** @brief Help from a generated HTML tree, which brings its own navigation, index and search. */
  CHelp(const QString& homepage, QWidget* parent);
  virtual ~CHelp();

  /**
     @brief Show it where it was when last closed, docked or floating; the first time floating, centred over @p main.

     Call after QMainWindow::addDockWidget(): docked, the help takes the width of its dock column.
   */
  void place(QMainWindow* main);

  /** @brief Show the generated HTML page @p file. */
  void setPage(const QString& file);

 private:
  void restoreSplitter();

  QHelpEngine* engine = nullptr;
  QTabWidget* tabWidget = nullptr;
  QSplitter* splitter = nullptr;
  CHelpBrowser* browser = nullptr;
  CHelpIndex* index = nullptr;
  CHelpSearch* search = nullptr;
};

#endif  // CHELP_H
