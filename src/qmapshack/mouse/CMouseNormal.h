/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CMOUSENORMAL_H
#define CMOUSENORMAL_H

#include "gis/Poi.h"
#include "mouse/IMouse.h"

#include <QPixmap>
#include <QPointer>
#include <QRect>
#include <QStringList>

class CCanvas;
class IScrOpt;
class CScrOptUnclutter;
class QMenu;

class CMouseNormal : public IMouse
{
    Q_OBJECT
public:
    CMouseNormal(CGisDraw * gis, CCanvas *canvas, CMouseAdapter *mouse);
    virtual ~CMouseNormal();

    void draw(QPainter& p,  CCanvas::redraw_e needsRedraw, const QRect &rect) override;

    void unfocus() override;
    void leftClicked(const QPoint& point) override;
    void mouseMoved(const QPoint& pos) override;
    void mouseDragged(const QPoint& start, const QPoint& last, const QPoint& end) override;
    void rightButtonDown(const QPoint& point) override;
    void longPress(const QPoint& point) override;
    void doubleClicked(const QPoint & point) override;
    void scaleChanged() override;

private slots:
    void slotAddPoi()           const;
    void slotAddWpt()           const;
    void slotAddTrk()           const;
    void slotAddRte()           const;
    void slotAddArea()          const;
    void slotRuler()            const;
    void slotSelectArea()       const;
    void slotCopyPosition()     const;
    void slotCopyPositionGrid() const;

private:
    bool setScreenOption(const QPoint& pt, IGisItem * item);
    void scrollToItem(IGisItem * item);
    void showContextMenu(const QPoint &point);

protected:
    void stopTracking() const;
    void resetState();

    enum item_selection_states_e
    {
        eStateIdle = 0
        , eStateHooverSingle      = 1 //< there is only a single item close to the cursor
        , eStateHooverMultiple    = 2 //< there are multiple items close to the cursor
        , eStateNoMapMovePossible = 3 //< not a real state, but at this value and above no map move is allowed to take place
        , eStateUnclutterMultiple = 3 //< the user clicked on multiple cluttered items and gets now an uncluttered representation
        , eStateShowItemOptions   = 4 //< the user has selected a single item, show options how to proceed
    };

    item_selection_states_e stateItemSel = eStateIdle;

    CScrOptUnclutter * screenUnclutter;
    QPointer<IScrOpt>  screenItemOption;

    poi_t curPOI;
};

#endif //CMOUSENORMAL_H

