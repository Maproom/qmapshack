/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "mouse/CMousePrint.h"

#include <QtWidgets>

#include "gis/CGisDraw.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/CScrOptPrint.h"
#include "print/CPrintDialog.h"

CMousePrint::CMousePrint(CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse) : IMouseSelect(gis, canvas, mouse) {
  cursor = QCursor(QPixmap("://cursors/cursorSave.png"), 0, 0);

  canvas->reportStatus("IMouseSelect", tr("<b>Save(Print) Map</b><br/>Hold down the primary mouse button "
                                          "and select a rectangular area on the map. Adjust the selection "
                                          "by using the markers on the corners. The buttons to print/save "
                                          "the area are beside the selection. If they are covered by other "
                                          "windows you have to move the map. Abort by using the secondary "
                                          "mouse button."));

  CScrOptPrint* scrOptPrint;
  scrOpt = scrOptPrint = new CScrOptPrint(this);

  connect(scrOptPrint->toolSave, &QToolButton::clicked, this, &CMousePrint::slotSave);
  connect(scrOptPrint->toolPrint, &QToolButton::clicked, this, &CMousePrint::slotPrint);
}

CMousePrint::~CMousePrint() {}

void CMousePrint::slotSave() {
  CPrintDialog dlg(CPrintDialog::eTypeImage, rectSelection, canvas);
  dlg.exec();
  canvas->resetMouse();
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);

  canvas->resetMouse();
}

void CMousePrint::slotPrint() {
  CPrintDialog dlg(CPrintDialog::eTypePrint, rectSelection, canvas);
  dlg.exec();
  canvas->resetMouse();
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawAll);

  canvas->resetMouse();
}
