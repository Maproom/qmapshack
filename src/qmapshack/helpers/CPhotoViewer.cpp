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

#include "CMainWindow.h"
#include "CPhotoViewer.h"

#include <QtWidgets>

CPhotoViewer::CPhotoViewer(QList<CGisItemWpt::image_t> &images, int idx, QWidget *parent)
    : QDialog(parent)
    , images(images)
    , idx(idx)
{
    setStyleSheet("background-color:black;");
    setAttribute(Qt::WA_TranslucentBackground);

    Qt::WindowFlags flags = windowFlags() & Qt::WindowType_Mask;
    setWindowFlags(flags | Qt::CustomizeWindowHint);
    showMaximized();

    // check if showMaximized() worked correctly by comparing our size with the MainWindow's size
    // if showMaximized() failed we change our size (manually) to match the MainWindow's size
    // this is hack, but does its job on px i3-wm
    QMainWindow &main = CMainWindow::self();
    if(width() < main.width() && height() < main.height())
    {
        qDebug() << "showMaximized() failed, using MainWindow.frameGeometry()";
        setGeometry(main.frameGeometry());
    }

    if(!images.isEmpty())
    {
        setImageAtIdx(idx);
    }
}

CPhotoViewer::~CPhotoViewer()
{
}


void CPhotoViewer::resizeEvent(QResizeEvent * e)
{
    QDialog::resizeEvent(e);
    setImageAtIdx(idx);
}


void CPhotoViewer::setImageAtIdx(int i)
{
    const QRect& rectScreen = rect();
    const QPoint& center    = rectScreen.center();
    QImage& pixmap          = images[i].pixmap;

    if(!images[i].filePath.isEmpty())
    {
        pixmap = QImage(images[i].filePath);
    }

    double width  = rectScreen.width() - 64;
    double height = rectScreen.height() - 64;

    if(pixmap.width() > width || pixmap.height() > height)
    {
        rectImage = pixmap.rect();

        if(pixmap.width() > width)
        {
            double ratio = width / rectImage.width();
            rectImage.setWidth(width);
            rectImage.setHeight(qFloor(rectImage.height() * ratio) - 1);
        }

        if(pixmap.height() > height)
        {
            double ratio = height / rectImage.height();
            rectImage.setHeight(height);
            rectImage.setWidth(qFloor(rectImage.width() * ratio) - 1);
        }

        rectImage.moveCenter(center);
    }
    else
    {
        rectImage = pixmap.rect();
        rectImage.moveCenter(center);
    }

    rectClose.moveCenter(rectImage.topRight());
    rectPrev.setHeight(rectImage.height());
    rectPrev.moveBottomLeft(rectImage.bottomLeft());
    rectNext.setHeight(rectImage.height());
    rectNext.moveBottomRight(rectImage.bottomRight());
}


void CPhotoViewer::paintEvent(QPaintEvent * e)
{
    QDialog::paintEvent(e);

    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0,0,0,190));
    p.drawRect(rect());

    p.setPen(QPen(Qt::white, 11, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    p.setBrush(Qt::white);
    p.drawRect(rectImage);

    p.drawImage(rectImage, images[idx].pixmap.scaled(rectImage.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    if(idx != (images.size() - 1))
    {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255,255,255,128));
        p.drawRect(rectNext);
        p.drawPixmap(rectNext.left(),rectNext.bottom() - 32, 32, 32, QPixmap("://icons/32x32/Right.png"));
    }
    if(idx != 0)
    {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255,255,255,128));
        p.drawRect(rectPrev);
        p.drawPixmap(rectPrev.left(), rectPrev.bottom() - 32, 32, 32, QPixmap("://icons/32x32/Left.png"));
    }

    p.drawPixmap(rectClose, QPixmap("://icons/32x32/Close.png"));
}

void CPhotoViewer::tryIdxStep(int delta)
{
    int prevIdx = idx;

    idx += delta;
    idx = qMin(idx, images.size() - 1);
    idx = qMax(idx, 0);

    if(prevIdx != idx)
    {
        setImageAtIdx(idx);
        update();
    }
}


void CPhotoViewer::mousePressEvent(QMouseEvent * e)
{
    QPoint pos  = e->pos();

    if(rectClose.contains(pos))
    {
        reject();
    }
    else if(rectNext.contains(pos))
    {
        tryIdxStep(1);
    }
    else if(rectPrev.contains(pos))
    {
        tryIdxStep(-1);
    }
    else if(!rectImage.contains(pos))
    {
        reject();
    }

    e->accept();
}

void CPhotoViewer::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Left:
        tryIdxStep(-1);
        break;

    case Qt::Key_Right:
        tryIdxStep(1);
        break;

    case Qt::Key_Q:
        reject();
        break;
    }

    QDialog::keyPressEvent(e);
}
