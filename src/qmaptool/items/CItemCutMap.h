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

#ifndef CITEMCUTMAP_H
#define CITEMCUTMAP_H

#include "items/CItemFile.h"

class COverlayCutMap;

class CItemCutMap : public CItemFile
{
public:
    CItemCutMap(const QString& filename, QStackedWidget * stackedWidget, QListWidget *parent);
    virtual ~CItemCutMap();

    void saveSettings(QSettings& cfg) override;
    void loadSettings(QSettings& cfg) override;

    void toFront() override;

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) override;
    void mouseMoveEventFx(QMouseEvent *e) override;
    void mouseReleaseEventFx(QMouseEvent *e) override;
    void leaveEventFx(QEvent *e) override;
    QCursor getCursorFx() override;

    void saveShape(const QString& filename) const;

    bool isOk() const override;

private:
    COverlayCutMap * overlay;
};

#endif //CITEMCUTMAP_H

