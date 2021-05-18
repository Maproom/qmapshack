/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CTABLETRKINFO_H
#define CTABLETRKINFO_H

#include <gis/trk/CGisItemTrk.h>
#include <QTreeWidget>

class CTableTrkInfo : public QTreeWidget, public INotifyTrk
{
    Q_OBJECT
public:
    CTableTrkInfo(QWidget* parent);
    virtual ~CTableTrkInfo();

    void setTrack(CGisItemTrk* track);

    void updateData() override;
    void setMouseFocus(const CTrackData::trkpt_t* pt) override {}
    void setMouseRangeFocus(const CTrackData::trkpt_t* pt1, const CTrackData::trkpt_t* pt2) override {}
    void setMouseClickFocus(const CTrackData::trkpt_t* pt) override {}

    enum columns_t
    {
        eColNum
        , eColDesc
        , eColMax
    };

signals:
    void sigHasTrkPtInfo(bool yes);

private slots:
    void slotContextMenu(const QPoint& point);
    void slotEdit();
    void slotDelete();

    void slotItemChanged(QTreeWidgetItem* item, int column);
    void slotItemSelectionChanged();

private:
    CGisItemTrk* trk = nullptr;
    QAction* actionDelete;
    QAction* actionEdit;
    QMenu* menu;
};

#endif //CTABLETRKINFO_H

