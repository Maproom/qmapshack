/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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
class QHelpEngine;
class QSplitter;
class QTabWidget;
class CHelpIndex;
class CHelpSearch;

class CHelp :  public QDockWidget
{
    Q_OBJECT
public:
    CHelp(const QString &helpfile, const QString &homepage, QWidget * parent);
    virtual ~CHelp();

private:
    QHelpEngine * engine;
    QTabWidget * tabWidget;
    QSplitter * splitter;
    CHelpIndex * index;
    CHelpSearch * search;
};

#endif //CHELP_H

