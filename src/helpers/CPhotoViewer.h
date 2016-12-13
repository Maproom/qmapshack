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

#ifndef CPHOTOVIEWER_H
#define CPHOTOVIEWER_H

#include <QDialog>

#include "gis/wpt/CGisItemWpt.h"

class CPhotoViewer : public QDialog
{
    Q_OBJECT
public:
    CPhotoViewer(QList<CGisItemWpt::image_t> &images, int idx, QWidget *parent);
    virtual ~CPhotoViewer();

protected:
    void paintEvent(QPaintEvent  *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent  *e) override;
    void keyPressEvent(QKeyEvent    *e) override;

private:
    void tryIdxStep(int delta);

    void setImageAtIdx(int i);

    QList<CGisItemWpt::image_t> images;
    int idx;
    QRect rectImage {0,0,100,100};
    QRect rectClose {0,0,32,32};
    QRect rectPrev {0,0,32,32};
    QRect rectNext {0,0,32,32};
};

#endif //CPHOTOVIEWER_H

