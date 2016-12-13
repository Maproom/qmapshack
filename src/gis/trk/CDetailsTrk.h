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
#include "helpers/CLimit.h"
#include "ui_IDetailsTrk.h"
#include <QWidget>

class CPlot;
class CPlotProfile;

class CDetailsTrk : public QWidget, public INotifyTrk, private Ui::IDetailsTrk
{
    Q_OBJECT
public:
    CDetailsTrk(CGisItemTrk &trk, QWidget * parent);
    virtual ~CDetailsTrk();

    void setMouseFocus(const CTrackData::trkpt_t *pt) override;
    void setMouseRangeFocus(const CTrackData::trkpt_t *pt1, const CTrackData::trkpt_t *pt2) override;
    void setMouseClickFocus(const CTrackData::trkpt_t *pt) override;

public slots:
    void updateData() override;

private slots:
    void slotNameChanged(const QString &name);
    void slotNameChangeFinished();
    void slotShowPlots();
    void slotColorChanged(int idx);
    void slotChangeReadOnlyMode(bool on);
    void slotLinkActivated(const QUrl& url);
    void slotMouseClickState(int);
    void slotSetActivities();
    void slotRemoveActivities();

    void slotColorSourceChanged(int idx);
    void slotColorLimitHighChanged();
    void slotColorLimitLowChanged();

    void slotSetupGraph(int idx);
    void slotSetLimitModeGraph(CLimit::mode_e mode, CLimit *limit, QDoubleSpinBox *spinMin, QDoubleSpinBox *spinMax, bool on);
    void slotSetLimitModeStyle(CLimit::mode_e mode, bool on);

    void slotLineWidthMode(bool isUser);
    void slotLineWidth(qreal f);
    void slotWithArrowsMode(bool isUser);
    void slotWithArrows(bool yes);

private:
    void loadGraphSource(QComboBox * comboBox, qint32 n, const QString cfgDefault);
    void saveGraphSource(QComboBox * comboBox, qint32 n);
    void setupGraphLimits(CLimit& limit, QToolButton * toolLimitAutoGraph, QToolButton * toolLimitUsrGraph, QToolButton * toolLimitSysGraph, QDoubleSpinBox * spinMinGraph, QDoubleSpinBox * spinMaxGraph);
    void setupStyleLimits(CLimit& limit, QToolButton *toolLimitAuto, QToolButton *toolLimitUsr, QToolButton *toolLimitSys, CDoubleSpinBox *spinMin, CDoubleSpinBox *spinMax);
    void setupGraph(CPlot * plot, const CLimit &limit, const QString& source, QDoubleSpinBox * spinMin, QDoubleSpinBox * spinMax);
    void setupLimits(CLimit *limit, QDoubleSpinBox * spinMin, QDoubleSpinBox * spinMax);


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

