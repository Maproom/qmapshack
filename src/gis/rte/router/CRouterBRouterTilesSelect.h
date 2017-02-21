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

#ifndef CROUTERBROUTERTILESSELECT_H
#define CROUTERBROUTERTILESSELECT_H

#include "canvas/CCanvas.h"
#include <QtCore>
#include <QtWidgets>

class CRouterBRouterTilesSelect : public QWidget
{
    Q_OBJECT
public:
    CRouterBRouterTilesSelect(QWidget * parent);
    virtual ~CRouterBRouterTilesSelect();
};

class CRouterBRouterTilesSelectArea : public QWidget
{
    Q_OBJECT
public:
    CRouterBRouterTilesSelectArea(QWidget * parent, CCanvas * canvas);
    virtual ~CRouterBRouterTilesSelectArea();

    void paintEvent(QPaintEvent *event) override;

protected:
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private slots:

private:

    void drawTileMark(QPoint tile, Qt::GlobalColor color);
    void drawNewTiles();
    void drawExistingTiles();

    CCanvas * canvas;
    QPoint pPressed;
    QVector<QPoint> newTiles;
    QVector<QPoint> existingTiles;
};

class CRouterBRouterTilesSelectLayout : public QLayout
{
public:
    CRouterBRouterTilesSelectLayout(QWidget * parent);
    virtual ~CRouterBRouterTilesSelectLayout();

    void addItem(QLayoutItem *) override;
    QSize sizeHint() const override;
    void setGeometry(const QRect & r) override;
    QLayoutItem * itemAt(int index) const override;
    QLayoutItem * takeAt(int index) override;
    int count() const override;
private:
    QList<QLayoutItem *> items;
};

#endif //CROUTERBROUTERTILESSELECT_H

