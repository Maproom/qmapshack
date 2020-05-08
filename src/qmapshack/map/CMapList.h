/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include <QTreeWidget>
#include <QWidget>

class CMapItem;
class QMenu;

class CMapTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    CMapTreeWidget(QWidget * parent) : QTreeWidget(parent)
    {
    }

signals:
    void sigChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent  *e) override;
    void dropEvent(QDropEvent      *e) override;
};

#include "ui_IMapList.h"

class CMapList : public QWidget, private Ui::IMapList
{
    Q_OBJECT
public:
    CMapList(QWidget * parent);
    virtual ~CMapList();

    void clear();
    void sort();
    int count();
    CMapItem * item(int i);
    operator QTreeWidget*()
    {
        return treeWidget;
    }

    void updateHelpText();

signals:
    void sigChanged();

public slots:
    static void slotMapHonk();

private slots:
    void slotActivate();
    void slotMoveUp();
    void slotMoveDown();
    void slotReloadMaps();
    void slotContextMenu(const QPoint &point);
    void slotFilter(const QString &str);

private:
    QMenu * menu;
};

#endif //CMAPLIST_H

