/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CCanvas.h"
#include "canvas/CCanvasSetup.h"
#include "grid/CProjWizard.h"

CCanvasSetup::CCanvasSetup(CCanvas* canvas)
    : QDialog(canvas)
    , canvas(canvas)
{
    setupUi(this);

    lineProjection->setText(canvas->getProjection());
    lineProjection->setCursorPosition(0);

    switch(canvas->getScalesType())
    {
    case CCanvas::eScalesDefault:
        radioScalesDefault->setChecked(true);
        break;

    case CCanvas::eScalesSquare:
        radioScalesSquare->setChecked(true);
        break;
    }

    connect(toolWizard, &QToolButton::clicked, this, &CCanvasSetup::slotProjWizard);
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
    if(!CProjWizard::validProjStr(lineProjection->text(), false))
    {
        return;
    }
    canvas->setProjection(lineProjection->text());
    if(radioScalesDefault->isChecked())
    {
        canvas->setScales(CCanvas::eScalesDefault);
    }
    else if(radioScalesSquare->isChecked())
    {
        canvas->setScales(CCanvas::eScalesSquare);
    }
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);
    QDialog::accept();
}
