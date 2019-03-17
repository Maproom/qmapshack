/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#include <QtWidgets>

CScreenshotDialog::CScreenshotDialog(CCanvas &canvas, QWidget *parent)
    : QDialog(parent)
    , canvas(canvas)
{
    setupUi(this);

    labelHelp->setText(tr("<b>Screenshot</b><br/>"
                          "You can either save or print the current view. "
                          ));

    QPixmap pixmap(canvas.size());
    QPainter p(&pixmap);
    canvas.render(&p);

    labelPreview->setPixmap(pixmap.scaled(300,200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    connect(pushSave, &QPushButton::clicked, this, &CScreenshotDialog::slotSave);
    connect(pushPrint, &QPushButton::clicked, this, &CScreenshotDialog::slotPrint);
}

void CScreenshotDialog::slotSave()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastScreenshotPath", "./").toString();
    QString filterPNG = "PNG Image (*.png)";
    QString filterJPG = "JPEG Image (*.jpg)";
    QString filter    = filterPNG;
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

    QSize s                     = canvas.size();
    bool renderProfile          = false;
    const IGisItem::key_t& key  = CGisItemTrk::getKeyUserFocus();
    CGisItemTrk * trk           = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    const CMainWindow& w        = CMainWindow::self();
    constexpr int heightProfile = 400;

    if(w.isShowTrackProfile() && w.profileIsWindow() && (trk != nullptr))
    {
        s.setHeight(s.height() + heightProfile);
        renderProfile = true;
    }

    QPixmap pixmap(s);
    QPainter p(&pixmap);

    canvas.render(&p);

    if(renderProfile)
    {
        CPlotProfile plot(trk, trk->limitsGraph1, IPlot::eModeNormal, w.getBestWidgetForParent());
        plot.resize(pixmap.width(), heightProfile);

        QImage image(plot.size(),QImage::Format_ARGB32);
        plot.save(image);
        p.drawImage(0,canvas.height(),image);
    }

    pixmap.save(filename);

    cfg.setValue("Paths/lastScreenshotPath", fi.absolutePath());
    QDialog::accept();
}

void CScreenshotDialog::slotPrint()
{
}

