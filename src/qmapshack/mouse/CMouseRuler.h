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

#ifndef CMOUSERULER_H
#define CMOUSERULER_H

#include "mouse/IMouse.h"
#include "units/IUnit.h"

class CScrOptRuler;

class CMouseRuler : public IMouse
{
    Q_OBJECT
public:
    CMouseRuler(CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse);
    virtual ~CMouseRuler();


    void rightButtonDown(const QPoint& pos) override;
    void leftClicked(const QPoint& pos) override;
    void mouseMoved(const QPoint& pos) override;
    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect& rect) override;

private slots:
    void slotUndo();
    void slotRedo();
    void slotReset();

    void slotToWpt();
    void slotToTrk();
    void slotToRte();
    void slotToArea();

private:
    void storeToHistory(const QPolygonF& line);
    void updateStatus(const QPolygonF& line);
    void meter2whatever(qreal meter, QString& val, QString& unit);

    enum mode_e
    {
        eModeIdle
        , eModePaused
        , eModeEdit
    };

    mode_e mode = eModePaused;
    QPolygonF ruler;
    qint32 idxHistory = NOIDX;
    QList<QPolygonF> history;

    CScrOptRuler* scrOptRuler;
};

#endif //CMOUSERULER_H

