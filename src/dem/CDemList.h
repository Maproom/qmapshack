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

#ifndef CDEMLIST_H
#define CDEMLIST_H

#include <QTreeWidget>
#include <QWidget>

class CDemItem;

class CDemTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    CDemTreeWidget(QWidget * parent) : QTreeWidget(parent)
    {
    }

signals:
    void sigChanged();

protected:
    void dragMoveEvent ( QDragMoveEvent  * event );
    void dropEvent ( QDropEvent  * event );
};


#include "ui_IDemList.h"

class CDemList : public QWidget, private Ui::IDemsList
{
    Q_OBJECT
public:
    CDemList(QWidget * parent);
    virtual ~CDemList();

    void clear();
    int count();
    CDemItem * item(int i);
    operator QTreeWidget*()
    {
        return( treeWidget);
    }

    void updateHelpText();

signals:
    void sigChanged();

private slots:
    void slotActivate();
    void slotContextMenu(const QPoint &point);

private:
    QMenu * menu;
};

#endif //CDEMLIST_H

