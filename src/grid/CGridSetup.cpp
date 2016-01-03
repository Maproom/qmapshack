/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "CMainWindow.h"
#include "grid/CGrid.h"
#include "grid/CGridSetup.h"
#include "grid/CProjWizard.h"
#include "map/CMapDraw.h"

#include <QtWidgets>

CGridSetup::CGridSetup(CGrid *grid, CMapDraw * map)
    : QDialog(CMainWindow::getBestWidgetForParent())
    , grid(grid)
    , map(map)
{
    setupUi(this);

    lineProjection->setText(grid->projstr);
    lineProjection->setCursorPosition(0);

    QPalette palette = labelGridColor->palette();
    palette.setColor(labelGridColor->foregroundRole(), grid->color);
    labelGridColor->setPalette(palette);

    connect(toolRestoreDefault, &QToolButton::clicked, this, &CGridSetup::slotRestoreDefault);
    connect(toolFromMap,        &QToolButton::clicked, this, &CGridSetup::slotProjFromMap);
    connect(toolProjWizzard,    &QToolButton::clicked, this, &CGridSetup::slotProjWizard);
    connect(toolGridColor,      &QToolButton::clicked, this, &CGridSetup::slotSelectGridColor);
}

CGridSetup::~CGridSetup()
{
}

void CGridSetup::accept()
{
    if (CProjWizard::validProjStr(lineProjection->text()))
    {
        QPalette palette = labelGridColor->palette();
        grid->setProjAndColor(lineProjection->text(), palette.color(labelGridColor->foregroundRole()));

        QDialog::accept();
    }
}

void CGridSetup::slotProjWizard()
{
    CProjWizard dlg(*lineProjection);
    dlg.exec();
}

void CGridSetup::slotSelectGridColor()
{
    QPalette palette = labelGridColor->palette();
    QColor color = palette.color(labelGridColor->foregroundRole());

    color = QColorDialog::getColor(color, this);

    if(color.isValid())
    {
        palette.setColor(labelGridColor->foregroundRole(), color);
        labelGridColor->setPalette(palette);
    }
}

void CGridSetup::slotRestoreDefault()
{
    lineProjection->setText("+proj=longlat +datum=WGS84 +no_defs");
    lineProjection->setCursorPosition(0);
}

void CGridSetup::slotProjFromMap()
{
    lineProjection->setText(map->getProjection());
    lineProjection->setCursorPosition(0);
}

