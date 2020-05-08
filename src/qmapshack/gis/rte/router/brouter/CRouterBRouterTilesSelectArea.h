/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include <QMouseEvent>
#include <QPen>
#include <QWidget>

class CCanvas;
class CRouterBRouterTilesSelect;

class CRouterBRouterTilesSelectArea : public QWidget
{
    Q_OBJECT
public:
    CRouterBRouterTilesSelectArea(QWidget * parent, CCanvas * canvas);
    virtual ~CRouterBRouterTilesSelectArea();

    void paintEvent(QPaintEvent *event) override;

    void setGridTiles(const QVector<QPoint> & tiles) { gridTiles = tiles; }
    void setInvalidTiles(const QVector<QPoint> & tiles) { invalidTiles = tiles; }
    void setOutdatedTiles(const QVector<QPoint> & tiles) { outdatedTiles = tiles; }
    void setCurrentTiles(const QVector<QPoint> & tiles) { currentTiles = tiles; }
    void setOutstandingTiles(const QVector<QPoint> & tiles) { outstandingTiles = tiles; }
    void setSelectedTiles(const QVector<QPoint> & tiles) { selectedTiles = tiles; }

    void setTileToolTip(const QString &toolTip) { tileToolTip = toolTip; }

    static const QPen gridPen;
    static const QPen currentTilesPen;
    static const QPen outdatedTilesPen;
    static const QPen selectedTilesPen;
    static const QPen outstandingTilesPen;
    static const QPen invalidTilesPen;
    static const QBrush currentTilesBrush;
    static const QBrush outdatedTilesBrush;
    static const QBrush selectedTilesBrush;
    static const QBrush outstandingTilesBrush;
    static const QBrush invalidTilesBrush;

signals:
    void sigTileClicked(const QPoint & tile);
    void sigTileToolTipChanged(const QPoint & tile);

protected:
    void mouseMoveEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    bool event(QEvent * event) override;

private slots:

private:
    void drawGrid();
    void drawTiles(const QPen &pen, const QBrush &brush, const QVector<QPoint> & tiles);
    QPoint tileUnderMouse(const QPointF & mousePos) const;
    QPolygonF gridPolygon(const QPoint & tile) const;
    QPolygonF tilePolygon(const QPoint & tile) const;

    CCanvas * canvas;
    QPoint mousePos;
    QPoint startPos;
    Qt::MouseButtons button;
    QPoint currentTile;
    QString tileToolTip;

    QVector<QPoint> gridTiles;
    QVector<QPoint> invalidTiles;
    QVector<QPoint> outdatedTiles;
    QVector<QPoint> currentTiles;
    QVector<QPoint> outstandingTiles;
    QVector<QPoint> selectedTiles;
};

#endif //CROUTERBROUTERTILESSELECTAREA_H

