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

#ifndef CMOUSENORMAL_H
#define CMOUSENORMAL_H

#include "IMouse.h"

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
    CMouseNormal(CGisDraw * gis, CCanvas *canvas);
    virtual ~CMouseNormal();

    void draw(QPainter& p,  bool needsRedraw,  const QRect &rect);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent * e);

private slots:
    void slotAddWpt();
    void slotAddTrk();
    void slotAddArea();
    void slotCopyPosition();

protected:
    void stopTracking();


    /// the flag is true if the map moving is in progress
    bool mapMove;
    /// the flag is true if the map has been moved actually
    bool mapDidMove;
    /// always the last seen mouse cursor position
    QPoint lastPos;

    enum item_selection_states_e
    {
        eStateIdle = 0
                     /// there is only a single item close to the cursor
        , eStateHooverSingle = 1
                               /// there are multiple items close to the cursor
        , eStateHooverMultiple = 2
                                 /// not a real state, but at this value and above no map move is allowed to take place
        , eStateNoMapMovePossible = 3
                                    /// the user clicked on multiple cluttered items and gets now an uncluttered representation
        , eStateUnclutterMultiple = 3
                                    /// the user has selected a single item, show options how to proceed
        , eStateShowItemOptions = 4
    };

    item_selection_states_e stateItemSel;

    CScrOptUnclutter * screenUnclutter;
    QPointer<IScrOpt>  screenItemOption;

    QMenu * menu;
};

#endif //CMOUSENORMAL_H

