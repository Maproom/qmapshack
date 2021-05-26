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

#ifndef COVERLAYGRIDTOOL_H
#define COVERLAYGRIDTOOL_H

#include "canvas/CCanvas.h"
#include "ui_IOverlayGridTool.h"

class CItemRefMap;
class COverlayRefMapPoint;

class COverlayGridTool : public QWidget, private Ui::IOverlayGridTool
{
    Q_OBJECT
public:
    COverlayGridTool(QWidget* parent);
    virtual ~COverlayGridTool() = default;

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw);
    void mouseMoveEventFx(QMouseEvent* e);
    void mouseReleaseEventFx(QMouseEvent* e);
    void leaveEventFx(QEvent* e);
    QCursor getCursorFx();

    void registerItem(CItemRefMap* item);

    QList<COverlayRefMapPoint*>& getRefPoints()
    {
        return refPoints;
    }

signals:
    void sigChanged(bool ok);

public slots:
    void slotReset();

private slots:
    void slotCheckInput();
    void slotSetArea(const QRectF& rect);
    void slotCalculate();

private:

    CItemRefMap* item = nullptr;
    const IDrawContext* context = nullptr;

    QList<COverlayRefMapPoint*> refPoints;
};

#endif //COVERLAYGRIDTOOL_H

