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

#ifndef CMAPTREEWIDGET_H
#define CMAPTREEWIDGET_H

#include <QTreeWidgetItem>

class CMapItem;

class CMapTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    CMapTreeWidget(QWidget * parent)
        : QTreeWidget(parent)
    {
    }

signals:
    void sigChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

    void resizeEvent(QResizeEvent * e) override;
};

#endif //CMAPTREEWIDGET_H

