/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CLINEOPSELECTRANGE_H
#define CLINEOPSELECTRANGE_H

#include "mouse/line/ILineOp.h"

#include <QPointer>

class CScrOptRangeLine;

class CLineOpSelectRange : public ILineOp
{
    Q_OBJECT
public:
    CLineOpSelectRange(SGisLine& points, CGisDraw *gis, CCanvas *canvas, IMouseEditLine *parent);
    virtual ~CLineOpSelectRange();

    void leftClick(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override;
    void rightButtonDown(const QPoint &pos) override;
    void scaleChanged() override;

    void drawFg(QPainter& p) override;

    bool abortStep() override;

private slots:
    void slotDelete();
    void slotCalc();

private:
    void resetState();
    enum state_e
    {
        eStateIdle
        , eState1st
        , eState2nd
    };

    state_e state = eStateIdle;

    qint32 idx2nd = NOIDX;

    QPointer<CScrOptRangeLine>  scrOptRangeLine;
};

#endif //CLINEOPSELECTRANGE_H

