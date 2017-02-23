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

class CRouterBRouterTilesSelectArea;

class CRouterBRouterTilesSelect : public QWidget
{
    Q_OBJECT
public:
    CRouterBRouterTilesSelect(QWidget * parent);
    virtual ~CRouterBRouterTilesSelect();

    void setExistingTiles(const QVector<QPoint> & tiles);
    void setOutdatedTiles(const QVector<QPoint> & tiles);
    void setSelectedTiles(const QVector<QPoint> & tiles);
    void setOutstandingTiles(const QVector<QPoint> & tiles);
    void setInvalidTiles(const QVector<QPoint> & tiles);

signals:
    void selectedTilesChanged(const QVector<QPoint> & tiles);

private:
    QVBoxLayout * outerLayout;

    QWidget * select;

    QFormLayout * progressLayout;
    QWidget * progress;

    QToolButton * toolSelectOld;
    QToolButton * toolClearSelection;
    QToolButton * toolDeleteSelection;
    QToolButton * toolDownload;

    CRouterBRouterTilesSelectArea * selectArea;
    void selectedTilesChangedEvent();

    friend class CRouterBRouterTilesSelectArea;
};

#endif //CROUTERBROUTERTILESSELECT_H

