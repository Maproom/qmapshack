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

#include <QtPrintSupport>
#include <QtWidgets>

CPrintDialog::CPrintDialog(const QRectF& area, CCanvas *canvas)
    : QDialog(&CMainWindow::self())
    , canvas(canvas)
    , area(area)
{
    setupUi(this);

    printer.setOrientation(QPrinter::Landscape);

    // clone canvas by a temporary configuration file
    QTemporaryFile temp;
    temp.open();
    temp.close();

    QSettings view(temp.fileName(), QSettings::IniFormat);
    view.clear();

    canvas->saveConfig(view);

    preview = new CCanvas(this, "preview");
    preview->loadConfig(view);
    preview->show();

    // add preview canvas to dialog
    QLayout * layout = new QVBoxLayout(frameCanvas);
    layout->addWidget(preview);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);

    connect(preview, SIGNAL(sigZoom()), this, SLOT(slotZoom()));    
    connect(pushProperties, SIGNAL(pressed()), this, SLOT(slotProperties()));
    connect(pushPrint, SIGNAL(pressed()), this, SLOT(slotPrint()));

    // update zoom info and print metrics
    QTimer::singleShot(100, this, SLOT(slotZoom()));

}

CPrintDialog::~CPrintDialog()
{
}

void CPrintDialog::resizeEvent(QResizeEvent * e)
{
    QDialog::resizeEvent(e);
    slotZoom();
}

void CPrintDialog::updateMetrics()
{
    // get pixel coordinates
    QPointF pt1 = area.topLeft();
    QPointF pt2 = area.bottomRight();
    preview->convertRad2Px(pt1);
    preview->convertRad2Px(pt2);

    // the map area in [pixel]
    QRectF rectArea(pt1, pt2);
    // the printer page in [pixel]
    QRectF rectPage     = printer.pageRect(QPrinter::DevicePixel);
    // the label to show the page matrix in [pixel]
    QRectF rectLabel    = labelPages->rect();

    // calculate number of pages
    qreal xPages = rectArea.width()  / rectPage.width();
    qreal yPages = rectArea.height() / rectPage.height();

    qreal wPages = rectPage.width()  * qCeil(xPages);
    qreal hPages = rectPage.height() * qCeil(yPages);

    // derive scale for map area to page preview either from x axis or
    // y axis. What ever fits better
    qreal scale = rectLabel.width() / wPages;
    if(hPages * scale > rectLabel.height())
    {
        scale = rectLabel.height() / hPages;
    }

    // create the preview image
    QPixmap img(wPages * scale, hPages * scale);
    img.fill(Qt::lightGray);

    // scaled page preview width and height
    qreal w = rectPage.width() * scale;
    qreal h = rectPage.height() * scale;

    // the page rectangle
    QRectF rectTile(1,1, w-2, h-2);

    // paint page matrix
    QPainter p(&img);
    p.setPen(Qt::black);
    p.setBrush(Qt::white);

    for(int y  = 0; y < qCeil(yPages); y++)
    {
        for(int x = 0; x < qCeil(xPages); x++)
        {
            rectTile.moveCenter(QPointF(w/2 + x * w, h/2 + y * h));
            p.drawRect(rectTile);
        }
    }
    p.setBrush(Qt::NoBrush);
    p.drawRect(img.rect().adjusted(0,0,-1,-1));

    p.setPen(QPen(Qt::darkGreen,2));
    p.setBrush(Qt::BDiagPattern);
    p.drawRect(0,0, rectArea.width() * scale, rectArea.height() * scale);

    labelPages->setPixmap(img);

    // update page matrix status
    labelPagesText->setText(tr("Pages: %1 x %2").arg(xPages,0,'f',1).arg(yPages,0,'f',1));
}

void CPrintDialog::slotZoom()
{
    // update map resolution information
    QPointF pt1 = area.topLeft();
    QPointF pt2 = area.bottomRight();

    qreal mWidth    = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt1.y());
    qreal mHeight   = GPS_Math_Distance(pt1.x(), pt1.y(), pt1.x(), pt2.y());

    preview->convertRad2Px(pt1);
    preview->convertRad2Px(pt2);

    qint32 pxWidth  = qRound(pt2.x() - pt1.x());
    qint32 pxHeight = qRound(pt2.y() - pt1.y());

    labelMapInfo->setText(tr("zoom with mouse wheel on map below to change resolution:\n\n%1x%2 pixel\nx: %3 m/px\ny: %4 m/px").arg(pxWidth).arg(pxHeight).arg(mWidth/pxWidth,0,'f',1).arg(mHeight/pxHeight,0,'f',1));

    // and the printer page metrics
    updateMetrics();
}

void CPrintDialog::slotProperties()
{
    QPrintDialog dlg(&printer, this);
    dlg.setWindowTitle(tr("Printer Properties..."));
    dlg.exec();

    updateMetrics();
}

void CPrintDialog::slotPrint()
{
    // get pixel coordinates
    QPointF pt1 = area.topLeft();
    QPointF pt2 = area.bottomRight();
    preview->convertRad2Px(pt1);
    preview->convertRad2Px(pt2);

    // the map area in [pixel]
    QRectF rectArea(pt1, pt2);
    // the printer page in [pixel]
    QRectF rectPage     = printer.pageRect(QPrinter::DevicePixel);

    // calculate number of pages
    qreal xPages = rectArea.width()  / rectPage.width();
    qreal yPages = rectArea.height() / rectPage.height();

    qreal wPage = rectPage.width();
    qreal hPage = rectPage.height();

    QPainter p;
    p.begin(&printer);    
    p.setClipRect(QRectF(QPointF(0,0),rectPage.size()));

    USE_ANTI_ALIASING(p,true);

    qreal yoff = pt1.y();
    for(int y = 0; y < qCeil(yPages); y++)
    {
        qint32 xoff = pt1.x();
        for(int x = 0; x < qCeil(xPages); x++)
        {            
            printer.newPage();

            QPointF p11 = QPointF(xoff, yoff);
            QPointF p22 = QPointF(xoff + wPage, yoff + hPage);

            qDebug() << p11 << p22;

            preview->convertPx2Rad(p11);
            preview->convertPx2Rad(p22);

            preview->print(p, QRectF(p11,p22));

            xoff += wPage;
        }

        yoff += hPage;
    }
    p.end();

    QDialog::accept();
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
