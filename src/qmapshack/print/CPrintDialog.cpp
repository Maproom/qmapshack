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

#include "print/CPrintDialog.h"

#include <QtPrintSupport>
#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/GeoMath.h"
#include "helpers/CDraw.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"

CPrintDialog::CPrintDialog(type_e type, const QRectF& area, CCanvas* source)
    : QDialog(&CMainWindow::self()), type(type), rectSelArea(area) {
  setupUi(this);

  // clone canvas by a temporary configuration file
  QTemporaryFile temp;
  temp.open();
  temp.close();

  QSettings view(temp.fileName(), QSettings::IniFormat);
  view.clear();

  source->saveConfig(view);

  canvas = new CCanvas(this, "print");
  canvas->loadConfig(view);
  canvas->show();
  canvas->allowShowTrackOverlays(false);

  // add canvas canvas to dialog
  QLayout* layout = new QVBoxLayout(frameCanvas);
  layout->addWidget(canvas);
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  connect(canvas, &CCanvas::sigZoom, this, &CPrintDialog::slotUpdateMetrics);
  connect(canvas, &CCanvas::sigMove, this, &CPrintDialog::slotUpdateMetrics);
  connect(pushPrint, &QPushButton::pressed, this, &CPrintDialog::slotPrint);
  connect(pushSave, &QPushButton::pressed, this, &CPrintDialog::slotSave);
  connect(checkScaleOnAll, &QCheckBox::toggled, this, &CPrintDialog::slotScaleOnAllChanged);

  if (type == eTypePrint) {
    setWindowTitle(tr("Print Map..."));
    frameImage->hide();
    // update zoom info and print metrics
    QTimer::singleShot(100, this, &CPrintDialog::slotGetPrinter);
  } else {
    setWindowTitle(tr("Save Map as Image..."));
    framePrint->hide();
  }
}

CPrintDialog::~CPrintDialog() {}

void CPrintDialog::resizeEvent(QResizeEvent* e) {
  QDialog::resizeEvent(e);
  slotUpdateMetrics();
}

void CPrintDialog::slotGetPrinter() {
  printer.setPageOrientation(QPageLayout::Landscape);

  QPrintDialog dlg(&printer, this);
  dlg.setWindowTitle(tr("Printer Properties..."));
  dlg.exec();

  slotUpdateMetrics();
}

void CPrintDialog::slotUpdateMetrics() {
  // get corner points of selected area
  QPointF pt1 = rectSelArea.topLeft();
  QPointF pt2 = rectSelArea.bottomRight();

  // calculate real meter dimensions from corner points
  qreal mWidth = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt1.y());
  qreal mHeight = GPS_Math_Distance(pt1.x(), pt1.y(), pt1.x(), pt2.y());

  // get pixel coordinates of corner points
  canvas->convertRad2Px(pt1);
  canvas->convertRad2Px(pt2);

  // the map area in [pixel]
  rectSelAreaPixel = QRectF(pt1, pt2);
  // the printer page in [pixel]
  rectPrinterPage = printer.pageRect(QPrinter::DevicePixel);
  // the label to show the page matrix in [pixel]
  QRectF rectLabel = labelPages->rect();

  // calculate number of pages
  xPages = rectSelAreaPixel.width() / rectPrinterPage.width();
  yPages = rectSelAreaPixel.height() / rectPrinterPage.height();

  // width and hight of page matrix for full pages
  qreal wPages = rectPrinterPage.width() * qCeil(xPages);
  qreal hPages = rectPrinterPage.height() * qCeil(yPages);

  // derive scale for map area to page canvas either from x axis or
  // y axis. What ever fits better
  qreal scale = rectLabel.width() / wPages;
  if (hPages * scale > rectLabel.height()) {
    scale = rectLabel.height() / hPages;
  }

  // create the canvas image
  QPixmap img(wPages * scale, hPages * scale);
  img.fill(Qt::lightGray);

  // scaled page canvas width and height
  qreal w = rectPrinterPage.width() * scale;
  qreal h = rectPrinterPage.height() * scale;

  // the page rectangle
  QRectF rectTile(1, 1, w - 2, h - 2);

  // paint page matrix
  QPainter p(&img);
  p.setPen(Qt::black);
  p.setBrush(Qt::white);

  for (int y = 0; y < qCeil(yPages); y++) {
    for (int x = 0; x < qCeil(xPages); x++) {
      rectTile.moveCenter(QPointF(w / 2 + x * w, h / 2 + y * h));
      p.drawRect(rectTile);
    }
  }
  p.setBrush(Qt::NoBrush);
  p.drawRect(img.rect().adjusted(0, 0, -1, -1));

  p.setPen(QPen(Qt::darkGreen, 2));
  p.setBrush(Qt::BDiagPattern);
  p.drawRect(0, 0, rectSelAreaPixel.width() * scale, rectSelAreaPixel.height() * scale);

  qreal xPageSizeMm = printer.pageRect(QPrinter::Millimeter).width();

  labelPages->setPixmap(img);
  labelPagesText->setText(tr("Pages: %1 x %2").arg(xPages, 0, 'f', 1).arg(yPages, 0, 'f', 1));
  QString labelMapInfoText =
      tr("Zoom with mouse wheel on map below to change resolution:\n\n%1x%2 pixel\nx: %3 m/px\ny: %4 m/px")
          .arg(rectSelAreaPixel.width())
          .arg(rectSelAreaPixel.height())
          .arg(mWidth / rectSelAreaPixel.width(), 0, 'f', 1)
          .arg(mHeight / rectSelAreaPixel.height(), 0, 'f', 1);
  labelMapInfoText +=
      tr("\n This equals to a scale of approx. 1:") + QString::number((int)(mWidth * 1000 / (xPages * xPageSizeMm)));
  labelMapInfo->setText(labelMapInfoText);
}

void CPrintDialog::slotPrint() {
  qreal wPage = rectPrinterPage.width();
  qreal hPage = rectPrinterPage.height();

  QPointF p11 = rectSelAreaPixel.topLeft();
  QPointF p22 = p11 + QPointF(wPage, hPage);

  canvas->convertPx2Rad(p11);
  canvas->convertPx2Rad(p22);

  QPointF pxCenter0 = QRectF(p11, p22).center();
  canvas->convertRad2Px(pxCenter0);

  QList<QPointF> centers;

  qreal yoff = 0;
  for (int y = 0; y < qCeil(yPages); y++) {
    qreal xoff = 0;
    for (int x = 0; x < qCeil(xPages); x++) {
      QPointF center = pxCenter0 + QPointF(xoff, yoff);
      canvas->convertPx2Rad(center);
      centers << center;

      xoff += wPage;
    }

    yoff += hPage;
  }

  QRectF rectPage(0, 0, wPage, hPage);

  bool first = true;
  QPainter p;
  p.begin(&printer);
  p.setClipRect(rectPage);
  USE_ANTI_ALIASING(p, true);

  int N = centers.size();
  int n = 0;
  PROGRESS_SETUP(tr("Printing pages."), 0, N, this);

  for (const QPointF& pt : std::as_const(centers)) {
    if (!first) {
      printer.newPage();
    } else {
      first = false;
    }
    if (printScaleOnAllPages || pt == centers.last()) {
      canvas->print(p, rectPage, pt, true);
    } else {
      canvas->print(p, rectPage, pt, false);
    }
    PROGRESS(++n, break);
  }

  p.end();

  QDialog::accept();
}

void CPrintDialog::slotSave() {
  QPointF pt1 = rectSelArea.topLeft();
  QPointF pt2 = rectSelArea.bottomRight();

  canvas->convertRad2Px(pt1);
  canvas->convertRad2Px(pt2);

  QRectF rect(pt1, pt2);
  QImage img(rect.size().toSize(), QImage::Format_ARGB32);

  QPainter p(&img);
  USE_ANTI_ALIASING(p, true);

  canvas->print(p, rect, rectSelArea.center());

  SETTINGS;
  QString path = cfg.value("Paths/lastImagePath", "./").toString();

  QString filterPNG = "PNG Image (*.png)";
  QString filterJPG = "JPEG Image (*.jpg)";
  QString filter = filterPNG;
  QString filename =
      QFileDialog::getSaveFileName(this, tr("Save map..."), path, filterPNG + ";; " + filterJPG, &filter);
  if (filename.isEmpty()) {
    return;
  }

  QString expectedSuffix;
  if (filter == filterPNG) {
    expectedSuffix = "png";
  } else if (filter == filterJPG) {
    expectedSuffix = "jpg";
  }

  QFileInfo fi(filename);
  if (fi.suffix().toLower() != expectedSuffix) {
    filename += "." + expectedSuffix;
  }

  img.save(filename);

  cfg.setValue("Paths/lastImagePath", fi.absolutePath());

  QDialog::accept();
}

void CPrintDialog::slotScaleOnAllChanged(bool checked) { printScaleOnAllPages = checked; }
