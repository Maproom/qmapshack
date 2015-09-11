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

#include "CMainWindow.h"
#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWidget.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "print/CPrintDialog.h"

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

    connect(preview, SIGNAL(sigZoom()), this, SLOT(slotZoom()));
    slotZoom();

    connect(pushButton, SIGNAL(pressed()), this, SLOT(slot()));
}

CPrintDialog::~CPrintDialog()
{
}

void CPrintDialog::slotZoom()
{
    QPointF pt1 = area.topLeft();
    QPointF pt2 = area.bottomRight();

    qreal mWidth    = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt1.y());
    qreal mHeight   = GPS_Math_Distance(pt1.x(), pt1.y(), pt1.x(), pt2.y());

    preview->convertRad2Px(pt1);
    preview->convertRad2Px(pt2);

    qint32 pxWidth  = qRound(pt2.x() - pt1.x());
    qint32 pxHeight = qRound(pt2.y() - pt1.y());

    labelMapInfo->setText(tr("zoom with mouse wheel on map below to change resolution:\n\n%1x%2 pixel\nx: %3 m/px\ny: %4 m/px").arg(pxWidth).arg(pxHeight).arg(mWidth/pxWidth,0,'f',1).arg(mHeight/pxHeight,0,'f',1));
}

void CPrintDialog::slot()
{
    QPointF pt1 = area.topLeft();
    QPointF pt2 = area.bottomRight();

    preview->convertRad2Px(pt1);
    preview->convertRad2Px(pt2);

    QImage img(pt2.x() - pt1.x(), pt2.y() - pt1.y(), QImage::Format_ARGB32);

    QPainter p(&img);
    USE_ANTI_ALIASING(p,true);

    preview->print(p, area);

    img.save("test.png", "PNG");

    QDialog::accept();
}
