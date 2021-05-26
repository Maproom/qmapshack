/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CPOILIST_H
#define CPOILIST_H

#include <QTreeWidget>

class CPoiItem;

class CPoiTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    CPoiTreeWidget(QWidget* parent) : QTreeWidget(parent)
    {
    }

signals:
    void sigChanged();
};

#include "ui_IPoiList.h"

class CPoiList : public QWidget, private Ui::IPoiList
{
    Q_OBJECT
public:
    CPoiList(QWidget* parent);
    virtual ~CPoiList() = default;

    void clear();
    int count();
    CPoiItem* item(int i);
    operator QTreeWidget*(){return treeWidget;}

    void updateHelpText();
    void sort();

signals:
    void sigChanged();

private slots:
    void slotActivate();
    void slotReloadPoi();
    void slotContextMenu(const QPoint& point);

private:
    QMenu* menu;
};

#endif //CPOILIST_H

