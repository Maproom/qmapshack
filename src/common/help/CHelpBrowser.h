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

#ifndef CHELPBROWSER_H
#define CHELPBROWSER_H

#include <QWebEngineView>

class QHelpEngine;
class QWebEngineProfile;

class CHelpBrowser : public QWebEngineView {
  Q_OBJECT
 public:
  CHelpBrowser(QHelpEngine* helpEngine, QWidget* parent);
  ~CHelpBrowser() override;

  /** @brief Register the qthelp URL scheme. Call once, before the QApplication is constructed. */
  static void registerUrlScheme();

 public slots:
  void setSource(const QUrl& url);

 protected:
  void contextMenuEvent(QContextMenuEvent* event) override;

  /** @brief Re-apply the themed colours to the open page on a palette switch. */
  void changeEvent(QEvent* e) override;

 private:
  void applyTheme();

  QHelpEngine* engine;
  QWebEngineProfile* profile;
  QUrl homeUrl;
};

#endif  // CHELPBROWSER_H
