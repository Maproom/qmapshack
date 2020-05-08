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

#include "CMainWindow.h"
#include "tool/CToolBox.h"

CToolBox::CToolBox(QWidget *parent)
    : QToolBox(parent)
{
    connect(this, &CToolBox::currentChanged, this, &CToolBox::slotToolChanged);
}

void CToolBox::setupChanged()
{
    const int N = count();
    for(int n = 0; n < N; n++)
    {
        ITool * tool = dynamic_cast<ITool*>(widget(n));
        if(nullptr != tool)
        {
            tool->setupChanged();
        }
    }
}

void CToolBox::slotToolChanged(int idx)
{
    CMainWindow::self().getCanvas()->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
}
