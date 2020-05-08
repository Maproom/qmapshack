/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "canvas/CCanvasSelect.h"
#include "CMainWindow.h"

CCanvasSelect::CCanvasSelect(CCanvas *&canvas, QWidget *parent)
    : QDialog(parent)
    , canvas(canvas)
{
    setupUi(this);

    const QList<CCanvas*>& allCanvas = CMainWindow::self().getCanvas();
    for(CCanvas * c : allCanvas)
    {
        comboCanvas->addItem(c->objectName(), QVariant::fromValue<CCanvas*>(c));
    }
}

int CCanvasSelect::exec()
{
    if(comboCanvas->count() == 0)
    {
        return QDialog::Rejected;
    }

    if(comboCanvas->count() == 1)
    {
        canvas = comboCanvas->currentData().value<CCanvas*>();
        return QDialog::Accepted;
    }

    return QDialog::exec();
}

void CCanvasSelect::accept()
{
    canvas = comboCanvas->currentData().value<CCanvas*>();
    QDialog::accept();
}
