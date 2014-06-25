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
#include <QTreeWidget>

class CMapItem;
class QMenu;

class CMapTreeWidget : public QTreeWidget
{
    Q_OBJECT
    public:
        CMapTreeWidget(QWidget * parent): QTreeWidget(parent){}

    signals:
        void sigChanged();

    protected:
        void dragEnterEvent(QDragEnterEvent * e);
        void dragMoveEvent (QDragMoveEvent  * e );
        void dropEvent ( QDropEvent  * e );
};

#include "ui_IMapList.h"

class CMapList : public QWidget, private Ui::IMapList
{
    Q_OBJECT
    public:
        CMapList(QWidget * parent);
        virtual ~CMapList();

        void clear();
        int count();
        CMapItem * item(int i);
        operator QTreeWidget*(){return treeWidget;}

        void updateHelpText();

    signals:
        void sigChanged();

    private slots:
        void slotItemSelectionChanged();
        void slotActivate();
        void slotContextMenu(const QPoint &point);

    private:
        QMenu * menu;
};

#endif //CMAPLIST_H

