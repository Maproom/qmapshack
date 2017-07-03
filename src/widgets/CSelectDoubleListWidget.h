/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#ifndef CSELECTDOUBLELISTWIDGET_H
#define CSELECTDOUBLELISTWIDGET_H

#include "ui_ISelectDoubleListWidget.h"

class CSelectDoubleListWidget : public QWidget, private Ui::ISelectDoubleListWidget
{
    Q_OBJECT
public:
    class IItemFilter
    {
public:
        virtual bool shouldBeMoved(QListWidgetItem * item) = 0;
    };

    CSelectDoubleListWidget(QWidget *parent, IItemFilter *filter = nullptr);
    virtual ~CSelectDoubleListWidget();

    void setAvailable(const QList<QListWidgetItem *> &available);
    void setSelected(const QList<QListWidgetItem *> &selected) const;
    void setLabelAvailable(const QString & label) const;
    void setLabelSelected(const QString & label) const;
    void setFilter(IItemFilter * const &filter);
    const QList<QListWidgetItem *> selected() const;
    void clear();

private slots:
    void slotSelectedClicked(const QModelIndex & index) const;
    void slotAvailableClicked(const QModelIndex & index) const;
    void slotAdd() const;
    void slotRemove() const;
    void slotUp() const;
    void slotDown() const;

private:
    void updateButtons() const;

    QList<QListWidgetItem *> available;
    IItemFilter * filter;
};
#endif //CSELECTDOUBLELISTWIDGET_H
