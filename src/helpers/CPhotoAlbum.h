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

#ifndef CPHOTOALBUM_H
#define CPHOTOALBUM_H

#include "ui_IPhotoAlbum.h"
#include <QWidget>

#include <gis/wpt/CGisItemWpt.h>

class CPhotoAlbum : public QWidget, private Ui::IPhotoAlbum
{
    Q_OBJECT
public:
    CPhotoAlbum(QWidget * parent);
    virtual ~CPhotoAlbum();

    void reload(const QList<CGisItemWpt::image_t>& imgs);

signals:
    void sigChanged(const QList<CGisItemWpt::image_t>& imgs);

public slots:
    void slotAddImage();
    void slotDelImage();

protected:
    void resizeEvent(QResizeEvent * e);
    void mouseDoubleClickEvent(QMouseEvent * e);

private slots:
    void slotRight();
    void slotLeft();

private:
    void updateView();
    QList<CGisItemWpt::image_t> images;
    QList<QRect> rects;

    qint32 idx1stVisible;
    qint32 idxSelected;
};

#endif //CPHOTOALBUM_H

