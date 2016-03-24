/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CTABLETRK_H
#define CTABLETRK_H

#include <gis/trk/CGisItemTrk.h>
#include <QTreeWidget>

class CTableTrk : public QTreeWidget, public INotifyTrk
{
Q_OBJECT
public:
    CTableTrk(QWidget * parent);
    virtual ~CTableTrk();

    void setTrack(CGisItemTrk * track);

    void updateData() override;
    void setMouseFocus(const CGisItemTrk::trkpt_t * pt) override {}
    void setMouseRangeFocus(const CGisItemTrk::trkpt_t * pt1, const CGisItemTrk::trkpt_t * pt2) override {}
    void setMouseClickFocus(const CGisItemTrk::trkpt_t * pt) override {}


    enum columns_t
    {
        eColNum
        ,eColTime
        ,eColEle
        ,eColDelta
        ,eColDist
        ,eColSpeed
        ,eColSlope
        ,eColAscend
        ,eColDescend
        ,eColPosition
        ,eColMax
    };

private slots:
    void slotItemSelectionChanged();

private:
    CGisItemTrk * trk = nullptr;
};

#endif //CTABLETRK_H

