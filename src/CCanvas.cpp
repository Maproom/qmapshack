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
#include "CCanvas.h"
#include "map/CMap.h"

#include <QtGui>



CCanvas::CCanvas(QWidget *parent)
    : QWidget(parent)
{
    int count = 1;
    while(1)
    {
        QString name = tr("Canvas %1").arg(count);
        if(CMainWindow::self().findChild<CCanvas*>(name) == 0)
        {
            setObjectName(name);
            return;
        }
        count++;
    }
}

CCanvas::~CCanvas()
{

}

