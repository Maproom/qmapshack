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
class T;
class QList<T>;
class QListView;
class QVariant;
class QModelIndex;
class QAbstractListModel;

class CSelectDoubleListWidget : public QWidget, private Ui::ISelectDoubleListWidget
{
    Q_OBJECT
public:
    struct sItem {
        const QIcon icon;
        const QString text;
        const QVariant data;
        sItem(const QIcon & icon, const QString & text, const QVariant data) : icon(icon), text(text), data(data) {}
        sItem(const sItem & item) : icon(item.icon), text(item.text), data(item.data) {}
        sItem operator=(const sItem & other) { return sItem(other); }
        bool operator==(const sItem & other) { return data == other.data; }
    };

    CSelectDoubleListWidget(QWidget *parent);
    virtual ~CSelectDoubleListWidget();

    void setAvailable(const QList<sItem> & available);
    void setSelected(const QList<sItem> & selected);
    const QList<sItem> selected();

private slots:
    void slotSelectedClicked(const QModelIndex & index);
    void slotAvailableClicked(const QModelIndex & index);
    void slotAdd();
    void slotRemove();
    void slotUp();
    void slotDown();

private:
    class CItemListModel : public QAbstractListModel
    {
    public:
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        const QModelIndex modelIndex(int row) const;

    private:
        QList<sItem> items;

        friend class CSelectDoubleListWidget;
    };

    void filterModAvailable();
    QList<sItem> available;
    CSelectDoubleListWidget::CItemListModel modAvailable;
    CSelectDoubleListWidget::CItemListModel modSelected;
};
#endif //CSELECTDOUBLELISTWIDGET_H
