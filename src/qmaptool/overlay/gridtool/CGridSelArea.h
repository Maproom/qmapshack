/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CGRIDSELAREA_H
#define CGRIDSELAREA_H

#include "canvas/CCanvas.h"
#include "ui_IGridSelArea.h"

class CItemRefMap;
class QSettings;
class IDrawContext;


class CGridSelArea : public QWidget, private Ui::IGridSelArea
{
    Q_OBJECT
public:
    CGridSelArea(QWidget * parent);
    virtual ~CGridSelArea() = default;

    void registerItem(CItemRefMap * item);

    void saveSettings(QSettings& cfg);
    void loadSettings(QSettings& cfg);

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw);
    void mouseMoveEventFx(QMouseEvent *e);
    void mouseReleaseEventFx(QMouseEvent *e);
    void leaveEventFx(QEvent *e);
    QCursor getCursorFx();

    const QRectF& getArea() const
    {
        return area;
    }

public slots:
    void slotSetArea(const QRectF& rect);
    void slotReset();


signals:
    void sigChanged();

private:
    CItemRefMap * item = nullptr;
    const IDrawContext * context   = nullptr;

    QRectF area;
    QRectF areaSave;
    QPointF offset;

    QRectF rectTopLeft     {0, 0, 20, 20};
    QRectF rectTopRight    {0, 0, 20, 20};
    QRectF rectBottomLeft  {0, 0, 20, 20};
    QRectF rectBottomRight {0, 0, 20, 20};

    enum state_e
    {
        eStateIdle
        ,eStateMove
    };

    state_e state = eStateIdle;

    enum corner_e
    {
        eCornerNone
        , eCornerTopLeft
        , eCornerTopRight
        , eCornerBottomLeft
        , eCornerBottomRight
        , eCornerPrint
        , eCornerImage
    };

    corner_e corner = eCornerNone;
};

#endif //CGRIDSELAREA_H

