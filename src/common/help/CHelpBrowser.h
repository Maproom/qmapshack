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

#include <QTextBrowser>

class QHelpEngine;

class CHelpBrowser : public QTextBrowser {
  Q_OBJECT
 public:
  CHelpBrowser(QHelpEngine* helpEngine, QWidget* parent);
  virtual ~CHelpBrowser() = default;

  QVariant loadResource(int type, const QUrl& name) override;

 public slots:
  void setSource(const QUrl& url);

 private:
  QHelpEngine* engine;
};

#endif  // CHELPBROWSER_H
