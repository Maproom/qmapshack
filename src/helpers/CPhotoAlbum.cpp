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

#include "CPhotoAlbum.h"

#include <QtWidgets>

CPhotoAlbum::CPhotoAlbum(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

CPhotoAlbum::~CPhotoAlbum()
{
}

void CPhotoAlbum::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);
    updateView();
}

void CPhotoAlbum::reload(const QList<CGisItemWpt::image_t>& imgs)
{
    images = imgs;
    updateView();
}

void CPhotoAlbum::slotAddImage()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Select images..."), "./");
    if(filenames.isEmpty())
    {
        return;
    }
    foreach(const QString& filename, filenames)
    {
        CGisItemWpt::image_t image;
        image.fileName = filename;
        image.pixmap.load(filename);

        images << image;
    }

    emit sigChanged(images);
}

void CPhotoAlbum::slotDelImage()
{

}


void CPhotoAlbum::updateView()
{
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
    for(int i = 0; i < images.size(); i++)
    {
        CGisItemWpt::image_t& image = images[i];

        QImage tmp = image.pixmap.scaledToHeight(height(), Qt::SmoothTransformation);

        p.drawImage(xoff,0,tmp);

        xoff += tmp.width();
    }

    label->setPixmap(img);
}
