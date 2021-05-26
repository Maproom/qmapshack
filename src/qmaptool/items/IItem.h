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

#ifndef IITEM_H
#define IITEM_H


#include "tool/ITool.h"
#include <QObject>

class QSettings;
class IDrawContext;

class IItem : public QObject, public ITool
{
    Q_OBJECT
public:
    IItem(const QString& filename);
    virtual ~IItem() = default;

    virtual void saveSettings(QSettings& cfg);
    virtual void loadSettings(QSettings& cfg);

    const QString& getFilename() const
    {
        return filename;
    }

    IDrawContext* getDrawContext() const
    {
        return drawContext;
    }

    bool getMapDidMove() const
    {
        return mapDidMove;
    }

    bool getMapIsMoving() const
    {
        return mapIsMoving;
    }

    /// reload file into draw context
    virtual void reload();
    /// item has been selected, bring everything to front (to be displayed)
    virtual void toFront(){}

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) override;
    void mousePressEventFx(QMouseEvent* e) override;
    void mouseMoveEventFx(QMouseEvent* e) override;
    void mouseReleaseEventFx(QMouseEvent* e) override;
    void wheelEventFx(QWheelEvent* e) override;

    virtual bool isOk() const
    {
        return false;
    }

signals:
    void sigChanged();

protected:
    QString filename;
    IDrawContext* drawContext = nullptr;

    /// true while left mouse button is pressed down
    bool mapIsMoving = false;
    /// true if map actually moved, if not it's a single click
    bool mapDidMove = false;

    /// last mouse position
    QPoint lastPos;
    /// mouse position when left button was pressed
    QPoint firstPos;

    /// current accumulated angleDelta, used/required for zooming on track pads
    int zoomAngleDelta = 0;
};

#endif //IITEM_H

