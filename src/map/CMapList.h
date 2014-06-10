/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CMAPLIST_H
#define CMAPLIST_H

#include <QWidget>
#include "ui_IMapList.h"

class CMap;
class CMapItem;

class CMapList : public QWidget, private Ui::IMapList
{
    Q_OBJECT
    public:
        CMapList(CMap * map, QWidget * parent);
        virtual ~CMapList();

        void clear();
        int count();
        CMapItem * item(int i);
        operator QListWidget*(){return listWidget;}

        /**
           @brief Tell the attached CMap object to rebuild the widget from the map path list
         */
        void updateList();

    private slots:
        void slotSelectionChanged();
        void slotActivate();
        void slotToTop();

    private:
        CMap * map;

};

#endif //CMAPLIST_H

