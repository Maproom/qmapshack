/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#ifndef CROUTERBROUTERTILESSELECTAREA_H
#define CROUTERBROUTERTILESSELECTAREA_H

#include "canvas/CCanvas.h"
#include "CRouterBRouterTilesSelect.h"
#include <QtCore>
#include <QtWidgets>
#include <QPolygonF>

class CRouterBRouterTilesSelectArea : public QWidget
{
    Q_OBJECT
public:
    CRouterBRouterTilesSelectArea(CRouterBRouterTilesSelect * parent, CCanvas * canvas);
    virtual ~CRouterBRouterTilesSelectArea();

    void paintEvent(QPaintEvent *event) override;

protected:
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private slots:

private:
    void drawSelectedTiles();
    void drawExistingTiles();
    void drawOutdatedTiles();
    QPoint tileUnderMouse(const QPointF & mousePos);
    QPolygonF tilePolygon(const QPoint & tile);

    CCanvas * canvas;
    CRouterBRouterTilesSelect * select;
    QPoint mousePos;
    QPoint startPos;
    QVector<QPoint> existingTiles;
    QVector<QPoint> outdatedTiles;
    QVector<QPoint> selectedTiles;

    QPen existingTilesPen;
    QPen outdatedTilesPen;
    QPen selectedTilesPen;
    QBrush existingTilesBrush;
    QBrush outdatedTilesBrush;
    QBrush selectedTilesBrush;

    friend class CRouterBRouterTilesSelect;
};

#endif //CROUTERBROUTERTILESSELECTAREA_H

