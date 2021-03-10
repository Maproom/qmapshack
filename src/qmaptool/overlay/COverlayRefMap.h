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

#ifndef COVERLAYREFMAP_H
#define COVERLAYREFMAP_H

#include "overlay/IOverlay.h"
#include "ui_IOverlayRefMap.h"
#include "units/IUnit.h"

class QStackedWidget;
class IDrawContext;
class QSettings;
class CItemRefMap;
class COverlayRefMapPoint;

class COverlayRefMap : public IOverlay, private Ui::IOverlayRefMap
{
    Q_OBJECT
public:
    COverlayRefMap(CItemRefMap *item, QStackedWidget * stackedWidget);
    virtual ~COverlayRefMap() = default;

    void saveSettings(QSettings& cfg);
    void loadSettings(QSettings& cfg);

    void addRefPoints(QList<COverlayRefMapPoint*>& points);
    QString getMapProjection() const;
    const QList<COverlayRefMapPoint *> getRefPoints() const;

    bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw);
    void mouseMoveEventFx(QMouseEvent *e);
    void mouseReleaseEventFx(QMouseEvent *e);
    bool keyPressEventFx(QKeyEvent *e);
    QCursor getCursorFx();

    void abortStep();

    enum mode_e
    {
        eModeNone
        , eModePointMove
        , eModePointAdd
        , eModePointDel
        , eModePointMoveAuto
    };

    bool isOk() const;

private slots:
    void slotSetMode(mode_e m, bool on);
    void slotSaveGcp();
    void slotLoadGcp();
    void slotResetRef();
    void slotProjWizard();
    void slotGridTool();
    void slotSelectionChanged();
    void slotDelRefPoints();
    void slotSortRefPoints();


private:
    void updateGui();
    QPointF isCloseTo(QPointF pt);

    void mouseMovePointAdd(const QPointF &pt);
    void mouseMovePointDel(const QPointF &pt);
    void mouseMovePointMove(const QPointF &pt);

    void mouseReleasePointAdd(const QPointF &pt, Qt::MouseButton button);
    void mouseReleasePointDel(const QPointF &pt, Qt::MouseButton button);
    void mouseReleasePointMove(const QPointF &pt, Qt::MouseButton button);

    IDrawContext* context;
    CItemRefMap * item;

    mode_e mode = eModeNone;

    QPointF ptFocus1 = NOPOINTF;
    QPointF ptFocus2 = NOPOINTF;
    bool movePoint   = false;

    QString gcpFilename;
};

#endif //COVERLAYREFMAP_H


