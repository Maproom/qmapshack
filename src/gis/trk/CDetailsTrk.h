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

#ifndef CDETAILSTRK_H
#define CDETAILSTRK_H

#include "gis/trk/CGisItemTrk.h"
#include "plot/CPlot.h"
#include "ui_IDetailsTrk.h"
#include <QWidget>

class CPlotProfile;

class CDetailsTrk : public QWidget, public INotifyTrk, private Ui::IDetailsTrk
{
    Q_OBJECT
public:
    CDetailsTrk(CGisItemTrk &trk, QWidget * parent);
    virtual ~CDetailsTrk();

    void setMouseFocus(const CGisItemTrk::trkpt_t * pt);
    void setMouseRangeFocus(const CGisItemTrk::trkpt_t * pt1, const CGisItemTrk::trkpt_t * pt2);
    void setMouseClickFocus(const CGisItemTrk::trkpt_t * pt);

public slots:
    void updateData();

private slots:
    void slotNameChanged(const QString &name);
    void slotNameChangeFinished();
    void slotShowPlots();
    void slotColorChanged(int idx);
    void slotChangeReadOnlyMode(bool on);
    void slotItemSelectionChanged();
    void slotLinkActivated(const QUrl& url);
    void slotMouseClickState(int);
    void slotActivitySelected(bool checked);

    void slotColorSourceChanged(int idx);
    void slotColorLimitHighChanged();
    void slotColorLimitLowChanged();

    void slotLimitLowFromData();
    void slotLimitHighFromData();

    void slotSetupGraph(int idx);

private:
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

    enum tabs_t
    {
        eTabInfo
        ,eTabStyle
        ,eTabGraphs
        ,eTabActivity
        ,eTabPoints
        ,eTabFilter
        ,eTabHistory
    };

    /**
       @brief Pointer to track item

       It is ok to store the pointer as this widget is created by the track item. The
       track item will destroy this object on it's own destruction.
     */
    CGisItemTrk& trk;

    bool originator = false;

    CPlotProfile * plot1;
    CPlot * plot2;
    CPlot * plot3;
};

#endif //CDETAILSTRK_H

