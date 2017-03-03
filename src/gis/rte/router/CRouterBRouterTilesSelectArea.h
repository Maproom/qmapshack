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

#include <QPen>
#include <QWidget>
#include <QMouseEvent>

class CCanvas;
class CRouterBRouterTilesSelect;

class CRouterBRouterTilesSelectArea : public QWidget
{
    Q_OBJECT
public:
    CRouterBRouterTilesSelectArea(QWidget * parent, CRouterBRouterTilesSelect * select, CCanvas * canvas);
    virtual ~CRouterBRouterTilesSelectArea();

    void paintEvent(QPaintEvent *event) override;

public slots:
    void updateTiles();

signals:
    void sigTileClicked(const QPoint & tile);

protected:
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    bool event(QEvent * event) override;

private slots:

private:
    void drawSelectedTiles();
    void drawCurrentTiles();
    void drawOutdatedTiles();
    void drawOutstandingTiles();
    void drawInvalidTiles();
    QPoint tileUnderMouse(const QPointF & mousePos);
    QPolygonF tilePolygon(const QPoint & tile);

    CCanvas * canvas;
    CRouterBRouterTilesSelect * select;
    QPoint mousePos;
    QPoint startPos;
    Qt::MouseButtons button;

    QPen currentTilesPen;
    QPen outdatedTilesPen;
    QPen selectedTilesPen;
    QPen outstandingTilesPen;
    QPen invalidTilesPen;
    QBrush currentTilesBrush;
    QBrush outdatedTilesBrush;
    QBrush selectedTilesBrush;
    QBrush outstandingTilesBrush;
    QBrush invalidTilesBrush;
};

#endif //CROUTERBROUTERTILESSELECTAREA_H

