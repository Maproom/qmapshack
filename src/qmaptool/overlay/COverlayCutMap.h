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

#ifndef COVERLAYCUTMAP_H
#define COVERLAYCUTMAP_H

#include "canvas/CCanvas.h"
#include "overlay/IOverlay.h"
#include "ui_IOverlayCutMap.h"
#include "units/IUnit.h"


class QStackedWidget;
class IDrawContext;
class QSettings;
class CItemCutMap;

class COverlayCutMap : public IOverlay, private Ui::IOverlayCutMap
{
    Q_OBJECT
public:
    COverlayCutMap(CItemCutMap *item, QStackedWidget * stackedWidget);
    virtual ~COverlayCutMap() = default;

    void saveSettings(QSettings& cfg);
    void loadSettings(QSettings& cfg);

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw);
    void mouseMoveEventFx(QMouseEvent *e);
    void mouseReleaseEventFx(QMouseEvent *e);
    QCursor getCursorFx();

    void abortStep();

    enum mode_e
    {
        eModeNone
        , eModePointMove
        , eModePointAdd
        , eModePointDel
    };

    void saveShape(const QString& filename);

    bool isOk() const
    {
        return !region.isEmpty();
    }

private slots:
    void slotSetMode(COverlayCutMap::mode_e m, bool on);
    void slotResetMask();
    void slotSaveShape();
    void slotLoadShape();

private:
    void restoreFromHistory(QPolygonF& line);
    void storeToHistory(const QPolygonF& line);
    void isCloseTo(QPointF pt, QPolygonF line, qint32& idx) const;
    void isCloseToLine(QPointF pt, QPolygonF line, qint32 &idx1, qint32 &idx2) const;
    void mouseReset();

    void mouseMovePointAdd(const QPointF &pt);
    void mouseMovePointDel(const QPointF &pt);
    void mouseMovePointMove(const QPointF &pt);

    void mouseReleasePointAdd(const QPointF &pt, Qt::MouseButton button);
    void mouseReleasePointDel(const QPointF &pt, Qt::MouseButton button);
    void mouseReleasePointMove(const QPointF &pt, Qt::MouseButton button);

    void updateGui();

    IDrawContext* context;

    mode_e mode = eModeNone;
    QPolygonF region;

    qint32 idxFocus1 = NOIDX;
    qint32 idxFocus2 = NOIDX;
    bool addPoint   = false;
    bool movePoint  = false;

    QList<QPolygonF> history;
    qint32 idxHistory = NOIDX;

    QString shapeFilename;
};

#endif //COVERLAYCUTMAP_H

