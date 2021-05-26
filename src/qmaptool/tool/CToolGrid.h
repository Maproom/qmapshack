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

#ifndef CTOOLGRID_H
#define CTOOLGRID_H

#include "tool/ITool.h"
#include "ui_IToolGrid.h"

class CItemRefMap;

class CToolGrid : public QWidget, public ITool, private Ui::IToolGrid
{
    Q_OBJECT
public:
    virtual ~CToolGrid() = default;

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) override;
    void mousePressEventFx(QMouseEvent* e) override;
    void mouseMoveEventFx(QMouseEvent* e) override;
    void mouseReleaseEventFx(QMouseEvent* e) override;
    void wheelEventFx(QWheelEvent* e) override;
    void leaveEventFx(QEvent* e) override;
    QCursor getCursorFx() override;

    void setupChanged() override {}

    void registerItem(CItemRefMap* item);

private slots:
    void slotOk();
    void slotCancel();

private:
    friend class CMainWindow;
    CToolGrid(QWidget* parent);

    CItemRefMap* item = nullptr;
};

#endif //CTOOLGRID_H

