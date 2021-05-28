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

#ifndef CSCROPTRANGETOOL_H
#define CSCROPTRANGETOOL_H

#include "gis/trk/CGisItemTrk.h"
#include "mouse/IScrOpt.h"
#include "ui_IScrOptRangeTool.h"

#include <functional>

class CCanvas;
class CPlot;
class CGisItemTrk;
class CMouseRangeTrk;

class CScrOptRangeTool : public IScrOpt, public INotifyTrk, private Ui::IScrOptRangeTool
{
    Q_OBJECT
public:
    CScrOptRangeTool(CGisItemTrk& trk, CMouseRangeTrk* mouse, CCanvas* canvas);
    virtual ~CScrOptRangeTool();

    void draw(QPainter& p) override;

    void updateData() override;
    void setMouseFocus(const CTrackData::trkpt_t* pt) override;
    void setMouseRangeFocus(const CTrackData::trkpt_t* pt1, const CTrackData::trkpt_t* pt2) override;
    void setMouseClickFocus(const CTrackData::trkpt_t* pt) override;

    enum class eAction : int
    {
        Nothing = 0,
        Reset = 1,
        ToEnd = 2,
        ToStart = 3,
        Default = Reset,
        Max = 4,
    };

private slots:
    void slotSetup();
    void slotResetRange();
    void slotHidePoints();
    void slotShowPoints();
    void slotActivity();
    void slotDelete();
    void slotCopy();
    void slotToRoute();
    void slotResetUpdateCanvas()
    {
        updateCanvas = false;
    }
    void slotCanvasResize(const QSize& sizeCanvas);

private:
    friend class CRangeToolSetup;
    const QString owner;
    CGisItemTrk& trk;
    CCanvas* canvas;
    CPlot* plot = nullptr;
    CMouseRangeTrk* myMouse;

    enum class eSelState
    {
        None,
        One,
        Multiple,
    };
    eSelState selState = eSelState::None;

    bool updateCanvas = false;
    bool enableUpdateCavas = true;

    eAction actionHidePoints = eAction::Reset;
    eAction actionShowPoints = eAction::Reset;
    eAction actionActivity = eAction::ToEnd;
    eAction actionCopy = eAction::Nothing;
    eAction actionDelete = eAction::ToStart;

    using fAction = std::function<void()>;
    QVector<fAction> actions {
        [this](){},
        [this](){slotResetRange();},
        [this](){
            updateCanvas = true;
            this->trk.newMouseRangeEnd(owner);
            updateCanvas = false;
        },
        [this](){
            updateCanvas = true;
            this->trk.newMouseRangeBegin(owner);
            updateCanvas = false;
        }
    };
};

Q_DECLARE_METATYPE(CScrOptRangeTool::eAction)

#endif //CSCROPTRANGETOOL_H

