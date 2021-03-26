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

#ifndef CGRIDPLACER_H
#define CGRIDPLACER_H

#include "canvas/CCanvas.h"
#include "overlay/gridtool/CGridPoint.h"

#include "ui_IGridPlacer.h"

class CItemRefMap;
class QSettings;

class CGridPlacer : public QWidget, private Ui::IGridPlacer
{
    Q_OBJECT
public:
    CGridPlacer(QWidget * parent);
    virtual ~CGridPlacer() = default;

    void registerItem(CItemRefMap * item);

    void saveSettings(QSettings& cfg);
    void loadSettings(QSettings& cfg);

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw);
    void mouseMoveEventFx(QMouseEvent *e);
    void mouseReleaseEventFx(QMouseEvent *e);
    void leaveEventFx(QEvent *e);
    QCursor getCursorFx();

    bool isOk() const
    {
        return statusIsOk;
    }

    const QPointF& getPoint(int idx) const
    {
        if(idx < points.count())
        {
            return points[idx].getPoint();
        }
        return NOPOINTF;
    }

signals:
    void sigChanged();
    void sigSetArea(const QRectF& area);

public slots:
    void slotReset();

private slots:
    void slotSetPoint(qint32 i, bool on);
    void slotSetArea();

private:
    void updateStatus();
    CItemRefMap * item = nullptr;

    qint32 idx = 0;

    QVector<CGridPoint> points;

    bool statusIsOk = false;
};

#endif //CGRIDPLACER_H

