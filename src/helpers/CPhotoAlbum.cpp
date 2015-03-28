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

#include "helpers/CPhotoAlbum.h"
#include "helpers/CPhotoViewer.h"
#include "helpers/CSettings.h"

#include <QtWidgets>

CPhotoAlbum::CPhotoAlbum(QWidget *parent)
    : QWidget(parent)
    , idx1stVisible(0)
    , idxSelected(0)
{
    setupUi(this);
    setFocusPolicy(Qt::WheelFocus);
    connect(toolLeft, SIGNAL(clicked()), this, SLOT(slotLeft()));
    connect(toolRight, SIGNAL(clicked()), this, SLOT(slotRight()));
}

CPhotoAlbum::~CPhotoAlbum()
{
}

void CPhotoAlbum::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);
    updateView();
}


void CPhotoAlbum::mouseReleaseEvent(QMouseEvent * e)
{
    CPhotoViewer dlg(images, 0,this);
    dlg.exec();

    e->accept();
}

void CPhotoAlbum::reload(const QList<CGisItemWpt::image_t>& imgs)
{
    images = imgs;

    if(idxSelected >= images.size())
    {
        idx1stVisible   = 0;
        idxSelected     = 0;
    }

    updateView();
}

void CPhotoAlbum::slotAddImage()
{
    SETTINGS;
    QString path = cfg.value("Paths/lastWptImagePath", QDir::homePath()).toString();
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Select images..."), path);
    if(filenames.isEmpty())
    {
        return;
    }
    foreach(const QString &filename, filenames)
    {
        CGisItemWpt::image_t image;
        image.fileName = filename;
        image.pixmap.load(filename);

        int w = image.pixmap.width();
        int h = image.pixmap.height();

        if(w < h)
        {
            h *= 400.0 / w;
            w  = 400;
        }
        else
        {
            h *= 600.0 / w;
            w  = 600;
        }
        image.pixmap = image.pixmap.scaled(w,h,Qt::KeepAspectRatio, Qt::SmoothTransformation);

        images << image;
    }

    QFileInfo fi(filenames.first());
    path = fi.absolutePath();
    cfg.setValue("Paths/lastWptImagePath", path);

    emit sigChanged(images);
}

void CPhotoAlbum::slotDelImage()
{
    images.removeAt(idxSelected);
    emit sigChanged(images);
}

void CPhotoAlbum::slotRight()
{
    idxSelected++;
    QRect r1 = rects[idxSelected];
    QRect r2 = label->rect();

    while(!r2.contains(r1))
    {
        int w = rects[idx1stVisible].width();
        r1.moveLeft(r1.left() - w);
        idx1stVisible++;
    }

    updateView();
}

void CPhotoAlbum::slotLeft()
{
    idxSelected--;
    QRect r1 = rects[idxSelected];
    QRect r2 = label->rect();

    while(!r2.contains(r1))
    {
        idx1stVisible--;
        int w = rects[idx1stVisible].width();
        r1.moveLeft(r1.left() + w);
    }

    updateView();
}


void CPhotoAlbum::updateView()
{
    toolLeft->setEnabled(idxSelected != 0);
    toolRight->setEnabled(idxSelected != (images.size() - 1));

    if(images.isEmpty())
    {
        hide();
        return;
    }
    setEnabled(true);
    show();

    QPixmap img(label->size());
    img.fill(Qt::black);
    QPainter p(&img);

    int xoff = 0;

    for(int i = 0; i < rects.size() && i < idx1stVisible; i++)
    {
        xoff -= rects[i].width();
    }

    rects.clear();
    for(int i = 0; i < images.size(); i++)
    {
        CGisItemWpt::image_t& image = images[i];

        QImage tmp  = image.pixmap.scaledToHeight(label->height(), Qt::SmoothTransformation);

        if(tmp.width() > label->width())
        {
            tmp  = image.pixmap.scaledToWidth(label->width(), Qt::SmoothTransformation);
        }

        QRect r     = tmp.rect();

        int yoff = (height()- r.height()) / 2;

        p.save();
        p.translate(xoff,yoff);
        p.drawImage(0,0,tmp);
        p.setPen(QPen(Qt::black, 3));
        p.setBrush(Qt::NoBrush);
        p.drawRect(r);
        p.restore();

        r.moveTopLeft(QPoint(xoff, yoff));
        rects << r;

        xoff += tmp.width();
    }

    if(idxSelected < rects.size())
    {
        p.setPen(QPen(Qt::yellow, 5));
        p.drawRect(rects[idxSelected]);
    }

    label->setPixmap(img);
}
