/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de
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

#include <QtCore>
#include "CRouterBRouterTilesSelect.h"
#include "CRouterBRouterTilesSelectArea.h"
#include "CRouterBRouterTilesSelectLayout.h"
#include "CMainWindow.h"

CRouterBRouterTilesSelect::CRouterBRouterTilesSelect(QWidget *parent)
    : QWidget(parent)
{
    QTemporaryFile temp;
    temp.open();
    temp.close();

    QSettings view(temp.fileName(), QSettings::IniFormat);
    view.clear();

    CCanvas * source = CMainWindow::self().getVisibleCanvas();
    if(source)
    {
        source->saveConfig(view);
    }
    //view.setValue("grid/color",QColor(Qt::red).name());
    view.setValue("map/zoomIndex",16);

    CCanvas * canvas = new CCanvas(this,"BRouterTileDownload");
    // clone canvas by a temporary configuration file

    canvas->loadConfig(view);

    selectArea = new CRouterBRouterTilesSelectArea(this,canvas);

    QLayout * layout = new CRouterBRouterTilesSelectLayout(this);
    layout->addWidget(canvas);
    layout->addWidget(selectArea);
    canvas->lower();
    canvas->show();
    selectArea->show();
}

CRouterBRouterTilesSelect::~CRouterBRouterTilesSelect()
{
}

void CRouterBRouterTilesSelect::setExistingTiles(const QVector<QPoint> & tiles)
{
    selectArea->existingTiles = tiles;
}

void CRouterBRouterTilesSelect::setOutdatedTiles(const QVector<QPoint> & tiles)
{
    selectArea->outdatedTiles = tiles;
}

void CRouterBRouterTilesSelect::setSelectedTiles(const QVector<QPoint> & tiles)
{
    selectArea->selectedTiles = tiles;
}

void CRouterBRouterTilesSelect::setOutstandingTiles(const QVector<QPoint> & tiles)
{
    selectArea->outstandingTiles = tiles;
}

void CRouterBRouterTilesSelect::setInvalidTiles(const QVector<QPoint> & tiles)
{
    selectArea->invalidTiles = tiles;
}

void CRouterBRouterTilesSelect::selectedTilesChangedEvent()
{
    emit selectedTilesChanged(selectArea->selectedTiles);
}
