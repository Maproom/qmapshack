/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "print/CPrintDialog.h"
#include "canvas/CCanvas.h"
#include "CMainWindow.h"

#include <QtWidgets>

CPrintDialog::CPrintDialog(const QRectF& area, CCanvas *canvas)
    : QDialog(&CMainWindow::self())
    , canvas(canvas)
    , area(area)
{
    setupUi(this);

    QTemporaryFile temp;
    temp.open();
    temp.close();

    QSettings view(temp.fileName(), QSettings::IniFormat);
    view.clear();

    canvas->saveConfig(view);

    preview = new CCanvas(this, "preview");
    preview->loadConfig(view);
    preview->show();

    QLayout * layout = new QVBoxLayout(frameCanvas);
    layout->addWidget(preview);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

}

CPrintDialog::~CPrintDialog()
{

}

void CPrintDialog::slotZoom()
{

}
