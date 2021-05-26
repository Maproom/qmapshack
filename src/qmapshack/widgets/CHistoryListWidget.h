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

#ifndef CHISTORYLISTWIDGET_H
#define CHISTORYLISTWIDGET_H

#include "gis/IGisItem.h"
#include <QListWidget>

class CHistoryListWidget : public QListWidget
{
    Q_OBJECT
public:
    CHistoryListWidget(QWidget* parent);
    virtual ~CHistoryListWidget();

    void setupHistory(IGisItem& gisItem);

signals:
    void sigChanged();

private slots:
    void slotSelectionChanged();
    void slotContextMenu(const QPoint& point);
    void slotCutHistoryAfter();
    void slotCutHistoryBefore();

private:
    IGisItem::key_t key;
    QMenu* menu;
    QAction* actionCutHistoryAfter;
    QAction* actionCutHistoryBefore;
};

#endif //CHISTORYLISTWIDGET_H

