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
#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CSettings.h"
#include "plot/CPlotProfile.h"
#include "print/CScreenshotDialog.h"

#include <QtPrintSupport>
#include <QtWidgets>

CScreenshotDialog::CScreenshotDialog(CCanvas& canvas, QWidget* parent)
    : QDialog(parent)
    , canvas(canvas)
{
    setupUi(this);

    labelHelp->setText(tr("<b>Screenshot</b><br/>"
                          "You can save or print a screenshot of your current map view. If the "
                          "track profile is shown in it's own window it is appended to the "
                          "screenshot. Check the pre-view below. "
                          ));

    const QPixmap& pixmap = getScreenshot(getTrackForProfile());

    labelPreview->setPixmap(pixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    connect(pushSave, &QPushButton::clicked, this, &CScreenshotDialog::slotSave);
    connect(pushPrint, &QPushButton::clicked, this, &CScreenshotDialog::slotPrint);

    adjustSize();
}

void CScreenshotDialog::slotSave()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastScreenshotPath", "./").toString();
    QString filterPNG = "PNG Image (*.png)";
    QString filterJPG = "JPEG Image (*.jpg)";
    QString filter = filterPNG;
    QString filename = QFileDialog::getSaveFileName(this, tr("Save screenshot..."), path, filterPNG + ";; " + filterJPG, &filter);
    if(filename.isEmpty())
    {
        return;
    }

    QString expectedSuffix;
    if(filter == filterPNG)
    {
        expectedSuffix = "png";
    }
    else if(filter == filterJPG)
    {
        expectedSuffix = "jpg";
    }

    QFileInfo fi(filename);
    if(fi.suffix().toLower() != expectedSuffix)
    {
        filename += "." + expectedSuffix;
    }

    const QPixmap& pixmap = getScreenshot(getTrackForProfile());
    pixmap.save(filename);

    cfg.setValue("Paths/lastScreenshotPath", fi.absolutePath());
    QDialog::accept();
}

CGisItemTrk* CScreenshotDialog::getTrackForProfile()
{
    const CMainWindow& w = CMainWindow::self();
    if(!(w.isShowTrackProfile() && w.profileIsWindow()))
    {
        return nullptr;
    }

    const IGisItem::key_t& key = CGisItemTrk::getKeyUserFocus();
    return dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
}

void CScreenshotDialog::slotPrint()
{
    const QPixmap& pixmap = getScreenshot(nullptr);
    const QSize& sizePixmap = pixmap.size();

    QPrinter printer;
    printer.setOrientation(sizePixmap.width() > sizePixmap.height() ? QPrinter::Landscape : QPrinter::Portrait);
    printer.setPaperSize(QPrinter::A4);
    printer.setColorMode(QPrinter::Color);
    printer.setResolution(1200);

    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Screenshot..."));
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const QRectF& r = printer.pageRect(QPrinter::DevicePixel);
    const QPixmap& canvasScaled = pixmap.scaled(r.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter p(&printer);
    p.drawPixmap(0, 0, canvasScaled);

    CGisItemTrk* trk = getTrackForProfile();
    if(trk != nullptr)
    {
        CPlotProfile plot(trk, trk->limitsGraph1, IPlot::eModeNormal, CMainWindow::self().getBestWidgetForParent());
        plot.resize(pixmap.width(), heightProfile);
        QImage image(plot.size(), QImage::Format_ARGB32);
        plot.save(image, nullptr);

        const QImage& profileScaled = image.scaled(r.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        if(r.height() > (canvasScaled.height() + profileScaled.height()))
        {
            p.translate(0, canvasScaled.height());
        }
        else
        {
            printer.newPage();
        }

        p.drawImage(0, 0, profileScaled);
    }


    QDialog::accept();
}


QPixmap CScreenshotDialog::getScreenshot(CGisItemTrk* trk)
{
    QSize s = canvas.size();

    if(trk != nullptr)
    {
        s.setHeight(s.height() + heightProfile);
    }

    QPixmap pixmap(s);
    QPainter p(&pixmap);

    canvas.render(&p);

    if(trk != nullptr)
    {
        CPlotProfile plot(trk, trk->limitsGraph1, IPlot::eModeNormal, CMainWindow::self().getBestWidgetForParent());
        plot.resize(pixmap.width(), heightProfile);

        QImage image(plot.size(), QImage::Format_ARGB32);
        plot.save(image, nullptr);
        p.drawImage(0, canvas.height(), image);
    }

    return pixmap;
}
