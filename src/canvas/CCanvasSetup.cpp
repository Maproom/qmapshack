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

#include "canvas/CCanvasSetup.h"
#include "canvas/CCanvas.h"
#include "grid/CProjWizard.h"

CCanvasSetup::CCanvasSetup(CCanvas * canvas)
    : QDialog(canvas)
    , canvas(canvas)
{
    setupUi(this);

    lineProjection->setText(canvas->getProjection());
    lineProjection->setCursorPosition(0);

    connect(toolWizard, SIGNAL(clicked()), this, SLOT(slotProjWizard()));
}

CCanvasSetup::~CCanvasSetup()
{

}

void CCanvasSetup::slotProjWizard()
{
    CProjWizard dlg(*lineProjection);
    dlg.exec();
}


void CCanvasSetup::accept()
{
    if(!CProjWizard::validProjStr(lineProjection->text()))
    {
        return;
    }
    canvas->setProjection(lineProjection->text());
    canvas->slotTriggerCompleteUpdate();
    QDialog::accept();
}
